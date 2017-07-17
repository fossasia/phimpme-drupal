<?php

namespace Drupal\ctools;

/**
 * Provides an interface for mapping context configurations to context objects.
 */
interface ContextMapperInterface {

  /**
   * Gathers the static context values.
   *
   * @param array[] $static_context_configurations
   *   An array of static context configurations.
   *
   * @return \Drupal\Component\Plugin\Context\ContextInterface[]
   *   An array of set context values, keyed by context name.
   */
  public function getContextValues(array $static_context_configurations);

}
