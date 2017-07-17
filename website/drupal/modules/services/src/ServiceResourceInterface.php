<?php

namespace Drupal\services;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Interface \Drupal\services\Entity\ServiceResourceInterface.
 */
interface ServiceResourceInterface extends ConfigEntityInterface {

  /**
   * Get resource allowed formats.
   *
   * @return array
   *   An array of allowed formats.
   */
  public function getFormats();

  /**
   * Get resource allowed authentication.
   *
   * @return array
   *   An array of allowed authentication.
   */
  public function getAuthentication();

  /**
   * Get resource no caching option.
   *
   * @return boolean
   */
  public function getNoCache();

}
