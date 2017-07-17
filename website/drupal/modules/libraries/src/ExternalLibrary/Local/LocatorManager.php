<?php

namespace Drupal\libraries\ExternalLibrary\Local;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\libraries\Annotation\Locator;

/**
 * Provides a plugin manager for library locator plugins.
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
 */
class LocatorManager extends DefaultPluginManager {

  /**
   * Constructs a locator manager.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct('Plugin/libraries/Locator', $namespaces, $module_handler, LocatorInterface::class, Locator::class);
    $this->alterInfo('libraries_locator_info');
    $this->setCacheBackend($cache_backend, 'libraries_locator_info');
  }

}
