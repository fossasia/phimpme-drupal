<?php

namespace Drupal\services\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\services\ServiceEndpointInterface;

/**
 * Defines the service endpoint entity.
 *
 * @ConfigEntityType(
 *   id = "service_endpoint",
 *   label = @Translation("service endpoint"),
 *   handlers = {
 *     "list_builder" = "Drupal\services\Controller\ServiceEndpointListBuilder",
 *     "form" = {
 *       "add" = "Drupal\services\Form\ServiceEndpointForm",
 *       "edit" = "Drupal\services\Form\ServiceEndpointForm",
 *       "delete" = "Drupal\services\Form\ServiceEndpointDeleteForm"
 *     }
 *   },
 *   config_prefix = "service_endpoint",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "collection" = "/admin/structure/service_endpoint",
 *     "canonical" = "/admin/structure/service_endpoint/{service_endpoint}",
 *     "edit-form" = "/admin/structure/service_endpoint/{service_endpoint}/edit",
 *     "delete-form" = "/admin/structure/service_endpoint/{service_endpoint}/delete",
 *     "resources" =  "/admin/structure/service_endpoint/{service_endpoint}/resources"
 *   }
 * )
 */
class ServiceEndpoint extends ConfigEntityBase implements ServiceEndpointInterface {

  /**
   * The services endpoint ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The services endpoint label.
   *
   * @var string
   */
  protected $label;

  /**
   * The services endpoint.
   *
   * @var string
   */
  protected $endpoint;

  /**
   * {@inheritdoc}
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  /**
   * {@inheritdoc}
   */
  public function loadResourceProviders() {
    return $this->getResourceStorage()
      ->loadByProperties([
        'service_endpoint_id' => $this->id(),
      ]);
  }

  /**
   * {@inheritdoc}
   */
  public function loadResourceProvider($plugin_id) {
    $entities = $this->getResourceStorage()
      ->loadByProperties([
        'service_plugin_id' => $plugin_id,
        'service_endpoint_id' => $this->id(),
      ]);

    return !empty($entities) ? reset($entities) : FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function delete() {
    parent::delete();
    $this->getResourceStorage()->delete($this->loadResourceProviders());
  }

  /**
   * Get resource storage object.
   *
   * @return \Drupal\Core\Entity\EntityStorageInterface
   *   Resource storage object.
   */
  protected function getResourceStorage() {
    return $this->entityTypeManager()->getStorage('service_endpoint_resource');
  }

}
