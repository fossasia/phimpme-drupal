<?php

namespace Drupal\ctools\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a Relationship item annotation object.
 *
 * @see \Drupal\ctools\Plugin\RelationshipManager
 * @see plugin_api
 *
 * @Annotation
 */
class Relationship extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The label of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

  /**
   * The returned data type of this relationship
   *
   * @var string
   */
  public $data_type;

  /**
   * The name of the property from which this relationship is derived.
   *
   * @var string
   */
  public $property_name;

  /**
   * The array of contexts requires or optional for this plugin.
   *
   * @var \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  public $context;

}
