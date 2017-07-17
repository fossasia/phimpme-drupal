<?php

namespace Drupal\ctools_wizard_test\Form;


use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Simple wizard step form.
 */
class OneForm extends FormBase {

  /**
   * Returns a unique string identifying the form.
   *
   * @return string
   *   The unique string identifying the form.
   */
  public function getFormId() {
    return 'ctools_wizard_test_one_form';
  }

  /**
   * Form constructor.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   *
   * @return array
   *   The form structure.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    $form['one'] = [
      '#title' => $this->t('One'),
      '#type' => 'textfield',
      '#default_value' => !empty($cached_values['one']) ? $cached_values['one'] : '',
    ];
    $form['dynamic'] = [
      '#title' => $this->t('Dynamic value'),
      '#type' => 'item',
      '#markup' => !empty($cached_values['dynamic']) ? $cached_values['dynamic'] : '',
    ];
    return $form;
  }

  /**
   * Form submission handler.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $keys = array(
      'one',
    );
    $cached_values = $form_state->getTemporaryValue('wizard');
    foreach ($keys as $key) {
      $cached_values[$key] = $form_state->getValue($key);
    }
    $form_state->setTemporaryValue('wizard', $cached_values);

    drupal_set_message($this->t('Dynamic value submitted: @value', ['@value' => $cached_values['dynamic']]));;
  }

}
