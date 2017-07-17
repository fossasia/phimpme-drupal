<?php

namespace Drupal\ctools\Plugin;

use Drupal\Core\Display\VariantInterface;

/**
 * Provides an interface for variant plugins that use block plugins.
 */
interface BlockVariantInterface extends VariantInterface {

  /**
   * Returns the human-readable list of regions keyed by machine name.
   *
   * @return array
   *   An array of human-readable region names keyed by machine name.
   */
  public function getRegionNames();

  /**
   * Returns the human-readable name of a specific region.
   *
   * @param string $region
   *   The machine name of a region.
   *
   * @return string
   *   The human-readable name of a region.
   */
  public function getRegionName($region);

  /**
   * Adds a block to this display variant.
   *
   * @param array $configuration
   *   An array of block configuration.
   *
   * @return string
   *   The block ID.
   */
  public function addBlock(array $configuration);

  /**
   * Returns the region a specific block is assigned to.
   *
   * @param string $block_id
   *   The block ID.
   *
   * @return string
   *   The machine name of the region this block is assigned to.
   */
  public function getRegionAssignment($block_id);

  /**
   * Returns an array of regions and their block plugins.
   *
   * @return array
   *   The array is first keyed by region machine name, with the values
   *   containing an array keyed by block ID, with block plugin instances as the
   *   values.
   */
  public function getRegionAssignments();

  /**
   * Returns a specific block plugin.
   *
   * @param string $block_id
   *   The block ID.
   *
   * @return \Drupal\Core\Block\BlockPluginInterface
   *   The block plugin.
   */
  public function getBlock($block_id);

  /**
   * Updates the configuration of a specific block plugin.
   *
   * @param string $block_id
   *   The block ID.
   * @param array $configuration
   *   The array of configuration to set.
   *
   * @return $this
   */
  public function updateBlock($block_id, array $configuration);

  /**
   * Removes a specific block from this display variant.
   *
   * @param string $block_id
   *   The block ID.
   *
   * @return $this
   */
  public function removeBlock($block_id);

}
