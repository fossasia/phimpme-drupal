<?php

namespace Drupal\libraries\Tests;

use Drupal\simpletest\KernelTestBase;

/**
 * Tests basic Libraries API functions.
 *
 * @group libraries
 */
class LibrariesUnitTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  public static $modules = array('libraries');

  /**
   * Tests libraries_get_path().
   */
  function testLibrariesGetPath() {
    // Note that, even though libraries_get_path() doesn't find the 'example'
    // library, we are able to make it 'installed' by specifying the 'library
    // path' up-front. This is only used for testing purposed and is strongly
    // discouraged as it defeats the purpose of Libraries API in the first
    // place.
    $this->assertEqual(libraries_get_path('example'), FALSE, 'libraries_get_path() returns FALSE for a missing library.');
  }

  /**
   * Tests libraries_prepare_files().
   */
  function testLibrariesPrepareFiles() {
    $expected = array(
      'files' => array(
        'js' => array('example.js' => array()),
        'css' => array('example.css' => array()),
        'php' => array('example.php' => array()),
      ),
    );
    $library = array(
      'files' => array(
        'js' => array('example.js'),
        'css' => array('example.css'),
        'php' => array('example.php'),
      ),
    );
    libraries_prepare_files($library, NULL, NULL);
    $this->assertEqual($expected, $library, 'libraries_prepare_files() works correctly.');
  }

}
