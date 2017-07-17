<?php

namespace Drupal\ctools\Plugin;

/**
 * Provides an interface for configuring a plugin via wizard steps.
 */
interface PluginWizardInterface {

  /**
   * Retrieve a list of FormInterface classes by their step key in the wizard.
   *
   * @param mixed $cached_values
   *   The cached values used in the wizard. The plugin we're editing will
   *    always be assigned to the 'plugin' key.
   *
   * @return array
   *   An associative array keyed on the step name with an array value with the
   *   following keys:
   *   - title (string): Human-readable title of the step.
   *   - form (string): Fully-qualified class name of the form for this step.
   */
  public function getWizardOperations($cached_values);

}
