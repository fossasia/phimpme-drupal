<?php

namespace Drupal\ctools\Plugin\Deriver;


use Drupal\Core\TypedData\DataDefinitionInterface;

class TypedDataEntityRelationshipDeriver extends TypedDataRelationshipDeriver {

  /**
   * {@inheritdoc}
   */
  protected $label = '@property Entity from @base';

  /**
   * {@inheritdoc}
   */
  protected function generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, DataDefinitionInterface $base_definition, $property_name, DataDefinitionInterface $property_definition) {
    if (method_exists($property_definition, 'getType') && $property_definition->getType() == 'entity_reference') {
      parent::generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, $base_definition, $property_name, $property_definition);
    }
  }

}
