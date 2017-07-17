<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Routing\Route;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "user_logout",
 *   methods = {
 *     "POST"
 *   },
 *   title = @Translation("User logout"),
 *   description = @Translation("Allows users to logout."),
 *   category = @Translation("User"),
 *   path = "user/logout"
 * )
 */
class UserLogout extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_user_is_logged_in', 'TRUE');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    user_logout();
    drupal_set_message(t('User successfully logged out'), 'status', FALSE);

    return [];
  }

}
