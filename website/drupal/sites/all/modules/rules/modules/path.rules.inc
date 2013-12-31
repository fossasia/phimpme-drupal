<?php

/**
 * @file rules integration for the path module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the path module.
 */
function rules_path_file_info() {
  return array('modules/path.eval');
}

/**
 * Implements hook_rules_action_info() on behalf of the path module.
 */
function rules_path_action_info() {
  return array(
    'path_alias' => array(
      'label' => t('Create or delete any URL alias'),
      'group' => t('Path'),
      'parameter' => array(
        'source' => array(
          'type' => 'text',
          'label' => t('Existing system path'),
          'description' => t('Specifies the existing path you wish to alias. For example: node/28, forum/1, taxonomy/term/1+2.') .' '. t('Leave it empty to delete URL aliases pointing to the given path alias.'),
          'optional' => TRUE,
        ),
        'alias' => array(
          'type' => 'text',
          'label' => t('URL alias'),
          'description' => t('Specify an alternative path by which this data can be accessed. For example, "about" for an about page. Use a relative path and do not add a trailing slash.') .' '. t('Leave it empty to delete URL aliases pointing to the given system path.'),
          'optional' => TRUE,
          'cleaning callback' => 'rules_path_clean_replacement_values',
        ),
        'language' => array(
          'type' => 'token',
          'label' => t('Language'),
          'description' => t('If specified, the language for which the URL alias applies.'),
          'options list' => 'entity_metadata_language_list',
          'optional' => TRUE,
          'default value' => LANGUAGE_NONE,
        ),
      ),
      'base' => 'rules_action_path_alias',
      'callbacks' => array('dependencies' => 'rules_path_dependencies'),
      'access callback' => 'rules_path_integration_access',
    ),
    'node_path_alias' => array(
      'label' => t("Create or delete a content's URL alias"),
      'group' => t('Path'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'save' => TRUE,
        ),
        'alias' => array(
          'type' => 'text',
          'label' => t('URL alias'),
          'description' => t('Specify an alternative path by which the content can be accessed. For example, "about" for an about page. Use a relative path and do not add a trailing slash.') .' '. t('Leave it empty to delete the URL alias.'),
          'optional' => TRUE,
          'cleaning callback' => 'rules_path_clean_replacement_values',
        ),
      ),
      'base' => 'rules_action_node_path_alias',
      'callbacks' => array('dependencies' => 'rules_path_dependencies'),
      'access callback' => 'rules_path_integration_access',
    ),
    'taxonomy_term_path_alias' => array(
      'label' => t("Create or delete a taxonomy term's URL alias"),
      'group' => t('Path'),
      'parameter' => array(
        'node' => array(
          'type' => 'taxonomy_term',
          'label' => t('Taxonomy term'),
          'save' => TRUE,
        ),
        'alias' => array(
          'type' => 'text',
          'label' => t('URL alias'),
          'description' => t('Specify an alternative path by which the term can be accessed. For example, "content/drupal" for a Drupal term. Use a relative path and do not add a trailing slash.') .' '. t('Leave it empty to delete the URL alias.'),
          'optional' => TRUE,
          'cleaning callback' => 'rules_path_clean_replacement_values',
        ),
      ),
      'base' => 'rules_action_node_path_alias',
      'callbacks' => array('dependencies' => 'rules_path_dependencies'),
      'access callback' => 'rules_path_integration_access',
    ),
  );
}

/**
 * Callback to specify the path module as dependency.
 */
function rules_path_dependencies() {
  return array('path');
}

/**
 * Path integration access callback.
 */
function rules_path_integration_access($type, $name) {
  if ($type == 'action' && $name == 'path_alias') {
    return user_access('administer url aliases');
  }
  return user_access('create url aliases');
}

/**
 * Implements hook_rules_condition_info() on behalf of the path module.
 */
function rules_path_condition_info() {
  return array(
    'path_has_alias' => array(
      'label' => t('Path has URL alias'),
      'group' => t('Path'),
      'parameter' => array(
        'source' => array(
          'type' => 'text',
          'label' => t('Existing system path'),
          'description' => t('Specifies the existing path you wish to check for. For example: node/28, forum/1, taxonomy/term/1+2.'),
          'optional' => TRUE,
        ),
        'language' => array(
          'type' => 'token',
          'label' => t('Language'),
          'description' => t('If specified, the language for which the URL alias applies.'),
          'options list' => 'entity_metadata_language_list',
          'optional' => TRUE,
          'default value' => LANGUAGE_NONE,
        ),
      ),
      'base' => 'rules_condition_path_has_alias',
      'callbacks' => array('dependencies' => 'rules_path_dependencies'),
      'access callback' => 'rules_path_integration_access',
    ),
    'path_alias_exists' => array(
      'label' => t('URL alias exists'),
      'group' => t('Path'),
      'parameter' => array(
        'alias' => array(
          'type' => 'text',
          'label' => t('URL alias'),
          'description' => t('Specify the URL alias to check for. For example, "about" for an about page.'),
          'optional' => TRUE,
          'cleaning callback' => 'rules_path_clean_replacement_values',
        ),
        'language' => array(
          'type' => 'token',
          'label' => t('Language'),
          'description' => t('If specified, the language for which the URL alias applies.'),
          'options list' => 'entity_metadata_language_list',
          'optional' => TRUE,
          'default value' => LANGUAGE_NONE,
        ),
      ),
      'base' => 'rules_condition_path_alias_exists',
      'callbacks' => array('dependencies' => 'rules_path_dependencies'),
      'access callback' => 'rules_path_integration_access',
    ),
  );
}

/**
 * @}
 */