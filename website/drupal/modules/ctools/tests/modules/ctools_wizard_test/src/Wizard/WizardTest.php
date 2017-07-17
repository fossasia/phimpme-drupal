<?php

namespace Drupal\ctools_wizard_test\Wizard;


use Drupal\Core\Form\FormStateInterface;
use Drupal\ctools\Wizard\FormWizardBase;

class WizardTest extends FormWizardBase {

  /**
   * {@inheritdoc}
   */
  public function getWizardLabel() {
    return $this->t('Wizard Information');
  }

  /**
   * {@inheritdoc}
   */
  public function getMachineLabel() {
    return $this->t('Wizard Test Name');
  }

  /**
   * {@inheritdoc}
   */
  public function getOperations($cached_values) {
    return array(
      'one' => [
        'form' => 'Drupal\ctools_wizard_test\Form\OneForm',
        'title' => $this->t('Form One'),
        'values' => ['dynamic' => 'Xylophone'],
        'validate' => ['::stepOneValidate'],
        'submit' => ['::stepOneSubmit'],
      ],
      'two' => [
        'form' => 'Drupal\ctools_wizard_test\Form\TwoForm',
        'title' => $this->t('Form Two'),
        'values' => ['dynamic' => 'Zebra'],
      ],
    );
  }

  /**
   * Validation callback for the first step.
   */
  public function stepOneValidate($form, FormStateInterface $form_state) {
    if ($form_state->getValue('one') == 'wrong') {
      $form_state->setErrorByName('one', $this->t('Cannot set the value to "wrong".'));
    }
  }

  /**
   * Submission callback for the first step.
   */
  public function stepOneSubmit($form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    if ($form_state->getValue('one') == 'magic') {
      $cached_values['one'] = 'Abraham';
    }
    $form_state->setTemporaryValue('wizard', $cached_values);
  }

  /**
   * {@inheritdoc}
   */
  public function getRouteName() {
    return 'ctools.wizard.test.step';
  }

  /**
   * {@inheritdoc}
   */
  public function finish(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    drupal_set_message($this->t('Value One: @one', ['@one' => $cached_values['one']]));
    drupal_set_message($this->t('Value Two: @two', ['@two' => $cached_values['two']]));
    parent::finish($form, $form_state);
  }

}
