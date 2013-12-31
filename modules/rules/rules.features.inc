<?php

/**
 * @file
 * Provides Features integration for the Rules module, based upon the features
 * integration provided by the Entity API.
 */


/**
 * Controller handling the features integration.
 */
class RulesFeaturesController extends EntityDefaultFeaturesController {

  /**
   * Defines the result for hook_features_api().
   */
  public function api() {
    $info = parent::api();
    $info['rules_config']['default_file'] = FEATURES_DEFAULTS_CUSTOM;
    $info['rules_config']['default_filename'] = 'rules_defaults';
    return $info;
  }

  /**
   * Generates the result for hook_features_export().
   * Overridden to add in rules specific stuff.
   */
  public function export($data, &$export, $module_name = '') {
    $pipe = parent::export($data, $export, $module_name);
    foreach (entity_load_multiple_by_name($this->type, $data) as $name => $rules_config) {
      // Add in the dependencies.
      $export['dependencies'] += drupal_map_assoc($rules_config->dependencies());
      // Add in plugin / element specific additions.
      $iterator = new RecursiveIteratorIterator($rules_config, RecursiveIteratorIterator::SELF_FIRST);
      foreach ($iterator as $element) {
        if ($element->facesAs('RulesPluginFeaturesIntegrationInterace')) {
          // Directly use __call() so we cann pass $export by reference.
          $element->__call('features_export', array(&$export, &$pipe, $module_name));
        }
      }
    }
    return $pipe;
  }
}

/**
 * Default extension callback used as default for the abstract plugin class.
 * Actions / conditions may override this with their own implementation, which
 * actually does something.
 *
 * @see RulesPluginFeaturesIntegrationInterace
 */
function rules_features_abstract_default_features_export(&$export, &$pipe, $module_name = '', $element) {

}

/**
 * Interface that allows rules plugins or actions/conditions to customize the
 * features export by implementing the interface using the faces extensions
 * mechanism.
 *
 * @see hook_rules_plugin_info()
 * @see hook_rules_action_info()
 */
interface RulesPluginFeaturesIntegrationInterace {

  /**
   * Allows customizing the features export for a given rule element.
   */
  function features_export(&$export, &$pipe, $module_name = '');
}
