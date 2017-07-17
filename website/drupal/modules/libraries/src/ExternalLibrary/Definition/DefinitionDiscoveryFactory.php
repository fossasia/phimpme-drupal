<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\Core\Config\ConfigFactoryInterface;
use GuzzleHttp\ClientInterface;

/**
 * Instantiates a library definition discovery based on configuration.
 */
class DefinitionDiscoveryFactory {

  /**
   * The configuration factory.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The serializer for local definition files.
   *
   * @var \Drupal\Component\Serialization\SerializationInterface
   */
  protected $localSerializer;

  /**
   * The HTTP client used to fetch remote definitions.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $httpClient;

  /**
   * The serializer for remote definitions.
   *
   * @var \Drupal\Component\Serialization\SerializationInterface
   */
  protected $remoteSerializer;

  /**
   * Constructs a definition discovery factory.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The configuration factory.
   * @param \Drupal\Component\Serialization\SerializationInterface $local_serializer
   *   The serializer for local definition files.
   * @param \GuzzleHttp\ClientInterface $http_client
   *   The HTTP client used to fetch remote definitions.
   * @param \Drupal\Component\Serialization\SerializationInterface $remote_serializer
   *   The serializer for remote definitions.
   */
  public function __construct(
    ConfigFactoryInterface $config_factory,
    SerializationInterface $local_serializer,
    ClientInterface $http_client,
    SerializationInterface $remote_serializer
  ) {
    $this->configFactory = $config_factory;
    $this->localSerializer = $local_serializer;
    $this->httpClient = $http_client;
    $this->remoteSerializer = $remote_serializer;
  }

  /**
   * Gets a library definition discovery.
   *
   * @return \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface
   *   The library definition discovery.
   */
  public function get() {
    $config = $this->configFactory->get('libraries.settings');

    if ($config->get('definition.remote.enable')) {
      $discovery = new ChainDefinitionDiscovery();

      $local_discovery = new WritableFileDefinitionDiscovery(
        $this->localSerializer,
        $config->get('definition.local.path')
      );
      $discovery->addDiscovery($local_discovery);

      foreach ($config->get('definition.remote.urls') as $remote_url) {
        $remote_discovery = new GuzzleDefinitionDiscovery(
          $this->httpClient,
          $this->remoteSerializer,
          $remote_url
        );

        $discovery->addDiscovery($remote_discovery);
      }
    }
    else {
      $discovery = new FileDefinitionDiscovery(
        $this->localSerializer,
        $config->get('definition.local.path')
      );
    }

    return $discovery;
  }

}
