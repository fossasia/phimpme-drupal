<?php

namespace Drupal\Tests\libraries\Kernel\ExternalLibrary\Asset;

use Drupal\Tests\libraries\Kernel\ExternalLibrary\TestLibraryFilesStream;

/**
 * Tests that external asset libraries can register multiple core libraries.
 *
 * @group libraries
 */
class MultipleAssetLibraryTest extends AssetLibraryTestBase {

  /**
   * {@inheritdoc}
   */
  protected function getLibraryTypeId() {
    return 'asset_multiple';
  }

  /**
   * Tests that attachable asset library info is correctly gathered.
   */
  public function testAttachableAssetInfo() {
    /** @var \Drupal\libraries\ExternalLibrary\Asset\AttachableAssetLibraryRegistrationInterface $library_type */
    $library_type = $this->getLibraryType();
    $library = $this->getLibrary();
    $expected = [
      'test_asset_multiple_library.first' => [
        'version' => '1.0.0',
        'css' => ['base' => ['http://example.com/example.first.css' => []]],
        'js' => ['http://example.com/example.first.js' => []],
        'dependencies' => [],
      ],
      'test_asset_multiple_library.second' => [
        'version' => '1.0.0',
        'css' => ['base' => ['http://example.com/example.second.css' => []]],
        'js' => ['http://example.com/example.second.js' => []],
        'dependencies' => [],
      ],
    ];
    $this->assertEquals($expected, $library_type->getAttachableAssetLibraries($library, $this->libraryManager));
  }

  /**
   * Tests that a remote asset library is registered as a core asset library.
   *
   * @see \Drupal\libraries\Extension\Extension
   * @see \Drupal\libraries\Extension\ExtensionHandler
   * @see \Drupal\libraries\ExternalLibrary\Asset\AssetLibrary
   * @see \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryTrait
   * @see \Drupal\libraries\ExternalLibrary\ExternalLibraryManager
   * @see \Drupal\libraries\ExternalLibrary\ExternalLibraryTrait
   * @see \Drupal\libraries\ExternalLibrary\Registry\ExternalLibraryRegistry
   */
  public function testAssetLibraryRemote() {
    $library = $this->coreLibraryDiscovery->getLibraryByName('libraries', 'test_asset_multiple_library.first');
    $expected = [
      'version' => '1.0.0',
      'css' => [[
        'weight' => -200,
        'group' => 0,
        'type' => 'external',
        'data' => 'http://example.com/example.first.css',
        'version' => '1.0.0',
      ]],
      'js' => [[
        'group' => -100,
        'type' => 'external',
        'data' => 'http://example.com/example.first.js',
        'version' => '1.0.0',
      ]],
      'dependencies' => [],
      'license' => [
        'name' => 'GNU-GPL-2.0-or-later',
        'url' => 'https://www.drupal.org/licensing/faq',
        'gpl-compatible' => TRUE,
      ]
    ];
    $this->assertEquals($expected, $library);

    $library = $this->coreLibraryDiscovery->getLibraryByName('libraries', 'test_asset_multiple_library.second');
    $expected = [
      'version' => '1.0.0',
      'css' => [[
        'weight' => -200,
        'group' => 0,
        'type' => 'external',
        'data' => 'http://example.com/example.second.css',
        'version' => '1.0.0',
      ]],
      'js' => [[
        'group' => -100,
        'type' => 'external',
        'data' => 'http://example.com/example.second.js',
        'version' => '1.0.0',
      ]],
      'dependencies' => [],
      'license' => [
        'name' => 'GNU-GPL-2.0-or-later',
        'url' => 'https://www.drupal.org/licensing/faq',
        'gpl-compatible' => TRUE,
      ]
    ];
    $this->assertEquals($expected, $library);
  }

  /**
   * Tests that a local asset library is registered as a core asset library.
   */
  public function testAssetLibraryLocal() {
    $this->container->set('stream_wrapper.asset_libraries', new TestLibraryFilesStream(
      $this->container->get('module_handler'),
      $this->container->get('string_translation'),
      'assets/vendor'
    ));
    $this->coreLibraryDiscovery->clearCachedDefinitions();

    $library = $this->coreLibraryDiscovery->getLibraryByName('libraries', 'test_asset_multiple_library.first');
    $expected = [
      'version' => '1.0.0',
      'css' => [[
        'weight' => -200,
        'group' => 0,
        'type' => 'file',
        'data' => $this->modulePath . '/tests/assets/vendor/test_asset_multiple_library/example.first.css',
        'version' => '1.0.0',
      ]],
      'js' => [[
        'group' => -100,
        'type' => 'file',
        'data' => $this->modulePath . '/tests/assets/vendor/test_asset_multiple_library/example.first.js',
        'version' => '1.0.0',
        'minified' => FALSE,
      ]],
      'dependencies' => [],
      'license' => [
        'name' => 'GNU-GPL-2.0-or-later',
        'url' => 'https://www.drupal.org/licensing/faq',
        'gpl-compatible' => TRUE,
      ]
    ];
    $this->assertEquals($expected, $library);

    $library = $this->coreLibraryDiscovery->getLibraryByName('libraries', 'test_asset_multiple_library.second');
    $expected = [
      'version' => '1.0.0',
      'css' => [[
        'weight' => -200,
        'group' => 0,
        'type' => 'file',
        'data' => $this->modulePath . '/tests/assets/vendor/test_asset_multiple_library/example.second.css',
        'version' => '1.0.0',
      ]],
      'js' => [[
        'group' => -100,
        'type' => 'file',
        'data' => $this->modulePath . '/tests/assets/vendor/test_asset_multiple_library/example.second.js',
        'version' => '1.0.0',
        'minified' => FALSE,
      ]],
      'dependencies' => [],
      'license' => [
        'name' => 'GNU-GPL-2.0-or-later',
        'url' => 'https://www.drupal.org/licensing/faq',
        'gpl-compatible' => TRUE,
      ]
    ];
    $this->assertEquals($expected, $library);
  }

}
