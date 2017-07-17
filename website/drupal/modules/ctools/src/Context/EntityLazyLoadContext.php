<?php

namespace Drupal\ctools\Context;

use Drupal\Core\Entity\EntityRepositoryInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinitionInterface;

/**
 * @todo.
 */
class EntityLazyLoadContext extends Context {

  /**
   * The entity UUID.
   *
   * @var string
   */
  protected $uuid;

  /**
   * The entity repository.
   *
   * @var \Drupal\Core\Entity\EntityRepositoryInterface
   */
  protected $entityRepository;

  /**
   * Construct an EntityLazyLoadContext object.
   *
   * @param \Drupal\Core\Plugin\Context\ContextDefinitionInterface $context_definition
   *   The context definition.
   * @param \Drupal\Core\Entity\EntityRepositoryInterface $entity_repository
   *   The entity repository.
   * @param string $uuid
   *   The UUID of the entity.
   */
  public function __construct(ContextDefinitionInterface $context_definition, EntityRepositoryInterface $entity_repository, $uuid) {
    parent::__construct($context_definition);
    $this->entityRepository = $entity_repository;
    $this->uuid = $uuid;
  }

  /**
   * {@inheritdoc}
   */
  public function getContextValue() {
    if (!$this->contextData) {
      $entity_type_id = substr($this->contextDefinition->getDataType(), 7);
      $this->setContextValue($this->entityRepository->loadEntityByUuid($entity_type_id, $this->uuid));
    }
    return parent::getContextValue();
  }

  /**
   * {@inheritdoc}
   */
  public function hasContextValue() {
    // Ensure that the entity is loaded before checking if it exists.
    if (!$this->contextData) {
      $this->getContextValue();
    }
    return parent::hasContextValue();
  }

}
