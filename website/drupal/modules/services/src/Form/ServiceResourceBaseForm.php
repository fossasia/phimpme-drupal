<?php

namespace Drupal\services\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Routing\RouteMatchInterface;

/**
 * Class \Drupal\services\Form\ServiceResourceBaseForm.
 */
abstract class ServiceResourceBaseForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $service_endpoint = NULL, $plugin_id = NULL) {
    $form = parent::buildForm($form, $form_state);

    $form['service_plugin_id'] = [
      '#type' => 'value',
      '#value' => $plugin_id,
    ];
    $form['service_endpoint_id'] = [
      '#type' => 'value',
      '#value' => $service_endpoint->id(),
    ];

    return $form;
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
