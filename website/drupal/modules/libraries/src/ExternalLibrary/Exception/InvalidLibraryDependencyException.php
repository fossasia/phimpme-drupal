<?php

namespace Drupal\libraries\ExternalLibrary\Exception;

use Drupal\libraries\ExternalLibrary\Utility\DependencyAccessorTrait;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorTrait;
use Drupal\libraries\ExternalLibrary\Utility\LibraryAccessorInterface;

/**
 * Provides an exception for an invalid library exception.
 */
class InvalidLibraryDependencyException extends \UnexpectedValueException implements LibraryAccessorInterface {

  use LibraryAccessorTrait;
  use DependencyAccessorTrait;

  /**
   * Constructs a library exception.
   *
   * @param \Drupal\libraries\ExternalLibrary\LibraryInterface $library
   *   The library with the invalid dependency.
   * @param \Drupal\libraries\ExternalLibrary\LibraryInterface $dependency
   *   The dependency.
   * @param string $message
   *   (optional) The exception message.
   * @param int $code
   *   (optional) The error code.
   * @param \Exception $previous
   *   (optional) The previous exception.
   */
  public function __construct(
    LibraryInterface $library,
    LibraryInterface $dependency,
    $message = '',
    $code = 0,
    \Exception $previous = NULL
  ) {
    $this->library = $library;
    $this->dependency = $dependency;
    $message = $message ?: "The library '{$this->library->getId()}' cannot depend on the library '{$this->dependency->getId()}'.";
    parent::__construct($message, $code, $previous);
  }

}
