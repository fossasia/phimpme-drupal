<?php

namespace Drupal\ctools\Plugin;

/**
 * Provides methods for VariantCollectionInterface.
 */
trait VariantCollectionTrait {

  /**
   * The plugin collection that holds the variants.
   *
   * @var \Drupal\ctools\Plugin\VariantPluginCollection
   */
  protected $variantCollection;

  /**
   * @see \Drupal\ctools\Plugin\VariantCollectionInterface::addVariant()
   */
  public function addVariant(array $configuration) {
    $configuration['uuid'] = $this->uuidGenerator()->generate();
    $this->getVariants()->addInstanceId($configuration['uuid'], $configuration);
    return $configuration['uuid'];
  }

  /**
   * @see \Drupal\ctools\Plugin\VariantCollectionInterface::getVariant()
   */
  public function getVariant($variant_id) {
    return $this->getVariants()->get($variant_id);
  }

  /**
   * @see \Drupal\ctools\Plugin\VariantCollectionInterface::removeVariant()
   */
  public function removeVariant($variant_id) {
    $this->getVariants()->removeInstanceId($variant_id);
    return $this;
  }

  /**
   * @see \Drupal\ctools\Plugin\VariantCollectionInterface::getVariants()
   */
  public function getVariants() {
    if (!$this->variantCollection) {
      $this->variantCollection = new VariantPluginCollection(\Drupal::service('plugin.manager.display_variant'), $this->getVariantConfig());
      $this->variantCollection->sort();
    }
    return $this->variantCollection;
  }

  /**
   * Returns the configuration for stored variants.
   *
   * @return array
   *   An array of variant configuration, keyed by the unique variant ID.
   */
  abstract protected function getVariantConfig();

  /**
   * Returns the UUID generator.
   *
   * @return \Drupal\Component\Uuid\UuidInterface
   */
  abstract protected function uuidGenerator();

}
