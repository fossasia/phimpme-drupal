<?php

namespace Drupal\ctools\Plugin\Relationship;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;

/**
 * @Relationship(
 *   id = "typed_data_entity_relationship",
 *   deriver = "\Drupal\ctools\Plugin\Deriver\TypedDataEntityRelationshipDeriver"
 * )
 */
class TypedDataEntityRelationship extends TypedDataRelationship {

  /**
   * {@inheritdoc}
   */
  public function getRelationship() {
    $plugin_definition = $this->getPluginDefinition();

    $entity_type = $this->getData($this->getContext('base'))->getDataDefinition()->getSetting('target_type');
    $context_definition = new ContextDefinition("entity:$entity_type", $plugin_definition['label']);
    $context_value = NULL;

    // If the 'base' context has a value, then get the property value to put on
    // the context (otherwise, mapping hasn't occurred yet and we just want to
    // return the context with the right definition and no value).
    if ($this->getContext('base')->hasContextValue()) {
      $context_value = $this->getData($this->getContext('base'))->entity;
    }

    $context_definition->setDefaultValue($context_value);
    return new Context($context_definition, $context_value);
  }

}
