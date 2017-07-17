<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Routing\Route;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_delete",
 *   methods = {
 *     "DELETE"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityDelete"
 * )
 */
class EntityDelete extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_entity_access', $this->getDerivativeId() . '.delete');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    /* @var $entity \Drupal\Core\Entity\EntityInterface */
    $entity = $this->getContextValue($this->getDerivativeId());
    $entity->delete();
  }

}
