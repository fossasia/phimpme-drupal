<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Component\Utility\SafeMarkup;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;

/**
 * Enforces a number of a type of character in passwords.
 *
 * @ServiceDefinition(
 *   id = "test_service_definition",
 *   title = @Translation("Testing Service Definition"),
 *   description = @Translation("Provided to test basic service provider definition."),
 *   translatable = true,
 *   arguments = {
 *     "method" = @ServiceArgument(
 *       "id" = "test_method",
 *       "title" = @Translation("Method of test service"),
 *       "required" = TRUE,
 *       "error_message" = @Translation("No method was sent to the request"),
 *     ),
 *     "uri" = @ServiceArgument(
 *       "id" = "test_uri",
 *       "title" = @Translation("URI of test service"),
 *       "required" = TRUE,
 *       "error_message" = @Translation("No URI was sent to the request"),
 *     ),
 *   }
 * )
 */
class TestServiceDefinition extends ServiceDefinitionBase {

  /**
   * Testing hello world style request.
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match) {
    return SafeMarkup::escape($request->getMethod() . ' - ' . $request->getUri());
  }

}
