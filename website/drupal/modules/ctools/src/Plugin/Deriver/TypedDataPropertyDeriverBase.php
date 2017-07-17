<?php

namespace Drupal\ctools\Plugin\Deriver;


use Drupal\Component\Plugin\Derivative\DeriverBase;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Plugin\Discovery\ContainerDeriverInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\StringTranslation\TranslationInterface;
use Drupal\Core\TypedData\ComplexDataInterface;
use Drupal\Core\TypedData\DataDefinitionInterface;
use Drupal\Core\TypedData\DataReferenceDefinitionInterface;
use Drupal\Core\TypedData\ListDataDefinitionInterface;
use Drupal\Core\TypedData\TypedDataManagerInterface;
use Drupal\field\Entity\FieldConfig;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class TypedDataPropertyDeriverBase extends DeriverBase implements ContainerDeriverInterface {

  use StringTranslationTrait;

  /**
   * @var \Drupal\Core\TypedData\TypedDataManagerInterface
   */
  protected $typedDataManager;

  /**
   * The label string for use with derivative labels.
   *
   * @var string
   */
  protected $label = '@property from @base';

  /**
   * TypedDataPropertyDeriverBase constructor.
   *
   * @param \Drupal\Core\TypedData\TypedDataManagerInterface $typed_data_manager
   *   The typed data manager.
   * @param \Drupal\Core\StringTranslation\TranslationInterface $string_translation
   *   The string translation service.
   */
  public function __construct(TypedDataManagerInterface $typed_data_manager, TranslationInterface $string_translation) {
    $this->typedDataManager = $typed_data_manager;
    $this->stringTranslation = $string_translation;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, $base_plugin_id) {
    return new static(
      $container->get('typed_data_manager'),
      $container->get('string_translation')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->typedDataManager->getDefinitions() as $data_type_id => $data_type_definition) {
      if (is_subclass_of($data_type_definition['class'], ComplexDataInterface::class, TRUE)) {
        /** @var \Drupal\Core\TypedData\ComplexDataDefinitionInterface $base_definition */
        $base_definition = $this->typedDataManager->createDataDefinition($data_type_id);
        foreach ($base_definition->getPropertyDefinitions() as $property_name => $property_definition) {
          if ($property_definition instanceof BaseFieldDefinition || $property_definition instanceof FieldConfig) {
            $this->generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, $base_definition, $property_name, $property_definition);
          }
        }
      }
    }
    return $this->derivatives;
  }

  /**
   * @param $property_definition
   *
   * @return mixed
   */
  protected function getDataType($property_definition) {
    if ($property_definition instanceof DataReferenceDefinitionInterface) {
      return $property_definition->getTargetDefinition()->getDataType();
    }
    if ($property_definition instanceof ListDataDefinitionInterface) {
      return $property_definition->getItemDefinition()->getDataType();
    }
    return $property_definition->getDataType();
  }

  /**
   * Generates and maintains a derivative definition.
   *
   * This method should directly manipulate $this->derivatives and not return
   * values. This allows implementations control over the derivative names.
   *
   * @param $base_plugin_definition
   *   The base plugin definition.
   * @param string $data_type_id
   *   The plugin id of the data type.
   * @param mixed $data_type_definition
   *   The plugin definition of the data type.
   * @param \Drupal\Core\TypedData\DataDefinitionInterface $base_definition
   *   The data type definition of a complex data object.
   * @param string $property_name
   *   The name of the property
   * @param \Drupal\Core\TypedData\DataDefinitionInterface $property_definition
   *   The property definition.
   *
   */
  abstract protected function generateDerivativeDefinition($base_plugin_definition, $data_type_id, $data_type_definition, DataDefinitionInterface $base_definition, $property_name, DataDefinitionInterface $property_definition);

}
