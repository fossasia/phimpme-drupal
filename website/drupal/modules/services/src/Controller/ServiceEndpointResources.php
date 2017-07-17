<?php

namespace Drupal\services\Controller;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Component\Serialization\Json;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Url;
use Drupal\services\ServiceEndpointInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class \Drupal\services\Controller\ServiceEndpointManageResource.
 */
class ServiceEndpointResources extends ControllerBase {

  /**
   * Service definition plugin manager.
   *
   * @var \Drupal\Component\Plugin\PluginManagerInterface.
   */
  protected $pluginManager;

  /**
   * Constructor for \Drupal\services\Form\ServiceEndpointResourceForm.
   */
  public function __construct(PluginManagerInterface $plugin_manager) {
    $this->pluginManager = $plugin_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('plugin.manager.services.service_definition')
    );
  }

  /**
   * List service resources.
   *
   * @param \Drupal\services\ServiceEndpointInterface|null $service_endpoint
   *   A service endpoint entity.
   *
   * @return array
   *   An renderable array.
   */
  public function displayList(ServiceEndpointInterface $service_endpoint = NULL) {
    $build = [];

    foreach ($this->getCategoryDefinitions($service_endpoint) as $category => $definitions) {
      if (!isset($category)) {
        continue;
      }
      $build[$category] = [
        '#type' => 'details',
        '#title' => $this->t('@category', ['@category' => $category]),
        '#tree' => TRUE,
      ];
      $rows = [];

      foreach ($definitions as $plugin_id => $definition) {
        $row = $this->buildRow($service_endpoint, $definition);

        $row['operations']['data'] = [
          '#type' => 'dropbutton',
          '#links' => $this->buildOperationLinks($service_endpoint, $plugin_id),
        ];

        $rows[] = $row;
      }

      $build[$category]['table'] = array(
        '#type' => 'table',
        '#rows' => $rows,
        '#header' => $this->buildHeader(),
        '#empty' => t('No service definitions exist'),
      );
    }

    return $build;
  }

  /**
   * Build service resource table header.
   *
   * @return array
   *   An array of table headers.
   */
  protected function buildHeader() {
    return [
      'title' => $this->t('Definition'),
      'endpoint' => $this->t('Endpoint'),
      'operations' => $this->t('Operations'),
    ];
  }

  /**
   * Build service resource table rows.
   *
   * @return array
   *   An array of row items.
   */
  protected function buildRow(ServiceEndpointInterface $service_endpoint, array $definition) {
    $row = [];

    $row['title']['data'] = [
      '#markup' => $definition['title'],
    ];

    $row['endpoint']['data'] = [
      '#markup' => $definition['path'],
    ];

    return $row;
  }

  /**
   * Build service resource operations links.
   *
   * @param \Drupal\services\ServiceEndpointInterface $service_endpoint
   *   An service endpoint object.
   * @param string $plugin_id
   *   An service plugin identifier.
   *
   * @return array
   *   An array of operations links.
   */
  protected function buildOperationLinks(ServiceEndpointInterface $service_endpoint, $plugin_id) {
    $links = [];

    $links['configure'] = [
      'title' => $this->t('Enable'),
      'url' => Url::fromRoute('entity.service_endpoint_resource.config_form', [
        'plugin_id' => $plugin_id,
        'service_endpoint' => $service_endpoint->id(),
      ]),
      'attributes' => $this->getModalAttributes(),
    ];

    if ($service_resource = $service_endpoint->loadResourceProvider($plugin_id)) {
      if ($service_resource->hasLinkTemplate('delete-form')) {
        $links['disable'] = [
          'title' => $this->t('Disable'),
          'url' => $service_resource->urlInfo('delete-form'),
          'attributes' => $this->getModalAttributes(),
        ];
      }
      $links['configure']['title'] = $this->t('Configure');
    }

    return $links;
  }

  /**
   * Get service definitions grouped by category.
   *
   * @return array
   *   An array of resource definitions keyed by category.
   */
  protected function getCategoryDefinitions() {
    $definitions = [];

    foreach ($this->pluginManager->getDefinitions() as $plugin_id => $definition) {
      if (!isset($definition['category'])) {
        continue;
      }
      $category = $definition['category']->render();

      $definitions[$category][$plugin_id] = $definition;
    }

    ksort($definitions);

    return $definitions;
  }

  /**
   * Get AJAX modal attributes.
   *
   * @return array
   *   An array of modal attributes.
   */
  protected function getModalAttributes() {
    return [
      'class' => ['use-ajax'],
      'data-dialog-type' => 'modal',
      'data-dialog-options' => Json::encode([
        'width' => 800,
      ]),
    ];
  }

}
