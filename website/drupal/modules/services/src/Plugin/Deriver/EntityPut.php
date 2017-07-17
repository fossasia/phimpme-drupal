<?php

namespace Drupal\services\Plugin\Deriver;

use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

/**
 * Class \Drupal\services\Plugin\Deriver\EntityPut.
 */
class EntityPut extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      $this->derivatives[$entity_type_id] = $base_plugin_definition;
      $this->derivatives[$entity_type_id]['title'] = $this->t('@label: Update', ['@label' => $entity_type->getLabel()]);
      $this->derivatives[$entity_type_id]['description'] = $this->t('Updates a @entity_type_id object and serializes it as a response to the current request.', ['@entity_type_id' => $entity_type_id]);
      $this->derivatives[$entity_type_id]['category'] = $this->t('@label', ['@label' => $entity_type->getLabel()]);
      $this->derivatives[$entity_type_id]['path'] = "$entity_type_id/{{$entity_type_id}}";
      $this->derivatives[$entity_type_id]['context'] = [
        "$entity_type_id" => new ContextDefinition("entity:$entity_type_id", $this->t('@label', ['@label' => $entity_type->getLabel()])),
      ];
    }

    return $this->derivatives;
  }

}
