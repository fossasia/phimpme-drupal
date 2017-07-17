<?php

namespace Drupal\Tests\libraries\Kernel\ExternalLibrary;

use Drupal\Tests\libraries\Kernel\ExternalLibrary\TestLibraryFilesStream;
use Drupal\Tests\libraries\Kernel\LibraryTypeKernelTestBase;

/**
 * Tests that a global locator can be properly used to load a libraries.
 *
 * @group libraries
 */
class GlobalLocatorTest extends LibraryTypeKernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    // Assign our test stream (which points to the test php lib) to the asset
    // scheme. This gives us a scheme to work with in the test that is not
    // used to locate a php lib by default.
    $this->container->set('stream_wrapper.asset_libraries', new TestLibraryFilesStream(
      $this->container->get('module_handler'),
      $this->container->get('string_translation'),
      'libraries'
    ));
  }

  /**
   * {@inheritdoc}
   */
  protected function getLibraryTypeId() {
    return 'php_file';
  }

  /**
   * Tests that the library is located via the global loactor.
   */
  public function testGlobalLocator() {
    // By default the library will not be locatable (control assertion) until we
    // add the asset stream to the global loctors conf list.
    $library = $this->getLibrary();
    $this->assertFalse($library->isInstalled());
    $config_factory = $this->container->get('config.factory');
    $config_factory->getEditable('libraries.settings')
      ->set('global_locators', [['id' => 'uri', 'configuration' => ['uri' => 'asset://']]])
      ->save();
    $library = $this->getLibrary();
    $this->assertTrue($library->isInstalled());
  }

}
