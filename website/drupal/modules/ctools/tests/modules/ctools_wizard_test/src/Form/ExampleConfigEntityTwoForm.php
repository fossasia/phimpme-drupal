<?php

namespace Drupal\ctools_wizard_test\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Simple wizard step form.
 */
class ExampleConfigEntityTwoForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_wizard_test_config_entity_two_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $form['two'] = array(
      '#title' => $this->t('Two'),
      '#type' => 'textfield',
      '#default_value' => $config_entity->getTwo() ?: '',
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $config_entity->set('two', $form_state->getValue('two'));
  }

}
