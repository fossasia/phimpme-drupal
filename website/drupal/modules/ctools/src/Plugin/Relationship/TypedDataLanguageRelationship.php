<?php

namespace Drupal\ctools\Plugin\Relationship;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;

/**
 * @Relationship(
 *   id = "typed_data_language_relationship",
 *   deriver = "\Drupal\ctools\Plugin\Deriver\TypedDataLanguageRelationshipDeriver"
 * )
 */
class TypedDataLanguageRelationship extends TypedDataRelationship {

  /**
   * {@inheritdoc}
   */
  public function getRelationship() {
    $plugin_definition = $this->getPluginDefinition();

    $context_definition = new ContextDefinition("language", $plugin_definition['label']);
    $context_value = NULL;

    // If the 'base' context has a value, then get the property value to put on
    // the context (otherwise, mapping hasn't occurred yet and we just want to
    // return the context with the right definition and no value).
    if ($this->getContext('base')->hasContextValue()) {
      $context_value = $this->getData($this->getContext('base'))->language;
    }

    $context_definition->setDefaultValue($context_value);
    return new Context($context_definition, $context_value);
  }

}
