<?php

namespace Drupal\services\Form;

use Drupal\Core\Entity\EntityConfirmFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Routing\RouteMatchInterface;

/**
 * Class \Drupal\services\Form\ServiceResourceDeleteForm.
 */
class ServiceResourceDeleteForm extends EntityConfirmFormBase {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete "@label" configurations?', [
      '@label' => $this->entity->label(),
    ]);
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    return $this->entity->getEndpoint()->urlInfo('resources');
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->entity->delete();

    drupal_set_message(
      $this->t('Resource "@label" configurations have been deleted!', [
        '@label' => $this->entity->label(),
      ])
    );

    $form_state->setRedirectUrl($this->getCancelUrl());
  }

  /**
   * {@inheritdoc}
   */
  public function getEntityFromRouteMatch(RouteMatchInterface $route_match, $entity_type_id) {
    if ($route_match->getRawParameter('service_endpoint') !== NULL &&
      $route_match->getRawParameter('plugin_id') !== NULL) {
      $service_endpoint = $route_match->getParameter('service_endpoint');

      $entity = $service_endpoint->loadResourceProvider(
        $route_match->getRawParameter('plugin_id')
      );
    }

    if (!isset($entity) || FALSE === $entity) {
      $entity = $this->entityTypeManager->getStorage($entity_type_id)->create([]);
    }

    return $entity;
  }

}
