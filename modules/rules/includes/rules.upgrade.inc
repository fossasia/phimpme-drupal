<?php

/**
 * @file
 * Contains code for upgrading rule configurations from 6.x-1.x to 7.x-2.x.
 */

/**
 * Form builder for the upgrade page.
 */
function rules_upgrade_form($form, &$form_state) {

  if (!empty($form_state['export'])) {
    foreach ($form_state['export'] as $key => $export) {
      // Rules have been already converted and exported, so show the export.
      $form['export'][$key] = array(
        '#type' => 'textarea',
        '#title' => t('Export %name', array('%name' => $key)),
        '#description' => t('For importing copy the content of the text area and paste it into the import page of the Rules admin UI. In case the export does not pass the integrity check during import, try using the save to database method instead and manually fix your configuration after conversion.'),
        '#rows' => 10,
        '#default_value' => $export,
      );
    }
    return $form;
  }

  $form['help'] = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#markup' => t('This form allows you to convert rules or rule sets from Rules 1.x to Rules 2.x.') . ' ' .
      t('In order to convert a rule or rule set make sure you have all dependend modules installed and upgraded, i.e. modules which provide Rules integration that has been used in your rules or rule sets. In addition those modules may need to implement some Rules specific update hooks for the conversion to properly work.') . ' ' .
      t('After conversion, the old rules and rule sets will stay in the database until you manually delete them. That way you can make sure the conversion has gone right before you delete the old rules and rule sets.')
  );

  $option_rules = $option_sets = array();
  if (!db_table_exists('rules_rules')) {
    drupal_set_message('There are no Rules 1.x rules or rule sets left to convert.', 'error');
  }
  else {
    foreach (_rules_upgrade_fetch_all_rules() as $name => $rule) {
      if (!empty($rule['#set']) && strpos($rule['#set'], 'event_') === 0) {
        $option_rules[$name] = $name . ': ' . $rule['#label'];
      }
    }
    $query = db_select('rules_sets', 'r')->fields('r');
    foreach ($query->execute() as $row) {
      $set = unserialize($row->data);
      $option_sets[$row->name] = $row->name . ': ' . $set['label'];
    }

    $form['clear'] = array(
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Once you have successfully converted your configuration, you can clean up your database and <a href="!url">delete</a> all Rules 1.x configurations.', array('!url' => url('admin/config/workflow/rules/upgrade/clear')))
    );
  }

  $form['rules'] = array(
    '#type' => 'select',
    '#title' => t('Rules'),
    '#options' => $option_rules,
    '#multiple' => TRUE,
  );

  $form['sets'] = array(
    '#type' => 'select',
    '#title' => t('Rule sets'),
    '#options' => $option_sets,
    '#multiple' => TRUE,
  );
  $form['method'] = array(
    '#type' => 'radios',
    '#title' => t('Method'),
    '#options' => array(
       'export' => t('Convert configuration and export it.'),
       'save' => t('Convert configuration and save it.'),
    ),
    '#default_value' => 'export',
  );

  $form['actions']['convert'] = array(
    '#type' => 'submit',
    '#value' => t('Convert'),
    '#disabled' => !db_table_exists('rules_rules')
  );
  return $form;
}

/**
 * Submit handler for the form.
 */
function rules_upgrade_form_submit($form, &$form_state) {
  // Load all rules includes and install files so modules may put there upgrade
  // information in both locations.
  module_load_all_includes('rules.inc');
  module_load_all_includes('install');

  $configs = array();

  try {
    foreach ($form_state['values']['rules'] as $name) {
      drupal_set_message(t('Converting %plugin %name...', array('%plugin' => t('rule'), '%name' => $name)));
      $configs[$name] = rules_upgrade_convert_rule($name, _rules_upgrade_fetch_item($name, 'rules_rules'));
    }
    foreach ($form_state['values']['sets'] as $name) {
      drupal_set_message(t('Converting %plugin %name...', array('%plugin' => t('rule set'), '%name' => $name)));
      $configs[$name] = rules_upgrade_convert_rule_set($name, _rules_upgrade_fetch_item($name, 'rules_sets'));
    }
    drupal_set_message(t('Completed.'));

    if ($form_state['values']['method'] == 'save') {
      foreach ($configs as $config) {
        $config->save();
      }
      drupal_set_message(t('Converted configurations have been saved to the database and will appear in the Rules administration interface.'));
    }
    elseif ($form_state['values']['method'] == 'export') {
      $export = array();
      foreach ($configs as $name => $config) {
        $export[$name] = $config->export();
      }
      $form_state['export'] = $export;
      $form_state['rebuild'] = TRUE;
    }
  }
  catch (RulesException $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}

/**
 * Confirm form for deleting data.
 */
function rules_upgrade_confirm_clear_form($form, $form_state) {
  $confirm_question = t('Are you sure you want to drop the Rules 1.x tables from the database?');
  $confirm_question_long = t('Are you sure you want to drop the Rules 1.x tables from the database? All Rules 1.x configurations will be deleted regardless whether they have been already converted.') . ' ' . t('This action cannot be undone.');
  return confirm_form($form, $confirm_question, 'admin/config/workflow/rules/upgrade', $confirm_question_long, t('Delete data'), t('Cancel'));
}

function rules_upgrade_confirm_clear_form_submit($form, &$form_state) {
  db_drop_table('rules_rules');
  db_drop_table('rules_sets');
  db_drop_table('rules_scheduler_d6');
  drupal_set_message(t('Rules 1.x configurations have been deleted.'));
  $form_state['redirect'] = 'admin';
}

/**
 * Fetches a single item (rule | rule set).
 */
function _rules_upgrade_fetch_item($name, $table) {
  $query = db_select($table, 'r')->fields('r')->condition('name', $name);
  $row = $query->execute()->fetchAssoc();
  return unserialize($row['data']);
}

/**
 * Fetches all rules.
 */
function _rules_upgrade_fetch_all_rules() {
  $static = drupal_static(__FUNCTION__);

  if (!isset($static)) {
    $query = db_select('rules_rules', 'r')->fields('r');
    $static['rules'] = array();
    foreach ($query->execute() as $row) {
      $static['rules'][$row->name] = unserialize($row->data);
    }
  }
  return $static['rules'];
}

/**
 * Converts a single reaction rule.
 */
function rules_upgrade_convert_rule($name, $cfg_old) {
  $config = rules_upgrade_plugin_factory($cfg_old);
  $config->name = $name;

  if ($config instanceof RulesReactionRule) {
    rules_upgrade_convert_element($cfg_old, $config);
  }
  return $config;
}

/**
 * Converts a single rule set, including all of its rules.
 */
function rules_upgrade_convert_rule_set($name, $cfg_old) {
  $config = rules_plugin_factory('rule set');
  $config->name = $name;
  foreach (array('label', 'weight') as $key) {
    if (isset($element[$key])) {
      $target->$key = $element[$key];
    }
  }
  if (isset($cfg_old['arguments'])) {
    $vars = &$config->componentVariables();
    foreach ($cfg_old['arguments'] as $var_name => $info) {
      // Map data types as required.
      if ($info['type'] == 'string') {
        $info['type'] = 'text';
      }
      $vars[$var_name] = $info;
    }
  }

  // Add in all rules of the set.
  foreach(_rules_upgrade_fetch_all_rules() as $rule_name => $rule) {
    if ($rule['#set'] == $name) {
      drupal_set_message(' >> ' . t('Converting %plugin %name...', array('%plugin' => t('rule'), '%name' => $rule_name . ': ' . $rule['#label'])));
      $new_rule = rules_upgrade_plugin_factory($rule);
      rules_upgrade_convert_element($rule, $new_rule);
      $new_rule->setParent($config);
    }
  }
  return $config;
}

/**
 * Convert a single element.
 *
 * @param $element
 *   The element to convert.
 * @param $target
 *   The converted element to write to.
 */
function rules_upgrade_convert_element(array $element, RulesPlugin $target) {
  foreach (array('active', 'label', 'weight') as $key) {
    if (isset($element['#' . $key])) {
      $target->$key = $element['#' . $key];
    }
  }
  // Go through the parameters and take over its configuration if possible.
  foreach ($target->pluginParameterInfo() as $name => $info) {
    rules_upgrade_element_parameter_settings($element, $target, $name);
  }
  // @todo: Care about php input evaluator for non-text parameters.

  // Take care of variable names and labels.
  foreach ($target->pluginProvidesVariables() as $name => $info) {
    rules_upgrade_element_variable_settings($element, $target, $name);
  }

  if ($target instanceof RulesConditionInterface && !empty($element['#negate'])) {
    $target->negate(TRUE);
  }
  if ($target instanceof RulesReactionRule) {
    // Cut of the 'event_' prefix.
    $target->event(substr($element['#set'], 6));
  }
  if ($element['#type'] == 'rule') {
    if (!empty($element['#conditions'])) {
      foreach (element_children($element['#conditions']) as $key) {
        $child = rules_upgrade_plugin_factory($element['#conditions'][$key]);
        rules_upgrade_convert_element($element['#conditions'][$key], $child);
        $target->condition($child);
      }
    }
    if (!empty($element['#actions'])) {
      foreach (element_children($element['#actions']) as $key) {
        $child = rules_upgrade_plugin_factory($element['#actions'][$key]);
        rules_upgrade_convert_element($element['#actions'][$key], $child);
        $target->action($child);
      }
    }
  }

  // Invoke action/condition specific hooks and a general one.
  if (($element['#type'] == 'action' || $element['#type'] == 'condition')) {
    if (function_exists($function = $element['#name'] .'_upgrade')) {
      $element_name = $function($element, $target);
    }
    elseif (isset($element['#info']['base']) && function_exists($function = $element['#info']['base'] .'_upgrade')) {
      $element_name = $function($element, $target);
    }
  }

  drupal_alter('rules_element_upgrade', $element, $target);
  // Recurse down, if necessary.
  foreach (element_children($element) as $key) {
    $child = rules_upgrade_plugin_factory($element[$key]);
    rules_upgrade_convert_element($element[$key], $child);
    $child->setParent($target);
  }
  if ($target instanceof RulesContainerPlugin) {
    $target->sortChildren();
  }
}

/**
 * Creates the right element.
 */
function rules_upgrade_plugin_factory($element) {
  if ($element['#type'] == 'rule' && !empty($element['#set']) && strpos($element['#set'], 'event_') === 0) {
    return rules_plugin_factory('reaction rule');
  }

  switch ($element['#type']) {
    case 'OR':
      return rules_plugin_factory('or');
    case 'AND':
      return rules_plugin_factory('and');
    default:
      return rules_plugin_factory($element['#type']);

    case 'action':
    case 'condition':
      if (isset($element['#name'])) {
        // Try to come up with the right action/condition name ourself, then
        // invoke a hook.
        $cache = rules_get_cache();
        $items = $cache[$element['#type'] == 'action' ? 'action_info' : 'condition_info'];

        if (isset($items[$element['#name']])) {
          $element_name = $element['#name'];
        }
        elseif (($name = str_replace('rules_', '', $element['#name'])) && isset($items[$name])) {
          $element_name = $name;
        }
        elseif (($name = str_replace($element['#type'] . '_', '', $element['#name'])) && isset($items[$name])) {
          $element_name = $name;
        }
        elseif (($name = str_replace('rules_' . $element['#type'] . '_', '', $element['#name'])) && isset($items[$name])) {
          $element_name = $name;
        }
        elseif (isset($element['#info']['base']) && isset($items[$element['#info']['base']])) {
          $element_name = $name;
        }

        // Call the upgrade callback if one has been defined.
        if (function_exists($function = $element['#name'] .'_upgrade_map_name') || (isset($element['#info']['base']) && function_exists($function = $element['#info']['base'] .'_upgrade_map_name'))) {
          $element_name = $function($element);
        }
        if (!isset($element_name)) {
          throw new RulesIntegrityException(t("Cannot find @plugin %name. Maybe a required is missing or the module has not implemented the upgrade functionality.", array('@plugin' => $element['#type'], '%name' => $element['#name'])));
        }
        return rules_plugin_factory($element['#type'], $element_name);
      }
      break;
  }
}

/**
 * Converts the settings for a given parameter.
 */
function rules_upgrade_element_parameter_settings($element, $target, $name, $new_name = NULL) {
  if (!isset($new_name)) {
    $new_name = $name;
  }
  if (isset($element['#settings'][$name])) {
    // In case a single token has been used, just convert it to a data
    // selector.
    if (is_string($element['#settings'][$name]) && preg_match("/\[(.*)\]$/", $element['#settings'][$name], $matches)) {
      $target->settings[$new_name . ':select'] = $matches[1];
    }
    else {
      $target->settings[$new_name] = $element['#settings'][$name];
    }
  }
  elseif (isset($element['#settings']['#argument map'][$name])) {
    $target->settings[$new_name . ':select'] = $element['#settings']['#argument map'][$name];
  }
}

/**
 * Converts the settings for a given variable.
 */
function rules_upgrade_element_variable_settings($element, $target, $name, $new_name = NULL) {
  if (!isset($new_name)) {
    $new_name = $name;
  }
  if (isset($element['#settings']['#argument map'][$name])) {
    $target->settings[$new_name . ':var'] = $element['#settings']['#argument map'][$name];
    $target->settings[$new_name . ':label'] = $element['#info']['new variables'][$target->settings[$new_name . ':var']]['label'];
  }
}

/**
 * Upgrade callbacks for upgrading the provided Rules 1.x integration.
 */

// Comment.module integration.
function rules_action_load_comment_upgrade_map_name($element) {
  return 'entity_fetch';
}
function rules_action_load_comment_upgrade($element, $target) {
  $target->settings['type'] = 'comment';
  rules_upgrade_element_parameter_settings($element, $target, 'cid', 'id');
  rules_upgrade_element_variable_settings($element, $target, 'comment_loaded', 'entity_fetched');
}

// Node.module integration.
function rules_condition_content_is_type_upgrade_map_name($element) {
  return 'node_is_of_type';
}
function rules_condition_content_is_published_upgrade_map_name($element) {
  return 'node_is_published';
}
function rules_condition_content_is_sticky_upgrade_map_name($element) {
  return 'node_is_sticky';
}
function rules_condition_content_is_promoted_upgrade_map_name($element) {
  return 'node_is_promoted';
}
function rules_condition_content_is_new_upgrade_map_name($element) {
  return 'entity_is_new';
}
function rules_condition_content_is_new_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'node', 'entity');
}
function rules_action_node_set_author_upgrade_map_name($element) {
  return 'data_set';
}
function rules_action_node_set_author_upgrade($element, $target) {
  $target->settings['data:select'] = $element['#settings']['#argument map']['node'] . ':author';
  $target->settings['value:select'] = $element['#settings']['#argument map']['author'];
}
function rules_action_node_load_author_upgrade_map_name($element) {
  return 'entity_fetch';
}
function rules_action_node_load_author_upgrade($element, $target) {
  $target->settings['type'] = 'user';
  $target->settings['id'] = $element['#settings']['#argument map']['node'] . ':author:uid';
}
function rules_action_set_node_title_upgrade_map_name($element) {
  return 'data_set';
}
function rules_action_set_node_title_upgrade($element, $target) {
  $target->settings['data:select'] = $element['#settings']['#argument map']['node'] . ':title';
  $target->settings['value'] = $element['#settings']['title'];
}
function rules_action_add_node_upgrade_map_name($element) {
  return 'entity_create';
}
function rules_action_add_node_upgrade($element, $target) {
  $target->settings['type'] = 'node';
  rules_upgrade_element_parameter_settings($element, $target, 'title', 'param_title');
  rules_upgrade_element_parameter_settings($element, $target, 'author', 'param_author');
  rules_upgrade_element_parameter_settings($element, $target, 'type', 'param_type');
  rules_upgrade_element_variable_settings($element, $target, 'node_added', 'entity_created');
  if (!empty($element['#settings']['node_access'])) {
    drupal_set_message(t('Warning: The node-access check option for the node creation action is not supported any more.'));
  }
}
function rules_action_load_node_upgrade_map_name($element) {
  return 'entity_fetch';
}
function rules_action_load_node_upgrade($element, $target) {
  $target->settings['type'] = 'node';
  rules_upgrade_element_parameter_settings($element, $target, 'nid', 'id');
  rules_upgrade_element_parameter_settings($element, $target, 'vid', 'revision_id');
  rules_upgrade_element_variable_settings($element, $target, 'node_loaded', 'entity_fetched');
}
function rules_action_delete_node_upgrade_map_name($element) {
  return 'entity_delete';
}
function rules_action_delete_node_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'node', 'entity');
}
function rules_core_node_publish_action_upgrade_map_name($element) {
  return 'node_publish';
}
function rules_core_node_unpublish_action_upgrade_map_name($element) {
  return 'node_unpublish';
}
function rules_core_node_make_sticky_action_upgrade_map_name($element) {
  return 'node_make_sticky_action';
}
function rules_core_node_make_unsticky_action_upgrade_map_name($element) {
  return 'node_make_unsticky_action';
}
function rules_core_node_promote_action_upgrade_map_name($element) {
  return 'node_promote_action';
}
function rules_core_node_unpromote_action_upgrade_map_name($element) {
  return 'node_unpromote_action';
}


// Path.module integration.
function rules_condition_url_has_alias_upgrade_map_name($element) {
  return 'path_has_alias';
}
function rules_condition_url_has_alias_upgrade($element, $target) {
  $target->settings['source'] = $element['#settings']['src'];
  $target->settings['alias'] = $element['#settings']['dst'];
}
function rules_condition_alias_exists_upgrade_map_name($element) {
  return 'path_alias_exists';
}
function rules_condition_alias_exists_upgrade($element, $target) {
  $target->settings['alias'] = $element['#settings']['dst'];
}
function rules_action_path_alias_upgrade($element, $target) {
  $target->settings['source'] = $element['#settings']['src'];
  $target->settings['alias'] = $element['#settings']['dst'];
}
function rules_action_node_path_alias_upgrade($element, $target) {
  $target->settings['alias'] = $element['#settings']['dst'];
}

// PHP.module integration.
function rules_condition_custom_php_upgrade_map_name($element) {
  return 'php_eval';
}
function rules_action_custom_php_upgrade_map_name($element) {
  return 'php_eval';
}

// General Rules integration.
function rules_condition_text_compare_upgrade_map_name($element) {
  // @todo: Support regex.
  return 'data_is';
}
function rules_condition_text_compare_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'text1', 'data');
  rules_upgrade_element_parameter_settings($element, $target, 'text2', 'value');
}
function rules_condition_number_compare_upgrade_map_name($element) {
  return 'data_is';
}
function rules_condition_number_compare_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'number1', 'data');
  rules_upgrade_element_parameter_settings($element, $target, 'number2', 'value');
}
function rules_condition_check_boolean_upgrade_map_name($element) {
  return 'data_is';
}
function rules_condition_check_boolean_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'boolean', 'data');
  $target->settings['value'] = TRUE;
}
function rules_action_invoke_set_upgrade_map_name($element) {
  return 'component_' . $element['#info']['set'];
}
function rules_action_invoke_set_upgrade($element, $target) {
  foreach ($element['#info']['arguments'] as $name => $info) {
    rules_upgrade_element_parameter_settings($element, $target, $name);
  }
}
function rules_action_save_variable_upgrade_map_name($element) {
  return isset($element['#info']['new variables']) ? 'variable_add' : 'entity_save';
}
function rules_action_save_variable_upgrade($element, $target) {
  $type = $element['#info']['arguments']['var_name']['default value'];
  if (isset($element['#info']['new variables'])) {
    $target->settings['type'] = $type;
    rules_upgrade_element_parameter_settings($element, $target, $type, 'value');
    rules_upgrade_element_variable_settings($element, $target, $type, 'variable_added');
  }
  else {
    rules_upgrade_element_parameter_settings($element, $target, $type, 'entity');
  }
}


// System.module integration.
function rules_action_set_breadcrumb_upgrade_map_name($element) {
  return 'breadcumb_set';
}
function rules_action_mail_to_user_upgrade_map_name($element) {
  return 'mail';
}
function rules_action_mail_to_user_upgrade($element, $target) {
  $target->settings['to:select'] = $element['#settings']['#argument map']['user'] . ':mail';
}
function rules_action_drupal_goto_upgrade_map_name($element) {
  return 'redirect';
}
function rules_action_drupal_goto_upgrade($element, $target) {
  $settings = $element['#settings'];
  $target->settings['url'] = $settings['path'];
  $target->settings['url'] .= $settings['query'] ? '?' . $settings['query'] : '';
  $target->settings['url'] .= $settings['fragment'] ? '#' . $settings['fragment'] : '';
  if ($settings['immediate']) {
    drupal_set_message(t("Warning: The 'immediate' option for the page redirect action has been dropped in Rules 2.x."));
  }
}

function rules_action_watchdog_upgrade_map_name($element) {
  // @todo: Support action in Rules 2.x!
  return NULL;
}

// Taxonomy.module integration.
// @todo: Finish.
function rules_action_taxonomy_load_term_upgrade_map_name($element) {
  return 'entity_fetch';
}
function rules_action_taxonomy_add_term_upgrade_map_name($element) {
  return 'entity_create';
}
function rules_action_taxonomy_delete_term_upgrade_map_name($element) {
  return 'entity_delete';
}
function rules_action_taxonomy_term_assign_to_content_upgrade_map_name($element) {
  // @todo : list.
  return NULL;
}
function rules_action_taxonomy_term_remove_from_content_upgrade_map_name($element) {
  // @todo : list.
  return NULL;
}
function rules_action_taxonomy_load_vocab_upgrade_map_name($element) {
  return 'entity_fetch';
}
function rules_action_taxonomy_add_vocab_upgrade_map_name($element) {
  return 'data_set';
}

// User.module integration.
function rules_condition_user_hasrole_upgrade_map_name($element) {
  return 'user_has_role';
}
function rules_condition_user_hasrole_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'user', 'account');
}
function rules_condition_user_comparison_upgrade_map_name($element) {
  return 'data_is';
}
function rules_condition_user_comparison_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'user1', 'data');
  rules_upgrade_element_parameter_settings($element, $target, 'user2', 'value');
}
function rules_action_user_addrole_upgrade_map_name($element) {
  return 'user_add_role';
}
function rules_action_user_addrole_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'user', 'account');
}
function rules_action_user_removerole_upgrade_map_name($element) {
  return 'user_remove_role';
}
function rules_action_user_removerole_upgrade($element, $target) {
  rules_upgrade_element_parameter_settings($element, $target, 'user', 'account');
}
function rules_action_load_user_upgrade_map_name($element) {
  if (!empty($element['#settings']['username'])) {
    drupal_set_message(t('Warning: Directly upgrading the load user by name action is not supported.'));
  }
  return 'entity_fetch';
}
function rules_action_load_user_upgrade($element, $target) {
  $target->settings['type'] = 'user';
  rules_upgrade_element_parameter_settings($element, $target, 'userid', 'id');
  rules_upgrade_element_variable_settings($element, $target, 'user_loaded', 'entity_fetched');
}
function rules_action_user_create_upgrade_map_name($element) {
  return 'entity_create';
}
function rules_action_user_create_upgrade($element, $target) {
  $target->settings['type'] = 'user';
  rules_upgrade_element_parameter_settings($element, $target, 'username', 'param_name');
  rules_upgrade_element_parameter_settings($element, $target, 'email', 'param_mail');
  rules_upgrade_element_variable_settings($element, $target, 'user_added', 'entity_created');

}
function rules_core_user_block_user_action_upgrade_map_name($element) {
  return 'user_block';
}
function rules_core_user_block_user_action_upgrade($element, $target) {
  $target->settings['account:select'] = $element['#settings']['#argument map']['user'];
}
