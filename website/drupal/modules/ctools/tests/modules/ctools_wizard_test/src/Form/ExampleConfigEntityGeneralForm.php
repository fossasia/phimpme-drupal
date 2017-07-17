<?php

namespace Drupal\ctools_wizard_test\Form;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ExampleConfigEntityForm.
 */
class ExampleConfigEntityGeneralForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_wizard_test_config_entity_general_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    // The label and id will be added by the EntityFormWizardBase.

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $config_entity->set('id', $form_state->getValue('id'));
    $config_entity->set('label', $form_state->getValue('label'));
  }

}
