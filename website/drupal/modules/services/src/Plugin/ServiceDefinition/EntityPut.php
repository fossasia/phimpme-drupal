<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\Exception\HttpException;
use Symfony\Component\Serializer\SerializerInterface;
use Symfony\Component\Routing\Route;

/**
 * @ServiceDefinition(
 *   id = "entity_put",
 *   methods = {
 *     "PUT"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityPut"
 * )
 */
class EntityPut extends ServiceDefinitionEntityRequestContentBase {
  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_entity_access', $this->getDerivativeId() . '.update');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    try {
      $updated_entity = parent::processRequest($request, $route_match, $serializer);
      /* @var $entity \Drupal\Core\Entity\EntityInterface */
      $entity = $this->getContextValue($this->getDerivativeId());
      if ($entity instanceof ContentEntityInterface) {
        foreach ($updated_entity as $field_name => $field) {
          $entity->set($field_name, $field->getValue());
        }
      }
      else {
        /* @var $updated_entity \Drupal\Core\Config\Entity\ConfigEntityInterface */
        foreach ($updated_entity->toArray() as $field_name => $field) {
          $entity->set($field_name, $field);
        }
      }
      $entity->save();

      return $entity->toArray();
    }
    catch (\Exception $e) {
      throw new HttpException(422, 'The supplied content body could not be serialized into an entity of the requested type.', $e);
    }
  }

}
