<?php

namespace Drupal\ctools\Plugin\Relationship;


use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\TypedData\FieldItemDataDefinition;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\Plugin\Context\ContextInterface;
use Drupal\Core\TypedData\DataReferenceInterface;
use Drupal\Core\TypedData\ListInterface;
use Drupal\ctools\Annotation\Relationship;
use Drupal\ctools\Plugin\RelationshipBase;

/**
 * @Relationship(
 *   id = "typed_data_relationship",
 *   deriver = "\Drupal\ctools\Plugin\Deriver\TypedDataRelationshipDeriver"
 * )
 */
class TypedDataRelationship extends RelationshipBase {

  /**
   * {@inheritdoc}
   */
  public function getRelationship() {
    $plugin_definition = $this->getPluginDefinition();

    $data_type = $plugin_definition['data_type'];
    $context_definition = new ContextDefinition($data_type, $plugin_definition['label']);
    $context_value = NULL;

    // If the 'base' context has a value, then get the property value to put on
    // the context (otherwise, mapping hasn't occurred yet and we just want to
    // return the context with the right definition and no value).
    if ($this->getContext('base')->hasContextValue()) {
      $data = $this->getData($this->getContext('base'));
      $property = $this->getMainPropertyName($data);
      $context_value = $data->get($property)->getValue();
    }

    $context_definition->setDefaultValue($context_value);
    return new Context($context_definition, $context_value);
  }

  public function getName() {
    return $this->getPluginDefinition()['property_name'];
  }

  protected function getData(ContextInterface $context) {
    /** @var \Drupal\Core\TypedData\ComplexDataInterface $base */
    $base = $context->getContextValue();
    $name = $this->getPluginDefinition()['property_name'];
    $data = $base->get($name);
    // @todo add configuration to get N instead of first.
    if ($data instanceof ListInterface) {
      $data = $data->first();
    }
    if ($data instanceof DataReferenceInterface) {
      $data = $data->getTarget();
    }
    return $data;
  }

  protected function getMainPropertyName(FieldItemInterface $data) {
    return $data->getFieldDefinition()->getFieldStorageDefinition()->getMainPropertyName();
  }

  public function getRelationshipValue() {
    $property = $this->getMainPropertyName();
    /** @var \Drupal\Core\TypedData\ComplexDataInterface $data */
    $data = $this->getRelationship()->getContextData();
    $data->get($property)->getValue();
  }

}
