<?php

namespace Drupal\ctools\Form;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class RequiredContext extends FormBase {

  /**
   * @var \Drupal\Core\TypedData\TypedDataManager
   */
  protected $typedDataManager;

  /**
   * @var string
   */
  protected $machine_name;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('typed_data_manager'));
  }

  public function __construct(PluginManagerInterface $typed_data_manager) {
    $this->typedDataManager = $typed_data_manager;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_required_context_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    $this->machine_name = $cached_values['id'];
    $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $options = [];
    foreach ($this->typedDataManager->getDefinitions() as $plugin_id => $definition) {
      $options[$plugin_id] = (string) $definition['label'];
    }
    $form['items'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="configured-contexts">',
      '#suffix' => '</div>',
      '#theme' => 'table',
      '#header' => array($this->t('Information'), $this->t('Description'), $this->t('Operations')),
      '#rows' => $this->renderContexts($cached_values),
      '#empty' => $this->t('No required contexts have been configured.')
    );
    $form['contexts'] = [
      '#type' => 'select',
      '#options' => $options,
    ];
    $form['add'] = [
      '#type' => 'submit',
      '#name' => 'add',
      '#value' => $this->t('Add required context'),
      '#ajax' => [
        'callback' => [$this, 'add'],
        'event' => 'click',
      ],
      '#submit' => [
        'callback' => [$this, 'submitform'],
      ]
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    list($route_name, $route_parameters) = $this->getOperationsRouteInfo($cached_values, $this->machine_name, $form_state->getValue('contexts'));
    $form_state->setRedirect($route_name . '.edit', $route_parameters);
  }

  /**
   * Custom ajax form submission handler.
   *
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *
   * @return \Drupal\Core\Ajax\AjaxResponse
   */
  public function add(array &$form, FormStateInterface $form_state) {
    $context = $form_state->getValue('contexts');
    $content = \Drupal::formBuilder()->getForm($this->getContextClass(), $context, $this->getTempstoreId(), $this->machine_name);
    $content['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $response = new AjaxResponse();
    $response->addCommand(new OpenModalDialogCommand($this->t('Configure Required Context'), $content, array('width' => '700')));
    return $response;
  }

  /**
   * @param $cached_values
   *
   * @return array
   */
  public function renderContexts($cached_values) {
    $configured_contexts = array();
    foreach ($this->getContexts($cached_values) as $row => $context) {
      list($plugin_id, $label, $machine_name, $description) = array_values($context);
      list($route_name, $route_parameters) = $this->getOperationsRouteInfo($cached_values, $cached_values['id'], $row);
      $build = array(
        '#type' => 'operations',
        '#links' => $this->getOperations($route_name, $route_parameters),
      );
      $configured_contexts[] = array(
        $this->t('<strong>Label:</strong> @label<br /> <strong>Type:</strong> @type', ['@label' => $label, '@type' => $plugin_id]),
        $this->t('@description', ['@description' => $description]),
        'operations' => [
          'data' => $build,
        ],
      );
    }
    return $configured_contexts;
  }

  protected function getOperations($route_name_base, array $route_parameters = array()) {
    $operations['edit'] = array(
      'title' => $this->t('Edit'),
      'url' => new Url($route_name_base . '.edit', $route_parameters),
      'weight' => 10,
      'attributes' => array(
        'class' => array('use-ajax'),
        'data-accepts' => 'application/vnd.drupal-modal',
        'data-dialog-options' => json_encode(array(
          'width' => 700,
        )),
      ),
      'ajax' => [
        ''
      ],
    );
    $route_parameters['id'] = $route_parameters['context'];
    $operations['delete'] = array(
      'title' => $this->t('Delete'),
      'url' => new Url($route_name_base . '.delete', $route_parameters),
      'weight' => 100,
      'attributes' => array(
        'class' => array('use-ajax'),
        'data-accepts' => 'application/vnd.drupal-modal',
        'data-dialog-options' => json_encode(array(
          'width' => 700,
        )),
      ),
    );
    return $operations;
  }

  /**
   * Return a subclass of '\Drupal\ctools\Form\ContextConfigure'.
   *
   * The ContextConfigure class is designed to be subclassed with custom route
   * information to control the modal/redirect needs of your use case.
   *
   * @return string
   */
  abstract protected function getContextClass();

  /**
   * Provide the tempstore id for your specified use case.
   *
   * @return string
   */
  abstract protected function getTempstoreId();

  /**
   * Document the route name and parameters for edit/delete context operations.
   *
   * The route name returned from this method is used as a "base" to which
   * ".edit" and ".delete" are appeneded in the getOperations() method.
   * Subclassing '\Drupal\ctools\Form\ContextConfigure' and
   * '\Drupal\ctools\Form\RequiredContextDelete' should set you up for using
   * this approach quite seamlessly.
   *
   * @param mixed $cached_values
   *
   * @param string $machine_name
   *
   * @param string $row
   *
   * @return array
   *   In the format of
   *   return ['route.base.name', ['machine_name' => $machine_name, 'context' => $row]];
   */
  abstract protected function getOperationsRouteInfo($cached_values, $machine_name, $row);

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return array
   */
  abstract protected function getContexts($cached_values);

}
