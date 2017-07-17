<?php

namespace Drupal\libraries\Config;

use Drupal\Core\Config\ConfigCrudEvent;
use Drupal\Core\Config\ConfigEvents;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;

/**
 * Reacts to configuration changes of the 'libraries.settings' configuration.
 */
class LibrariesConfigSubscriber implements EventSubscriberInterface {

  /**
   * The service container.
   *
   * @var \Symfony\Component\DependencyInjection\ContainerInterface
   */
  protected $container;

  /**
   * Constructs a Libraries API configuration subscriber.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The service container.
   */
  public function __construct(ContainerInterface $container) {
    $this->container = $container;
  }

  /**
   * Unsets the definition discovery service when its configuration changes.
   *
   * @param \Drupal\Core\Config\ConfigCrudEvent $event
   *   The configuration event.
   */
  public function onConfigSave(ConfigCrudEvent $event) {
    if (($event->getConfig()->getName() === 'libraries.settings') && $event->isChanged('definition')) {
      $this->container->set('libraries.definition.discovery', NULL);
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function getSubscribedEvents() {
    return [ConfigEvents::SAVE => 'onConfigSave'];
  }

}
