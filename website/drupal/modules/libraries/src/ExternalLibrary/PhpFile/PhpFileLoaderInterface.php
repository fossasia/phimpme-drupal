<?php

namespace Drupal\libraries\ExternalLibrary\PhpFile;

/**
 * Provides an interface for PHP file loaders.
 *
 * @see \Drupal\libraries\ExternalLibrary\PhpFile\PhpFileLibraryInterface
 */
interface PhpFileLoaderInterface {

  /**
   * Loads a PHP file.
   *
   * @param string $file
   *   The absolute file path to the PHP file.
   */
  public function load($file);

}
