<?php

namespace Drupal\ctools\Form;

use Drupal\Component\Serialization\Json;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\TypedDataManagerInterface;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class ManageContext extends FormBase {

  /**
   * The machine name of the wizard we're working with.
   *
   * @var string
   */
  protected $machine_name;

  /**
   * The typed data manager.
   *
   * @var \Drupal\Core\TypedData\TypedDataManagerInterface
   */
  protected $typedDataManager;

  /**
   * The form builder.
   *
   * @var \Drupal\Core\Form\FormBuilderInterface
   */
  protected $formBuilder;

  /**
   * An array of property types that are eligible as relationships.
   *
   * @var array
   */
  protected $property_types = [];

  /**
   * A property for controlling usage of relationships in an implementation.
   *
   * @var bool
   */
  protected $relationships = TRUE;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('typed_data_manager'), $container->get('form_builder'));
  }

  /**
   * ManageContext constructor.
   *
   * @param \Drupal\Core\TypedData\TypedDataManagerInterface $typed_data_manager
   *   The typed data manager.
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder.
   */
  public function __construct(TypedDataManagerInterface $typed_data_manager, FormBuilderInterface $form_builder) {
    $this->typedDataManager = $typed_data_manager;
    $this->formBuilder = $form_builder;
  }


  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_manage_context_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    $this->machine_name = $cached_values['id'];
    $form['items'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="configured-contexts">',
      '#suffix' => '</div>',
      '#theme' => 'table',
      '#header' => array($this->t('Context ID'), $this->t('Label'), $this->t('Data Type'), $this->t('Options')),
      '#rows' => $this->renderRows($cached_values),
      '#empty' =>  $this->t('No contexts or relationships have been added.')
    );
    foreach ($this->typedDataManager->getDefinitions() as $type => $definition) {
      $types[$type] = $definition['label'];
    }
    if (isset($types['entity'])) {
      unset($types['entity']);
    }
    asort($types);
    $form['context'] = [
      '#type' => 'select',
      '#options' => $types,
    ];
    $form['add'] = [
      '#type' => 'submit',
      '#name' => 'add',
      '#value' => $this->t('Add new context'),
      '#ajax' => [
        'callback' => [$this, 'addContext'],
        'event' => 'click',
      ],
      '#submit' => [
        'callback' => [$this, 'submitForm'],
      ]
    ];

    $form['relationships'] = [
      '#type' => 'select',
      '#title' => $this->t('Add a relationship'),
      '#options' => $this->getAvailableRelationships($cached_values),
      '#access' => $this->relationships,
    ];
    $form['add_relationship'] = [
      '#type' => 'submit',
      '#name' => 'add_relationship',
      '#value' =>  $this->t('Add Relationship'),
      '#ajax' => [
        'callback' => [$this, 'addRelationship'],
        'event' => 'click',
      ],
      '#submit' => [
        'callback' => [$this, 'submitForm'],
      ],
      '#access' => $this->relationships,
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->getTriggeringElement()['#name'] == 'add') {
      $cached_values = $form_state->getTemporaryValue('wizard');
      list(, $route_parameters) = $this->getContextOperationsRouteInfo($cached_values, $this->machine_name, $form_state->getValue('context'));
      $form_state->setRedirect($this->getContextAddRoute($cached_values), $route_parameters);
    }
    if ($form_state->getTriggeringElement()['#name'] == 'add_relationship') {
      $cached_values = $form_state->getTemporaryValue('wizard');
      list(, $route_parameters) = $this->getRelationshipOperationsRouteInfo($cached_values, $this->machine_name, $form_state->getValue('relationships'));
      $form_state->setRedirect($this->getRelationshipAddRoute($cached_values), $route_parameters);
    }
  }

  public function addContext(array &$form, FormStateInterface $form_state) {
    $context = $form_state->getValue('context');
    $content = $this->formBuilder->getForm($this->getContextClass(), $context, $this->getTempstoreId(), $this->machine_name);
    $content['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $cached_values = $form_state->getTemporaryValue('wizard');
    list(, $route_parameters) = $this->getContextOperationsRouteInfo($cached_values, $this->machine_name, $context);
    $content['submit']['#attached']['drupalSettings']['ajax'][$content['submit']['#id']]['url'] = $this->url($this->getContextAddRoute($cached_values), $route_parameters, ['query' => [FormBuilderInterface::AJAX_FORM_REQUEST => TRUE]]);
    $response = new AjaxResponse();
    $response->addCommand(new OpenModalDialogCommand($this->t('Add new context'), $content, array('width' => '700')));
    return $response;
  }

  public function addRelationship(array &$form, FormStateInterface $form_state) {
    $relationship = $form_state->getValue('relationships');
    $content = $this->formBuilder->getForm($this->getRelationshipClass(), $relationship, $this->getTempstoreId(), $this->machine_name);
    $content['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $cached_values = $form_state->getTemporaryValue('wizard');
    list(, $route_parameters) = $this->getRelationshipOperationsRouteInfo($cached_values, $this->machine_name, $relationship);
    $content['submit']['#attached']['drupalSettings']['ajax'][$content['submit']['#id']]['url'] = $this->url($this->getRelationshipAddRoute($cached_values), $route_parameters, ['query' => [FormBuilderInterface::AJAX_FORM_REQUEST => TRUE]]);
    $response = new AjaxResponse();
    $response->addCommand(new OpenModalDialogCommand($this->t('Configure Relationship'), $content, array('width' => '700')));
    return $response;
  }

  protected function getAvailableRelationships($cached_values) {
    /** @var \Drupal\ctools\TypedDataResolver $resolver */
    $resolver = \Drupal::service('ctools.typed_data.resolver');
    return $resolver->getTokensForContexts($this->getContexts($cached_values));
  }

  /**
   * @param $cached_values
   *
   * @return array
   */
  protected function renderRows($cached_values) {
    $contexts = array();
    foreach ($this->getContexts($cached_values) as $row => $context) {
      list($route_name, $route_parameters) = $this->getContextOperationsRouteInfo($cached_values, $this->machine_name, $row);
      $build = array(
        '#type' => 'operations',
        '#links' => $this->getOperations($cached_values, $row, $route_name, $route_parameters),
      );
      $contexts[$row] = array(
        $row,
        $context->getContextDefinition()->getLabel(),
        $context->getContextDefinition()->getDataType(),
        'operations' => [
          'data' => $build,
        ],
      );
    }
    return $contexts;
  }

  /**
   * @param array $cached_values
   * @param string $row
   * @param string $route_name_base
   * @param array $route_parameters
   *
   * @return mixed
   */
  protected function getOperations($cached_values, $row, $route_name_base, array $route_parameters = array()) {
    $operations = [];
    if ($this->isEditableContext($cached_values, $row)) {
      $operations['edit'] = array(
        'title' =>  $this->t('Edit'),
        'url' => new Url($route_name_base . '.edit', $route_parameters),
        'weight' => 10,
        'attributes' => array(
          'class' => ['use-ajax'],
          'data-dialog-type' => 'modal',
          'data-dialog-options' => Json::encode([
            'width' => 700,
          ]),
        ),
      );
      $operations['delete'] = array(
        'title' =>  $this->t('Delete'),
        'url' => new Url($route_name_base . '.delete', $route_parameters),
        'weight' => 100,
        'attributes' => array(
          'class' => array('use-ajax'),
          'data-dialog-type' => 'modal',
          'data-dialog-options' => Json::encode([
            'width' => 700,
          ]),
        ),
      );
    }
    return $operations;
  }

  /**
   * Return a subclass of '\Drupal\ctools\Form\ContextConfigure'.
   *
   * The ContextConfigure class is designed to be subclassed with custom
   * route information to control the modal/redirect needs of your use case.
   *
   * @return string
   */
  abstract protected function getContextClass($cached_values);

  /**
   * Return a subclass of '\Drupal\ctools\Form\RelationshipConfigure'.
   *
   * The RelationshipConfigure class is designed to be subclassed with custom
   * route information to control the modal/redirect needs of your use case.
   *
   * @return string
   */
  abstract protected function getRelationshipClass($cached_values);

  /**
   * The route to which context 'add' actions should submit.
   *
   * @return string
   */
  abstract protected function getContextAddRoute($cached_values);

  /**
   * The route to which relationship 'add' actions should submit.
   *
   * @return string
   */
  abstract protected function getRelationshipAddRoute($cached_values);

  /**
   * Provide the tempstore id for your specified use case.
   *
   * @return string
   */
  abstract protected function getTempstoreId();

  /**
   * Returns the contexts already available in the wizard.
   *
   * @param mixed $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  abstract protected function getContexts($cached_values);

  /**
   * @param mixed $cached_values
   * @param string $machine_name
   * @param string $row
   *
   * @return array
   */
  abstract protected function getContextOperationsRouteInfo($cached_values, $machine_name, $row);

  /**
   * @param mixed $cached_values
   * @param string $machine_name
   * @param string $row
   *
   * @return array
   */
  abstract protected function getRelationshipOperationsRouteInfo($cached_values, $machine_name, $row);

  /**
   * @param mixed $cached_values
   * @param string $row
   *
   * @return bool
   */
  abstract protected function isEditableContext($cached_values, $row);

}
