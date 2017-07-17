<?php

namespace Drupal\services;

use Drupal\Core\Plugin\ContextAwarePluginBase;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Routing\Route;

/**
 * Class \Drupal\services\ServiceDefinitionBase.
 */
abstract class ServiceDefinitionBase extends ContextAwarePluginBase implements ServiceDefinitionInterface {

  /**
   * {@inheritdoc}
   */
  public function getTitle() {
    return $this->pluginDefinition['title'];
  }

  /**
   * {@inheritdoc}
   */
  public function getCategory() {
    return $this->pluginDefinition['category'];
  }

  /**
   * {@inheritdoc}
   */
  public function getPath() {
    return $this->pluginDefinition['path'];
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->pluginDefinition['description'];
  }

  /**
   * {@inheritdoc}
   */
  public function supportsTranslation() {
    return $this->pluginDefinition['translatable'];
  }

  /**
   * {@inheritdoc}
   */
  public function getMethods() {
    return $this->pluginDefinition['methods'];
  }

  /**
   * {@inheritdoc}
   */
  public function getArguments() {
    return $this->pluginDefinition['arguments'];
  }

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->addRequirements(array('_access' => 'TRUE'));
  }

  /**
   * {@inheritdoc}
   */
  public function processResponse(Response $response) {}

}
