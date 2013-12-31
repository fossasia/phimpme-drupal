<?php

/**
 * Decorates an entity to provide getters/setters.
 *
 * @example
 *
 * $node = new FieldRSIPreventor($node);
 *
 * // This still works,
 * $node->created
 *
 * // Gets the first value of body for LANGUAGE_NONE.
 * $node->getValue('body');
 *
 * // Gets the 2nd value of body in spanish
 * $node->getValue('body', 2, 'esp');
 */ 
class FieldsRSIPreventor {
  private $entity;
  
  function __construct($entity) {
    // Prevent this thing from chaining if people accidentally use it twice.
    if (is_a($entity, 'FieldRSIPreventor')) {
      $entity = $entity->entity;
    }
    $this->entity = $entity;
  }
  
  function getValue($field_name, $delta = 0, $language = LANGUAGE_NONE) {
    if ($item = $this->getItem($field_name, $delta, $language)) {
      return $item['value'];
    }
  }
  
  function getItem($field_name, $delta = 0, $language = LANGUAGE_NONE) {
    if (!isset($this->entity->{$field_name}[$language]) || !isset($this->entity->{$field_name}[$language][$delta])) {
      return FALSE;
    }
    return $this->entity->{$field_name}[$language][$delta];
  }
  
  function getAllItems($field_name, $language = LANGUAGE_NONE) {
    if (isset($this->entity->{$field_name}) && !isset($this->entity->{$field_name}[$language])) {
      return array();
    }
    return $this->entity->{$field_name}[$language];
  }
  
  function getAllValues($field_name, $language = LANGUAGE_NONE) {
    $ret = array();
    foreach ($this->getAllItems($field_name, $language) as $item) {
      $ret[] = $item['value'];
    }
    return $ret;
  }
  
  function __get($key) {
    return $this->entity->{$key};
  }
  
  function __set($key, $value) {
    $this->entity->{$key} = $value;
  }
}

?>