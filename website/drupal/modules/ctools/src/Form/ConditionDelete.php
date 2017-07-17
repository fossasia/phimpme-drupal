<?php

namespace Drupal\ctools\Form;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\ConfirmFormHelper;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\ctools\ConstraintConditionInterface;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class ConditionDelete extends ConfirmFormBase {

  /**
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * @var \Drupal\Core\Condition\ConditionManager
   */
  protected $manager;

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
    return new static($container->get('user.shared_tempstore'), $container->get('plugin.manager.condition'));
  }

  function __construct(SharedTempStoreFactory $tempstore, PluginManagerInterface $manager) {
    $this->tempstore = $tempstore;
    $this->manager = $manager;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_condition_delete';
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
    $conditions = $this->getConditions($cached_values);
    /** @var  $instance \Drupal\ctools\ConstraintConditionInterface */
    $instance = $this->manager->createInstance($conditions[$this->id]['id'], $conditions[$this->id]);
    if ($instance instanceof ConstraintConditionInterface) {
      $instance->removeConstraints($this->getContexts($cached_values));
    }
    unset($conditions[$this->id]);
    $cached_values = $this->setConditions($cached_values, $conditions);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $form_state->setRedirect($route_name, $route_parameters);
  }

  public function getQuestion($id = NULL, $cached_values = NULL) {
    $condition = $this->getConditions($cached_values)[$id];
    return $this->t('Are you sure you want to delete the @label condition?', array(
      '@label' => $condition['id'],
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
   * {@inheritdoc}
   */
  protected function actions(array $form, FormStateInterface $form_state) {
    return array(
      'submit' => array(
        '#type' => 'submit',
        '#value' => $this->getConfirmText(),
        '#validate' => array(
          array($this, 'validateForm'),
        ),
        '#submit' => array(
          array($this, 'submitForm'),
        ),
      ),
      'cancel' => ConfirmFormHelper::buildCancelLink($this, $this->getRequest()),
    );
  }

  /**
   * Returns the route to go to if the user cancels the action.
   *
   * @return \Drupal\Core\Url
   *   A URL object.
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
   * Custom logic for retrieving the conditions array from cached_values.
   *
   * @param $cached_values
   *
   * @return array
   */
  abstract protected function getConditions($cached_values);

  /**
   * Custom logic for setting the conditions array in cached_values.
   *
   * @param $cached_values
   *
   * @param $conditions
   *   The conditions to set within the cached values.
   *
   * @return mixed
   *   Return the $cached_values
   */
  abstract protected function setConditions($cached_values, $conditions);

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  abstract protected function getContexts($cached_values);

}
