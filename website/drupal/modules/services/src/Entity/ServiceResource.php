<?php

namespace Drupal\services\Entity;

use Drupal\Core\Authentication\AuthenticationProviderInterface;
use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\services\ServiceResourceInterface;

/**
 * Defines service resource entity.
 *
 * @ConfigEntityType(
 *   id = "service_endpoint_resource",
 *   label = @Translation("Resource"),
 *   handlers = {
 *     "form" = {
 *       "config" = "\Drupal\services\Form\ServiceResourceConfigForm",
 *       "delete" = "\Drupal\services\Form\ServiceResourceDeleteForm"
 *     }
 *   },
 *   entity_keys = {
 *     "id" = "id",
 *     "uuid" = "uuid",
 *   },
 *   config_prefix = "endpoint_resource",
 *   admin_permission = "administer site configuration",
 *   links = {
 *     "config-form" = "/admin/structure/service_endpoint/{service_endpoint}/resource/{plugin_id}",
 *     "delete-form" = "/admin/structure/service_endpoint/{service_endpoint}/resource/{plugin_id}/delete"
 *   }
 * )
 */
class ServiceResource extends ConfigEntityBase implements ServiceResourceInterface {

  /**
   * Resource ID.
   *
   * @var string
   */
  protected $id;

  /**
   * Service plugin ID.
   *
   * @var string
   */
  protected $service_plugin_id;

  /**
   * Service endpoint ID.
   *
   * @var string
   */
  protected $service_endpoint_id;

  /**
   * Resource formats.
   *
   * @var array
   */
  protected $formats = [];

  /**
   * Resource authentication.
   *
   * @var array
   */
  protected $authentication = [];

  /**
   * Resource no cache option.
   *
   * @var array
   */
  protected $no_cache = NULL;

  /**
   * {@inheritdoc}
   */
  public function id() {
    return $this->service_endpoint_id . '.' . strtr($this->service_plugin_id, ':', '.');
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    if ($service_plugin = $this->getServicePlugin()) {
      return $service_plugin['title'];
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getFormats() {
    if (!empty($this->formats)) {
      return array_filter($this->formats);
    }

    return $this->getDefaultSettings()->get('default_formats');
  }

  /**
   * {@inheritdoc}
   */
  public function getAuthentication() {
    if (!empty($this->authentication)) {
      return array_filter($this->authentication);
    }

    return $this->getDefaultSettings()->get('default_authentication');
  }

  /**
   * {@inheritdoc}
   */
  public function getNoCache() {
    if (isset($this->no_cache)) {
      return $this->no_cache;
    }

    return $this->getDefaultSettings()->get('default_no_cache');
  }

  /**
   * Get service plugin definition.
   *
   * @return \Drupal\Component\Plugin\PluginManagerInterface
   *   A service plugin definition.
   */
  public function getServicePlugin() {
    if (!$this->hasServicePlugin()) {
      return FALSE;
    }

    return $this->serviceDefinition()->getDefinition($this->service_plugin_id);
  }

  /**
   * Create service plugin instance.
   *
   * @param array $values
   *   An array of values to pass into the instance.
   *
   * @return \Drupal\services\ServiceDefinitionInterface
   *   A service definition instance.
   */
  public function createServicePluginInstance(array $values = []) {
    if (!$this->hasServicePlugin()) {
      return FALSE;
    }

    return $this->serviceDefinition()->createInstance($this->service_plugin_id, $values);
  }

  /**
   * Has a service plugin definition.
   *
   * @return bool
   *   TRUE if a service plugin exists; otherwise FALSE.
   */
  public function hasServicePlugin() {
    if (!isset($this->service_plugin_id)) {
      return FALSE;
    }

    return $this->serviceDefinition()->hasDefinition($this->service_plugin_id);
  }

  /**
   * Service endpoint object.
   */
  public function getEndpoint() {
    return $this->entityTypeManager()->getStorage('service_endpoint')->load($this->service_endpoint_id);
  }

  /**
   * {@inheritdoc}
   */
  public function postSave(EntityStorageInterface $storage, $update = TRUE) {
    parent::postSave($storage, $update);
    \Drupal::service('router.builder')->setRebuildNeeded();
  }

  /**
   * {@inheritdoc}
   */
  public static function postDelete(EntityStorageInterface $storage, array $entities) {
    parent::postDelete($storage, $entities);
    \Drupal::service('router.builder')->setRebuildNeeded();
  }

  /**
   * Service default global settings.
   *
   * @return \Drupal\Core\Config\Config
   *   A configuration object.
   */
  public function getDefaultSettings() {
    return $this->getConfigManager()->getConfigFactory()->get('services.settings');
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    parent::calculateDependencies();

    // Calculate the dependencies based on the selected authentications that
    // have been selected for a given services resource.
    if ($this instanceof ServiceResourceInterface) {
      foreach ($this->getAuthentication() as $provider_id) {
        $provider = $this->authenticationCollector()->getProvider($provider_id);

        if (!$provider instanceof AuthenticationProviderInterface) {
          continue;
        }
        $class_info = explode('\\', get_class($provider));
        $module_name = $class_info[1];

        // Make sure the module exists prior to requiring it as a dependency.
        if (\Drupal::moduleHandler()->moduleExists($module_name)) {
          $this->addDependency('module', $module_name);
        }
      }
    }

    return $this;
  }

  /**
   * Service plugin definition.
   *
   * @return \Drupal\Component\Plugin\PluginManagerInterface
   *   A plugin manager for the service definition.
   */
  protected function serviceDefinition() {
    return \Drupal::service('plugin.manager.services.service_definition');
  }

  /**
   * {@inheritdoc}
   */
  protected function urlRouteParameters($rel) {
    if (!in_array($rel, ['collection', 'add-page', 'add-form'], TRUE)) {
      $uri_route_parameters['plugin_id'] = $this->service_plugin_id;
      $uri_route_parameters['service_endpoint'] = $this->service_endpoint_id;
    }

    return $uri_route_parameters;
  }

  /**
   * Authentication collector.
   *
   * @return \Drupal\Core\Authentication\AuthenticationCollectorInterface.
   *   An authentication collection object.
   */
  protected function authenticationCollector() {
    return \Drupal::service('authentication_collector');
  }

}
