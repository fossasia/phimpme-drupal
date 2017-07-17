<?php

namespace Drupal\libraries\ExternalLibrary\PhpFile;

use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;

/**
 * Provides an interface for libraries with PHP files.
 *
 * @see \Drupal\libraries\ExternalLibrary\PhpFile\PhpFileLoaderInterface
 */
interface PhpFileLibraryInterface extends LocalLibraryInterface {

  /**
   * Returns the PHP files of this library.
   *
   * @return string[]
   *   An array of absolute file paths of PHP files.
   */
  public function getPhpFiles();

}
