<?php

namespace Drupal\libraries_test\Controller;

use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ExampleController implements ContainerInjectionInterface {

  /**
   * Injects BookManager Service.
   */
  public static function create(ContainerInterface $container) {
    return new static();
  }

  /**
   * Loads a specified library (variant) for testing.
   *
   * JavaScript and CSS files can be checked directly by SimpleTest, so we only
   * need to manually check for PHP files. We provide information about the loaded
   * JavaScript and CSS files for easier debugging. See example/README.txt for
   * more information.
   */
  private function buildPage($library, $variant = NULL) {
    libraries_load($library, $variant);
    // JavaScript and CSS files can be checked directly by SimpleTest, so we only
    // need to manually check for PHP files.
    $output = '';

    // For easer debugging of JS loading, a text is shown that the JavaScript will
    // replace.
    $output .= '<h2>JavaScript</h2>';
    $output .= '<div class="libraries-test-javascript">';
    $output .= 'If this text shows up, no JavaScript test file was loaded.';
    $output .= '</div>';

    // For easier debugging of CSS loading, the loaded CSS files will color the
    // following text.
    $output .= '<h2>CSS</h2>';
    $output .= '<div class="libraries-test-css">';
    $output .= 'If one of the CSS test files has been loaded, this text will be colored:';
    $output .= '<ul>';
    // Do not reference the actual CSS files (i.e. including '.css'), because that
    // breaks testing.
    $output .= '<li>example_1: red</li>';
    $output .= '<li>example_2: green</li>';
    $output .= '<li>example_3: orange</li>';
    $output .= '<li>example_4: blue</li>';
    $output .= '<li>libraries_test: purple</li>';
    $output .= '</ul>';
    $output .= '</div>';

    $output .= '<h2>PHP</h2>';
    $output .= '<div class="libraries-test-php">';
    $output .= 'The following is a list of all loaded test PHP files:';
    $output .= '<ul>';
    $files = get_included_files();
    foreach ($files as $file) {
      if ((strpos($file, 'libraries/test') || strpos($file, 'libraries_test')) && !strpos($file, 'libraries_test.module') && !strpos($file, 'lib/Drupal/libraries_test')) {
        $output .= '<li>' . str_replace(DRUPAL_ROOT . '/', '', $file) . '</li>';
      }
    }
    $output .= '</ul>';
    $output .= '</div>';

    return ['#markup' => $output];
  }

  public function files() {
    return $this->buildPage('example_files');
  }

  public function integration() {
    return $this->buildPage('example_integration_files');
  }

  public function versions() {
    return $this->buildPage('example_versions');
  }

  public function variant() {
    return $this->buildPage('example_variant', 'example_variant');
  }

  public function versionsAndVariants() {
    return $this->buildPage('example_versions_and_variants', 'example_variant_2');
  }

  public function cache() {
    return $this->buildPage('example_callback');
  }

}
