<?php

namespace Drupal\libraries\ExternalLibrary\Exception;

use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorTrait;
use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;

/**
 * Provides an exception for libraries whose version has not been detected.
 */
class UnknownLibraryVersionException extends \RuntimeException implements LibraryAccessorInterface {

  use LibraryAccessorTrait;

  /**
   * Constructs a library exception.
   *
   * @param \Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface $library
   *   The library.
   * @param string $message
   *   (optional) The exception message.
   * @param int $code
   *   (optional) The error code.
   * @param \Exception $previous
   *   (optional) The previous exception.
   */
  public function __construct(
    VersionedLibraryInterface $library,
    $message = '',
    $code = 0,
    \Exception $previous = NULL
  ) {
    $this->library = $library;
    $message = $message ?: "The version of library '{$this->library->getId()}' could not be detected.";
    parent::__construct($message, $code, $previous);
  }

}
