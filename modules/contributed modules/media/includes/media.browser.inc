<?php

/**
 * @file
 * Media Browser page callback
 */
function media_browser($selected = NULL) {
  $output = array();
  $output['#attached']['library'][] = array('media', 'media_browser_page');

  $params = drupal_get_query_parameters();
  array_walk_recursive($params, '_media_recursive_check_plain');
  $params = media_set_browser_params($params);

  // If one or more files have been selected, the browser interaction is now
  // complete. Return empty page content to the dialog which now needs to close,
  // but populate Drupal.settings with information about the selected files.
  if (isset($params['fid'])) {
    $fids = is_array($params['fid']) ? $params['fid'] : array($params['fid']);
    if (!is_numeric($fids[0])) {
      throw new Exception('Error selecting media, fid param is not an fid or an array of fids');
    }
    $files = file_load_multiple($fids);
    foreach ($files as $file) {
      media_browser_build_media_item($file);
    }
    $setting = array('media' => array('selectedMedia' => array_values($files)));
    drupal_add_js($setting, 'setting');
    return $output;
  }

  // Normal browser operation.
  foreach (module_implements('media_browser_plugin_info') as $module) {
    foreach (module_invoke($module, 'media_browser_plugin_info') as $key => $plugin_data) {
      $plugins[$key] = $plugin_data + array(
        '#module' => $module,
        '#weight' => 0,
      );
      $plugins[$key]['#weight'] += count($plugins)/1000;
    }
  }

  // Only the plugins in this array are loaded.
  if (!empty($params['enabledPlugins'])) {
    $plugins = array_intersect_key($plugins, array_fill_keys($params['enabledPlugins'], 1));
  }
  elseif (!empty($params['disabledPlugins'])) {
    $plugins = array_diff_key($plugins, array_fill_keys($params['disabledPlugins'], 1));
  }

  foreach ($plugins as $key => &$plugin) {
    $plugin += module_invoke($plugin['#module'], 'media_browser_plugin_view', $key, $params);
  }

  // Allow modules to change the tab names or whatever else they want to change
  // before we render.  Perhaps this should be an alter on the theming function
  // that we should write to be making the tabs.
  drupal_alter('media_browser_plugins', $plugins);

  $tabs = array(); // List of tabs to render.

  $settings = array('media' => array('browser' => array()));
  $browser_settings =& $settings['media']['browser'];

  //@todo: replace with Tabs module if it gets upgraded.
  foreach (element_children($plugins, TRUE) as $key) {
    $plugin =& $plugins[$key];

    //Add any JS settings
    $browser_settings[$key] = isset($plugin['#settings']) ? $plugin['#settings'] : array();

    // If this is a "ajax" style tab, add the href, otherwise an id.
    $href = isset($plugin['#callback']) ? $plugin['#callback'] : "#media-tab-$key";
    $tabs[] = "<a href='$href'><span>{$plugin['#title']}</span></a>";

    // Create a div for each tab's content.
    $plugin['#prefix'] = <<<EOS
    <div class="media-browser-tab" id="media-tab-$key">
EOS;
    $plugin['#suffix'] = <<<EOS
    </div>
    <!-- End #media-tab-$key -->
EOS;
  }

  drupal_add_js($settings, 'setting');

  $output['tabset'] = array(
    '#prefix' => '<div id="media-browser-tabset">',
    '#suffix' => '</div>',
  );
  $output['tabset']['list'] = array(
    '#markup' => '<ul><li>' . implode('</li><li>', $tabs) . '</li></ul>'
  );

  $output['tabset']['plugins'] = $plugins;
  return $output;
}

/**
 * Provides a singleton of the params passed to the media browser.
 *
 * This is useful in situations like form alters because callers can pass
 * id="wysiywg_form" or whatever they want, and a form alter could pick this up.
 * We may want to change the hook_media_browser_plugin_view() implementations to
 * use this function instead of being passed params for consistency.
 *
 * It also offers a chance for some meddler to meddle with them.
 *
 * @param array $params
 *   An array of parameters provided when a media_browser is launched.
 *
 * @see media_browser()
 */
function media_set_browser_params(array $params = NULL) {
  $stored_params = &drupal_static(__FUNCTION__, array());

  if (isset($params)) {
    $stored_params = $params;
    // Allow modules to alter the parameters.
    drupal_alter('media_browser_params', $stored_params);
  }

  return $stored_params;
}

/**
 * For sanity in grammar.
 *
 * @see media_set_browser_params()
 */
function media_get_browser_params() {
  return media_set_browser_params();
}

/**
 * AJAX Callback function to return a list of media files
 */
function media_browser_list() {
  $params = drupal_get_query_parameters();
  // How do we validate these?  I don't know.
  // I think PDO should protect them, but I'm not 100% certain.
  array_walk_recursive($params, '_media_recursive_check_plain');

  $remote_types = !empty($params['types']) ? $params['types'] : NULL;
  $url_include_patterns = !empty($params['url_include_patterns']) ? $params['url_include_patterns'] : NULL;
  $url_exclude_patterns = !empty($params['url_exclude_patterns']) ? $params['url_exclude_patterns'] : NULL;
  $allowed_schemes = !empty($params['schemes']) ? array_filter($params['schemes']) : array();

  $start = isset($params['start']) ? $params['start'] : 0;
  $limit = isset($params['limit']) ? $params['limit'] : media_variable_get('browser_pager_limit');

  $query = db_select('file_managed', 'f');
  $query->fields('f', array('fid'));
  $query->range($start, $limit);
  $query->orderBy('f.timestamp', 'DESC');

  // Add conditions based on remote file type *or* local allowed extensions.
  $or_condition = db_or();

  // Include local files with the allowed extensions.
  if (!empty($params['file_extensions'])) {
    $extensions = array_filter(explode(' ', $params['file_extensions']));
    $local_wrappers = array_intersect_key(media_get_local_stream_wrappers(), $allowed_schemes);
    if (!empty($local_wrappers) && !empty($extensions)) {
      $local_condition = db_or();
      foreach (array_keys($local_wrappers) as $scheme) {
        foreach ($extensions as $extension) {
          $local_condition->condition('f.uri', db_like($scheme . '://') . '%' . db_like('.' . $extension), 'LIKE');
        }
      }
      $or_condition->condition($local_condition);
    }
  }

  // Include remote files with the allowed file types.
  if (!empty($remote_types)) {
    $remote_wrappers = array_intersect_key(media_get_remote_stream_wrappers(), $allowed_schemes);
    if (!empty($remote_wrappers)) {
      $remote_condition = db_and();
      $wrapper_condition = db_or();
      foreach (array_keys($remote_wrappers) as $scheme) {
        $wrapper_condition->condition('f.uri', db_like($scheme . '://') . '%', 'LIKE');
      }
      $remote_condition->condition($wrapper_condition);
      $remote_condition->condition('f.type', $remote_types, 'IN');
      $or_condition->condition($remote_condition);
    }
  }

  if ($or_condition->count()) {
    $query->condition($or_condition);
  }

  if ($url_include_patterns) {
    $query->condition('f.uri', '%' . db_like($url_include_patterns) . '%', 'LIKE');
    // Insert stream related restrictions here.
  }
  if ($url_exclude_patterns) {
    $query->condition('f.uri', '%' . db_like($url_exclude_patterns) . '%', 'NOT LIKE');
  }

  // @todo Implement granular editorial access: http://drupal.org/node/696970.
  //   In the meantime, protect information about private files from being
  //   discovered by unprivileged users. See also media_view_page().
  if (!user_access('administer media')) {
    $query->condition('f.uri', db_like('private://') . '%', 'NOT LIKE');
  }

  $query->condition('f.status', FILE_STATUS_PERMANENT);

  foreach (array_keys(media_get_hidden_stream_wrappers()) as $name) {
    $query->condition('f.uri', db_like($name . '://') . '%', 'NOT LIKE');
  }

  $fids = $query->execute()->fetchCol();
  $files = file_load_multiple($fids);
  foreach ($files as $file) {
    media_browser_build_media_item($file);
  }

  drupal_json_output(array('media' => array_values($files)));
  exit();
}

/**
 * Silly function to recursively run check_plain on an array.
 *
 * There is probably something in core I am not aware of that does this.
 *
 * @param $value
 * @param $key
 */
function _media_recursive_check_plain(&$value, $key) {
  $value = check_plain($value);
}

/**
 * Attaches media browser javascript to an element.
 *
 * @param $element
 *  The element array to attach to.
 */
function media_attach_browser_js(&$element) {
  $javascript = media_browser_js();
  foreach ($javascript as $key => $definitions) {
    foreach ($definitions as $definition) {
      $element['#attached'][$key][] = $definition;
    }
  }
}

/**
 * Helper function to define browser javascript.
 */
function media_browser_js() {
  $settings = array(
    'browserUrl' => url('media/browser',
      array('query' => array('render' => 'media-popup'))),
    'styleSelectorUrl' => url('media/-media_id-/format-form',
      array('query' => array('render' => 'media-popup'))),
  );

  $js = array(
    'library' => array(
      array('media', 'media_browser'),
    ),
    'js' => array(
      array(
       'data' => array('media' => $settings),
       'type' => 'setting',
      ),
    ),
  );
  return $js;
}

/**
 * Menu callback for testing the media browser
 */
function media_browser_testbed($form) {
  media_attach_browser_js($form);

  $form['test_element'] = array(
    '#type' => 'media',
    '#media_options' => array(
      'global' => array(
        'types' => array('video', 'audio'),
      ),
    )
  );

  $launcher = '<a href="#" id="launcher"> Launch Media Browser</a>';

  $form['options'] = array(
    '#type' => 'textarea',
    '#title' => 'Options (JSON)',
    '#rows' => 10,
  );

  $form['launcher'] = array(
    '#markup' => $launcher,
  );

  $form['result'] = array(
    '#type' => 'textarea',
    '#title' => 'Result',
  );


  $js = <<<EOF
    Drupal.behaviors.mediaTest = {
    attach: function(context, settings) {
      var delim = "---";
      var recentOptions = [];
      var recentOptionsCookie = jQuery.cookie("recentOptions");
      if (recentOptionsCookie) {
        recentOptions = recentOptionsCookie.split("---");
      }

      var recentSelectBox = jQuery('<select id="recent_options" style="width:100%"></select>').change(function() { jQuery('#edit-options').val(jQuery(this).val())});

      jQuery('.form-item-options').append('<label for="recent_options">Recent</a>');
      jQuery('.form-item-options').append(recentSelectBox);
      jQuery('.form-item-options').append(jQuery('<a href="#">Reset</a>').click(function() {alert('reset'); jQuery.cookie("recentOptions", null); window.location.reload(); }));

      jQuery.each(recentOptions, function (idx, val) {
        recentSelectBox.append(jQuery('<option></option>').val(val).html(val));
      });


      jQuery('#launcher').click(function () {
        jQuery('#edit-result').val('');
        var options = {};
        var optionsTxt = jQuery('#edit-options').val();
        if (optionsTxt) {
          // Store it in the recent box
          recentOptionsCookie += "---" + optionsTxt
          jQuery.cookie("recentOptions", recentOptionsCookie, { expires: 7 });
          recentSelectBox.append(jQuery('<option></option>').val(optionsTxt).html(optionsTxt));
          options = eval('(' + optionsTxt + ')');
        }
        Drupal.media.popups.mediaBrowser(Drupal.behaviors.mediaTest.mediaSelected, options);
        return false;
      });
    },

    mediaSelected: function(selectedMedia) {
      var result = JSON.stringify(selectedMedia);
        jQuery('#edit-result').val(result);
    }
  }

EOF;

  drupal_add_js($js, array('type' => 'inline'));
  return $form;
}

/**
 * Adds properties to the passed in file that are needed by the media browser JS code.
 */
function media_browser_build_media_item($file) {
  $preview = media_get_thumbnail_preview($file);
  $file->preview = drupal_render($preview);
  $file->url = file_create_url($file->uri);
}
