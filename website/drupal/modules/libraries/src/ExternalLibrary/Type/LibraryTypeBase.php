<?php

namespace Drupal\libraries\ExternalLibrary\Type;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Utility\IdAccessorTrait;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a base class for library types.
 */
abstract class LibraryTypeBase implements
  LibraryTypeInterface,
  LibraryCreationListenerInterface,
  ContainerFactoryPluginInterface
{

  use IdAccessorTrait;

  /**
   * The locator factory.
   *
   * @var \Drupal\Component\Plugin\Factory\FactoryInterface
   */
  protected $locatorFactory;

  /**
   * The version detector factory.
   *
   * @var \Drupal\Component\Plugin\Factory\FactoryInterface
   */
  protected $detectorFactory;

  /**
   * Constructs the asset library type.
   *
   * @param string $plugin_id
   *   The plugin ID taken from the class annotation.
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $locator_factory
   *   The locator factory.
   * @param \Drupal\Component\Plugin\Factory\FactoryInterface $detector_factory
   *   The version detector factory.
   */
  public function __construct($plugin_id, FactoryInterface $locator_factory, FactoryInterface $detector_factory) {
    $this->id = $plugin_id;
    $this->locatorFactory = $locator_factory;
    $this->detectorFactory = $detector_factory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $plugin_id,
      $container->get('plugin.manager.libraries.locator'),
      $container->get('plugin.manager.libraries.version_detector')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function onLibraryCreate(LibraryInterface $library) {
    if ($library instanceof LocalLibraryInterface) {
      $library->getLocator($this->locatorFactory)->locate($library);
      // Fallback on global locators.
      // @todo Consider if global locators should be checked as a fallback or as
      // the primary locator source.
      if (!$library->isInstalled()) {
        $this->locatorFactory->createInstance('global')->locate($library);
      }
      // Also fetch version information.
      if ($library instanceof VersionedLibraryInterface) {
        // @todo Consider if this should be wrapped in some conditional logic
        // or exception handling so that version detection errors do not
        // prevent a library from being loaded.
        $library->getVersionDetector($this->detectorFactory)->detectVersion($library);
      }
    }
  }

}
