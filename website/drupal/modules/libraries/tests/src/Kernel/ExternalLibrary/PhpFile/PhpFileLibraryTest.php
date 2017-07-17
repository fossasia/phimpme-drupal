<?php

namespace Drupal\Tests\libraries\Kernel\ExternalLibrary\PhpFile;

use Drupal\Tests\libraries\Kernel\ExternalLibrary\TestLibraryFilesStream;
use Drupal\Tests\libraries\Kernel\LibraryTypeKernelTestBase;

/**
 * Tests that the external library manager properly loads PHP file libraries.
 *
 * @group libraries
 */
class PhpFileLibraryTest extends LibraryTypeKernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $this->container->set('stream_wrapper.php_file_libraries', new TestLibraryFilesStream(
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
   * Tests that the list of PHP files is correctly gathered.
   */
  public function testPhpFileInfo() {
    /** @var \Drupal\libraries\ExternalLibrary\PhpFile\PhpFileLibrary $library */
    $library = $this->getLibrary();
    $this->assertTrue($library->isInstalled());
    $library_path = $this->modulePath . '/tests/libraries/test_php_file_library';
    $this->assertEquals($library_path, $library->getLocalPath());
    $this->assertEquals(["$library_path/test_php_file_library.php"], $library->getPhpFiles());
  }

  /**
   * Tests that the external library manager properly loads PHP files.
   *
   * @see \Drupal\libraries\ExternalLibrary\ExternalLibraryManager
   * @see \Drupal\libraries\ExternalLibrary\ExternalLibraryTrait
   * @see \Drupal\libraries\ExternalLibrary\PhpFile\PhpRequireLoader
   */
  public function testFileLoading() {
    $function_name = '_libraries_test_php_function';
    if (function_exists($function_name)) {
      $this->markTestSkipped('Cannot test file inclusion if the file to be included has already been included prior.');
      return;
    }

    $this->assertFalse(function_exists($function_name));
    $this->libraryManager->load('test_php_file_library');
    $this->assertTrue(function_exists($function_name));
  }

}
