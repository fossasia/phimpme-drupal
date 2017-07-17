<?php

namespace Drupal\libraries\Tests;

use Drupal\Component\Utility\Html;
use Drupal\simpletest\WebTestBase;

/**
 * Tests basic detection and loading of libraries.
 *
 * @group libraries
 */
class LibrariesWebTest extends WebTestBase {

  /**
   * {@inheritdoc}
   */
  protected $profile = 'testing';

  /**
   * Modules to install.
   *
   * @var array
   */
  public static $modules = array('libraries', 'libraries_test');

  /**
   * The URL generator used in this test.
   *
   * @var \Drupal\Core\Utility\UnroutedUrlAssemblerInterface
   */
  protected $urlAssembler;

  /**
   * The state service used in this test.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected $state;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $this->urlAssembler = $this->container->get('unrouted_url_assembler');
    $this->state = $this->container->get('state');
  }

  /**
   * Tests libraries_detect_dependencies().
   */
  function testLibrariesDetectDependencies() {
    $library = array(
      'name' => 'Example',
      'dependencies' => array('example_missing'),
    );
    libraries_detect_dependencies($library);
    $this->assertEqual($library['error'], 'missing dependency', 'libraries_detect_dependencies() detects missing dependency');
    $error_message = t('The %dependency library, which the %library library depends on, is not installed.', array(
      '%dependency' => 'Example missing',
      '%library' => $library['name'],
    ));
    $this->verbose("Expected:<br>$error_message");
    $this->verbose('Actual:<br>' . $library['error message']);
    $this->assertEqual($library['error message'], $error_message, 'Correct error message for a missing dependency');
    // Test versioned dependencies.
    $version = '1.1';
    $compatible = array(
      '1.1',
      '<=1.1',
      '>=1.1',
      '<1.2',
      '<2.0',
      '>1.0',
      '>1.0-rc1',
      '>1.0-beta2',
      '>1.0-alpha3',
      '>0.1',
      '<1.2, >1.0',
      '>0.1, <=1.1',
    );
    $incompatible = array(
      '1.2',
      '2.0',
      '<1.1',
      '>1.1',
      '<=1.0',
      '<=1.0-rc1',
      '<=1.0-beta2',
      '<=1.0-alpha3',
      '>=1.2',
      '<1.1, >0.9',
      '>=0.1, <1.1',
    );
    $library = array(
      'name' => 'Example',
    );
    foreach ($compatible as $version_string) {
      $library['dependencies'][0] = "example_dependency ($version_string)";
      // libraries_detect_dependencies() is a post-detect callback, so
      // 'installed' is already set, when it is called. It sets the value to
      // FALSE for missing or incompatible dependencies.
      $library['installed'] = TRUE;
      libraries_detect_dependencies($library);
      $this->assertTrue($library['installed'], "libraries_detect_dependencies() detects compatible version string: '$version_string' is compatible with '$version'");
    }
    foreach ($incompatible as $version_string) {
      $library['dependencies'][0] = "example_dependency ($version_string)";
      $library['installed'] = TRUE;
      unset($library['error'], $library['error message']);
      libraries_detect_dependencies($library);
      $this->assertEqual($library['error'], 'incompatible dependency', "libraries_detect_dependencies() detects incompatible version strings: '$version_string' is incompatible with '$version'");
    }
    // Instead of repeating this assertion for each version string, we just
    // re-use the $library variable from the foreach loop.
    $error_message = t('The version %dependency_version of the %dependency library is not compatible with the %library library.', array(
      '%dependency_version' => $version,
      '%dependency' => 'Example dependency',
      '%library' => $library['name'],
    ));
    $this->verbose("Expected:<br>$error_message");
    $this->verbose('Actual:<br>' . $library['error message']);
    $this->assertEqual($library['error message'], $error_message, 'Correct error message for an incompatible dependency');
  }

  /**
   * Tests libraries_scan_info_files().
   */
  function testLibrariesScanInfoFiles() {
    $expected = array('example_info_file' => (object) array(
      'uri' => drupal_get_path('module', 'libraries') . '/tests/example/example_info_file.libraries.info.yml',
      'filename' => 'example_info_file.libraries.info.yml',
      'name' => 'example_info_file.libraries.info',
    ));
    $actual = libraries_scan_info_files();
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($actual, TRUE) . '</pre>');
    $this->assertEqual($actual, $expected, 'libraries_scan_info_files() correctly finds the example info file.');
    $this->verbose('<pre>' . var_export(libraries_scan_info_files(), TRUE) . '</pre>');
  }

  /**
   * Tests libraries_info().
   */
  function testLibrariesInfo() {
    // Test that library information is found correctly.
    $expected = array(
      'name' => 'Example files',
      'library path' => drupal_get_path('module', 'libraries') . '/tests/example',
      'version' => '1',
      'files' => array(
        'js' => array('example_1.js' => array()),
        'css' => array('example_1.css' => array()),
        'php' => array('example_1.php' => array()),
      ),
      'module' => 'libraries_test',
    );
    libraries_info_defaults($expected, 'example_files');
    $library = libraries_info('example_files');
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Library information is correctly gathered.');

    // Test a library specified with an .info file gets detected.
    $expected = array(
      'name' => 'Example info file',
      'info file' => drupal_get_path('module', 'libraries') . '/tests/example/example_info_file.libraries.info.yml',
    );
    libraries_info_defaults($expected, 'example_info_file');
    $library = libraries_info('example_info_file');
    // If this module was downloaded from Drupal.org, the Drupal.org packaging
    // system has corrupted the test info file.
    // @see http://drupal.org/node/1606606
    unset($library['core'], $library['datestamp'], $library['project'], $library['version']);
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Library specified with an .info file found');
  }

  /**
   * Tests libraries_detect().
   */
  function testLibrariesDetect() {
    // Test missing library.
    $library = libraries_detect('example_missing');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['error'], 'not found', 'Missing library not found.');
    $error_message = t('The %library library could not be found.', array(
      '%library' => $library['name'],
    ));
    $this->assertEqual($library['error message'], $error_message, 'Correct error message for a missing library.');

    // Test unknown library version.
    $library = libraries_detect('example_undetected_version');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['error'], 'not detected', 'Undetected version detected as such.');
    $error_message = t('The version of the %library library could not be detected.', array(
      '%library' => $library['name'],
    ));
    $this->assertEqual($library['error message'], $error_message, 'Correct error message for a library with an undetected version.');

    // Test unsupported library version.
    $library = libraries_detect('example_unsupported_version');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['error'], 'not supported', 'Unsupported version detected as such.');
    $error_message = t('The installed version %version of the %library library is not supported.', array(
      '%version' => $library['version'],
      '%library' => $library['name'],
    ));
    $this->assertEqual($library['error message'], $error_message, 'Correct error message for a library with an unsupported version.');

    // Test supported library version.
    $library = libraries_detect('example_supported_version');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['installed'], TRUE, 'Supported library version found.');

    // Test libraries_get_version().
    $library = libraries_detect('example_default_version_callback');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['version'], '1', 'Expected version returned by default version callback.');

    // Test a multiple-parameter version callback.
    $library = libraries_detect('example_multiple_parameter_version_callback');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['version'], '1', 'Expected version returned by multiple parameter version callback.');

    // Test a top-level files property.
    $library = libraries_detect('example_files');
    $files = array(
      'js' => array('example_1.js' => array()),
      'css' => array('example_1.css' => array()),
      'php' => array('example_1.php' => array()),
    );
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['files'], $files, 'Top-level files property works.');

    // Test version-specific library files.
    $library = libraries_detect('example_versions');
    $files = array(
      'js' => array('example_2.js' => array()),
      'css' => array('example_2.css' => array()),
      'php' => array('example_2.php' => array()),
    );
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['files'], $files, 'Version-specific library files found.');

    // Test missing variant.
    $library = libraries_detect('example_variant_missing');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['variants']['example_variant']['error'], 'not found', 'Missing variant not found');
    $error_message = t('The %variant variant of the %library library could not be found.', array(
      '%variant' => 'example_variant',
      '%library' => 'Example variant missing',
    ));
    $this->assertEqual($library['variants']['example_variant']['error message'], $error_message, 'Correct error message for a missing variant.');

    // Test existing variant.
    $library = libraries_detect('example_variant');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['variants']['example_variant']['installed'], TRUE, 'Existing variant found.');
  }

  /**
   * Tests libraries_load().
   *
   * @todo Remove or rewrite to accomodate integration with core Libraries.
   */
  function _testLibrariesLoad() {
    // Test dependencies.
    $library = libraries_load('example_dependency_missing');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertFalse($library['loaded'], 'Library with missing dependency cannot be loaded');
    $library = libraries_load('example_dependency_incompatible');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertFalse($library['loaded'], 'Library with incompatible dependency cannot be loaded');
    $library = libraries_load('example_dependency_compatible');
    $this->verbose('<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library['loaded'], 1, 'Library with compatible dependency is loaded');
    $loaded = &drupal_static('libraries_load');
    $this->verbose('<pre>' . var_export($loaded, TRUE) . '</pre>');
    $this->assertEqual($loaded['example_dependency']['loaded'], 1, 'Dependency library is also loaded');
  }

  /**
   * Tests the applying of callbacks.
   */
  function testCallbacks() {
    $expected = array(
      'name' => 'Example callback',
      'library path' => drupal_get_path('module', 'libraries') . '/tests/example',
      'version' => '1',
      'versions' => array(
        '1' => array(
          'variants' => array(
            'example_variant' => array(
              'info callback' => 'not applied',
              'pre-detect callback' => 'not applied',
              'post-detect callback' => 'not applied',
              'pre-load callback' => 'not applied',
              'post-load callback' => 'not applied',
            ),
          ),
          'info callback' => 'not applied',
          'pre-detect callback' => 'not applied',
          'post-detect callback' => 'not applied',
          'pre-load callback' => 'not applied',
          'post-load callback' => 'not applied',
        ),
      ),
      'variants' => array(
        'example_variant' => array(
          'info callback' => 'not applied',
          'pre-detect callback' => 'not applied',
          'post-detect callback' => 'not applied',
          'pre-load callback' => 'not applied',
          'post-load callback' => 'not applied',
        ),
      ),
      'callbacks' => array(
        'info' => array('_libraries_test_info_callback'),
        'pre-detect' => array('_libraries_test_pre_detect_callback'),
        'post-detect' => array('_libraries_test_post_detect_callback'),
        'pre-load' => array('_libraries_test_pre_load_callback'),
        'post-load' => array('_libraries_test_post_load_callback'),
      ),
      'info callback' => 'not applied',
      'pre-detect callback' => 'not applied',
      'post-detect callback' => 'not applied',
      'pre-load callback' => 'not applied',
      'post-load callback' => 'not applied',
      'module' => 'libraries_test',
    );
    libraries_info_defaults($expected, 'example_callback');

    // Test a callback in the 'info' group.
    $expected['info callback'] = 'applied (top-level)';
    $expected['versions']['1']['info callback'] = 'applied (version 1)';
    $expected['versions']['1']['variants']['example_variant']['info callback'] = 'applied (version 1, variant example_variant)';
    $expected['variants']['example_variant']['info callback'] = 'applied (variant example_variant)';
    $library = libraries_info('example_callback');
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Prepare callback was applied correctly.');

    // Test a callback in the 'pre-detect' and 'post-detect' phases.
    // Successfully detected libraries should only contain version information
    // for the detected version and thus, be marked as installed.
    unset($expected['versions']);
    $expected['installed'] = TRUE;
    // Additionally, version-specific properties of the detected version are
    // supposed to override the corresponding top-level properties.
    $expected['info callback'] = 'applied (version 1)';
    $expected['variants']['example_variant']['installed'] = TRUE;
    $expected['variants']['example_variant']['info callback'] = 'applied (version 1, variant example_variant)';
    // Version-overloading takes place after the 'pre-detect' callbacks have
    // been applied.
    $expected['pre-detect callback'] = 'applied (version 1)';
    $expected['post-detect callback'] = 'applied (top-level)';
    $expected['variants']['example_variant']['pre-detect callback'] = 'applied (version 1, variant example_variant)';
    $expected['variants']['example_variant']['post-detect callback'] = 'applied (variant example_variant)';
    $library = libraries_detect('example_callback');
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Detect callback was applied correctly.');

    // Test a callback in the 'pre-load' and 'post-load' phases.
    // Successfully loaded libraries should only contain information about the
    // already loaded variant.
    unset($expected['variants']);
    $expected['loaded'] = 0;
    $expected['pre-load callback'] = 'applied (top-level)';
    $expected['post-load callback'] = 'applied (top-level)';
    $library = libraries_load('example_callback');
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Pre-load and post-load callbacks were applied correctly.');
    // This is not recommended usually and is only used for testing purposes.
    drupal_static_reset('libraries_load');
    // Successfully loaded library variants are supposed to contain the specific
    // variant information only.
    $expected['info callback'] = 'applied (version 1, variant example_variant)';
    $expected['pre-detect callback'] = 'applied (version 1, variant example_variant)';
    $expected['post-detect callback'] = 'applied (variant example_variant)';
    $library = libraries_load('example_callback', 'example_variant');
    $this->verbose('Expected:<pre>' . var_export($expected, TRUE) . '</pre>');
    $this->verbose('Actual:<pre>' . var_export($library, TRUE) . '</pre>');
    $this->assertEqual($library, $expected, 'Pre-detect and post-detect callbacks were applied correctly to a variant.');
  }

  /**
   * Tests that library files are properly added to the page output.
   *
   * We check for JavaScript and CSS files directly in the DOM and add a list of
   * included PHP files manually to the page output.
   *
   * @todo Remove or rewrite to accomodate integration with core Libraries.
   * @see _libraries_test_load()
   */
  function _testLibrariesOutput() {
    // Test loading of a simple library with a top-level files property.
    $this->drupalGet('libraries_test/files');
    $this->assertLibraryFiles('example_1', 'File loading');

    // Test loading of integration files.
    $this->drupalGet('libraries_test/integration_files');
    $this->assertRaw('libraries_test.js', 'Integration file loading: libraries_test.js found');
    $this->assertRaw('libraries_test.css', 'Integration file loading: libraries_test.css found');
    $this->assertRaw('libraries_test.inc', 'Integration file loading: libraries_test.inc found');

    // Test version overloading.
    $this->drupalGet('libraries_test/versions');
    $this->assertLibraryFiles('example_2', 'Version overloading');

    // Test variant loading.
    $this->drupalGet('libraries_test/variant');
    $this->assertLibraryFiles('example_3', 'Variant loading');

    // Test version overloading and variant loading.
    $this->drupalGet('libraries_test/versions_and_variants');
    $this->assertLibraryFiles('example_4', 'Concurrent version and variant overloading');

    // Test caching.
    \Drupal::state()->set('libraries_test.cache', TRUE);
    \Drupal::cache('libraries')->delete('example_callback');
    // When the library information is not cached, all callback groups should be
    // invoked.
    $this->drupalGet('libraries_test/cache');
    $this->assertRaw('The <em>info</em> callback group was invoked.', 'Info callback invoked for uncached libraries.');
    $this->assertRaw('The <em>pre-detect</em> callback group was invoked.', 'Pre-detect callback invoked for uncached libraries.');
    $this->assertRaw('The <em>post-detect</em> callback group was invoked.', 'Post-detect callback invoked for uncached libraries.');
    $this->assertRaw('The <em>pre-load</em> callback group was invoked.', 'Pre-load callback invoked for uncached libraries.');
    $this->assertRaw('The <em>post-load</em> callback group was invoked.', 'Post-load callback invoked for uncached libraries.');
    // When the library information is cached only the 'pre-load' and
    // 'post-load' callback groups should be invoked.
    $this->drupalGet('libraries_test/cache');
    $this->assertNoRaw('The <em>info</em> callback group was not invoked.', 'Info callback not invoked for cached libraries.');
    $this->assertNoRaw('The <em>pre-detect</em> callback group was not invoked.', 'Pre-detect callback not invoked for cached libraries.');
    $this->assertNoRaw('The <em>post-detect</em> callback group was not invoked.', 'Post-detect callback not invoked for cached libraries.');
    $this->assertRaw('The <em>pre-load</em> callback group was invoked.', 'Pre-load callback invoked for cached libraries.');
    $this->assertRaw('The <em>post-load</em> callback group was invoked.', 'Post-load callback invoked for cached libraries.');
    \Drupal::state()->set('libraries_test.cache', FALSE);
  }

  /**
   * Helper function to assert that a library was correctly loaded.
   *
   * Asserts that all the correct files were loaded and all the incorrect ones
   * were not.
   *
   * @param $name
   *   The name of the files that should be loaded. The current testing system
   *   knows of 'example_1', 'example_2', 'example_3' and 'example_4'. Each name
   *   has an associated JavaScript, CSS and PHP file that will be asserted. All
   *   other files will be asserted to not be loaded. See
   *   tests/example/README.txt for more information on how the loading of the
   *   files is tested.
   * @param $label
   *   (optional) A label to prepend to the assertion messages, to make them
   *   less ambiguous.
   * @param $extensions
   *   (optional) The expected file extensions of $name. Defaults to
   *   array('js', 'css', 'php').
   */
  function assertLibraryFiles($name, $label = '', $extensions = array('js', 'css', 'php')) {
    $label = ($label !== '' ? "$label: " : '');

    // Test that the wrong files are not loaded...
    $names = array(
      'example_1' => FALSE,
      'example_2' => FALSE,
      'example_3' => FALSE,
      'example_4' => FALSE,
    );
    // ...and the correct ones are.
    $names[$name] = TRUE;

    // Test for the specific HTML that the different file types appear as in the
    // DOM.
    $html = array(
      'js' => array('<script src="', '"></script>'),
      'css' => array('<link rel="stylesheet" href="', '" media="all" />'),
      // PHP files do not get added to the DOM directly.
      // @see _libraries_test_load()
      'php' => array('<li>', '</li>'),
    );

    $html_expected = array();
    $html_not_expected = array();

    foreach ($names as $name => $expected) {
      foreach ($extensions as $extension) {
        $filepath = drupal_get_path('module', 'libraries') . "/tests/example/$name.$extension";
        // JavaScript and CSS files appear as full URLs and with an appended
        // query string.
        if (in_array($extension, array('js', 'css'))) {
          $filepath = $this->urlAssembler->assemble("base://$filepath", [
            'query' => [
              $this->state->get('system.css_js_query_string') ?: '0' => NULL,
            ],
            'absolute' => TRUE,
          ]);
          // If index.php is part of the generated URLs, we need to strip it.
          //$filepath = str_replace('index.php/', '', $filepath);
        }
        list($prefix, $suffix) = $html[$extension];
        $raw = $prefix . $filepath . $suffix;
        if ($expected) {
          $html_expected[] = Html::escape($raw);
          $this->assertRaw($raw, "$label$name.$extension found.");
        }
        else {
          $html_not_expected[] = Html::escape($raw);
          $this->assertNoRaw($raw, "$label$name.$extension not found.");
        }
      }
    }

    $html_expected = '<ul><li><pre>' . implode('</pre></li><li><pre>', $html_expected) . '</pre></li></ul>';
    $html_not_expected = '<ul><li><pre>' . implode('</pre></li><li><pre>', $html_not_expected) . '</pre></li></ul>';
    $this->verbose("Strings of HTML that are expected to be present:{$html_expected}Strings of HTML that are expected to not be present:{$html_not_expected}");
  }

}
