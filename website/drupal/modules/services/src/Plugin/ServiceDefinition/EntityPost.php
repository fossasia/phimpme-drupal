<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Entity\EntityStorageException;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\Exception\HttpException;
use Symfony\Component\Routing\Route;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_post",
 *   methods = {
 *     "POST"
 *   },
 *   translatable = true,
 *   response_code = 201,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityPost"
 * )
 */
class EntityPost extends ServiceDefinitionEntityRequestContentBase {
  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_services_entity_access_create', $this->getDerivativeId());
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $entity = parent::processRequest($request, $route_match, $serializer);
    if ($entity) {
      try {
        $entity->save();
        if ($entity->id()) {
          drupal_set_message($this->t('Entity of type @type was created.', ['@type' => $entity->getEntityType()->id()]));

          return $entity->toArray();
        }
      }
      catch (EntityStorageException $e) {
        throw new HttpException('500', $e->getMessage());
      }
    }
    throw new HttpException('500', 'The entity could not be created.');
  }

}
