<?php

namespace Drupal\services\Controller;

use Drupal\Core\Cache\CacheableResponse;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * Class \Drupal\services\Controller\Services.
 */
class Services extends ControllerBase {

  /**
   * Serializer service.
   *
   * @var \Symfony\Component\Serializer\SerializerInterface
   */
  protected $serializer;

  /**
   * Constructor for \Drupal\services\Controller\Services.
   */
  public function __construct(SerializerInterface $serializer) {
    $this->serializer = $serializer;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('serializer')
    );
  }

  /**
   * Processing the service API request.
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, $service_endpoint_id, $service_definition_id) {
    /* @var $service_endpoint \Drupal\services\ServiceEndpointInterface */
    $service_endpoint = $this->entityManager()->getStorage('service_endpoint')->load($service_endpoint_id);
    $service_resource = $service_endpoint->loadResourceProvider($service_definition_id);

    // TODO - pull in settings from service API and alter response.
    /* @var $service_def \Drupal\services\ServiceDefinitionInterface */
    $service_def = $service_resource->createServicePluginInstance([]);

    /*
     * Iterate over any contexts defined for this plugin and extract them from
     * the request defaults if the naming is identical. This means that a
     * context named 'node' would match to a url parameter {node} or a route
     * default named 'node'.
     */
    foreach ($service_def->getContextDefinitions() as $context_id => $context_definition) {
      if ($request->attributes->has($context_id)) {
        $context = new Context($context_definition, $request->attributes->get($context_id));
        $service_def->setContext($context_id, $context);
      }
    }

    // Get the data from the plugin.
    $data = $service_def->processRequest($request, $route_match, $this->serializer);
    $code = $service_def->getPluginDefinition()['response_code'];
    $headers = [];
    $messages = drupal_get_messages();
    if ($messages) {
      foreach ($messages as $type => $type_message) {
        $headers["X-Drupal-Services-Messages-$type"] = implode('; ', $type_message);
      }
    }
    // Find the request format to determin how we're going to serialize this
    // data.
    $format = $request->getRequestFormat();
    $data = $this->serializer->serialize($data, $format);
    /*
     * Create a new Cacheable Response object with our serialized data, set its
     * Content-Type to match the format of our Request and add the service
     * definition plugin as a cacheable dependency.
     *
     * This last step will extract the cache context, tags and max-ages from
     * any context the plugin required to operate.
     */
    $response = new CacheableResponse($data, $code, $headers);
    $response->headers->add(['Content-Type' => $request->getMimeType($format)]);
    $response->addCacheableDependency($service_def);
    // Be explicit about the caching needs of this response.
    $response->setVary('Accept');
    $service_def->processResponse($response);

    return $response;
  }

  /**
   * Generate a CSRF session token.
   *
   * @return \Symfony\Component\HttpFoundation\Response.
   *   A HTTP response object.
   */
  public function csrfToken() {
    return new Response(
      \Drupal::csrfToken()->get('services'), 200, ['Content-Type' => 'text/plain']
    );
  }

}
