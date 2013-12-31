<?php

/**
 * @file
 *   Views integration for the Statuses Domain Access module.
 */

/**
 * Implementats hook_views_data().
 */
function fbss_domain_views_data() {
  // Basic table information.
  $data['fbss_domain']['table']['join'] = array(
    'statuses' => array(
      'left_field' => 'sid',
      'field' => 'sid',
      'type' => 'LEFT',
    ),
  );
  $data['domain']['table']['join'] = array(
    'fbss_domain' => array(
      'left_field' => 'domain_id',
      'field' => 'domain_id',
      'type' => 'INNER',
    ),
  );
  return $data;
}
