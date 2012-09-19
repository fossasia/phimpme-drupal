<?php

/**
 * @file
 *   Views handler for a "re-post" link.
 */

/**
 * Formats the "re-post" link.
 */
class statuses_views_handler_field_repost extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['repost_text'] = array(
      'default' => t('Share'),
      'translatable' => TRUE,
    );
    return $options;
  }
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['repost_text'] = array(
      '#title' => t('Re-post text'),
      '#type' => 'textfield',
      '#description' => t('The text that will display for the "Re-post this" link.'),
      '#default_value' => $this->options['repost_text'],
    );
  }
  function construct() {
    parent::construct();
    $this->additional_fields['sender'] = 'sender';
  }
  function render($values) {
    $sender_uid = $values->{$this->aliases['sender']};
    // Don't allow sharing your own status.
    if (statuses_user_access('add') && $sender_uid != $GLOBALS['user']->uid) {
      drupal_add_css(drupal_get_path('module', 'statuses') . '/resources/statuses.css');
      $options = array(
        'attributes' => array('class' => array('statuses-repost')),
        'query' => array(
          'sid' => $values->{$this->field_alias},
          'destination' => $_GET['q'],
        ),
      );
      return l($this->options['repost_text'], 'statuses/share', $options);
    }
  }
}
