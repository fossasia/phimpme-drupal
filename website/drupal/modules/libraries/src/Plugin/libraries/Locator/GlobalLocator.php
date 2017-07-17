<?php

namespace Drupal\libraries\Plugin\libraries\Locator;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Local\LocatorInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a locator based on global configuration.
 *
 * @Locator("global")
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
 */
class GlobalLocator implements LocatorInterface, ContainerFactoryPluginInterface {

  /**
   * The Drupal config factory service.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The locator factory.
   *
   * @var \Drupal\Component\Plugin\Factory\FactoryInterface
   */
  protected $locatorFactory;

  /**
   * Constructs a global locator.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The Drupal config factory service.
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $locator_factory
   *   The locator factory.
   */
  public function __construct(ConfigFactoryInterface $config_factory, FactoryInterface $locator_factory) {
    $this->configFactory = $config_factory;
    $this->locatorFactory = $locator_factory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $container->get('config.factory'),
      $container->get('plugin.manager.libraries.locator')
    );
  }

  /**
   * Locates a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface $library
   *   The library to locate.
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface::locate()
   */
  public function locate(LocalLibraryInterface $library) {
    foreach ($this->configFactory->get('libraries.settings')->get('global_locators') as $locator) {
      $this->locatorFactory->createInstance($locator['id'], $locator['configuration'])->locate($library);
      if ($library->isInstalled()) {
        return;
      }
    }
    $library->setUninstalled();
  }

}
