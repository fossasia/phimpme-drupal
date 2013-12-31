<?php

/**
 * @file
 *   Rules integration.
 */

/**
 * Implements hook_rules_event_info().
 */
function statuses_tags_rules_event_info() {
  return array(
    'statuses_tags_user_was_tagged' => array(
      'label' => t('User was tagged in a status'),
      'module' => 'Statuses',
      'arguments' => array(
        'status' => array(
          'type' => 'statuses',
          'label' => t('The status.'),
        ),
        'account' => array(
          'type' => 'user',
          'label' => t('The user who was tagged in the status.'),
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function statuses_tags_rules_condition_info() {
  return array(
    'statuses_tags_has_tags_condition' => array(
      'label' => t('Status has tags'),
      'arguments' => array(
        'status' => array(
          'type' => 'statuses',
          'label' => t('The status'),
        ),
      ),
      'module' => 'Statuses',
      'eval input' => array('type'),
    ),
    'statuses_tags_has_specific_tag_condition' => array(
      'label' => t('Status has specific tag'),
      'arguments' => array(
        'status' => array(
          'type' => 'statuses',
          'label' => t('The status'),
        ),
      ),
      'module' => 'Statuses',
      'eval input' => array('type', 'tag'),
    ),
  );
}

/**
 * Determines whether a status has tags of a given type.
 */
function statuses_tags_has_tags_condition($status, $settings) {
  return (bool) statuses_tags_has_tags($status->sid, $settings['type']);
}

/**
 * Builds the form for the "status has tags" condition.
 */
function statuses_tags_has_tags_condition_form($settings, &$form) {
  $settings += array('type' => '');
  $form['settings']['type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Tag type'),
    '#default_value' => $settings['type'],
    '#options' => array(
      'user' => t('User @mention'),
      'term' => t('Term #hashtag'),
    ),
  );
}

/**
 * Determines whether a status has a specific tag of a given type.
 */
function statuses_tags_has_specific_tag_condition($status, $settings) {
  return (bool) statuses_tags_status_has_tag_by_name($status->sid, $settings['tag'], $settings['type']);
}

/**
 * Builds the form for the "status has specific tag" condition.
 */
function statuses_tags_has_specific_tag_condition_form($settings, &$form) {
  $settings += array(
    'type' => '',
    'tag' => '',
  );
  $form['settings']['type'] = array(
    '#type' => 'select',
    '#title' => t('Tag type'),
    '#default_value' => $settings['type'],
    '#options' => array(
      'user' => t('User @mention'),
      'term' => t('Term #hashtag'),
    ),
  );
  $form['settings']['tag'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag'),
    '#default_value' => $settings['tag'],
    '#description' => t('Do not include the identifying hash (#) or "at" symbol (@).'),
  );
}
