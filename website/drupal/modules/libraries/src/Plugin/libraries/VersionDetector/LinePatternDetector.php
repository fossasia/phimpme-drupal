<?php

namespace Drupal\libraries\Plugin\libraries\VersionDetector;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Plugin\PluginBase;
use Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionDetectorInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Detects the version by matching lines in a file against a specified pattern.
 *
 * This version detector can be used if the library version is denoted in a
 * particular format in a changelog or readme file, for example.
 *
 * @VersionDetector("line_pattern")
 *
 * @ingroup libraries
 */
class LinePatternDetector extends PluginBase implements VersionDetectorInterface, ContainerFactoryPluginInterface {

  /**
   * The app root.
   *
   * @var string
   */
  protected $appRoot;

  /**
   * Constructs a line pattern version detector.
   *
   * @param array $configuration
   * @param string $plugin_id
   * @param array $plugin_definition
   * @param string $app_root
   */
  public function __construct(array $configuration, $plugin_id, array $plugin_definition, $app_root) {
    $configuration += [
      'file' => '',
      'pattern' => '',
      'lines' => 20,
      'columns' => 200,
    ];
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->appRoot = $app_root;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('app.root')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function detectVersion(VersionedLibraryInterface $library) {
    if (!($library instanceof LocalLibraryInterface)) {
      throw new UnknownLibraryVersionException($library);
    }

    $filepath = $this->appRoot . '/' . $library->getLocalPath() . '/' . $this->configuration['file'];
    if (!file_exists($filepath)) {
      throw new UnknownLibraryVersionException($library);
    }

    $file = fopen($filepath, 'r');
    $lines = $this->configuration['lines'];
    while ($lines && $line = fgets($file, $this->configuration['columns'])) {
      if (preg_match($this->configuration['pattern'], $line, $version)) {
        fclose($file);
        $library->setVersion($version[1]);
        return;
      }
      $lines--;
    }
    fclose($file);
  }

}
