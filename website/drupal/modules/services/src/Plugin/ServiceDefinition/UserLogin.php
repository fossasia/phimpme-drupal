<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Flood\FloodInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Drupal\user\UserAuthInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Session\Session;
use Symfony\Component\HttpKernel\Exception\HttpException;
use Symfony\Component\Routing\Route;
use Symfony\Component\Serializer\Encoder\DecoderInterface;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "user_login",
 *   methods = {
 *     "POST"
 *   },
 *   title = @Translation("User login"),
 *   description = @Translation("Allows users to login."),
 *   category = @Translation("User"),
 *   path = "user/login"
 * )
 */
class UserLogin extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * Constructs a HTTP basic authentication provider object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   * @param \Drupal\user\UserAuthInterface $user_auth
   *   The user authentication service.
   * @param \Drupal\Core\Flood\FloodInterface $flood
   *   The flood service.
   * @param \Drupal\Core\Entity\EntityManagerInterface $entity_manager
   *   The entity manager service.
   * @param Session $session
   */
  public function __construct($configuration, $plugin_id, $plugin_definition, ConfigFactoryInterface $config_factory, UserAuthInterface $user_auth, FloodInterface $flood, EntityManagerInterface $entity_manager, Session $session) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->configFactory = $config_factory;
    $this->userAuth = $user_auth;
    $this->flood = $flood;
    $this->entityManager = $entity_manager;
    $this->session = $session;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('config.factory'),
      $container->get('user.auth'),
      $container->get('flood'),
      $container->get('entity.manager'),
      $container->get('session')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_user_is_logged_in', 'FALSE');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    if ($serializer instanceof DecoderInterface) {
      $content = $serializer->decode($request->getContent(), $request->getContentType());
    }
    else {
      throw new HttpException(500, $this->t('The appropriate DecoderInterface was not found.'));
    }
    if (!isset($content)) {
      throw new HttpException(500, $this->t('The content of the request was empty.'));
    }
    $flood_config = $this->configFactory->get('user.flood');
    $username = $content['username'];
    $password = $content['password'];
    // Flood protection: this is very similar to the user login form code.
    // @see \Drupal\user\Form\UserLoginForm::validateAuthentication()
    // Do not allow any login from the current user's IP if the limit has been
    // reached. Default is 50 failed attempts allowed in one hour. This is
    // independent of the per-user limit to catch attempts from one IP to log
    // in to many different user accounts.  We have a reasonably high limit
    // since there may be only one apparent IP for all users at an institution.
    if ($this->flood->isAllowed('services.failed_login_ip', $flood_config->get('ip_limit'), $flood_config->get('ip_window'))) {
      $accounts = $this->entityManager->getStorage('user')->loadByProperties(array('name' => $username, 'status' => 1));
      $account = reset($accounts);
      if ($account) {
        if ($flood_config->get('uid_only')) {
          // Register flood events based on the uid only, so they apply for any
          // IP address. This is the most secure option.
          $identifier = $account->id();
        }
        else {
          // The default identifier is a combination of uid and IP address. This
          // is less secure but more resistant to denial-of-service attacks that
          // could lock out all users with public user names.
          $identifier = $account->id() . '-' . $request->getClientIP();
        }
        // Don't allow login if the limit for this user has been reached.
        // Default is to allow 5 failed attempts every 6 hours.
        if ($this->flood->isAllowed('services.failed_login_user', $flood_config->get('user_limit'), $flood_config->get('user_window'), $identifier)) {
          $uid = $this->userAuth->authenticate($username, $password);
          if ($uid) {
            $this->flood->clear('services.failed_login_user', $identifier);
            $this->session->start();
            user_login_finalize($account);
            drupal_set_message(t('User successfully logged in'), 'status', FALSE);

            return [
              'id' => $this->session->getId(),
              'name' => $this->session->getName(),
            ];
            // Return $this->entityManager->getStorage('user')->load($uid);
          }
          else {
            // Register a per-user failed login event.
            $this->flood->register('services.failed_login_user', $flood_config->get('user_window'), $identifier);
          }
        }
      }
    }
    // Always register an IP-based failed login event.
    $this->flood->register('services.failed_login_ip', $flood_config->get('ip_window'));

    return [];
  }

}
