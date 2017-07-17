<?php

namespace Drupal\ctools\Plugin;

use Drupal\Core\Plugin\DefaultLazyPluginCollection;

/**
 * Provides a collection of variants plugins.
 */
class VariantPluginCollection extends DefaultLazyPluginCollection {

  /**
   * {@inheritdoc}
   *
   * @return \Drupal\Core\Display\VariantInterface
   */
  public function &get($instance_id) {
    return parent::get($instance_id);
  }

  /**
   * {@inheritdoc}
   */
  public function sort() {
    // @todo Determine the reason this needs error suppression.
    @uasort($this->instanceIDs, [$this, 'sortHelper']);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function sortHelper($aID, $bID) {
    $a_weight = $this->get($aID)->getWeight();
    $b_weight = $this->get($bID)->getWeight();
    if ($a_weight == $b_weight) {
      return strcmp($aID, $bID);
    }

    return ($a_weight < $b_weight) ? -1 : 1;
  }

}
