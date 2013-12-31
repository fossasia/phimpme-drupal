<?php

/**
 * @file
 *   Preprocess functions for the Statuses theme templates.
 */
function template_preprocess_statuses_item(&$vars) {
  $status = $vars['status'];
  $options = $vars['options'];
  $context = statuses_determine_context($status->type);
  $vars['context'] = $context;
  $vars['sid'] = $status->sid;
  $vars['self'] = $status->type == 'user' && $status->sender == $status->recipient;
  $vars['page'] = $options['page'];
  $vars['type'] = $status->type;
  $vars['message'] = _statuses_run_filter($status->message);
  if (variable_get('statuses_nl2br', 0)) {
    $vars['message'] = nl2br($vars['message']);
  }
  $vars += array(
    'recipient' => '',
    'recipient_name' => '',
    'recipient_picture' => '',
    'meta' => '',
    'links' => '',
  );
  $image_vars = array();
  if (module_exists('imagecache_profiles')) {
    $image_vars['user_picture_style'] = variable_get('statuses_imagecache_preset', variable_get('user_picture_imagecache_profiles_default', ''));
  }
  $vars['recipient'] = $context['handler']->load_recipient($status->recipient);
  $vars['recipient_name'] = check_plain($context['handler']->recipient_name($vars['recipient']));
  $vars['recipient_link'] = $context['handler']->recipient_link($vars['recipient']);
  if ($status->type == 'user') {
    $image_vars['account'] = $vars['recipient'];
    $vars['recipient_picture'] = theme('user_picture', $image_vars);
  }
  $vars['sender'] = user_load($status->sender);
  $vars['sender_name'] = check_plain($vars['sender']->name);
  $vars['sender_link'] = theme('username', array('account' => $vars['sender']));
  $image_vars['account'] = $vars['sender'];
  $vars['sender_picture'] = theme('user_picture', $image_vars);
  if ($options['cacheable']) {
    $vars['created'] = format_date($status->created, 'short');
  }
  else {
    $vars['created'] = theme('statuses_time', array('time' => $status->created));
    if ($status->type == 'user') {
      if ($status->sender != $status->recipient) {
        $vars['meta'] = t('to !recipient', array('!recipient' => $vars['recipient_link']));
      }
    }
    elseif (!empty($vars['recipient_name'])) {
      $vars['meta'] = t('on !entity', array('!entity' => $vars['recipient_link']));
    }
    if ($options['links']) {
      $vars['links'] = !empty($status->links) ? theme('links', array('links' => $status->links, 'attributes' => array('class' => array('links', 'inline')))) : '';
    }
  }
  $vars['status_url'] = url('statuses/'. $status->sid, array('absolute' => TRUE));
  
  // Try to determine if the recipient should be obvious based on the context.
  if ($vars['self']) {
    $vars['in_context'] = TRUE;
  }
  elseif ($vars['page']) {
    $vars['in_context'] = FALSE;
  }
  // If the status is between users, we're out of context if we're not on a
  // page where we could write a message to another user. This avoids
  // situations off of user pages where the recipient is the current user, but
  // not all statuses on that page are "to" the current user (for example,
  // on /statuses/all).
  elseif ($status->type == 'user' && arg(0) != 'user' && (arg(0) != 'statuses' || arg(1) != 'conversation')) {
    $vars['in_context'] = FALSE;
  }
  // In the generic situation, we're "in context" if the status recipient and
  // the current context recipient are the same. The context recipient should
  // be the entity we're "looking at" on the current page, so the implication
  // is that if we're looking at an entity we assume that statuses on the page
  // are "to" that entity.
  else {
    $current_context = facebook_status_determine_context();
    $handler = $current_context['handler'];
    $current_recipient_id = $handler->recipient_id($handler->find_recipient());
    $vars['in_context'] = $status->type == $handler->type() && $status->recipient == $current_recipient_id;
  }
}

/**
 * Returns themed HTML for a status.
 * @see statuses_show()
 */
function _statuses_show($status, $options) {
  if (!statuses_user_access('view', $status)) {
    return;
  }
  $options += array(
    'links' => TRUE,
    'page' => FALSE,
    'cacheable' => FALSE,
    'extras' => TRUE,
  );
  if ($options['links']) {
    $status->links = module_invoke_all('link', 'statuses', $status);
    drupal_alter('statuses_link', $status->links, $status);
    uasort($status->links, '_statuses_links_sort');
  }
  $message = trim($status->message);
  if ($options['page']) {
    $title = '';
    $sender = user_load($status->sender);
    if ($status->type == 'user' && $status->sender == $status->recipient) {
      $title = t('@sender: @message', array('@sender' => $sender->name, '@message' => $message));
    }
    else {
      $context = statuses_determine_context($status->type);
      $recipient = $context['handler']->load_recipient($status->recipient);
      $recipient_name = $context['handler']->recipient_name($recipient);
      // "\xC2\xBB" is the unicode escape sequence for the HTML entity &raquo; (a double right angle bracket)
      $title = t("@sender \xC2\xBB @recipient: @message",
        array('@sender' => $sender->name, '@recipient' => $recipient_name, '@message' => $message)
      );
    }
    if (drupal_strlen($title) > 80) {
      // "\xE2\x80\xA6" is the unicode escape sequence for the HTML entity &hellip; (an ellipsis)
      $title = drupal_substr($title, 0, 79) . "\xE2\x80\xA6";
    }
    drupal_set_title($title);
  }
  drupal_add_css(drupal_get_path('module', 'statuses') . '/resources/statuses.css');
  return theme('statuses_item', array('status' => $status, 'options' => $options));
}

/**
 * Helps sort links by weight.
 */
function _statuses_links_sort($a, $b) {
  if (!isset($a['weight'])) {
    $a['weight'] = 0;
  }
  if (!isset($b['weight'])) {
    $b['weight'] = 0;
  }
  return $a['weight'] > $b['weight'] ? 1 : ($a['weight'] < $b['weight'] ? -1 : 0);
}
