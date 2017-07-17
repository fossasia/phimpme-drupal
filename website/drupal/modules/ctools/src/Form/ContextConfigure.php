<?php

namespace Drupal\ctools\Form;


use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\CloseModalDialogCommand;
use Drupal\Core\Ajax\RedirectCommand;
use Drupal\Core\Entity\Entity;
use Drupal\Core\Entity\Plugin\DataType\EntityAdapter;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\Plugin\Context\ContextInterface;
use Drupal\Core\Url;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class ContextConfigure extends FormBase {

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
  public function buildForm(array $form, FormStateInterface $form_state, $context_id = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    $contexts = $this->getContexts($cached_values);
    $edit = FALSE;
    if (!empty($contexts[$context_id])) {
      $context = $contexts[$context_id];
      $machine_name = $context_id;
      $edit = TRUE;
    }
    else {
      $context_definition = new ContextDefinition($context_id);
      $context = new Context($context_definition);
      $machine_name = '';
    }
    $label = $context->getContextDefinition()->getLabel();
    $description = $context->getContextDefinition()->getDescription();
    $data_type = $context->getContextDefinition()->getDataType();
    $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $form['context_id'] = [
      '#type' => 'value',
      '#value' => $context_id
    ];
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#default_value' => $label,
      '#required' => TRUE,
    ];
    $form['machine_name'] = [
      '#type' => 'machine_name',
      '#title' => $this->t('Machine Name'),
      '#default_value' => $machine_name,
      '#required' => TRUE,
      '#maxlength' => 128,
      '#machine_name' => [
        'source' => ['label'],
        'exists' => [$this, 'contextExists'],
      ],
      '#disabled' => $this->disableMachineName($cached_values, $machine_name),
    ];
    $form['description'] = [
      '#type' => 'textarea',
      '#title' => $this->t('Description'),
      '#default_value' => $description,
    ];
    if (strpos($data_type, 'entity:') === 0) {
      list(, $entity_type) = explode(':', $data_type);
      /** @var EntityAdapter $entity */
      $entity = $edit ? $context->getContextValue() : NULL;
      $form['context_value'] = [
        '#type' => 'entity_autocomplete',
        '#required' => TRUE,
        '#target_type' => $entity_type,
        '#default_value' => $entity,
        '#title' => $this->t('Select entity'),
      ];
    }
    else {
      $value = $context->getContextData()->getValue();
      $form['context_value'] = [
        '#title' => $this->t('Set a context value'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => $value,
      ];
    }
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
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // If these are not equal, then we're adding a new context and should not override an existing context.
    if ($form_state->getValue('machine_name') != $form_state->getValue('context_id')) {
      $machine_name = $form_state->getValue('machine_name');
      $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
      if (!empty($this->getContexts($cached_values)[$machine_name])) {
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
    if ($form_state->getValue('machine_name') != $form_state->getValue('context_id')) {
      $data_type = $form_state->getValue('context_id');
      $context_definition = new ContextDefinition($data_type, $form_state->getValue('label'), TRUE, FALSE, $form_state->getValue('description'));
    }
    else {
      $context = $contexts[$form_state->getValue('machine_name')];
      $context_definition = $context->getContextDefinition();
      $context_definition->setLabel($form_state->getValue('label'));
      $context_definition->setDescription($form_state->getValue('description'));
    }
    // We're dealing with an entity and should make sure it's loaded.
    if (strpos($context_definition->getDataType(), 'entity:') === 0) {
      list(, $entity_type) = explode(':', $context_definition->getDataType());
      if (is_numeric($form_state->getValue('context_value'))) {
        $value = \Drupal::entityTypeManager()->getStorage($entity_type)->load($form_state->getValue('context_value'));
      }
    }
    // No loading required for non-entity values.
    else {
      $value = $form_state->getValue('context_value');
    }
    $context = new Context($context_definition, $value);

    $cached_values = $this->addContext($cached_values, $form_state->getValue('machine_name'), $context);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $form_state->setRedirect($route_name, $route_parameters);
  }

  public function ajaxSave(array &$form, FormStateInterface $form_state) {
    $response = new AjaxResponse();
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    list($route_name, $route_parameters) = $this->getParentRouteInfo($cached_values);
    $url = new Url($route_name, $route_parameters);
    $response->addCommand(new RedirectCommand($url->toString()));
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
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  abstract protected function getContexts($cached_values);

  /**
   * Custom logic for adding a context to the cached_values contexts array.
   *
   * @param array $cached_values
   *   The cached_values currently in use.
   * @param string $context_id
   *   The context identifier.
   * @param \Drupal\Core\Plugin\Context\ContextInterface $context
   *   The context to add or update within the cached values.
   *
   * @return mixed
   *   Return the $cached_values
   */
  abstract protected function addContext($cached_values, $context_id, ContextInterface $context);

  /**
   * Custom "exists" logic for the context to be created.
   *
   * @param string $value
   *   The name of the context.
   * @param $element
   *   The machine_name element
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state.
   *
   * @return bool
   *   Return true if a context of this name exists.
   */
  abstract public function contextExists($value, $element, $form_state);

  /**
   * Determines if the machine_name should be disabled.
   *
   * @param $cached_values
   *
   * @return bool
   */
  abstract protected function disableMachineName($cached_values, $machine_name);

}
