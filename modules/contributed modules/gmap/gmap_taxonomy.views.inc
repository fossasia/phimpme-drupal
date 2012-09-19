<?php

/**
 * @file
 * Views data for gmap_taxonomy.
 */

/**
 * Implementation of hook_views_data().
 */
function gmap_taxonomy_views_data() {
  $data['gmap_taxonomy_node']['table']['group'] = t('Node');
  $data['gmap_taxonomy_node']['table']['join'] = array(
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    // @@@ Node_revisions?
  );

  $data['gmap_taxonomy_node']['marker'] = array(
    'title' => t('Marker'),
    'help' => t('The marker name GMap Taxonomy associates with the node.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
