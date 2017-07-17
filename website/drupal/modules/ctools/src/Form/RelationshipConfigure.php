<?php

namespace Drupal\ctools\Form;

use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\CloseModalDialogCommand;
use Drupal\Core\Ajax\RedirectCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\ctools\TypedDataResolver;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class RelationshipConfigure extends FormBase {

  /**
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * @var \Drupal\ctools\TypedDataResolver
   */
  protected $resolver;

  /**
   * @var string
   */
  protected $tempstore_id;

  /**
   * @var string;
   */
  protected $machine_name;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('user.shared_tempstore'), $container->get('ctools.typed_data.resolver'));
  }

  public function __construct(SharedTempStoreFactory $tempstore, TypedDataResolver $resolver) {
    $this->tempstore = $tempstore;
    $this->resolver = $resolver;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_relationship_configure';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $context_id = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);

    /** @var \Drupal\Core\Plugin\Context\ContextInterface[] $contexts */
    $contexts = $this->getContexts($cached_values);
    $context_object = $this->resolver->convertTokenToContext($context_id, $contexts);
    $form['id'] = [
      '#type' => 'value',
      '#value' => $context_id
    ];
    $form['context_object'] = [
      '#type' => 'value',
      '#value' => $context_object,
    ];
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Context label'),
      '#default_value' => !empty($contexts[$context_id]) ? $contexts[$context_id]->getContextDefinition()->getLabel() : $this->resolver->getLabelByToken($context_id, $contexts),
      '#required' => TRUE,
    ];
    $form['context_data'] = [
      '#type' => 'item',
      '#title' => $this->t('Data type'),
      '#markup' => $context_object->getContextDefinition()->getDataType(),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#ajax' => [
        'callback' => [$this, 'ajaxSave'],
      ]
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    list($route_name, $route_options) = $this->getParentRouteInfo($cached_values);
    $form_state->setRedirect($route_name, $route_options);
  }

  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return \Drupal\Core\Ajax\AjaxResponse
   */
  public function ajaxSave(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $response = new AjaxResponse();
    $response->addCommand(new RedirectCommand($this->url($route_name, $route_parameters)));
    $response->addCommand(new CloseModalDialogCommand());
    return $response;
  }

  /**
   * Document the route name and parameters for redirect after submission.
   *
   * @param array $cached_values
   *
   * @return array In the format of
   * In the format of
   * return ['route.name', ['machine_name' => $this->machine_name, 'step' => 'step_name']];
   */
  abstract protected function getParentRouteInfo($cached_values);

  /**
   * Custom logic for setting the conditions array in cached_values.
   *
   * @param $cached_values
   *
   * @param $contexts
   *   The conditions to set within the cached values.
   *
   * @return mixed
   *   Return the $cached_values
   */
  abstract protected function setContexts($cached_values, $contexts);

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  abstract protected function getContexts($cached_values);

}
