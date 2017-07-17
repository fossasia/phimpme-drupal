<?php

namespace Drupal\ctools\Form;

use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\ConfirmFormHelper;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Base class for adding a required contexts step to your wizard.
 */
abstract class RequiredContextDelete extends ConfirmFormBase {

  /**
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * @var string
   */
  protected $tempstore_id;

  /**
   * @var string;
   */
  protected $machine_name;

  /**
   * @var int;
   */
  protected $id;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('user.shared_tempstore'));
  }

  /**
   * @param \Drupal\user\SharedTempStoreFactory $tempstore
   */
  function __construct(SharedTempStoreFactory $tempstore) {
    $this->tempstore = $tempstore;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_required_context_delete';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $id = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $this->id = $id;

    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    $form ['#title'] = $this->getQuestion($id, $cached_values);

    $form ['#attributes']['class'][] = 'confirmation';
    $form ['description'] = array('#markup' => $this->getDescription());
    $form [$this->getFormName()] = array('#type' => 'hidden', '#value' => 1);

    // By default, render the form using theme_confirm_form().
    if (!isset($form ['#theme'])) {
      $form ['#theme'] = 'confirm_form';
    }
    $form['actions'] = array('#type' => 'actions');
    $form['actions'] += $this->actions($form, $form_state);
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    $contexts = $this->getContexts($cached_values);
    unset($contexts[$this->id]);
    $cached_values = $this->setContexts($cached_values, $contexts);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $form_state->setRedirect($route_name, $route_parameters);
  }

  /**
   * {@inheritdoc}
   */
  public function getQuestion($id = NULL, $cached_values = NULL) {
    $context = $this->getContexts($cached_values)[$id];
    return $this->t('Are you sure you want to delete the @label context?', array(
      '@label' => $context['label'],
    ));
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->t('This action cannot be undone.');
  }

  /**
   * {@inheritdoc}
   */
  public function getFormName() {
    return 'confirm';
  }

  /**
   * Provides the action buttons for submitting this form.
   */
  protected function actions(array $form, FormStateInterface $form_state) {
    return array(
      'submit' => array(
        '#type' => 'submit',
        '#value' => $this->getConfirmText(),
        '#validate' => array(
          array($this, 'validate'),
        ),
        '#submit' => array(
          array($this, 'submitForm'),
        ),
      ),
      'cancel' => ConfirmFormHelper::buildCancelLink($this, $this->getRequest()),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    return new Url($route_name, $route_parameters);
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelText() {
    return $this->t('Cancel');
  }

  /**
   * Document the route name and parameters for redirect after submission.
   *
   * @param $cached_values
   *
   * @return array
   *   In the format of
   *   return ['route.name', ['machine_name' => $this->machine_name, 'step' => 'step_name]];
   */
  abstract protected function getParentRouteInfo($cached_values);

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return array
   */
  abstract protected function getContexts($cached_values);

  /**
   * Custom logic for setting the contexts array in cached_values.
   *
   * @param $cached_values
   *
   * @param $contexts
   *   The contexts to set within the cached values.
   *
   * @return mixed
   *   Return the $cached_values
   */
  abstract protected function setContexts($cached_values, $contexts);

}
