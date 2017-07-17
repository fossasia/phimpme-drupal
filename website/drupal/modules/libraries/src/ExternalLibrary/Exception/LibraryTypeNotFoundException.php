<?php

namespace Drupal\libraries\ExternalLibrary\Exception;

use Drupal\libraries\ExternalLibrary\Utility\LibraryIdAccessorTrait;
use Drupal\libraries\ExternalLibrary\Utility\LibraryIdAccessorInterface;

/**
 * Provides an exception for a library definition without a type declaration.
 */
class LibraryTypeNotFoundException extends \RuntimeException implements LibraryAccessorInterface {

  use LibraryIdAccessorTrait;

  /**
   * Constructs a library exception.
   *
   * @param string $library_id
   *   The library ID.
   * @param string $message
   *   (optional) The exception message.
   * @param int $code
   *   (optional) The error code.
   * @param \Exception $previous
   *   (optional) The previous exception.
   */
  public function __construct(
    $library_id,
    $message = '',
    $code = 0,
    \Exception $previous = NULL
  ) {
    $this->libraryId = (string) $library_id;
    $message = $message ?: "The library type for the library '{$this->libraryId}' could not be found.";
    parent::__construct($message, $code, $previous);
  }

}
