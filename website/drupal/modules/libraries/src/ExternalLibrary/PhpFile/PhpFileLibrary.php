<?php

namespace Drupal\libraries\ExternalLibrary\PhpFile;

use Drupal\Component\Plugin\Factory\FactoryInterface;
use Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException;
use Drupal\libraries\ExternalLibrary\LibraryBase;
use Drupal\libraries\ExternalLibrary\Local\LocalLibraryTrait;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface;

/**
 * Provides a base PHP file library implementation.
 */
class PhpFileLibrary extends LibraryBase implements PhpFileLibraryInterface {

  use LocalLibraryTrait;

  /**
   * An array of PHP files for this library.
   *
   * @var array
   */
  protected $files = [];

  /**
   * Constructs a PHP file library.
   *
   * @param string $id
   *   The library ID.
   * @param array $definition
   *   The library definition array.
   * @param \Drupal\libraries\ExternalLibrary\Type\LibraryTypeInterface $type
   *   The library type of this library.
   */
  public function __construct($id, array $definition, LibraryTypeInterface $type) {
    parent::__construct($id, $definition, $type);
    $this->files = $definition['files'];
  }

  /**
   * {@inheritdoc}
   */
  protected static function processDefinition(array &$definition) {
    parent::processDefinition($definition);
    $definition += [
      'files' => [],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getPhpFiles() {
    if (!$this->isInstalled()) {
      throw new LibraryNotInstalledException($this);
    }

    $processed_files = [];
    foreach ($this->files as $file) {
      $processed_files[] = $this->getLocalPath() . '/' . $file;
    }
    return $processed_files;
  }

  /**
   * {@inheritdoc}
   */
  public function getLocator(FactoryInterface $locator_factory) {
    // @todo Consider refining the stream wrapper used here.
    return $locator_factory->createInstance('uri', ['uri' => 'php-file://']);
  }

}
