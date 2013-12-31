<?php

/**
 * @file
 * Contains rules integration for the system module needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action: Show a drupal message.
 */
function rules_action_drupal_message($message, $status, $repeat) {
  drupal_set_message(filter_xss_admin($message), $status, $repeat);
}

/**
 * Action: Page redirect.
 *
 * @see rules_page_build()
 * @see rules_drupal_goto_alter()
 */
function rules_action_drupal_goto($url, $force = FALSE, $destination = FALSE) {
  // Don't let administrators lock them out from Rules administration pages.
  if (isset($_GET['q']) && strpos($_GET['q'], 'admin/config/workflow/rules') === 0) {
    rules_log('Skipped page redirect on Rules administration page.', array(), RulesLog::WARN);
    return;
  }
  // Do not redirect during batch processing.
  if (($batch = batch_get()) && isset($batch['current_set'])) {
    rules_log('Skipped page redirect during batch processing.');
    return;
  }

  // Keep the current destination parameter if there is one set.
  if ($destination) {
    $url .= strpos($url, '?') === FALSE ? '?' : '&';
    $url .= drupal_http_build_query(drupal_get_destination());
  }
  // If force is enabled, remove any destination parameter.
  if ($force && isset($_GET['destination'])) {
    unset($_GET['destination']);
  }
  // We don't invoke drupal_goto() right now, as this would end the the current
  // page execution unpredictly for modules. So we'll take over drupal_goto()
  // calls from somewhere else via hook_drupal_goto_alter() and make sure
  // a drupal_goto() is invoked before the page is output with
  // rules_page_build().
  $GLOBALS['_rules_action_drupal_goto_do'] = array($url, $force);
}

/**
 * Action: Set breadcrumb.
 */
function rules_action_breadcrumb_set(array $titles, array $paths) {
  $trail = array(l(t('Home'), ''));
  foreach ($titles as $i => $title) {
    // Skip empty titles.
    if ($title = trim($title)) {
      // Output plaintext instead of a link if there is a title
      // without a path.
      $path = trim($paths[$i]);
      if (!empty($paths[$i]) && $paths[$i] != '<none>') {
        $trail[] = l($title, trim($paths[$i]));
      }
      else {
        $trail[] = check_plain($title);
      }
    }
  }
  drupal_set_breadcrumb($trail);
}

/**
 * Action Implementation: Send mail.
 */
function rules_action_mail($to, $subject, $message, $from = NULL, $settings, RulesState $state, RulesPlugin $element) {
  $to = str_replace(array("\r", "\n"), '', $to);
  $from = !empty($from) ? str_replace(array("\r", "\n"), '', $from) : NULL;
  $params = array(
    'subject' => $subject,
    'message' => $message,
  );
  // Set a unique key for this mail.
  $name = isset($element->root()->name) ? $element->root()->name : 'unnamed';
  $key = 'rules_action_mail_' . $name . '_' . $element->elementId();

  $message = drupal_mail('rules', $key, $to, language_default(), $params, $from);
  if ($message['result']) {
    watchdog('rules', 'Successfully sent email to %recipient', array('%recipient' => $to));
  }
}

/**
 * Action: Send mail to all users of a specific role group(s).
 */
function rules_action_mail_to_users_of_role($roles, $subject, $message, $from = NULL, $settings, RulesState $state, RulesPlugin $element) {
  $from = !empty($from) ? str_replace(array("\r", "\n"), '', $from) : NULL;

  // All authenticated users, which is everybody.
  if (in_array(DRUPAL_AUTHENTICATED_RID, $roles)) {
    $result = db_query('SELECT mail FROM {users} WHERE uid > 0');
  }
  else {
    $rids = implode(',', $roles);
    // Avoid sending emails to members of two or more target role groups.
    $result = db_query('SELECT DISTINCT u.mail FROM {users} u INNER JOIN {users_roles} r ON u.uid = r.uid WHERE r.rid IN ('. $rids .')');
  }

  // Now, actually send the mails.
  $params = array(
    'subject' => $subject,
    'message' => $message,
    'action' => $element,
    'state' => $state,
  );
  // Set a unique key for this mail.
  $name = isset($element->root()->name) ? $element->root()->name : 'unnamed';
  $key = 'rules_action_mail_to_users_of_role_' . $name . '_' . $element->elementId();

  $message = array('result' => TRUE);
  foreach ($result as $row) {
    $message = drupal_mail('rules', $key, $row->mail, language_default(), $params, $from);
    if (!$message['result']) {
      break;
    }
  }
  if ($message['result']) {
    $role_names = array_intersect_key(user_roles(TRUE), array_flip($roles));
    watchdog('rules', 'Successfully sent email to the role(s) %roles.', array('%roles' => implode(', ', $role_names)));
  }
}

/**
 * Implements hook_mail().
 *
 * Set's the message subject and body as configured.
 */
function rules_mail($key, &$message, $params) {

  $message['subject'] .= str_replace(array("\r", "\n"), '', $params['subject']);
  $message['body'][] = $params['message'];
}

/**
 * A class implementing a rules input evaluator processing tokens.
 */
class RulesTokenEvaluator extends RulesDataInputEvaluator {

  public function prepare($text, $var_info) {
    $text = is_array($text) ? implode('', $text) : $text;
    // Skip this evaluator if there are no tokens.
    $this->setting = token_scan($text) ? TRUE : NULL;
  }

  /**
   * We replace the tokens on our own as we cannot use token_replace(), because
   * token usually assumes that $data['node'] is a of type node, which doesn't
   * hold in general in our case.
   * So we properly map variable names to variable data types and then run the
   * replacement ourself.
   */
  public function evaluate($text, $options, RulesState $state) {
    $var_info = $state->varInfo();
    $options += array('sanitize' => FALSE);

    $replacements = array();
    $data = array();
    // We also support replacing tokens in a list of textual values.
    $whole_text = is_array($text) ? implode('', $text) : $text;
    foreach (token_scan($whole_text) as $var_name => $tokens) {
      $var_name = str_replace('-', '_', $var_name);
      if (isset($var_info[$var_name]) && ($token_type = _rules_system_token_map_type($var_info[$var_name]['type']))) {
        // We have to key $data with the type token uses for the variable.
        $data = rules_unwrap_data(array($token_type => $state->get($var_name)), array($token_type => $var_info[$var_name]));
        $replacements += token_generate($token_type, $tokens, $data, $options);
      }
      else {
        $replacements += token_generate($var_name, $tokens, array(), $options);
      }
    }

    // Optionally clean the list of replacement values.
    if (!empty($options['callback']) && function_exists($options['callback'])) {
      $function = $options['callback'];
      $function($replacements, $data, $options);
    }

    // Actually apply the replacements.
    $tokens = array_keys($replacements);
    $values = array_values($replacements);
    if (is_array($text)) {
      foreach ($text as $i => $text_item) {
        $text[$i] = str_replace($tokens, $values, $text_item);
      }
      return $text;
    }
    return str_replace($tokens, $values, $text);
  }

  public static function help($var_info) {
    $render = array(
      '#type' => 'fieldset',
      '#title' => t('Replacement patterns'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('Note that token replacements containing chained objects – such as [node:author:uid] – are not listed here, but are still available. The <em>data selection</em> input mode may help you find more complex replacement patterns. See <a href="@url">the online documentation</a> for more information about complex replacement patterns.',
        array('@url' => rules_external_help('chained-tokens'))),
    );
    $token_info = token_info();
    foreach ($var_info as $name => $info) {
      $token_types[$name] = _rules_system_token_map_type($info['type']);
    }

    foreach ($token_types as $name => $token_type) {
      if (isset($token_info['types'][$token_type])) {
        $render[$name] = array(
          '#theme' => 'table',
          '#header' => array(t('Token'), t('Label'), t('Description')),
          '#prefix' => '<h3>' . t('Replacement patterns for %label', array('%label' => $var_info[$name]['label'])) . '</h3>',
        );
        foreach ($token_info['tokens'][$token_type] as $token => $info) {
          $token = '[' . str_replace('_', '-', $name) . ':' . $token . ']';
          $render[$name]['#rows'][$token] = array(check_plain($token), check_plain($info['name']), check_plain($info['description']));
        }
      }
    }
    return $render;
  }
}

/**
 * Looks for a token type mapping. Defaults to passing through the type.
 */
function _rules_system_token_map_type($type) {
  $entity_info = entity_get_info();
  if (isset($entity_info[$type]['token type'])) {
    return $entity_info[$type]['token type'];
  }
  $cache = rules_get_cache();
  if (isset($cache['data_info'][$type]['token type'])) {
    return $cache['data_info'][$type]['token type'];
  }
  return $type;
}

/**
 * @}
 */
