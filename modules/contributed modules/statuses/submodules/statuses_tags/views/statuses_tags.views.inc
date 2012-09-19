<?php

/**
 * @file
 *   Provide views data and handlers for the Statuses Tags module.
 */

/**
 * Implementation of hook_views_default_views_alter().
 */
function statuses_tags_views_default_views_alter(&$views) {
  // We can't assume that $views always contains the full set of Views;
  // one case where it doesn't is when evaluating Features.
  if (!isset($views['facebook_status_tags'])) {
    return;
  }

  // Use the #hashtag URL from the administrative settings in the view.
  $url = variable_get('statuses_tags_url', 'statuses/term');
  $views['statuses_tags']->display['page_1']->display_options['path'] = $url;
  if (strpos($url, '%') === FALSE) {
    $url .= '/%';
  }
  $url .= '/feed';
  $views['statuses_tags']->display['feed_1']->display_options['path'] = $url;

  // Use the correct #hashtag vocabulary ID.
  $vid = variable_get('statuses_tags_vid', -1);
  if ($vid != -1) {
    foreach ($views['statuses_tags']->display as $id => $display) {
      if (isset($views['statuses_tags']->display[$id]->display_options['arguments']['has-specific-tag'])) {
        $views['statuses_tags']->display[$id]->display_options['arguments']['has-specific-tag']['validate_argument_vocabulary'] = array($vid => $vid);
      }
    }
  }
}

/**
 * Implements hook_views_data().
 */
function statuses_tags_views_data() {
  $data['statuses_tags']['table']['group']  = t('Statuses Tags');

  // Advertise this table as a possible base table.
  $data['statuses_tags']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('Statuses Tags'),
    'help' => t('#hashtags and @mentions.'),
    'weight' => 10,
  );

  $data['statuses_tags']['sid'] = array(
    'title' => t('Status ID'),
    'help' => t('The ID of the status update.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['statuses_tags']['rid'] = array(
    'title' => t('Tag ID'),
    'help' => t('The Tag ID of the status - either a UID or Term ID depending on the reference type.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['statuses_tags']['type'] = array(
    'title' => t('Tag type'),
    'help' => t('The type of tag - User or Taxonomy Term.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'statuses_tags_views_handler_filter_string_type',
    ),
  );

  $data['statuses_tags']['name'] = array(
    'title' => t('Tag Name'),
    'help' => t('The name of the tag - a username or taxonomy term name.'),
    'field' => array(
      'handler' => 'statuses_tags_views_handler_field_name',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['statuses']['has-tags'] = array(
    'title' => t('Status has tags'),
    'help' => t('Only show statuses with tags.'),
    'filter' => array(
      'field' => 'sid',
      'handler' => 'statuses_tags_views_handler_filter_has_tag',
    ),
  );

  $data['statuses']['has-specific-tag'] = array(
    'title' => t('Status has specific tag'),
    'help' => t('Only show statuses with a specific tag.'),
    'filter' => array(
      'field' => 'sid',
      'handler' => 'statuses_tags_views_handler_filter_has_this_tag',
    ),
    'argument' => array(
      'field' => 'sid',
      'handler' => 'statuses_tags_views_handler_argument_has_this_tag',
    ),
  );

  $data['statuses']['has-specific-tag-id'] = array(
    'title' => t('Status has specific tag ID'),
    'help' => t('Only show statuses with a specific tag ID.'),
    'argument' => array(
      'field' => 'sid',
      'handler' => 'statuses_tags_views_handler_argument_has_this_tag_id',
    ),
  );

  $data['statuses']['tags-terms'] = array(
    'title' => t('All Tags'),
    'help' => t('Show all #hashtags and/or @mentions associated with the status'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'statuses_tags_views_handler_field_all_terms',
      'click sortable' => FALSE,
    ),
  );

  return $data;
}

/**
 * Implements hook_views_handlers().
 */
function statuses_tags_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'statuses_tags') . '/views',
    ),
    'handlers' => array(
      'statuses_tags_views_handler_argument_has_this_tag' => array(
        'parent' => 'views_handler_argument_string',
      ),
      'statuses_tags_views_handler_argument_has_this_tag_id' => array(
        'parent' => 'views_handler_argument_numeric',
      ),
      'statuses_tags_views_handler_field_all_terms' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_tags_views_handler_field_message' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_tags_views_handler_field_name' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_tags_views_handler_filter_has_tag' => array(
        'parent' => 'views_handler_filter',
      ),
      'statuses_tags_views_handler_filter_has_this_tag' => array(
        'parent' => 'views_handler_filter_string',
      ),
      'statuses_tags_views_handler_filter_string_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}
