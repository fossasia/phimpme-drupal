<?php
namespace Drupal\ctools\Wizard;

interface WizardFactoryInterface {
  /**
   * Get the wizard form.
   *
   * @param FormWizardInterface $wizard
   *   The form wizard
   * @param array $parameters
   *   The array of default parameters specific to this wizard.
   * @param bool $ajax
   *   Whether or not this wizard is displayed via ajax modals.
   *
   * @return array
   */
  public function getWizardForm(FormWizardInterface $wizard, array $parameters = [], $ajax = FALSE);

  /**
   * @param string $class
   *   A class name implementing FormWizardInterface.
   * @param array $parameters
   *   The array of parameters specific to this wizard.
   *
   * @return \Drupal\ctools\Wizard\FormWizardInterface
   */
  public function createWizard($class, array $parameters);

  /**
   * Get the wizard form state.
   *
   * @param \Drupal\ctools\Wizard\FormWizardInterface $wizard
   *   The form wizard.
   * @param array $parameters
   *   The array of parameters specific to this wizard.
   * @param bool $ajax
   *
   * @return \Drupal\Core\Form\FormState
   */
  public function getFormState(FormWizardInterface $wizard, array $parameters, $ajax = FALSE);

}
