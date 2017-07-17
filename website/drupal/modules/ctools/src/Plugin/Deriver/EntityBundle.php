<?php

namespace Drupal\ctools\Plugin\Deriver;

use Drupal\Core\Plugin\Context\ContextDefinition;

/**
 * Deriver that creates a condition for each entity type with bundles.
 */
class EntityBundle extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      if ($entity_type->hasKey('bundle')) {
        $this->derivatives[$entity_type_id] = $base_plugin_definition;
        $this->derivatives[$entity_type_id]['label'] = $this->getEntityBundleLabel($entity_type);
        $this->derivatives[$entity_type_id]['context'] = [
          "$entity_type_id" => new ContextDefinition('entity:' . $entity_type_id),
        ];
      }
    }
    return $this->derivatives;
  }

  /**
   * Provides the bundle label with a fallback when not defined.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type we are looking the bundle label for.
   *
   * @return \Drupal\Core\StringTranslation\TranslatableMarkup
   *   The entity bundle label or a fallback label.
   */
  protected function getEntityBundleLabel($entity_type) {

    if ($label = $entity_type->getBundleLabel()) {
      return $this->t('@label', ['@label' => $label]);
    }

    $fallback = $entity_type->getLabel();
    if ($bundle_entity_type = $entity_type->getBundleEntityType()) {
      // This is a better fallback.
      $fallback =  $this->entityManager->getDefinition($bundle_entity_type)->getLabel();
    }

    return $this->t('@label bundle', ['@label' => $fallback]);

  }

}
