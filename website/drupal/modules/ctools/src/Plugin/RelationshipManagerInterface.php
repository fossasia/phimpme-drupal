<?php

namespace Drupal\ctools\Plugin;
use Drupal\Component\Plugin\Discovery\CachedDiscoveryInterface;
use Drupal\Core\Plugin\Context\ContextAwarePluginManagerInterface;

/**
 * Provides the Relationship plugin manager.
 */
interface RelationshipManagerInterface extends ContextAwarePluginManagerInterface, CachedDiscoveryInterface {}
