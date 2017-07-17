<?php

namespace Drupal\ctools\Plugin\Deriver;


use Drupal\Core\TypedData\DataDefinitionInterface;

class TypedDataLanguageRelationshipDeriver extends TypedDataRelationshipDeriver {

  /**
   * {@inheritdoc}
   *
   * @todo this results in awful labels like "Language Language from Content"
   * Fix it.
   */
  protected $label = '@property Language from @base';

  /**
   * {@inheritdoc}
   */
  protected function generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, DataDefinitionInterface $base_definition, $property_name, DataDefinitionInterface $property_definition) {
    if (method_exists($property_definition, 'getType') && $property_definition->getType() == 'language') {
      parent::generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, $base_definition, $property_name, $property_definition);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    parent::getDerivativeDefinitions($base_plugin_definition);
    // The data types will all be set to string since language extends string
    // and the parent class finds the related primitive.
    foreach ($this->derivatives as $plugin_id => $derivative) {
      $this->derivatives[$plugin_id]['data_type'] = 'language';
    }
    return $this->derivatives;
  }

}
