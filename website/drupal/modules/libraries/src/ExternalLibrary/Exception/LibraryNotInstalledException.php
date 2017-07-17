<?php

namespace Drupal\libraries\ExternalLibrary\Exception;

use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorTrait;
use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorInterface;

/**
 * Provides an exception for a library that is not installed.
 */
class LibraryNotInstalledException extends \RuntimeException implements LibraryAccessorInterface {

  use LibraryAccessorTrait;

  /**
   * Constructs a library exception.
   *
   * @param \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface $library
   *   The library that is not installed.
   * @param string $message
   *   (optional) The exception message.
   * @param int $code
   *   (optional) The error code.
   * @param \Exception $previous
   *   (optional) The previous exception.
   */
  public function __construct(
    LocalLibraryInterface $library,
    $message = '',
    $code = 0,
    \Exception $previous = NULL
  ) {
    $this->library = $library;
    $message = $message ?: "The library '{$this->library->getId()}' is not installed.";
    parent::__construct($message, $code, $previous);
  }

}
