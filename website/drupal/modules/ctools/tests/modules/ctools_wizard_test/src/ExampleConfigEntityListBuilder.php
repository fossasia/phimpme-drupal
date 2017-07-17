<?php

namespace Drupal\ctools_wizard_test;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provides a listing of Example config entity entities.
 */
class ExampleConfigEntityListBuilder extends ConfigEntityListBuilder {
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Example config entity');
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
   * @inheritDoc
   */
  public function getOperations(EntityInterface $entity) {
    $operations = parent::getOperations($entity);

    if (!empty($operations['edit'])) {
      /** @var \Drupal\Core\Url $edit */
      $edit = $operations['edit']['url'];
      $edit->setRouteParameters([
        'machine_name' => $entity->id(),
        'step' => 'general',
      ]);
    }

    return $operations;
  }

}
