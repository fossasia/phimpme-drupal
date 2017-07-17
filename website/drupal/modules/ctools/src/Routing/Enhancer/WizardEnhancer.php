<?php

namespace Drupal\ctools\Routing\Enhancer;

use Drupal\Core\Routing\Enhancer\RouteEnhancerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Routing\Route;

/**
 * Sets the request format onto the request object.
 */
class WizardEnhancer implements RouteEnhancerInterface {

  /**
   * {@inheritdoc}
   */
  public function applies(Route $route) {
    return !$route->hasDefault('_controller') && ($route->hasDefault('_wizard') || $route->hasDefault('_entity_wizard'));
  }

  /**
   * {@inheritdoc}
   */
  public function enhance(array $defaults, Request $request) {
    if (!empty($defaults['_wizard'])) {
      $defaults['_controller'] = 'ctools.wizard.form:getContentResult';
    }
    if (!empty($defaults['_entity_wizard'])) {
      $defaults['_controller'] = 'ctools.wizard.entity.form:getContentResult';
    }
    return $defaults;
  }

}
