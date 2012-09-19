<?php

/**
 * @file
 * Internationalization integration based upon the entity API i18n stuff.
 */

/**
 * Rules i18n integration controller.
 */
class RulesI18nStringController extends EntityDefaultI18nStringController {

  /**
   * Overriden to customize i18n object info.
   *
   * @see EntityDefaultI18nStringController::hook_object_info()
   */
  public function hook_object_info() {
    $info = parent::hook_object_info();
    $info['rules_config']['class'] = 'RulesI18nStringObjectWrapper';
    return $info;
  }

  /**
   * Overriden to customize the used menu wildcard.
   */
  protected function menuWildcard() {
    return '%rules_config';
  }

  /**
   * Provide the menu base path. We can provide only one though.
   */
  protected function menuBasePath() {
    return 'admin/config/workflow/rules/reaction';
  }
}

/**
 * Custom I18n String object wrapper, which register custom properties per config.
 */
class RulesI18nStringObjectWrapper extends i18n_string_object_wrapper {

  /**
   * Get translatable properties
   */
  protected function build_properties() {
    $strings = parent::build_properties();
    $properties = array();

    // Also add in the configuration label, as the i18n String UI requires
    // a String to be available always.
    $properties['label'] = array(
      'title' => t('Configuration name'),
      'string' => $this->object->label,
    );

    $this->buildElementProperties($this->object, $properties);

    // Add in translations for all elements.
    foreach ($this->object->elements() as $element) {
      $this->buildElementProperties($element, $properties);
    }
    $strings[$this->get_textgroup()]['rules_config'][$this->object->name] = $properties;
    return $strings;
  }

  /**
   * Adds in translatable properties of the given element.
   */
  protected function buildElementProperties($element, &$properties) {

    foreach ($element->pluginParameterInfo() as $name => $info) {
      // Add in all directly provided input variables.
      if (!empty($info['translatable']) && isset($element->settings[$name])) {
        // If its an array of textual values, translate each value on its own.
        if (is_array($element->settings[$name])) {
          foreach ($element->settings[$name] as $i => $value) {
            $properties[$element->elementId() . ':' . $name . ':' . $i] = array(
              'title' => t('@plugin "@label" (id @id), @parameter, Value @delta', array('@plugin' => drupal_ucfirst($element->plugin()), '@label' => $element->label(), '@id' => $element->elementId(), '@parameter' => $info['label'], '@delta' => $i + 1)),
              'string' => $value,
            );
          }
        }
        else {
          $properties[$element->elementId() . ':' . $name] = array(
            'title' => t('@plugin "@label" (id @id), @parameter', array('@plugin' => drupal_ucfirst($element->plugin()), '@label' => $element->label(), '@id' => $element->elementId(), '@parameter' => $info['label'])),
            'string' => $element->settings[$name],
          );
        }
      }
    }
  }
}
