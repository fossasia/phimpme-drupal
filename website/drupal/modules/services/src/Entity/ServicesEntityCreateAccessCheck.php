<?php
/**
 * @file
 * Contains Drupal\services\Entity\EntityAccessCheck
 */
namespace Drupal\services\Entity;

use Drupal\Core\Access\AccessResult;
use Drupal\Core\Routing\Access\AccessInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\Session\AccountInterface;
use Symfony\Component\Routing\Route;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Entity\EntityAccessCheck;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * Defines an access checker for entities in services endpoint.
 */
class ServicesEntityCreateAccessCheck implements AccessInterface {

  /**
   * The entity manager.
   *
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $entityManager;

  protected $request;

  protected $serializer;

  /**
   * The key used by the routing requirement.
   *
   * @var string
   */
  protected $requirementsKey = '_services_entity_access_create';

  /**
   * Constructs a EntityCreateAccessCheck object.
   *
   * @param \Drupal\Core\Entity\EntityManagerInterface $entity_manager
   *   The entity manager.
   */
  public function __construct(EntityManagerInterface $entity_manager, SerializerInterface $serializer) {
    $this->entityManager = $entity_manager;
    $this->request = \Drupal::request();
    $this->serializer = $serializer;
  }

  /**
   * Checks access to create the entity type and bundle for the given route.
   *
   * @param \Symfony\Component\Routing\Route $route
   *   The route to check against.
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *   The parametrized route.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The currently logged in account.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The access result.
   */
  public function access(Route $route, RouteMatchInterface $route_match, AccountInterface $account) {
    $entity_type_id = $route->getRequirement($this->requirementsKey);
    $entity_type = $this->entityManager->getDefinition($entity_type_id);
    $format = $this->request->getContentType();
    $content = $this->request->getContent();
    $content_decoded = $this->serializer->decode($content, $format);
    $entity = \Drupal::entityTypeManager()->getStorage($entity_type_id)->create($content_decoded);
    $bundle_value = $entity->bundle();
    $bundle = is_array($bundle_value) ? reset(call_user_func_array('array_merge', $bundle_value)) : $bundle_value;

    // The bundle argument can contain request argument placeholders like
    // {name}, loop over the raw variables and attempt to replace them in the
    // bundle name. If a placeholder does not exist, it won't get replaced.
    if ($bundle && strpos($bundle, '{') !== FALSE) {
      foreach ($route_match->getRawParameters()->all() as $name => $value) {
        $bundle = str_replace('{' . $name . '}', $value, $bundle);
      }
      // If we were unable to replace all placeholders, deny access.
      if (strpos($bundle, '{') !== FALSE) {
        return AccessResult::neutral();
      }
    }
    return $this->entityManager->getAccessControlHandler($entity_type_id)->createAccess($bundle, $account, [], TRUE);
  }

}
