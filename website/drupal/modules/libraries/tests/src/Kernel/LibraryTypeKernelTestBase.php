<?php

namespace Drupal\Tests\libraries\Kernel;

use Drupal\Component\Plugin\Exception\PluginException;
use Drupal\KernelTests\KernelTestBase;
use Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException;
use Drupal\libraries\ExternalLibrary\Exception\LibraryTypeNotFoundException;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface;

/**
 * Provides an improved version of the core kernel test base class.
 */
abstract class LibraryTypeKernelTestBase extends KernelTestBase {

  /**
   * The external library manager.
   *
   * @var \Drupal\libraries\ExternalLibrary\LibraryManagerInterface
   */
  protected $libraryManager;

  /**
   * The library type factory.
   *
   * @var \Drupal\Component\Plugin\Factory\FactoryInterface
   */
  protected $libraryTypeFactory;

  /**
   * The absolute path to the Libraries API module.
   *
   * @var string
   */
  protected $modulePath;

  /**
   * {@inheritdoc}
   */
  public static $modules = ['libraries', 'libraries_test'];

  /**
   * Gets the ID of the library type that is being tested.
   *
   * @return string
   */
  abstract protected function getLibraryTypeId();

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    /** @var \Drupal\Core\Extension\ModuleHandlerInterface $module_handler */
    $module_handler = $this->container->get('module_handler');
    $this->modulePath = $module_handler->getModule('libraries')->getPath();

    $this->installConfig('libraries');
    // Disable remote definition fetching and set the local definitions path to
    // the module directory.
    /** @var \Drupal\Core\Config\ConfigFactoryInterface $config_factory */
    $config_factory = $this->container->get('config.factory');
    $config_factory->getEditable('libraries.settings')
      ->set('definition.local.path', "{$this->modulePath}/tests/library_definitions")
      ->set('definition.remote.enable', FALSE)
      ->save();

    // LibrariesConfigSubscriber::onConfigSave() invalidates the container so
    // that it is rebuilt on the next request. We need the container rebuilt
    // immediately, however.
    /** @var \Drupal\Core\DrupalKernelInterface $kernel */
    $kernel = $this->container->get('kernel');
    $this->container = $kernel->rebuildContainer();

    $this->libraryManager = $this->container->get('libraries.manager');
    $this->libraryTypeFactory = $this->container->get('plugin.manager.libraries.library_type');
  }

  /**
   * Tests that the library type can be instantiated.
   */
  public function testLibraryType() {
    $type_id = $this->getLibraryTypeId();
    try {
      $this->libraryTypeFactory->createInstance($type_id);
      $this->assertTrue(TRUE, "Library type '$type_id' can be instantiated.");
    }
    catch (PluginException $exception) {
      $this->fail("Library type '$type_id' cannot be instantiated.");
    }
  }

  /**
   * Tests that the test library can be instantiated.
   */
  public function testLibrary() {
    $type_id = $this->getLibraryTypeId();
    $id = $this->getLibraryId();
    try {
      $library = $this->libraryManager->getLibrary($id);
      $this->assertTrue(TRUE, "Test $type_id library can be instantiated.");
      $this->assertInstanceOf($this->getLibraryType()->getLibraryClass(), $library);
      $this->assertEquals($this->getLibraryId(), $library->getId());

    }
    catch (LibraryDefinitionNotFoundException $exception) {
      $this->fail("Missing library definition for test $type_id library.");
    }
    catch (LibraryTypeNotFoundException $exception) {
      $this->fail("Missing library type declaration for test $type_id library.");
    }
  }

  /**
   * Returns the library type that is being tested.
   *
   * @return \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface
   *   The test library type.
   */
  protected function getLibraryType() {
    try {
      $library_type = $this->libraryTypeFactory->createInstance($this->getLibraryTypeId());
    }
    catch (PluginException $exception) {
      $library_type = $this->prophesize(LibraryTypeInterface::class)->reveal();
    }
    finally {
      return $library_type;
    }
  }

  /**
   * Retuns the library ID of the library used in the test.
   *
   * Defaults to 'test_[library_type]_library', where [library_type] is the
   * ID of the library type being tested.
   *
   * @return string
   */
  protected function getLibraryId() {
    $type_id = $this->getLibraryTypeId();
    return "test_{$type_id}_library";
  }

  /**
   * Returns the test library for this library type.
   *
   * @return \Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The test library.
   */
  protected function getLibrary() {
    try {
      $library = $this->libraryManager->getLibrary($this->getLibraryId());
    }
    catch (LibraryDefinitionNotFoundException $exception) {
      $library = $this->prophesize(LibraryInterface::class)->reveal();
    }
    catch (LibraryTypeNotFoundException $exception) {
      $library = $this->prophesize(LibraryInterface::class)->reveal();
    }
    catch (PluginException $exception) {
      $library = $this->prophesize(LibraryInterface::class)->reveal();
    }
    finally {
      return $library;
    }
  }

}
