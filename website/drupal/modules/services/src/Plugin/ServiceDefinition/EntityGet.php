<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;
use Symfony\Component\Routing\Route;

/**
 * @ServiceDefinition(
 *   id = "entity_get",
 *   methods = {
 *     "GET"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityGet"
 * )
 */
class EntityGet extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_entity_access', $this->getDerivativeId() . '.view');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    /* @var $entity \Drupal\Core\Entity\EntityInterface */
    $entity = $this->getContextValue($this->getDerivativeId());

    return $entity->toArray();
  }

}
