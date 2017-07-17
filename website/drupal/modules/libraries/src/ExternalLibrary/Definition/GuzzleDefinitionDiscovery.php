<?php

namespace Drupal\libraries\ExternalLibrary\Definition;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException;
use GuzzleHttp\ClientInterface;
use GuzzleHttp\Exception\GuzzleException;

/**
 * Provides a definition discovery that fetches remote definitions using Guzzle.
 *
 * By default JSON files are assumed to be in JSON format.
 *
 * @todo Cache responses statically by ID to avoid multiple HTTP requests when
 *   calling hasDefinition() and getDefinition() sequentially.
 */
class GuzzleDefinitionDiscovery extends FileDefinitionDiscoveryBase implements DefinitionDiscoveryInterface {

  /**
   * The HTTP client.
   *
   * @var \GuzzleHttp\ClientInterface
   */
  protected $httpClient;

  /**
   * Constructs a Guzzle-based definition discvoery.
   *
   * @param \GuzzleHttp\ClientInterface $http_client
   *   The HTTP client.
   * @param \Drupal\Component\Serialization\SerializationInterface $serializer
   *   The serializer for the library definition files.
   * @param string $base_url
   *   The base URL for the library files.
   */
  public function __construct(ClientInterface $http_client, SerializationInterface $serializer, $base_url) {
    parent::__construct($serializer, $base_url);
    $this->httpClient = $http_client;
  }

  /**
   * {@inheritdoc}
   */
  public function hasDefinition($id) {
    try {
      $response = $this->httpClient->request('GET', $this->getFileUri($id));
      return $response->getStatusCode() === 200;
    }
    catch (GuzzleException $exception) {
      return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function getSerializedDefinition($id) {
    try {
      $response = $this->httpClient->request('GET', $this->getFileUri($id));
      return (string) $response->getBody();
    }
    catch (GuzzleException $exception) {
      throw new LibraryDefinitionNotFoundException($id, '', 0, $exception);
    }
    catch (\RuntimeException $exception) {
      throw new LibraryDefinitionNotFoundException($id, '', 0, $exception);
    }
  }

}
