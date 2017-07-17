<?php

namespace Drupal\ctools\Plugin\Deriver;

use Drupal\Core\Plugin\Context\ContextDefinition;

/**
 * Provides entity view block definitions for each entity type.
 */
class EntityViewDeriver extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      if ($entity_type->hasViewBuilderClass()) {
        $this->derivatives[$entity_type_id] = $base_plugin_definition;
        $this->derivatives[$entity_type_id]['admin_label'] = $this->t('Entity view (@label)', ['@label' => $entity_type->getLabel()]);
        $this->derivatives[$entity_type_id]['context'] = [
          'entity' => new ContextDefinition('entity:' . $entity_type_id),
        ];
      }
    }
    return $this->derivatives;
  }

}
