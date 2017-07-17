<?php

namespace Drupal\services\Controller;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provides a listing of service endpoint entities.
 */
class ServiceEndpointListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Services endpoint');
    $header['id'] = $this->t('Machine name');

    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $this->getLabel($entity);
    $row['id'] = $entity->id();
    // You probably want a few more properties here...
    return $row + parent::buildRow($entity);
  }

  /**
   * {@inheritdoc}
   */
  public function getOperations(EntityInterface $entity) {
    $operations = parent::getOperations($entity);

    if ($entity->hasLinkTemplate('resources')) {
      $operations['resource'] = [
        'title' => $this->t('Resources'),
        'url' => $entity->urlInfo('resources'),
        'weight' => 10,
      ];
    }

    return $operations;
  }

}
