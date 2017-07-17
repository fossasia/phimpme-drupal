<?php

namespace Drupal\services\Routing;

use Symfony\Component\Routing\Route;

/**
 * Class \Drupal\services\Entity\ServiceEndpoint.
 */
class ServiceEndpoint {

  /**
   * {@inheritdoc}
   *
   * @todo does this implement some interface that we're not documenting?
   */
  public function routes() {
    $routes = array();

    foreach (\Drupal::entityManager()->getStorage('service_endpoint')->loadMultiple() as $endpoint) {
      foreach ($endpoint->loadResourceProviders() as $resource) {

        $instance = $resource->createServicePluginInstance();
        $parameters = [];

        // Build an array of parameter to pass to the Route definitions.
        foreach ($instance->getContextDefinitions() as $context_id => $context) {
          $parameters[$context_id] = [
            'type' => $context->getDataType(),
          ];
        }

        // Dynamically building custom routes per enabled plugin on an endpoint
        // entity.
        $route = (new Route('/' . $endpoint->getEndpoint() . '/' . $instance->getPath()))
          ->setDefaults([
            '_controller' => '\Drupal\services\Controller\Services::processRequest',
            'service_endpoint_id' => $endpoint->id(),
            'service_definition_id' => $instance->getPluginId(),
          ])
          ->setOptions([
            'parameters' => $parameters,
            '_auth' => $resource->getAuthentication(),
          ])
          ->setMethods($instance->getMethods());

        if ($formats = $resource->getFormats()) {
          $route->setRequirement('_format', implode('|', array_keys($formats)));
        }

        if($resource->getNoCache()) {
          $route->setOption('no_cache', true);
        }

        // Enable CSRF tokens for restricted HTTP methods.
        $route->setRequirement('_check_services_csrf', 'TRUE');

        $instance->processRoute($route);

        $routes['services.endpoint.' . $resource->id()] = $route;
      }
    }

    return $routes;
  }

}
