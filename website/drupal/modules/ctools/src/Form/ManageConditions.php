<?php

namespace Drupal\ctools\Form;


use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Component\Serialization\Json;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Symfony\Component\DependencyInjection\ContainerInterface;

abstract class ManageConditions extends FormBase {

  /**
   * @var \Drupal\Core\Condition\ConditionManager
   */
  protected $manager;

  /**
   * @var string
   */
  protected $machine_name;

  public static function create(ContainerInterface $container) {
    return new static($container->get('plugin.manager.condition'));
  }

  function __construct(PluginManagerInterface $manager) {
    $this->manager = $manager;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_manage_conditions_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    $this->machine_name = $cached_values['id'];
    $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $options = [];
    $contexts = $this->getContexts($cached_values);
    foreach ($this->manager->getDefinitionsForContexts($contexts) as $plugin_id => $definition) {
      $options[$plugin_id] = (string) $definition['label'];
    }
    $form['items'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="configured-conditions">',
      '#suffix' => '</div>',
      '#theme' => 'table',
      '#header' => array($this->t('Plugin Id'), $this->t('Summary'), $this->t('Operations')),
      '#rows' => $this->renderRows($cached_values),
      '#empty' => $this->t('No required conditions have been configured.')
    );
    $form['conditions'] = [
      '#type' => 'select',
      '#options' => $options,
    ];
    $form['add'] = [
      '#type' => 'submit',
      '#name' => 'add',
      '#value' => $this->t('Add Condition'),
      '#ajax' => [
        'callback' => [$this, 'add'],
        'event' => 'click',
      ],
      '#submit' => [
        'callback' => [$this, 'submitForm'],
      ]
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    list(, $route_parameters) = $this->getOperationsRouteInfo($cached_values, $this->machine_name, $form_state->getValue('conditions'));
    $form_state->setRedirect($this->getAddRoute($cached_values), $route_parameters);
  }

  public function add(array &$form, FormStateInterface $form_state) {
    $condition = $form_state->getValue('conditions');
    $content = \Drupal::formBuilder()->getForm($this->getConditionClass(), $condition, $this->getTempstoreId(), $this->machine_name);
    $content['#attached']['library'][] = 'core/drupal.dialog.ajax';
    $cached_values = $form_state->getTemporaryValue('wizard');
    list(, $route_parameters) = $this->getOperationsRouteInfo($cached_values, $this->machine_name, $form_state->getValue('conditions'));
    $content['submit']['#attached']['drupalSettings']['ajax'][$content['submit']['#id']]['url'] = $this->url($this->getAddRoute($cached_values), $route_parameters, ['query' => [FormBuilderInterface::AJAX_FORM_REQUEST => TRUE]]);
    $response = new AjaxResponse();
    $response->addCommand(new OpenModalDialogCommand($this->t('Configure Required Context'), $content, array('width' => '700')));
    return $response;
  }

  /**
   * @param $cached_values
   *
   * @return array
   */
  public function renderRows($cached_values) {
    $configured_conditions = array();
    foreach ($this->getConditions($cached_values) as $row => $condition) {
      /** @var $instance \Drupal\Core\Condition\ConditionInterface */
      $instance = $this->manager->createInstance($condition['id'], $condition);
      list($route_name, $route_parameters) = $this->getOperationsRouteInfo($cached_values, $cached_values['id'], $row);
      $build = array(
        '#type' => 'operations',
        '#links' => $this->getOperations($route_name, $route_parameters),
      );
      $configured_conditions[] = array(
        $instance->getPluginId(),
        $instance->summary(),
        'operations' => [
          'data' => $build,
        ],
      );
    }
    return $configured_conditions;
  }

  protected function getOperations($route_name_base, array $route_parameters = array()) {
    $operations['edit'] = array(
      'title' => $this->t('Edit'),
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
    $route_parameters['id'] = $route_parameters['condition'];
    $operations['delete'] = array(
      'title' => $this->t('Delete'),
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
    return $operations;
  }

  /**
   * Return a subclass of '\Drupal\ctools\Form\ConditionConfigure'.
   *
   * The ConditionConfigure class is designed to be subclassed with custom
   * route information to control the modal/redirect needs of your use case.
   *
   * @return string
   */
  abstract protected function getConditionClass();

  /**
   * The route to which condition 'add' actions should submit.
   *
   * @param mixed $cached_values
   *
   * @return string
   */
  abstract protected function getAddRoute($cached_values);

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
   * Subclassing '\Drupal\ctools\Form\ConditionConfigure' and
   * '\Drupal\ctools\Form\ConditionDelete' should set you up for using this
   * approach quite seamlessly.
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
   * Custom logic for retrieving the conditions array from cached_values.
   *
   * @param $cached_values
   *
   * @return array
   */
  abstract protected function getConditions($cached_values);

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  abstract protected function getContexts($cached_values);

}
