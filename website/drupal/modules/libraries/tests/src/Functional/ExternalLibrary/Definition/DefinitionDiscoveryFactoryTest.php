<?php

namespace Drupal\Tests\libraries\Functional\ExternalLibrary\Definition;

use Drupal\Tests\BrowserTestBase;

/**
 * Tests that remote library definitions are found and downloaded.
 *
 * This is a browser test because Guzzle is not usable from a kernel test.
 *
 * @group libraries
 *
 * @todo Make this a kernel test when https://www.drupal.org/node/2571475 is in.
 */
class DefinitionDiscoveryFactoryTest extends BrowserTestBase {

  /**
   * The 'libraries.settings' configuration object.
   *
   * @var \Drupal\Core\Config\Config
   */
  protected $config;

  /**
   * The path to the test library definitions.
   *
   * @var string
   */
  protected $definitionPath;

  /**
   * {@inheritdoc}
   */
  public static $modules = ['libraries'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    /** @var \Drupal\Core\Config\ConfigFactoryInterface $config_factory */
    $config_factory = $this->container->get('config.factory');
    $this->config = $config_factory->getEditable('libraries.settings');

    // Set up the remote library definition URL to point to the local website.
    /** @var \Drupal\Core\Extension\ModuleHandlerInterface $module_handler */
    $module_handler = $this->container->get('module_handler');
    $module_path = $module_handler->getModule('libraries')->getPath();
    $this->definitionPath = "$module_path/tests/library_definitions";
  }

  /**
   * Tests that the discovery works according to the configuration.
   */
  public function testDiscovery() {
    $library_id = 'test_asset_library';
    $expected_definition = [
      'type' => 'asset',
      'version_detector' => [
        'id' => 'static',
        'configuration' => [
          'version' => '1.0.0'
        ],
      ],
      'remote_url' => 'http://example.com',
      'css' => [
        'base' => [
          'example.css' => [],
        ],
      ],
      'js' => [
        'example.js' => [],
      ],
    ];
    $discovery_service_id = 'libraries.definition.discovery';

    // Test the local discovery with an incorrect path.
    $this->config
      ->set('definition.local.path', 'path/that/does/not/exist')
      ->set('definition.remote.enable', FALSE)
      ->save();
    $discovery = $this->container->get($discovery_service_id);
    $this->assertFalse($discovery->hasDefinition($library_id));

    // Test the local discovery with a proper path.
    $this->config
      ->set('definition.local.path', $this->definitionPath)
      ->save();
    $discovery = $this->container->get($discovery_service_id);
    $this->assertTrue($discovery->hasDefinition($library_id));

    // Test a remote discovery with an incorrect path.
    $definitions_directory = 'public://library-definitions';
    $this->config
      ->set('definition.local.path', $definitions_directory)
      ->set('definition.remote.enable', TRUE)
      ->set('definition.remote.urls', ["$this->baseUrl/path/that/does/not/exist"])
      ->save();
    $discovery = $this->container->get($discovery_service_id);
    $this->assertFalse($discovery->hasDefinition($library_id));

    // Test a remote discovery with a proper path.
    $this->config
      ->set('definition.remote.urls', ["$this->baseUrl/$this->definitionPath"])
      ->save();
    /** @var \Drupal\libraries\ExternalLibrary\Definition\DefinitionDiscoveryInterface $discovery */
    $discovery = $this->container->get($discovery_service_id);
    $definition_file = "$definitions_directory/$library_id.json";
    $this->assertFalse(file_exists($definition_file));
    $this->assertTrue($discovery->hasDefinition($library_id));
    $this->assertFalse(file_exists($definition_file));
    $this->assertEquals($discovery->getDefinition($library_id), $expected_definition);
    $this->assertTrue(file_exists($definition_file));
  }

}
