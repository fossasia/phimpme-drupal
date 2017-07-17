<?php

namespace Drupal\ctools\Plugin;

/**
 * Provides an interface for objects that have variants e.g. Pages.
 */
interface VariantCollectionInterface {

  /**
   * Adds a new variant to the entity.
   *
   * @param array $configuration
   *   An array of configuration for the new variant.
   *
   * @return string
   *   The variant ID.
   */
  public function addVariant(array $configuration);

  /**
   * Retrieves a specific variant.
   *
   * @param string $variant_id
   *   The variant ID.
   *
   * @return \Drupal\Core\Display\VariantInterface
   *   The variant object.
   */
  public function getVariant($variant_id);

  /**
   * Removes a specific variant.
   *
   * @param string $variant_id
   *   The variant ID.
   *
   * @return $this
   */
  public function removeVariant($variant_id);

  /**
   * Returns the variants available for the entity.
   *
   * @return \Drupal\Core\Display\VariantInterface[]
   *   An array of the variants.
   */
  public function getVariants();

}
