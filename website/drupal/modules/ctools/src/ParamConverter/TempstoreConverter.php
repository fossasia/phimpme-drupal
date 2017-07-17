<?php

namespace Drupal\ctools\ParamConverter;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\ParamConverter\ParamConverterInterface;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\Routing\Route;

/**
 * Parameter converter for pulling entities out of the tempstore.
 *
 * This is particularly useful when building non-wizard forms (like dialogs)
 * that operate on data in the wizard and getting the route access correct.
 *
 * There are four different ways to use this!
 *
 * In the most basic way, you specify the 'tempstore_id' in the defaults (so
 * that the form/controller has access to it as well) and in the parameter type
 * we simply give 'tempstore'. This assumes the entity is the full value
 * returned from the tempstore.
 *
 * @code
 * example.route:
 *   path: foo/{example}
 *   defaults:
 *     tempstore_id: example.foo
 *   options:
 *     parameters:
 *       example:
 *         type: tempstore
 * @endcode
 *
 * If the value returned from the tempstore is an array, and the entity is
 * one of the keys, then we specify that after 'tempstore:', for example:
 *
 * @code
 * example.route:
 *   path: foo/{example}
 *   defaults:
 *     tempstore_id: example.foo
 *   options:
 *     parameters:
 *       example:
 *         # Get the 'foo' key from the array returned by the tempstore.
 *         type: tempstore:foo
 * @endcode
 *
 * You can also specify the 'tempstore_id' under the parameter rather than in
 * the defaults, for example:
 *
 * @code
 * example.route:
 *   path: foo/{example}
 *   options:
 *     parameters:
 *       example:
 *         type: tempstore:foo
 *         tempstore_id: example.foo
 * @endcode
 *
 * Or, if you have two parameters which are represented by two keys on the same
 * array from the tempstore, put the slug which represents the id for the
 * tempstore in the 2nd key. For example:
 *
 * @code
 * example.route:
 *   path: foo/{example}/{other}
 *   defaults:
 *     tempstore_id: example.foo
 *   options:
 *     parameters:
 *       example:
 *         type: tempstore:foo
 *       other:
 *         type: tempstore:{example}:other
 * @endcode
 */
class TempstoreConverter implements ParamConverterInterface {

  /**
   * The tempstore factory.
   *
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * Constructs a TempstoreConverter.
   *
   * @param \Drupal\user\SharedTempStoreFactory $tempstore
   */
  public function __construct(SharedTempStoreFactory $tempstore, EntityTypeManagerInterface $entity_type_manager) {
    $this->tempstore = $tempstore;
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public function convert($value, $definition, $name, array $defaults) {
    $tempstore_id = !empty($definition['tempstore_id']) ? $definition['tempstore_id'] : $defaults['tempstore_id'];
    $machine_name = $this->convertVariable($value, $defaults);

    list(, $parts) = explode(':', $definition['type'], 2);
    $parts = explode(':', $parts);
    foreach ($parts as $key => $part) {
      $parts[$key] = $this->convertVariable($part, $defaults);
    }
    $cached_values = $this->tempstore->get($tempstore_id)->get($machine_name);
    // Entity type upcasting is most common, so we just assume that here.
    // @todo see if there's a better way to do this.
    if (!$cached_values && $this->entityTypeManager->hasDefinition($name)) {
      $value = $this->entityTypeManager->getStorage($name)->load($machine_name);
      return $value;
    }
    elseif (!$cached_values) {
      return NULL;
    }
    else {
      $value = NestedArray::getValue($cached_values, $parts, $key_exists);
      return $key_exists ? $value : NULL;
    }
  }

  /**
   * A helper function for converting string variable names from the defaults.
   *
   * @param mixed $name
   *   If name is a string in the format of {var} it will parse the defaults
   *   for a 'var' default. If $name isn't a string or isn't a slug, it will
   *   return the raw $name value. If no default is found, it will return NULL
   * @param array $defaults
   *   The route defaults array.
   *
   * @return mixed
   *   The value of a variable in defaults.
   */
  protected function convertVariable($name, $defaults) {
    if (is_string($name) && strpos($name, '{') === 0) {
      $length = strlen($name);
      $name = substr($name, 1, $length -2);
      return isset($defaults[$name]) ? $defaults[$name] : NULL;
    }
    return $name;
  }

  /**
   * {@inheritdoc}
   */
  public function applies($definition, $name, Route $route) {
    if (!empty($definition['type']) && ($definition['type'] == 'tempstore' || strpos($definition['type'], 'tempstore:') === 0)) {
      if (!empty($definition['tempstore_id']) || $route->hasDefault('tempstore_id')) {
        return TRUE;
      }
    }
    return FALSE;
  }

}
