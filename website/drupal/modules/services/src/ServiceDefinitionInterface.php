<?php

namespace Drupal\services;

use Drupal\Core\Cache\CacheableDependencyInterface;
use Drupal\Core\Plugin\ContextAwarePluginInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Serializer\SerializerInterface;
use Symfony\Component\Routing\Route;

/**
 * Interface \Drupal\services\ServiceDefinitionInterface.
 */
interface ServiceDefinitionInterface extends ContextAwarePluginInterface, CacheableDependencyInterface {

  /**
   * Returns a translated string for the service title.
   *
   * @return string
   */
  public function getTitle();

  /**
   * Returns a translated string for the category.
   *
   * @return string
   */
  public function getCategory();

  /**
   * Returns the appended path for the service.
   *
   * @return string
   */
  public function getPath();

  /**
   * Returns a translated description for the constraint description.
   *
   * @return string
   */
  public function getDescription();

  /**
   * Return an array of allowed methods.
   *
   * @return array
   */
  public function getMethods();

  /**
   * Returns an array of service request arguments.
   *
   * @return array
   */
  public function getArguments();

  /**
   * Returns a boolean if this service definition supports translations.
   *
   * @return bool
   */
  public function supportsTranslation();

  /**
   * Checks access for the ServiceDefintion.
   *
   * @param \Symfony\Component\Routing\Route $route
   *   The route match object.
   */
  public function processRoute(Route $route);

  /**
   * Processes the request and returns an array of data as appropriate.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The request object.
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *   The route match object.
   * @param \Symfony\Component\Serializer\SerializerInterface $serializer
   *   The serializer. Some methods might require the plugin to leverage the
   *   serializer after extracting the request contents.
   *
   * @return array
   *   The response.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\HttpException
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer);

  /**
   * Allow plugins to alter the response object before it is returned.
   *
   * @param Response $response
   *   The response object that is about to be returned.
   */
  public function processResponse(Response $response);

}
