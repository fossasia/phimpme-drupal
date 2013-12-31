<?php

/**
 * @file Includes any rules integration provided by the module.
 */


/**
 * Implements hook_rules_file_info().
 */
function rules_test_rules_file_info() {
  return array('rules_test.test');
}

/**
 * Implements hook_rules_condition_info().
 */
function rules_test_rules_condition_info() {
  $items = array();
  $defaults = array(
    'parameter' => array(
      'node' => array('type' => 'node', 'label' => t('Content')),
    ),
    'group' => t('Node'),
  );
  $items['rules_condition_content_is_type'] = array(
    'label' => t('Content has type'),
    'parameter' => array(
      'node' => array('type' => 'node', 'label' => t('Content')),
      'type' => array('type' => 'list<text>', 'label' => t('Content types')),
    ),
    'help' => t('Evaluates to TRUE, if the given content has one of the selected content types.'),
  ) + $defaults;
  $items['rules_condition_content_is_published'] = $defaults + array(
    'label' => t('Content is published'),
  );
  $items['rules_test_condition_true'] = array(
    'label' => t('Test condition returning true'),
    'group' => t('Rules test'),
  );
  $items['rules_test_condition_false'] = array(
    'label' => t('Test condition returning false'),
    'group' => t('Rules test'),
  );
  // A condition for testing passing entities wrapped.
  $items['rules_test_condition_node_wrapped'] = array(
    'label' => t('Content is published'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'wrapped' => TRUE,
      ),
    ),
    'group' => t('Node'),
  );
  return $items;
}

/**
 * Condition implementation returning true.
 */
function rules_test_condition_true($settings, $state, $element) {
  if (!$element instanceof RulesCondition) {
    throw new Exception('Rules element has not been passed to condition.');
  }
  rules_log('condition true called');
  return TRUE;
}

/**
 * Condition implementation returning false.
 */
function rules_test_condition_false() {
  rules_log('condition false called');
  return FALSE;
}

/**
 * Condition implementation receiving the node wrapped.
 */
function rules_test_condition_node_wrapped($wrapper) {
  return $wrapper instanceof EntityMetadataWrapper;
}

/**
 * Implements hook_rules_action_info().
 */
function rules_test_rules_action_info() {
  $items['rules_test_action'] = array(
    'label' => t('Test action'),
    'group' => t('Rules test'),
  );
  return $items + array(
    'rules_node_publish_action' => array(
      'label' => t('Publish content, but do not save'),
      'parameter' => array(
        'node' => array('type' => 'node', 'label' => t('Content')),
      ),
      'callbacks' => array(
        'help' => 'rules_test_custom_help',
      ),
      'base' => 'node_publish_action',
    ),
    'rules_node_publish_action_save' => array(
      'label' => t('Publish content'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'save' => TRUE,
        ),
      ),
      'base' => 'node_publish_action',
    ),
    'rules_node_make_sticky_action' => array(
      'label' => t('Make content sticky'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'save' => TRUE,
        ),
      ),
      'base' => 'node_make_sticky_action',
    ),
    // The same action again requiring a 'page' node.
    'rules_node_page_make_sticky_action' => array(
      'label' => t('Mage page content sticky'),
      'parameter' => array(
        'node' => array(
          'type' => 'node',
          'label' => t('Content'),
          'save' => TRUE,
          'bundles' => array('page'),
        ),
      ),
      'base' => 'node_make_sticky_action',
    ),
    'rules_action_test_reference' => array(
      'label' => t('Change argument passed by reference'),
      'parameter' => array(
         // For references working right, we need a data type with a wrapper.
        'arg' => array('type' => 'test'),
      ),
    ),
    'rules_action_load_node' => array(
      'label' => t('Fetch content by id'),
      'parameter' => array(
        'nid' => array('type' => 'integer', 'label' => t('Content ID')),
        'vid' => array(
          'type' => 'integer',
          'label' => t('Content Revision ID'),
          'description' => t("If you want to fetch a specific revision, specify it's revision id. Else leave it empty to fetch the currently active revision."),
          'optional' => TRUE,
        ),
      ),
      'provides' => array(
        'node_loaded' => array(
          'type' => 'node',
          'label' => t('Loaded content'),
          'label callback' => 'rules_action_load_node_variable_label',
        ),
      ),
      'group' => t('Node'),
      'access callback' => 'rules_node_integration_access',
    ),
    'rules_action_delete_node' => array(
      'label' => t('Delete content'),
      'parameter' => array(
        'node' => array('type' => 'node', 'label' => t('Content')),
      ),
      'group' => t('Node'),
      'access callback' => 'rules_node_integration_access',
    ),
    // An action for testing named parameters.
    'rules_action_node_set_title' => array(
      'label' => t('Content'),
      'parameter' => array(
        'node' => array('type' => 'node', 'label' => t('Content')),
        'title' => array('type' => 'text', 'label' => t('Text')),
      ),
      'named parameter' => TRUE,
      'group' => t('Node'),
      'access callback' => 'rules_node_integration_access',
    ),
    // Tests automatic saving with a non-entity data type.
    'test_type_save' => array(
      'base' => 'rules_test_type_save',
      'label' => t('Save test type'),
      'parameter' => array(
        'node' => array('type' => 'rules_test_type', 'label' => t('Test content'), 'save' => TRUE),
      ),
      'group' => t('Node'),
    ),
  );
}

/**
 * Test action doing nothing exception logging it has been called.
 */
function rules_test_action() {
  rules_log('action called');
}

/**
 * Implements hook_rules_data_info().
 */
function rules_test_rules_data_info() {
  return array(
    'rules_test_type' => array(
      'label' => t('test type'),
      'wrap' => TRUE,
      'wrapper class' => 'RulesTestTypeWrapper',
    ),
  );
}

/**
 * Implements hook_rules_data_info_alter().
 */
function rules_test_rules_data_info_alter(&$data_info) {
  $data_info['log_entry']['creation callback'] = 'rules_action_data_create_array';
}

/**
 * The custom wrapper class for the rules test type.
 *
 * For testing we internally just make use of nodes.
 */
class RulesTestTypeWrapper extends RulesIdentifiableDataWrapper implements RulesDataWrapperSavableInterface {

  protected function extractIdentifier($data) {
    return $data->nid;
  }

  protected function load($id) {
    return node_load($id);
  }

  public function save() {
    node_save($this->value());
  }
}
