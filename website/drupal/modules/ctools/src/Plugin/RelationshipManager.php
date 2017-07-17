<?php

namespace Drupal\ctools\Plugin;

use Drupal\Core\Plugin\Context\ContextAwarePluginManagerTrait;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides the Relationship plugin manager.
 */
class RelationshipManager extends DefaultPluginManager implements RelationshipManagerInterface {

  use ContextAwarePluginManagerTrait;

  /**
   * Constructor for RelationshipManager objects.
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
    parent::__construct('Plugin/Relationship', $namespaces, $module_handler, 'Drupal\ctools\Plugin\RelationshipInterface', 'Drupal\ctools\Annotation\Relationship');

    $this->alterInfo('ctools_relationship_info');
    $this->setCacheBackend($cache_backend, 'ctools_relationship_plugins');
  }

}
