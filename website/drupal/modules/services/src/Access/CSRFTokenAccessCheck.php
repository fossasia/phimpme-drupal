<?php

namespace Drupal\services\Access;

use Drupal\Core\Access\AccessCheckInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Session\SessionConfigurationInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Routing\Route;

/**
 * Class \Drupal\services\Access\CSRFTokenAccessCheck.
 */
class CSRFTokenAccessCheck implements AccessCheckInterface {

  /**
   * The session configuration.
   *
   * @var \Drupal\Core\Session\SessionConfigurationInterface
   */
  protected $sessionConfiguration;

  /**
   * Constructor for \Drupal\services\Access\CSRFTokenAccessCheck.
   */
  public function __construct(SessionConfigurationInterface $session_configuration) {
    $this->sessionConfiguration = $session_configuration;
  }

  /**
   * {@inheritdoc}
   */
  public function applies(Route $route) {
    $requirements = $route->getRequirements();

    if (!isset($requirements['_check_services_csrf'])) {
      return FALSE;
    }

    return $this->hasRestrictedMethod($route->getMethods());
  }

  /**
   * {@inheritdoc}
   */
  public function access(Request $request, AccountInterface $account) {
    if ($account->isAuthenticated()
      && in_array($request->getMethod(), $this->restrictedMethods())
      && $this->sessionConfiguration->hasSession($request)
      ) {
      $csrf_token = $request->headers->get('X-CSRF-Token');

      if (!\Drupal::csrfToken()->validate($csrf_token, 'services')) {
        return AccessResult::forbidden('CSRF validation failed')
          ->setCacheMaxAge(0);
      }
    }

    return AccessResult::allowed()->setCacheMaxAge(0);
  }

  /**
   * Define restricted methods that need a CSRF token.
   *
   * @return array
   *   An array of restricted methods.
   */
  protected function restrictedMethods() {
    return ['PUT', 'POST'];
  }

  /**
   * Determine if the methods are restricted.
   *
   * @param array $methods
   *   An array of HTTP methods.
   *
   * @return bool
   *   Return TRUE if a restricted method was found; otherwise FALSE.
   */
  protected function hasRestrictedMethod(array $methods) {
    foreach ($methods as $method) {
      if (in_array(strtoupper($method), $this->restrictedMethods())) {
        return TRUE;
      }
    }

    return FALSE;
  }

}
