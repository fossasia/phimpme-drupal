<?php

namespace Drupal\ctools_wizard_test;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Example config entity entities.
 */
interface ExampleConfigEntityInterface extends ConfigEntityInterface {

  /**
   * Get first piece of information.
   *
   * @return string
   */
  public function getOne();

  /**
   * Get second piece of information;
   *
   * @return string
   */
  public function getTwo();

}
