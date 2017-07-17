<?php

namespace Drupal\ctools\Plugin\Deriver;


use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\Core\TypedData\DataDefinitionInterface;
use Drupal\field\FieldConfigInterface;

class TypedDataRelationshipDeriver extends TypedDataPropertyDeriverBase implements ContainerDeriverInterface {

  /**
   * {@inheritdoc}
   */
  protected function generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, DataDefinitionInterface $base_definition, $property_name, DataDefinitionInterface $property_definition) {
    $bundle_info = $base_definition->getConstraint('Bundle');
    // Identify base definitions that appear on bundle-able entities.
    if ($bundle_info && array_filter($bundle_info) && $base_definition->getConstraint('EntityType')) {
      $base_data_type =  'entity:' . $base_definition->getConstraint('EntityType');
    }
    // Otherwise, just use the raw data type identifier.
    else {
      $base_data_type = $data_type_id;
    }
    // If we've not processed this thing before.
    if (!isset($this->derivatives[$base_data_type . ':' . $property_name])) {
      $derivative = $base_plugin_definition;

      $derivative['label'] = $this->t($this->label, [
        '@property' => $property_definition->getLabel(),
        '@base' => $data_type_definition['label'],
      ]);
      $derivative['data_type'] = $property_definition->getFieldStorageDefinition()->getPropertyDefinition($property_definition->getFieldStorageDefinition()->getMainPropertyName())->getDataType();
      $derivative['property_name'] = $property_name;
      $context_definition = new ContextDefinition($base_data_type, $this->typedDataManager->createDataDefinition($base_data_type));
      // Add the constraints of the base definition to the context definition.
      if ($base_definition->getConstraint('Bundle')) {
        $context_definition->addConstraint('Bundle', $base_definition->getConstraint('Bundle'));
      }
      $derivative['context'] = [
        'base' => $context_definition,
      ];
      $derivative['property_name'] = $property_name;

      $this->derivatives[$base_data_type . ':' . $property_name] = $derivative;
    }
    // Individual fields can be on multiple bundles.
    elseif ($property_definition instanceof FieldConfigInterface) {
      // We should only end up in here on entity bundles.
      $derivative = $this->derivatives[$base_data_type . ':' . $property_name];
      // Update label
      /** @var \Drupal\Core\StringTranslation\TranslatableMarkup $label */
      $label = $derivative['label'];
      list(,, $argument_name) = explode(':', $data_type_id);
      $arguments = $label->getArguments();
      $arguments['@'. $argument_name] = $data_type_definition['label'];
      $string_args = $arguments;
      array_shift($string_args);
      $last = array_slice($string_args, -1);
      // The slice doesn't remove, so do that now.
      array_pop($string_args);
      $string = count($string_args) >= 2 ? '@property from '. implode(', ', array_keys($string_args)) .' and '. array_keys($last)[0] : '@property from @base and '. array_keys($last)[0];
      $this->derivatives[$base_data_type . ':' . $property_name]['label'] = $this->t($string, $arguments);
      if ($base_definition->getConstraint('Bundle')) {
        // Add bundle constraints
        $context_definition = $derivative['context']['base'];
        $bundles = $context_definition->getConstraint('Bundle') ?: [];
        $bundles = array_merge($bundles, $base_definition->getConstraint('Bundle'));
        $context_definition->addConstraint('Bundle', $bundles);
      }
    }
  }

}
