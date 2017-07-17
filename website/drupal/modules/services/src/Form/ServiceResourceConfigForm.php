<?php

namespace Drupal\services\Form;

use Drupal\Core\Form\FormStateInterface;
use Drupal\services\ServiceResourceOptionsTrait;

/**
 * Class \Drupal\services\Form\ServiceResourceConfigForm.
 */
class ServiceResourceConfigForm extends ServiceResourceBaseForm {
  use ServiceResourceOptionsTrait;

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);
    $entity = $this->entity;

    $form['formats'] = [
      '#type' => 'checkboxes',
      '#title' => $this->t('Allowed formats'),
      '#description' => $this->t('Select the allowed formats for serializing the
        HTTP response.'),
      '#options' => $this->getFormatOptions(),
      '#default_value' => $entity->getFormats(),
      '#required' => TRUE,
    ];
    $form['authentication'] = [
      '#type' => 'checkboxes',
      '#title' => $this->t('Allowed authentication'),
      '#description' => $this->t('Select any authentication providers that are
        allowed to access the resource. <br/> <strong>NOTE:</strong> If none
        are selected then only public accessible data will be displayed.'),
      '#options' => $this->getAuthOptions(),
      '#default_value' => $entity->getAuthentication(),
    ];
    $form['no_cache'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Disable cache'),
      '#description' => $this->t('Do not cache the response of the resource.'),
      '#default_value' => $entity->getNoCache()
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $status = $this->entity->save();

    if ($status) {
      drupal_set_message('Resource has been saved successfully.');
    }

    $form_state->setRedirectUrl($this->entity->getEndpoint()->urlInfo('resources'));
  }

}
