<?php

namespace Drupal\services\Plugin\Deriver;

use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

/**
 * Class \Drupal\services\Plugin\Deriver\EntityIndex.
 */
class EntityIndex extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      $this->derivatives[$entity_type_id] = $base_plugin_definition;
      $this->derivatives[$entity_type_id]['title'] = $this->t('@label: Index', ['@label' => $entity_type->getLabel()]);
      $this->derivatives[$entity_type_id]['description'] = $this->t('Index of @entity_type_id objects.', ['@entity_type_id' => $entity_type_id]);
      $this->derivatives[$entity_type_id]['category'] = $this->t('@label', ['@label' => $entity_type->getLabel()]);
      $this->derivatives[$entity_type_id]['path'] = "$entity_type_id";
    }

    return $this->derivatives;
  }

}
