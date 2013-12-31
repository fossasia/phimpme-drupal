<?php

/**
 * @file
 *   Functions related to the status update form itself.
 */

/**
 * Returns the status update form.
 *
 * @param $context
 *   One of the following:
 *   - A fully qualified context array
 *   - NULL, to have the context automatically determined
 *   - An object representing the owner of the status stream, i.e. the entity
 *     to which status messages will be posted using this form.
 * @param $type
 *   The context stream type. Only applicable if a recipient object is
 *   specified for $context, but it is required in that case.
 * @param $view
 *   The machine name of the view to display with the form. Overrides the one
 *   requested by the context.
 * @param $display
 *   The machine name of the view display. This is not used internally but
 *   provided in case any external applications need it. There is no error
 *   checking on this parameter; it is the caller's responsibility to ensure
 *   that the specified display is valid and belongs to the relevant view.
 * @return
 *   HTML to display the status update form, or FALSE if:
 *   - The current user has no access to both the form and the associated
 *     status messages, OR
 *   - There are no relevant status messages and the current user does not
 *     have access to use the form
 */
function theme_statuses_form_display($variables) {
  $context = $variables['recipient'];
  $type = $variables['type'];
  $view = $variables['view'];
  $display = $variables['display'];
  global $user;
  // $context is NULL, so find everything.
  if (empty($context)) {
    $context = statuses_determine_context();
    $recipient = $context['handler']->find_recipient();
    $type = $context['handler']->type();
  }
  // Neither $context nor $type is NULL, so $context is really $recipient.
  elseif (!empty($type)) {
    $recipient = $context;
    $context = statuses_determine_context($type);
  }
  // $type is NULL and $context is not, so $context is a full context array.
  else {
    $recipient = $context['handler']->find_recipient();
    $type = $context['handler']->type();
  }
  if (empty($view) && isset($context['view'])) {
    $view = $context['view'];
    // Special case, user context has 2 default views
    if ($type == 'user' && $user->uid != $context['handler']->recipient_id($recipient)) {
      $view = variable_get('statuses_user_other_view', 'statuses_stream');
    }
  }
  $selectors = module_invoke_all('statuses_refresh_selectors', $recipient, $type) + explode("\n", $context['selectors']);
  $conversation = (arg(0) == 'statuses' && arg(1) == 'conversation' && $type == 'user');
  $output = '';
  if (statuses_user_access('add', $recipient, $type, $user)) {
    $self = '';
    if ($type == 'user' && $user->uid == $recipient->uid) {
      $self = ' statuses-form-self-update';
    }
    $converse = '';
    if ($conversation) {
      $converse = ' statuses-conversation-form';
    }
    $form = drupal_get_form('statuses_box', $recipient, $type);
    $output .= '<div class="clear-block statuses-update statuses-update-main facebook-status-form-type-' . $type . $self . $converse . '">' .
      drupal_render($form) . '</div>';
  }
  if ($conversation) {
    $args = explode(',', arg(2));
    $count = count($args);
    if (($count === 1 && $args[0] != $user->uid) || ($count === 2 && is_numeric($args[1])) && is_numeric($args[0])) {
      if (count($args) === 1) {
        $args[1] = $user->uid;
      }
      $output .= views_embed_view('statuses_conversation', $display, $args[0], $args[1]);
      $selectors[] = '.view-id-statuses_conversation';
    }
  }
  elseif (!empty($view) && statuses_user_access('view_stream', $recipient, $type, $user)) {
    if ($view == 'activity_log_stream') {
      $al_type = ($type == 'og' ? 'node' : ($type == 'term' ? 'taxonomy_term' : $type));
      $output .= views_embed_view($view, $display, $al_type, $context['handler']->recipient_id($recipient));
    }
    else {
      $output .= views_embed_view($view, $display, $context['handler']->recipient_id($recipient), $type);
    }
    $selectors[] = '.view-id-' . $view;
  }
  if (!empty($selectors)) {
    drupal_alter('statuses_refresh_selectors', $selectors, $recipient, $type);
    drupal_add_js(array('statuses' => array(
        'refreshIDs' => array_unique($selectors),
      )), array('type' => 'setting', 'scope' => JS_DEFAULT));
  }
  return $output;
}

/**
 * The status form.
 *
 * @param $recipient
 *   An object representing the recipient of the status message.
 * @param $type
 *   The stream type.
 * @return
 *   A $form array.
 */
function statuses_box($form, &$form_state, $recipient, $type = 'user') {
  global $user;
  _statuses_use_autogrow();
  if (isset($form_state['statuses'])) {
    $type = $form_state['statuses']['type'];
    $context = statuses_determine_context($type);
    $rid = $form_state['statuses']['recipient'];
    $recipient = $context['handler']->load_recipient($rid);
  }
  else {
    $context = statuses_determine_context($type);
    $rid = $context['handler']->recipient_id($recipient);
  }

  $default = variable_get('statuses_default_text' . ($type == 'user' && $user->uid == $recipient->uid ? '' : '_other'), '');
  $intro = '';
  if (arg(0) == 'statuses' && (arg(1) == 'share' || arg(1) == 'conversation')) {
    // This page is autofocused. We don't want default text there since the user would have to manually remove it.
    $default = '';
  }
  if (isset($_GET['s']) && arg(0) == 'statuses' && arg(1) == 'share') {
    // Other modules might use "s," so only accept it on the share-status page.
    $default = $_GET['s'];
  }
  elseif (isset($_GET['share-status'])) {
    $default = $_GET['share-status'];
  }
  // TODO: Replace with a "true" re-posting system.
  if (isset($_GET['sid']) && is_numeric($_GET['sid'])) {
    $rs = statuses_load($_GET['sid']);
    $sender = user_load($rs->sender);
    if ($sender->uid != $user->uid && $rs->message) {
      $name = '@' . $sender->name;
      // Properly reference tags with word-break characters in them.
      if (preg_match('/.+\b.+/', $sender->name)) {
        $name = '[' . $name . ']';
      }
      $default = str_replace(array('@name', '@message'), array($name, $rs->message), variable_get('statuses_repost', 'Re: @name @message '));
      $intro = '';
    }
  }
  elseif (isset($_GET['rsid']) && is_numeric($_GET['rsid'])) {
    $status = statuses_load($_GET['rsid']);
    if ($status->sid) {
      $sender = user_load($status->sender);
      if ($sender->uid != $user->uid) {
        $name = '@' . $sender->name;
        // Properly reference tags with word-break characters in them.
        if (preg_match('/.+\b.+/', $sender->name)) {
          $name = '[' . $name . ']';
        }
        $default = $name . ' ';
      }
      $intro = t('In response to: !status', array('!status' => statuses_show($status, array('links' => FALSE, 'extras' => FALSE))));
    }
  }

  $path = drupal_get_path('module', 'statuses') . '/resources';
  drupal_add_js($path . '/statuses.js');
  if (variable_get('statuses_enter', 0)) {
    drupal_add_js($path .'/statuses_enter.js');
  }
  drupal_add_js($path . '/statuses_ahah.js', array('scope' => 'footer', 'defer' => TRUE));
  drupal_add_css($path . '/statuses.css');
  // Form elements between ['opendiv'] and ['closediv'] will be refreshed via AHAH on form submission.
  $form['opendiv'] = array(
    '#markup' => '<div id="statuses-replace" class="statuses-form">',
    '#weight' => -50,
  );
  if (!empty($intro)) {
    $form['intro'] = array(
      '#markup' => '<span class="statuses-intro">' . $intro . '</span>',
      '#weight' => -45,
    );
  }
  if (variable_get('statuses_length', 140) > 0) {
    $form['chars'] = array(
      '#markup' => '<span class="statuses-chars">' . t('%chars characters allowed', array('%chars' => variable_get('statuses_length', 140))) . '</span>',
      '#weight' => -41,
    );
  }
  $form['fbss-status'] = array(
    '#type' => 'textarea',
    '#rows' => variable_get('statuses_box_rows', 2),
    '#default_value' => $default,
    '#attributes' => array('class' => array('statuses-text', 'statuses-text-main')),
    '#resizable' => FALSE,
    '#prefix' => '<div class="statuses-textarea-wrapper">',
    '#suffix' => '</div>',
    '#weight' => -40,
  );
  $form['fbss-submit'] = array(
    '#type' => 'submit',
    '#value' => t('Share'),
    '#submit' => array('statuses_box_submit'),
    '#attributes' => array('class' => array('statuses-submit', 'button', 'primary')),
    '#weight' => -25,
  );
  if (variable_get('statuses_ahah', 1)) {
    $form['fbss-submit']['#ajax'] = array(
      'path' => 'statuses/ajax',
      'callback' => 'statuses_save_js',
      'wrapper' => 'statuses-replace',
      'keypress' => TRUE,
    );
  }
  $form['sdefault'] = array(
    '#type' => 'value',
    '#value' => $default,
    '#weight' => -10,
  );
  // Form elements between ['opendiv'] and ['closediv'] will be refreshed via AHAH on form submission.
  $form['closediv'] = array(
    '#markup' => '</div>',
    '#weight' => -1,
  );
  $form['recipient'] = array(
    '#type' => 'value',
    '#value' => $context['handler']->recipient_id($recipient),
    '#weight' => 10,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
    '#weight' => 10,
  );
  if (arg(0) == 'statuses' && arg(1) == 'share' && !empty($_GET['destination']) && $_GET['destination'] != $_GET['q']) {
    $form['back'] = array(
      '#markup' => l(t('Back'), $_GET['destination'], array('attributes' => array('class' => array('statuses-back')))),
      '#weight' => 20,
    );
  }
  return $form;
}

/**
 * Validate function for the status update form.
 */
function statuses_box_validate($form, &$form_state) {
  $maxlen = variable_get('statuses_length', 140);
  $len = drupal_strlen($form_state['values']['fbss-status']);
  if ($len > $maxlen && $maxlen != 0) {
    form_set_error('status',
      t('You may use a maximum of %maxchars characters, but you are using %chars characters.',
        array('%maxchars' => $maxlen, '%chars' => $len)
      )
    );
  }
}

/**
 * Submit function for the status update form.
 */
function statuses_box_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $rid = $form_state['values']['recipient'];
  $context = statuses_determine_context($type);
  $recipient = $context['handler']->load_recipient($rid);
  $new_status = trim($form_state['values']['fbss-status']);
  $default = $form_state['values']['sdefault'];
  $form_state['statuses'] = array();
  // Don't save the status if it wasn't changed from the default.
  if ($new_status != $default) {
    $new_status_obj = statuses_save_status($recipient, $type, $new_status, $GLOBALS['user'], array('form_state' => $form_state));
    if (isset($new_status_obj->sid)) {
      $form_state['statuses']['sid'] = $new_status_obj->sid;
    }
  }
  $form_state['statuses']['type'] = $type;
  $form_state['statuses']['recipient'] = $rid;
  $form_state['rebuild'] = TRUE;
  if (!variable_get('statuses_ahah', 1)) {
    $form_state['redirect'] = array($_GET['q']);
    $form_state['rebuild'] = FALSE;
  }
}

/**
 * Saves statuses via AHAH.
 */
function statuses_save_js($form, $form_state) {
  $new_form = array();
  // Get HTML for the replacement form. Only these elements will be AHAH-refreshed.
  $new_form['opendiv']     = $form['opendiv'];
  if (!empty($form['intro'])) {
    $new_form['intro']       = $form['intro'];
  }
  $new_form['fbss-status'] = $form['fbss-status'];
  $new_form['chars']       = $form['chars'];
  $new_form['fbss-submit'] = $form['fbss-submit'];
  $new_form['sdefault']    = $form['sdefault'];
  $new_form['closediv']    = $form['closediv'];
  $new_form['fbss-status']['#value'] = ''; // Clear the old value. See https://drupal.org/node/1265048
  // Calls hook_statuses_form_ahah_alter(&$new_form, $form).
  drupal_alter('statuses_form_ahah', $new_form, $form);
  return $new_form;
}

/**
 * Executes the AJAX process.
 *
 * This is required because we want all the form-related functions to be in
 * this file, and to do that we have to implement a custom AJAX path instead
 * of using the default (system/ajax).
 */
function statuses_ajax_form_callback() {
  require_once DRUPAL_ROOT . '/includes/form.inc';
  return ajax_form_callback();
}

/**
 * Adds the Autogrow jQuery extension.
 *
 * @return
 *   TRUE if the plugin was added; FALSE if it wasn't found.
 */
function _statuses_use_autogrow() {
  $path = cache_get('fbss:autogrow');
  if (!empty($path)) {
    if (drupal_strlen($path->data) > 3) {
      drupal_add_js($path->data);
      return TRUE;
    }
    return FALSE;
  }
  if (module_exists('libraries')) {
    $path = libraries_get_path('autogrow') . '/jquery.autogrow.js';
    if (file_exists($path)) {
      drupal_add_js($path);
      cache_set('fbss:autogrow', $path);
      return TRUE;
    }
  }
  if (file_exists('sites/all/libraries/jquery.autogrow.js')) {
    drupal_add_js('sites/all/libraries/jquery.autogrow.js');
    cache_set('fbss:autogrow', 'sites/all/libraries/jquery.autogrow.js');
    return TRUE;
  }
  elseif (file_exists('libraries/jquery.autogrow.js')) {
    drupal_add_js('libraries/jquery.autogrow.js');
    cache_set('fbss:autogrow', 'libraries/jquery.autogrow.js');
    return TRUE;
  }
  else {
    $path = drupal_get_path('profile', drupal_get_profile()) . '/libraries/jquery.autogrow.js';
    if (file_exists($path)) {
      drupal_add_js($path);
      cache_set('fbss:autogrow', $path);
      return TRUE;
    }
    else {
      cache_set('fbss:autogrow', 'n/a');
    }
  }
  return FALSE;
}
