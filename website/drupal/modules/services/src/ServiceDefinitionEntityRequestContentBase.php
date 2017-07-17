<?php

namespace Drupal\services;

use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * Class \Drupal\services\ServiceDefinitionEntityRequestContentBase.
 */
class ServiceDefinitionEntityRequestContentBase extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $manager;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static ($configuration, $plugin_id, $plugin_definition, $container->get('entity.manager'));
  }

  /**
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param EntityManagerInterface $manager
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityManagerInterface $manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->manager = $manager;
  }

  /**
   * {@inheritdoc}
   *
   * @return \Drupal\Core\Entity\EntityInterface|array
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    // Unserialize the content of the request if there is any.
    $content = $request->getContent();
    if (!empty($content)) {
      $entity_type_id = $this->getDerivativeId();
      /* @var $entity_type \Drupal\Core\Entity\EntityTypeInterface */
      $entity_type = $this->manager->getDefinition($entity_type_id);

      return $serializer->deserialize($content, $entity_type->getClass(), $request->getContentType(), ['entity_type' => $entity_type_id]);
    }

    return [];
  }

}
