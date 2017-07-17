<?php

namespace Drupal\services;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining service endpoint entities.
 */
interface ServiceEndpointInterface extends ConfigEntityInterface {

  /**
   * Returns the endpoint path to the API.
   *
   * @return string
   */
  public function getEndpoint();

  /**
   * Load service resource providers.
   *
   * @return array
   *   An array of \Drupal\services\Entity\ServiceEndpointResource objects.
   */
  public function loadResourceProviders();

  /**
   * Load service resource provider.
   *
   * @param string $plugin_id
   *   Service plugin identifier.
   *
   * @return \Drupal\services\Entity\ServiceEndpointResource
   *   A service resource object; otherwise FALSE.
   */
  public function loadResourceProvider($plugin_id);

}
