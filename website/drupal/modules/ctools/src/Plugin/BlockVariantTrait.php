<?php

namespace Drupal\ctools\Plugin;

/**
 * Provides methods for \Drupal\ctools\Plugin\BlockVariantInterface.
 */
trait BlockVariantTrait {

  /**
   * The block manager.
   *
   * @var \Drupal\Core\Block\BlockManager
   */
  protected $blockManager;

  /**
   * The plugin collection that holds the block plugins.
   *
   * @var \Drupal\ctools\Plugin\BlockPluginCollection
   */
  protected $blockPluginCollection;

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::getRegionNames()
   */
  abstract public function getRegionNames();

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::getBlock()
   */
  public function getBlock($block_id) {
    return $this->getBlockCollection()->get($block_id);
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::addBlock()
   */
  public function addBlock(array $configuration) {
    $configuration['uuid'] = $this->uuidGenerator()->generate();
    $this->getBlockCollection()->addInstanceId($configuration['uuid'], $configuration);
    return $configuration['uuid'];
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::removeBlock()
   */
  public function removeBlock($block_id) {
    $this->getBlockCollection()->removeInstanceId($block_id);
    return $this;
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::updateBlock()
   */
  public function updateBlock($block_id, array $configuration) {
    $existing_configuration = $this->getBlock($block_id)->getConfiguration();
    $this->getBlockCollection()->setInstanceConfiguration($block_id, $configuration + $existing_configuration);
    return $this;
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::getRegionAssignment()
   */
  public function getRegionAssignment($block_id) {
    $configuration = $this->getBlock($block_id)->getConfiguration();
    return isset($configuration['region']) ? $configuration['region'] : NULL;
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::getRegionAssignments()
   */
  public function getRegionAssignments() {
    // Build an array of the region names in the right order.
    $empty = array_fill_keys(array_keys($this->getRegionNames()), []);
    $full = $this->getBlockCollection()->getAllByRegion();
    // Merge it with the actual values to maintain the ordering.
    return array_intersect_key(array_merge($empty, $full), $empty);
  }

  /**
   * @see \Drupal\ctools\Plugin\BlockVariantInterface::getRegionName()
   */
  public function getRegionName($region) {
    $regions = $this->getRegionNames();
    return isset($regions[$region]) ? $regions[$region] : '';
  }

  /**
   * Gets the block plugin manager.
   *
   * @return \Drupal\Core\Block\BlockManager
   *   The block plugin manager.
   */
  protected function getBlockManager() {
    if (!$this->blockManager) {
      $this->blockManager = \Drupal::service('plugin.manager.block');
    }
    return $this->blockManager;
  }

  /**
   * Returns the block plugins used for this display variant.
   *
   * @return \Drupal\Core\Block\BlockPluginInterface[]|\Drupal\ctools\Plugin\BlockPluginCollection
   *   An array or collection of configured block plugins.
   */
  protected function getBlockCollection() {
    if (!$this->blockPluginCollection) {
      $this->blockPluginCollection = new BlockPluginCollection($this->getBlockManager(), $this->getBlockConfig());
    }
    return $this->blockPluginCollection;
  }

  /**
   * Returns the UUID generator.
   *
   * @return \Drupal\Component\Uuid\UuidInterface
   */
  abstract protected function uuidGenerator();

  /**
   * Returns the configuration for stored blocks.
   *
   * @return array
   *   An array of block configuration, keyed by the unique block ID.
   */
  abstract protected function getBlockConfig();

}
