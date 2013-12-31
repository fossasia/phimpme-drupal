<?php

/**
 * @file
 *   Integrates Rules with the Statuses Domain Access module.
 */

/**
 * Implements hook_rules_condition_info().
 */
function fbss_domain_rules_condition_info() {
  return array(
    'fbss_domain_uses_domain' => array(
      'label' => t('Status was posted in a specific domain'),
      'arguments' => array(
        'status' => array('type' => 'statuses', 'label' => t('The status')),
      ),
      'module' => 'Statuses',
    ),
  );
}

/**
 * Builds the form for the fbss_domain_uses_domain condition.
 */
function fbss_domain_uses_domain_form($settings, &$form) {
  $settings += array('domain_id' => 0);
  $options = array();
  foreach (domain_domains() as $data) {
    // Cannot pass zero in checkboxes.
    ($data['domain_id'] == 0) ? $key = -1 : $key = $data['domain_id'];
    // The domain must be valid.
    if ($data['valid'] || user_access('access inactive domains')) {
      // Checkboxes must be filtered, select lists should not.
      $options[$key] = empty($format) ? check_plain($data['sitename']) : $data['sitename'];
    }
  }
  $form['settings']['domain_id'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Status is associated with domain'),
    '#default_value' => $settings['domain_id'],
    '#options' => $options,
    '#required' => TRUE,
  );
}

/**
 * Checks if the status is associated with one of the chosen domains.
 */
function fbss_domain_uses_domain($status, $settings) {
  $domains = array_filter($settings['domain_id']);
  if (isset($domains[-1])) {
    $domains[0] = 0;
    unset($domains[-1]);
  }
  $domain_id = db_query("SELECT domain_id FROM {fbss_domain} WHERE sid = :sid", array(':sid' => $status->sid))->fetchField();
  return in_array($domain_id, $domains);
}
