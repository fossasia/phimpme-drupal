<?php

namespace Drupal\ctools\Plugin;

use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Plugin\DefaultLazyPluginCollection;

/**
 * Provides a collection of block plugins.
 */
class BlockPluginCollection extends DefaultLazyPluginCollection {

  /**
   * {@inheritdoc}
   *
   * @return \Drupal\Core\Block\BlockPluginInterface
   */
  public function &get($instance_id) {
    return parent::get($instance_id);
  }

  /**
   * Returns all blocks keyed by their region.
   *
   * @return array
   *   An associative array keyed by region, containing an associative array of
   *   block plugins.
   */
  public function getAllByRegion() {
    $region_assignments = [];
    foreach ($this as $block_id => $block) {
      $configuration = $block->getConfiguration();
      $region = isset($configuration['region']) ? $configuration['region'] : NULL;
      $region_assignments[$region][$block_id] = $block;
    }
    foreach ($region_assignments as $region => $region_assignment) {
      // @todo Determine the reason this needs error suppression.
      @uasort($region_assignment, function (BlockPluginInterface $a, BlockPluginInterface $b) {
        $a_config = $a->getConfiguration();
        $a_weight = isset($a_config['weight']) ? $a_config['weight'] : 0;
        $b_config = $b->getConfiguration();
        $b_weight = isset($b_config['weight']) ? $b_config['weight'] : 0;
        if ($a_weight == $b_weight) {
          return strcmp($a->label(), $b->label());
        }
        return $a_weight > $b_weight ? 1 : -1;
      });
      $region_assignments[$region] = $region_assignment;
    }
    return $region_assignments;
  }

}
