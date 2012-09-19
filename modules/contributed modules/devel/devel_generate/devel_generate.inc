<?php

/**
 * Generate some random users.
 *
 * @param $num
 *  Number of users to generate.
 * @param $kill
 *  Boolean that indicates if existing users should be removed first.
 * @param $age
 *  The max age of each randomly-generated user, in seconds.
 * @param $roles
 *  An array of role IDs that the users should receive.
 */
function devel_create_users($num, $kill, $age = 0, $roles = array()) {
  $url = parse_url($GLOBALS['base_url']);
  if ($kill) {
    $uids = db_select('users', 'u')
            ->fields('u', array('uid'))
            ->condition('uid', 1, '>')
            ->execute()
            ->fetchAllAssoc('uid');
    user_delete_multiple(array_keys($uids));
    drupal_set_message(format_plural(count($uids), '1 user deleted', '@count users deleted.'));
  }
  // Determine if we should create user pictures.
  $pic_config = FALSE;
  module_load_include('inc', 'system', 'image.gd');
  if (variable_get('user_pictures', 0) && function_exists('image_gd_check_settings') && image_gd_check_settings()) {
    $pic_config['path'] = variable_get('user_picture_path', 'pictures');
    list($pic_config['width'], $pic_config['height']) = explode('x', variable_get('user_picture_dimensions', '85x85'));
  }

  if ($num > 0) {
    $names = array();
    while (count($names) < $num) {
      $name = devel_generate_word(mt_rand(6, 12));
      $names[$name] = '';
    }

    if (empty($roles)) {
      $roles = array(DRUPAL_AUTHENTICATED_RID);
    }
    foreach ($names as $name => $value) {
      $edit = array(
        'uid'     => NULL,
        'name'    => $name,
        'pass'    => NULL, // No password avoids user_hash_password() which is expensive.
        'mail'    => $name . '@' . $url['host'],
        'status'  => 1,
        'created' => REQUEST_TIME - mt_rand(0, $age),
        'roles' => drupal_map_assoc($roles),
      );

      // Populate all core fields on behalf of field.module
      module_load_include('inc', 'devel_generate', 'devel_generate.fields');
      $edit = (object) $edit;
      $edit->language = LANGUAGE_NONE;
      devel_generate_fields($edit, 'user', 'user');
      $edit = (array) $edit;

      $account = user_save(drupal_anonymous_user(), $edit);

      if ($pic_config) {
        // Since the image.module should scale the picture just pick an
        // arbitrary size that it's too big for our font.
        $im = imagecreatetruecolor(200, 200);

        // Randomize the foreground using the md5 of the user id, then invert it
        // for the background color so there's enough contrast to read the text.
        $parts = array_map('hexdec', str_split(md5($account->uid), 2));
        $fg = imagecolorallocate($im, $parts[1], $parts[3], $parts[5]);
        $bg = imagecolorallocate($im, 255 - $parts[0], 255 - $parts[1], 255 - $parts[2]);

        // Fill the background then print their user info.
        imagefill($im, 0, 0, $bg);
        imagestring($im, 5, 5, 5, "#" . $account->uid, $fg);
        imagestring($im, 5, 5, 25, $account->name, $fg);


        // Create an empty, managed file where we want the user's picture to
        // be so we can have GD overwrite it with the image.
        $picture_directory =  variable_get('file_default_scheme', 'public') . '://' . variable_get('user_picture_path', 'pictures');
        file_prepare_directory($picture_directory, FILE_CREATE_DIRECTORY);
        $destination = file_stream_wrapper_uri_normalize($picture_directory . '/picture-' . $account->uid . '.png');
        $file = file_save_data('', $destination);

        // GD doesn't like stream wrapped paths so convert the URI to a normal
        // file system path.
        if (isset($file) && $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri)) {
          imagepng($im, $wrapper->realpath());
        }
        imagedestroy($im);

        // Clear the cached filesize, set the owner and MIME-type then re-save
        // the file.
        clearstatcache();
        $file->uid = $account->uid;
        $file->filemime = 'image/png';
        $file = file_save($file);

        // Save the user record with the new picture.
        $edit = (array) $account;
        $edit['picture'] = $file;
        user_save($account, $edit);
      }
    }
  }
  drupal_set_message(t('!num_users created.', array('!num_users' => format_plural($num, '1 user', '@count users'))));
}


/**
 * The main API function for creating content.
 *
 * See devel_generate_content_form() for the supported keys in $form_state['values'].
 * Other modules may participate by form_alter() on that form and then handling their data during hook_nodeapi('pre_save') or in own submit handler for the form.
 *
 * @param string $form_state
 * @return void
 */
function devel_generate_content($form_state) {
  if (!empty($form_state['values']['kill_content'])) {
    devel_generate_content_kill($form_state['values']);
  }

  if (count($form_state['values']['node_types'])) {
    // Generate nodes.
    devel_generate_content_pre_node($form_state['values']);
    $start = time();
    for ($i = 1; $i <= $form_state['values']['num_nodes']; $i++) {
      devel_generate_content_add_node($form_state['values']);
      if (function_exists('drush_log') && $i % drush_get_option('feedback', 1000) == 0) {
        $now = time();
        drush_log(dt('Completed !feedback nodes (!rate nodes/min)', array('!feedback' => drush_get_option('feedback', 1000), '!rate' => (drush_get_option('feedback', 1000)*60)/($now-$start))), 'ok');
        $start = $now;
      }
    }
  }

  devel_generate_set_message(format_plural($form_state['values']['num_nodes'], '1 node created.', 'Finished creating @count nodes'));
}

function devel_generate_add_comments($node, $users, $max_comments, $title_length = 8) {
  $num_comments = mt_rand(1, $max_comments);
  for ($i = 1; $i <= $num_comments; $i++) {
    $comment = new stdClass;
    $comment->nid = $node->nid;
    $comment->cid = NULL;
    $comment->name = 'devel generate';
    $comment->mail = 'devel_generate@example.com';
    $comment->timestamp = mt_rand($node->created, REQUEST_TIME);

    switch ($i % 3) {
      case 1:
        $comment->pid = db_query_range("SELECT cid FROM {comment} WHERE pid = 0 AND nid = :nid ORDER BY RAND()", 0, 1, array(':nid' => $comment->nid))->fetchField();
        break;
      case 2:
        $comment->pid = db_query_range("SELECT cid FROM {comment} WHERE pid > 0 AND nid = :nid ORDER BY RAND()", 0, 1, array(':nid' => $comment->nid))->fetchField();
        break;
      default:
        $comment->pid = 0;
    }

    // The subject column has a max character length of 64
    // See bug: http://drupal.org/node/1024340
    $comment->subject = substr(devel_create_greeking(mt_rand(2, $title_length), TRUE), 0, 63);
    $comment->uid = $users[array_rand($users)];
    $comment->language = LANGUAGE_NONE;
    // Populate all core fields on behalf of field.module
    module_load_include('inc', 'devel_generate', 'devel_generate.fields');
    devel_generate_fields($comment, 'comment', 'comment_node_' . $node->type);
    comment_save($comment);
  }
}

function devel_generate_vocabs($records, $maxlength = 12, $types = array('page', 'article')) {
  $vocs = array();

  // Insert new data:
  for ($i = 1; $i <= $records; $i++) {
    $voc = new stdClass();
    $voc->name = devel_generate_word(mt_rand(2, $maxlength));
    $voc->machine_name = drupal_strtolower($voc->name);
    $voc->description = "description of ". $voc->name;
    // TODO: not working
    $voc->nodes = array_flip(array($types[array_rand($types)]));
    foreach ($voc->nodes as $key => $value) {
      $voc->nodes[$key] = $key;
    }

    $voc->multiple = 1;
    $voc->required = 0;
    $voc->relations = 1;
    $voc->hierarchy = 1;
    $voc->weight = mt_rand(0,10);
    $voc->language = LANGUAGE_NONE;

    taxonomy_vocabulary_save($voc);
    $vocs[] = $voc->name;

    unset($voc);
  }
  return $vocs;
}

function devel_generate_terms($records, $vocabs, $maxlength = 12) {
  $terms = array();

  // Insert new data:
  $max = db_query('SELECT MAX(tid) FROM {taxonomy_term_data}')->fetchField();
  $start = time();
  for ($i = 1; $i <= $records; $i++) {
    $term = new stdClass;
    switch ($i % 2) {
      case 1:
        // Set vid and vocabulary_machine_name properties.
        $vocab = $vocabs[array_rand($vocabs)];
        $term->vid = $vocab->vid;
        $term->vocabulary_machine_name = $vocab->machine_name;
        // Don't set a parent. Handled by taxonomy_save_term()
        // $term->parent = 0;
        break;
      default:
        while (TRUE) {
          // Keep trying to find a random parent.
          $candidate = mt_rand(1, $max);
          $query = db_select('taxonomy_term_data', 't');
          $query->innerJoin('taxonomy_vocabulary', 'v', 't.vid = v.vid');
          $parent = $query
                    ->fields('t', array('tid', 'vid'))
                    ->fields('v', array('machine_name'))
                    ->condition('v.vid', array_keys($vocabs), 'IN')
                    ->condition('t.tid', $candidate, '>=')
                    ->range(0,1)
                    ->execute()
                    ->fetchAssoc();
          if ($parent['tid']) {
            break;
          }
        }
        $term->parent = $parent['tid'];
        // Slight speedup due to this property being set.
        $term->vocabulary_machine_name = $parent['machine_name'];
        $term->vid = $parent['vid'];
        break;
    }

    $term->name = devel_generate_word(mt_rand(2, $maxlength));
    $term->description = "description of ". $term->name;
    $term->format = filter_fallback_format();
    $term->weight = mt_rand(0, 10);
    $term->language = LANGUAGE_NONE;

    // Populate all core fields on behalf of field.module
    module_load_include('inc', 'devel_generate', 'devel_generate.fields');
    devel_generate_fields($term, 'term', $term->vocabulary_machine_name);

    if ($status = taxonomy_term_save($term)) {
      $max += 1;
      if (function_exists('drush_log')) {
        $feedback = drush_get_option('feedback', 1000);
        if ($i % $feedback == 0) {
          $now = time();
          drush_log(dt('Completed !feedback terms (!rate terms/min)', array('!feedback' => $feedback, '!rate' => $feedback*60 / ($now-$start) )), 'ok');
          $start = $now;
        }
      }

      // Limit memory usage. Only report first 20 created terms.
      if ($i < 20) {
        $terms[] = $term->name;
      }

      unset($term);
    }
  }
  return $terms;
}

// TODO: use taxonomy_get_entries once that exists.
function devel_generate_get_terms($vids) {
  return db_select('taxonomy_term_data', 'td')
           ->fields('td', array('tid'))
           ->condition('vid', $vids, 'IN')
           ->orderBy('tid', 'ASC')
           ->execute()
           ->fetchCol('tid');
}

function devel_generate_term_data($vocabs, $num_terms, $title_length, $kill) {
  if ($kill) {
    foreach (devel_generate_get_terms(array_keys($vocabs)) as $tid) {
      taxonomy_term_delete($tid);
    }
    drupal_set_message(t('Deleted existing terms.'));
  }

  $new_terms = devel_generate_terms($num_terms, $vocabs, $title_length);
  if (!empty($new_terms)) {
    drupal_set_message(t('Created the following new terms: !terms', array('!terms' => theme('item_list', array('items' => $new_terms)))));
  }
}

function devel_generate_vocab_data($num_vocab, $title_length, $kill) {

  if ($kill) {
    foreach (taxonomy_get_vocabularies() as $vid => $vocab) {
      taxonomy_vocabulary_delete($vid);
    }
    drupal_set_message(t('Deleted existing vocabularies.'));
  }

  $new_vocs = devel_generate_vocabs($num_vocab, $title_length);
  if (!empty($new_vocs)) {
    drupal_set_message(t('Created the following new vocabularies: !vocs', array('!vocs' => theme('item_list', array('items' => $new_vocs)))));
  }
}

function devel_generate_menu_data($num_menus, $existing_menus, $num_links, $title_length, $link_types, $max_depth, $max_width, $kill) {
  // Delete menus and menu links.
  if ($kill) {
    if (module_exists('menu')) {
      foreach (menu_get_menus(FALSE) as $menu => $menu_title) {
        if (strpos($menu, 'devel-') === 0) {
          $menu = menu_load($menu);
          menu_delete($menu);
        }
      }
    }
    // Delete menu links generated by devel.
    $result = db_select('menu_links', 'm')
      ->fields('m', array('mlid'))
      ->condition('m.menu_name', 'devel', '<>')
      // Look for the serialized version of 'devel' => TRUE.
      ->condition('m.options', '%' . db_like('s:5:"devel";b:1') . '%', 'LIKE')
      ->execute();
    foreach ($result as $link) {
      menu_link_delete($link->mlid);
    }
    drupal_set_message(t('Deleted existing menus and links.'));
  }

  // Generate new menus.
  $new_menus = devel_generate_menus($num_menus, $title_length);
  if (!empty($new_menus)) {
    drupal_set_message(t('Created the following new menus: !menus', array('!menus' => theme('item_list', array('items' => $new_menus)))));
  }

  // Generate new menu links.
  $menus = $new_menus + $existing_menus;
  $new_links = devel_generate_links($num_links, $menus, $title_length, $link_types, $max_depth, $max_width);
  drupal_set_message(t('Created @count new menu links.', array('@count' => count($new_links))));
}

/**
 * Generates new menus.
 */
function devel_generate_menus($num_menus, $title_length = 12) {
  $menus = array();

  if (!module_exists('menu')) {
    $num_menus = 0;
  }

  for ($i = 1; $i <= $num_menus; $i++) {
    $menu = array();
    $menu['title'] = devel_generate_word(mt_rand(2, $title_length));
    $menu['menu_name'] = 'devel-' . drupal_strtolower($menu['title']);
    $menu['description'] = t('Description of @name', array('@name' => $menu['title']));
    menu_save($menu);
    $menus[$menu['menu_name']] = $menu['title'];
  }

  return $menus;
}

/**
 * Generates menu links in a tree structure.
 */
function devel_generate_links($num_links, $menus, $title_length, $link_types, $max_depth, $max_width) {
  $links = array();
  $menus = array_keys(array_filter($menus));
  $link_types = array_keys(array_filter($link_types));

  $nids = array();
  for ($i = 1; $i <= $num_links; $i++) {
    // Pick a random menu.
    $menu_name = $menus[array_rand($menus)];
    // Build up our link.
    $link = array(
      'menu_name'   => $menu_name,
      'options'     => array('devel' => TRUE),
      'weight'      => mt_rand(-50, 50),
      'mlid'        => 0,
      'link_title'  => devel_generate_word(mt_rand(2, $title_length)),
    );
    $link['options']['attributes']['title'] = t('Description of @title.', array('@title' => $link['link_title']));

    // For the first $max_width items, make first level links.
    if ($i <= $max_width) {
      $depth = 0;
    }
    else {
      // Otherwise, get a random parent menu depth.
      $depth = mt_rand(1, $max_depth - 1);
    }
    // Get a random parent link from the proper depth.
    do {
      $link['plid'] = db_select('menu_links', 'm')
        ->fields('m', array('mlid'))
        ->condition('m.menu_name', $menus, 'IN')
        ->condition('m.depth', $depth)
        ->range(0, 1)
        ->orderRandom()
        ->execute()
        ->fetchField();
      $depth--;
    } while (!$link['plid'] && $depth > 0);
    if (!$link['plid']) {
      $link['plid'] = 0;
    }

    $link_type = array_rand($link_types);
    switch ($link_types[$link_type]) {
      case 'node':
        // Grab a random node ID.
        $select = db_select('node', 'n')
          ->fields('n', array('nid', 'title'))
          ->condition('n.status', 1)
          ->range(0, 1)
          ->orderRandom();
        // Don't put a node into the menu twice.
        if (!empty($nids[$menu_name])) {
          $select->condition('n.nid', $nids[$menu_name], 'NOT IN');
        }
        $node = $select->execute()->fetchAssoc();
        if (isset($node['nid'])) {
          $nids[$menu_name][] = $node['nid'];
          $link['link_path'] = $link['router_path'] = 'node/' . $node['nid'];
          $link['link_title'] = $node['title'];
          break;
        }
      case 'external':
        $link['link_path'] = 'http://www.example.com/';
        break;
      case 'front':
        $link['link_path'] = $link['router_path'] = '<front>';
        break;
      default:
        $link['devel_link_type'] = $link_type;
        break;
    }

    menu_link_save($link);

    $links[$link['mlid']] = $link['link_title'];
  }

  return $links;
}

function devel_generate_word($length){
  mt_srand((double)microtime()*1000000);

  $vowels = array("a", "e", "i", "o", "u");
  $cons = array("b", "c", "d", "g", "h", "j", "k", "l", "m", "n", "p", "r", "s", "t", "u", "v", "w", "tr",
  "cr", "br", "fr", "th", "dr", "ch", "ph", "wr", "st", "sp", "sw", "pr", "sl", "cl", "sh");

  $num_vowels = count($vowels);
  $num_cons = count($cons);
  $word = '';

  while(strlen($word) < $length){
    $word .= $cons[mt_rand(0, $num_cons - 1)] . $vowels[mt_rand(0, $num_vowels - 1)];
  }

  return substr($word, 0, $length);
}

function devel_create_content($type = NULL) {
  $nparas = mt_rand(1,12);
  $type = empty($type) ? mt_rand(0,3) : $type;

  $output = "";
  switch($type % 3) {
    // MW: This appears undesireable. Was giving <p> in text fields
    // case 1: // html
    //       for ($i = 1; $i <= $nparas; $i++) {
    //         $output .= devel_create_para(mt_rand(10,60),1);
    //       }
    //       break;
    //
    //     case 2: // brs only
    //       for ($i = 1; $i <= $nparas; $i++) {
    //         $output .= devel_create_para(mt_rand(10,60),2);
    //       }
    //       break;

    default: // plain text
      for ($i = 1; $i <= $nparas; $i++) {
        $output .= devel_create_para(mt_rand(10,60)) ."\n\n";
      }
  }

  return $output;
}

function devel_create_para($words, $type = 0) {
  $output = '';
  switch ($type) {
    case 1:
      $output .= "<p>" . devel_create_greeking($words)  . "</p>";
      break;

    case 2:
      $output .= devel_create_greeking($words) . "<br />";
      break;

    default:
      $output .= devel_create_greeking($words);
  }
  return $output;
}

function devel_create_greeking($word_count, $title = FALSE) {
  $dictionary = array("abbas", "abdo", "abico", "abigo", "abluo", "accumsan",
    "acsi", "ad", "adipiscing", "aliquam", "aliquip", "amet", "antehabeo",
    "appellatio", "aptent", "at", "augue", "autem", "bene", "blandit",
    "brevitas", "caecus", "camur", "capto", "causa", "cogo", "comis",
    "commodo", "commoveo", "consectetuer", "consequat", "conventio", "cui",
    "damnum", "decet", "defui", "diam", "dignissim", "distineo", "dolor",
    "dolore", "dolus", "duis", "ea", "eligo", "elit", "enim", "erat",
    "eros", "esca", "esse", "et", "eu", "euismod", "eum", "ex", "exerci",
    "exputo", "facilisi", "facilisis", "fere", "feugiat", "gemino",
    "genitus", "gilvus", "gravis", "haero", "hendrerit", "hos", "huic",
    "humo", "iaceo", "ibidem", "ideo", "ille", "illum", "immitto",
    "importunus", "imputo", "in", "incassum", "inhibeo", "interdico",
    "iriure", "iusto", "iustum", "jugis", "jumentum", "jus", "laoreet",
    "lenis", "letalis", "lobortis", "loquor", "lucidus", "luctus", "ludus",
    "luptatum", "macto", "magna", "mauris", "melior", "metuo", "meus",
    "minim", "modo", "molior", "mos", "natu", "neo", "neque", "nibh",
    "nimis", "nisl", "nobis", "nostrud", "nulla", "nunc", "nutus", "obruo",
    "occuro", "odio", "olim", "oppeto", "os", "pagus", "pala", "paratus",
    "patria", "paulatim", "pecus", "persto", "pertineo", "plaga", "pneum",
    "populus", "praemitto", "praesent", "premo", "probo", "proprius",
    "quadrum", "quae", "qui", "quia", "quibus", "quidem", "quidne", "quis",
    "ratis", "refero", "refoveo", "roto", "rusticus", "saepius",
    "sagaciter", "saluto", "scisco", "secundum", "sed", "si", "similis",
    "singularis", "sino", "sit", "sudo", "suscipere", "suscipit", "tamen",
    "tation", "te", "tego", "tincidunt", "torqueo", "tum", "turpis",
    "typicus", "ulciscor", "ullamcorper", "usitas", "ut", "utinam",
    "utrum", "uxor", "valde", "valetudo", "validus", "vel", "velit",
    "veniam", "venio", "vereor", "vero", "verto", "vicis", "vindico",
    "virtus", "voco", "volutpat", "vulpes", "vulputate", "wisi", "ymo",
    "zelus");
  $dictionary_flipped = array_flip($dictionary);

  $greeking = '';

  if (!$title) {
    $words_remaining = $word_count;
    while ($words_remaining > 0) {
      $sentence_length = mt_rand(3, 10);
      $words = array_rand($dictionary_flipped, $sentence_length);
      $sentence = implode(' ', $words);
      $greeking .= ucfirst($sentence) . '. ';
      $words_remaining -= $sentence_length;
    }
  }
  else {
    // Use slightly different method for titles.
    $words = array_rand($dictionary_flipped, $word_count);
    $words = is_array($words) ? implode(' ', $words) : $words;
    $greeking = ucwords($words);
  }

  // Work around possible php garbage collection bug. Without an unset(), this
  // function gets very expensive over many calls (php 5.2.11).
  unset($dictionary, $dictionary_flipped);
  return trim($greeking);
}

function devel_generate_add_terms(&$node) {
  $vocabs = taxonomy_get_vocabularies($node->type);
  foreach ($vocabs as $vocab) {
    $sql = "SELECT tid FROM {taxonomy_term_data} WHERE vid = :vid ORDER BY RAND()";
    $result = db_query_range($sql, 0, 5 , array(':vid' => $vocab->vid));
    foreach($result as $row) {
      $node->taxonomy[] = $row->tid;
      if (!$vocab->multiple) {
        break;
      }
    }
  }
}

function devel_get_users() {
  $users = array();
  $result = db_query_range("SELECT uid FROM {users}", 0, 50);
  foreach ($result as $record) {
    $users[] = $record->uid;
  }
  return $users;
}

/**
 * Generate statistics information for a node.
 *
 * @param $node
 *   A node object.
 */
function devel_generate_add_statistics($node) {
  $statistic = array(
    'nid' => $node->nid,
    'totalcount' => mt_rand(0, 500),
    'timestamp' => REQUEST_TIME - mt_rand(0, $node->created),
  );
  $statistic['daycount'] = mt_rand(0, $statistic['totalcount']);
  db_insert('node_counter')->fields($statistic)->execute();
}

/**
 * Handle the devel_generate_content_form request to kill all of the content.
 * This is used by both the batch and non-batch branches of the code.
 *
 * @param $num
 *   array of options obtained from devel_generate_content_form.
 */
function devel_generate_content_kill($values) {
  $results = db_select('node', 'n')
              ->fields('n', array('nid'))
              ->condition('type', $values['node_types'], 'IN')
              ->execute();
  foreach ($results as $result) {
    $nids[] = $result->nid;
  }

  if (!empty($nids)) {
    node_delete_multiple($nids);
    drupal_set_message(t('Deleted %count nodes.', array('%count' => count($nids))));
  }
}

/**
 * Pre-process the devel_generate_content_form request.  This is needed so
 * batch api can get the list of users once.  This is used by both the batch
 * and non-batch branches of the code.
 *
 * @param $num
 *   array of options obtained from devel_generate_content_form.
 */
function devel_generate_content_pre_node(&$results) {
  // Get user id.
  $users = devel_get_users();
  $users = array_merge($users, array('0'));
  $results['users'] = $users;
}

/**
 * Create one node. Used by both batch and non-batch code branches.
 *
 * @param $num
 *   array of options obtained from devel_generate_content_form.
 */
function devel_generate_content_add_node(&$results) {
  $node = new stdClass();
  $node->nid = NULL;

  // Insert new data:
  $node->type = array_rand($results['node_types']);
  node_object_prepare($node);
  $users = $results['users'];
  $node->uid = $users[array_rand($users)];
  $type = node_type_get_type($node);
  $node->title = $type->has_title ? devel_create_greeking(mt_rand(2, $results['title_length']), TRUE) : '';
  $node->revision = mt_rand(0,1);
  $node->promote = mt_rand(0, 1);
  // Avoid NOTICE.
  if (!isset($results['time_range'])) {
    $results['time_range'] = 0;
  }

  devel_generate_set_language($results, $node);

  $node->created = REQUEST_TIME - mt_rand(0, $results['time_range']);

  // A flag to let hook_nodeapi() implementations know that this is a generated node.
  $node->devel_generate = $results;

  // Populate all core fields on behalf of field.module
  module_load_include('inc', 'devel_generate', 'devel_generate.fields');
  devel_generate_fields($node, 'node', $node->type);

  // See devel_generate_nodeapi() for actions that happen before and after this save.
  node_save($node);
}

/*
 * Populate $object->language based on $results
 */
function devel_generate_set_language($results, $object) {
  if (isset($results['add_language'])) {
    $languages = $results['add_language'];
    $object->language = $languages[array_rand($languages)];
  }
  else {
    $default = language_default('language');
    $object->language =  $default == 'en' ? LANGUAGE_NONE : $default;
  }
}
