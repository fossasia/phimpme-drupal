<?php

namespace Drupal\ctools_wizard_test\Form;


use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * Simple wizard step form.
 */
class ExampleConfigEntityOneForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_wizard_test_config_entity_one_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $form['one'] = [
      '#title' => $this->t('One'),
      '#type' => 'textfield',
      '#default_value' => $config_entity->getOne() ?: '',
    ];

    $form['external'] = [
      '#type' => 'link',
      '#title' => $this->t('Show on dialog'),
      '#url' => new Url('entity.ctools_wizard_test_config_entity.external_form', [
        'machine_name' => $config_entity->id(),
      ]),
      '#attributes' => [
        'class' => 'use-ajax',
        'data-dialog-type' => 'modal',
      ],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $config_entity->set('one', $form_state->getValue('one'));
  }

}
