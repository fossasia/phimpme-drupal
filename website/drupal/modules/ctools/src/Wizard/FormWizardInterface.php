<?php

namespace Drupal\ctools\Wizard;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form wizard interface.
 */
interface FormWizardInterface extends FormInterface {

  /**
   * Constant value for wizard load event.
   */
  const LOAD_VALUES = 'wizard.load';

  /**
   * Return an array of parameters required to construct this wizard.
   *
   * @return array
   */
  public static function getParameters();

  /**
   * Initialize wizard values.
   *
   * return mixed.
   */
  public function initValues();

  /**
   * The shared temp store factory collection name.
   *
   * @return string
   */
  public function getTempstoreId();

  /**
   * The active SharedTempStore for this wizard.
   *
   * @return \Drupal\user\SharedTempStore
   */
  public function getTempstore();

  /**
   * The SharedTempStore key for our current wizard values.
   *
   * @return null|string
   */
  public function getMachineName();

  /**
   * Retrieve the current active step of the wizard.
   *
   * This will return the first step of the wizard if no step has been set.
   *
   * @param mixed $cached_values
   *   The values returned by $this->getTempstore()->get($this->getMachineName());
   *
   * @return string
   */
  public function getStep($cached_values);

  /**
   * Retrieve a list of FormInterface classes by their step key in the wizard.
   *
   * @param mixed $cached_values
   *   The values returned by $this->getTempstore()->get($this->getMachineName());   *
   *
   * @return array
   *   An associative array keyed on the step name with an array value with the
   *   following keys:
   *   - title (string): Human-readable title of the step.
   *   - form (string): Fully-qualified class name of the form for this step.
   *   - values (array): Optional array of cached values to override when on
   *     this step.
   *   - validate (array): Optional array of callables to be called when this
   *     step is validated.
   *   - submit (array): Optional array of callables to be called when this
   *     step is submitted.
   */
  public function getOperations($cached_values);

  /**
   * Retrieve the current Operation.
   *
   * @param mixed $cached_values
   *   The values returned by $this->getTempstore()->get($this->getMachineName());
   *
   * @return string
   *   The class name to instantiate.
   */
  public function getOperation($cached_values);

  /**
   * The name of the route to which forward or backwards steps redirect.
   *
   * @return string
   */
  public function getRouteName();

  /**
   * The Route parameters for a 'next' step.
   *
   * If your route requires more than machine_name and step keys, override and
   * extend this method as needed.
   *
   * @param mixed $cached_values
   *   The values returned by $this->getTempstore()->get($this->getMachineName());
   *
   * @return array
   *   An array keyed by:
   *     machine_name
   *     step
   */
  public function getNextParameters($cached_values);

  /**
   * The Route parameters for a 'previous' step.
   *
   * If your route requires more than machine_name and step keys, override and
   * extend this method as needed.
   *
   * @param mixed $cached_values
   *   The values returned by $this->getTempstore()->get($this->getMachineName());
   *
   * @return array
   *   An array keyed by:
   *     machine_name
   *     step
   */
  public function getPreviousParameters($cached_values);

  /**
   * Form validation handler that populates the cached values from tempstore.
   *
   * Temporary values are only available for a single page load so form
   * submission will lose all the values. This was we reload and provide them
   * to the validate and submit process.
   *
   * @param array $form
   *   Drupal form array
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The initial form state before validation or submission of the steps.
   */
  public function populateCachedValues(array &$form, FormStateInterface $form_state);

  /**
   * Form submit handler to step backwards in the wizard.
   *
   * "Next" steps are handled by \Drupal\Core\Form\FormInterface::submitForm().
   *
   * @param array $form
   *   Drupal form array
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current form state of the wizard. This will not contain values from
   *   the current step since the previous button does not actually submit
   *   those values.
   */
  public function previous(array &$form, FormStateInterface $form_state);

  /**
   * Form submit handler for finalizing the wizard values.
   *
   * If you need to generate an entity or save config or raw table data
   * subsequent to your form wizard, this is the responsible method.
   *
   * @param array $form
   *   Drupal form array
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The final form state of the wizard.
   */
  public function finish(array &$form, FormStateInterface $form_state);

  public function ajaxSubmit(array $form, FormStateInterface $form_state);

  public function ajaxPrevious(array $form, FormStateInterface $form_state);

  public function ajaxFinish(array $form, FormStateInterface $form_state);

}
