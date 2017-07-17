<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Serializer\SerializerInterface;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Routing\RouteMatchInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_index",
 *   methods = {
 *     "GET"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityIndex"
 * )
 */
class EntityIndex extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * The entity query factory.
   *
   * @var \Drupal\Core\Entity\Query\QueryFactoryInterface
   */
  protected $queryFactory;

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity.query'),
      $container->get('entity_type.manager')
    );
  }

  /**
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param \Drupal\Core\Entity\Query\QueryFactory $query_factory
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, QueryFactory $query_factory, EntityTypeManagerInterface $entity_type_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->queryFactory = $query_factory;
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $entity_type_id = $this->getDerivativeId();
    $start = 0;
    $limit = 30;
    if ($request->query->has('start') && is_numeric($request->query->get('start'))) {
      $start = $request->query->get('start');
    }
    if ($request->query->has('limit') && is_numeric($request->query->get('limit'))) {
      $limit = $request->query->get('limit');
    }
    $result = $this->queryFactory->get($entity_type_id, 'AND')
      ->range($start, $limit)
      ->execute();

    $map_function = function ($id) use ($entity_type_id) {
      return $this->entityTypeManager->getStorage($entity_type_id)
        ->load($id)
        ->label();
    };

    return array_map($map_function, $result);
  }

}
