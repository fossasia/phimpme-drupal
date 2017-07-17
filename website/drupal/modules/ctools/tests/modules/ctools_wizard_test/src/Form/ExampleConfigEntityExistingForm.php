<?php

namespace Drupal\ctools_wizard_test\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Simple wizard step form.
 */
class ExampleConfigEntityExistingForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_wizard_test_config_entity_existing_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['existing'] = array(
      '#markup' => '<p>This step only shows if the entity is already existing!</p>',
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Do nothing!
  }

}
