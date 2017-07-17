<?php

namespace Drupal\libraries\ExternalLibrary\Type;

use Drupal\libraries\ExternalLibrary\LibraryInterface;

/**
 * An interface for library types that want to react to library instantiation.
 */
interface LibraryCreationListenerInterface {

  /**
   * Reacts to the instantiation of a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\LibraryInterface $library
   *   The library that is being instantiated.
   */
  public function onLibraryCreate(LibraryInterface $library);

}
