<?php

namespace Drupal\ctools_block\Plugin\Deriver;

use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

/**
 * Provides entity field block definitions for every field.
 */
class EntityFieldDeriver extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    $entity_type_labels = $this->entityManager->getEntityTypeLabels();
    foreach ($this->entityManager->getFieldMap() as $entity_type_id => $entity_field_map) {
      foreach ($this->entityManager->getFieldStorageDefinitions($entity_type_id) as $field_storage_definition) {
        $field_name = $field_storage_definition->getName();

        // The blocks are based on fields. However, we are looping through field
        // storages for which no fields may exist. If that is the case, skip
        // this field storage.
        if (!isset($entity_field_map[$field_name])) {
          continue;
        }

        $field_info = $entity_field_map[$field_name];
        $derivative_id = $entity_type_id . ":" . $field_name;

        // Get the admin label for both base and configurable fields.
        if ($field_storage_definition->isBaseField()) {
          $admin_label = $field_storage_definition->getLabel();
        }
        else {
          // We take the field label used on the first bundle.
          $first_bundle = reset($field_info['bundles']);
          $bundle_field_definitions = $this->entityManager->getFieldDefinitions($entity_type_id, $first_bundle);

          // The field storage config may exist, but it's possible that no
          // fields are actually using it. If that's the case, skip to the next
          // field.
          if (empty($bundle_field_definitions[$field_name])) {
            continue;
          }
          $admin_label = $bundle_field_definitions[$field_name]->getLabel();
        }

        // Set plugin definition for derivative.
        $derivative = $base_plugin_definition;
        $derivative['category'] = $this->t('@entity', ['@entity' => $entity_type_labels[$entity_type_id]]);
        $derivative['admin_label'] = $admin_label;
        $derivative['context'] = [
          'entity' => new ContextDefinition('entity:' . $entity_type_id, $entity_type_labels[$entity_type_id], TRUE),
        ];

        $this->derivatives[$derivative_id] = $derivative;

      }
    }
    return $this->derivatives;
  }

}
