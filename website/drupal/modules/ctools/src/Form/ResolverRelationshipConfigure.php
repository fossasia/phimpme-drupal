<?php

namespace Drupal\ctools\Form;


use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\CloseModalDialogCommand;
use Drupal\Core\Ajax\RedirectCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class ResolverRelationshipConfigure extends FormBase {

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
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('user.shared_tempstore'));
  }

  function __construct(SharedTempStoreFactory $tempstore) {
    $this->tempstore = $tempstore;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_context_configure';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $context = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    if (is_numeric($context)) {
      $id = $context;
      $contexts = $this->getContexts($cached_values);
      $context = $contexts[$id]['context'];
      $label = $contexts[$id]['label'];
      $machine_name = $contexts[$id]['machine_name'];
      $description = $contexts[$id]['description'];
      // Conditionally set this form element so that we can update or add.
      $form['id'] = [
        '#type' => 'value',
        '#value' => $id
      ];
    }
    else {
      $label = '';
      $machine_name = '';
      $description = '';
    }
    $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $form['context'] = [
      '#type' => 'value',
      '#value' => $context
    ];
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#default_value' => $label,
      '#required' => TRUE,
    ];
    $form['machine_name'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Machine Name'),
      '#default_value' => $machine_name,
      '#required' => TRUE,
    ];
    $form['description'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Description'),
      '#default_value' => $description,
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

  public function validateForm(array &$form, FormStateInterface $form_state) {
    $machine_name = $form_state->getValue('machine_name');
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    foreach ($this->getContexts($cached_values) as $id => $context) {
      if ($context['machine_name'] == $machine_name) {
        $form_state->setError($form['machine_name'], $this->t('That machine name is in use by another context definition.'));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    $contexts = $this->getContexts($cached_values);
    $context = [
      'context' => $form_state->getValue('context'),
      'label' => $form_state->getValue('label'),
      'machine_name' => $form_state->getValue('machine_name'),
      'description' => $form_state->getValue('description'),
    ];
    if ($form_state->hasValue('id')) {
      $contexts[$form_state->getValue('id')] = $context;
    }
    else {
      $contexts[] = $context;
    }
    $cached_values = $this->setContexts($cached_values, $contexts);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $form_state->setRedirect($route_name, $route_parameters);
  }

  public function ajaxSave(array &$form, FormStateInterface $form_state) {
    $response = new AjaxResponse();
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $response->addCommand(new RedirectCommand($this->url($route_name, $route_parameters)));
    $response->addCommand(new CloseModalDialogCommand());
    return $response;
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
