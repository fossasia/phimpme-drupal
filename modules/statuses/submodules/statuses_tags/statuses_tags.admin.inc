<?php

/**
 * @file
 *   Administrative settings for Facebook-style Statuses Tags.
 */

/**
 * The administrative settings form.
 */
function statuses_tags_admin($form, $form_state) {
  $form = array();
  if (module_exists('taxonomy')) {
    if (variable_get('statuses_tags_vid', -1) == -1) {
      drupal_set_message(
        t('You have the Taxonomy and Statuses Tags modules enabled, but you have not set them up to work together!') . ' ' .
        t('You must set a vocabulary for #hashtags below in order for the #hashtags feature to work.'),
        'error');
    }
    $options = array('-1' => t('None'));
    $vocabularies = taxonomy_get_vocabularies();
    foreach ($vocabularies as $vocabulary) {
      $options[$vocabulary->vid] = check_plain($vocabulary->name);
    }
    if (count($options) < 2) {
      drupal_set_message(
        t('You must <a href="!vocab">create a vocabulary</a> for use with #hashtags in order to take advantage of that feature.',
          array('!vocab' => url('admin/structure/taxonomy/add/vocabulary'))),
        'error');
    }
    $form['statuses_tags_vid'] = array(
      '#type' => 'select',
      '#title' => t('Vocabulary for #hashtags'),
      '#default_value' => variable_get('statuses_tags_vid', -1),
      '#options' => $options,
      '#weight' => -10,
    );
    $form['statuses_tags_url'] = array(
      '#type' => 'textfield',
      '#title' => t('#Hashtag URL path'),
      '#description' => t('The URL that #hashtags are linked to in statuses.') . ' ' .
        t('Use % for the #hashtag name. If you do not use % in the URL, <code>/%</code> will automatically be added to the end of the URL.') . ' ' .
        t('The default path is "statuses/term" which is the URL the default statuses_tags View uses.') . ' ' .
        t('However, the setting here will override the path set in the statuses_tags View unless the default View is overridden (changed manually).'),
      '#required' => TRUE,
      '#default_value' => variable_get('statuses_tags_url', 'statuses/term'),
      '#weight' => -5,
    );
  }
  else {
    drupal_set_message(
      t('You have the Statuses Tags module enabled.') . ' ' .
      t('However, you will not be able to use #hashtags unless you <a href="!enable">enable the core Taxonomy module</a>.',
        array('!enable' => url('admin/modules'))) .' '.
      t('Return here after enabling the Taxonomy module to configure it to accept #hashtags.')
    );
  }
  return system_settings_form($form);
}

/**
 * The validate callback for the administrative settings form.
 */
function statuses_tags_admin_validate($form, $form_state) {
  if (strpos($form_state['values']['statuses_tags_url'], '%') === 0) {
    form_set_error('statuses_tags_url', t('You cannot start the #Hashtag URL path with the term name.') .' '.
      t('Doing so could allow malicious users to override system-critical paths.')
    );
  }
}
