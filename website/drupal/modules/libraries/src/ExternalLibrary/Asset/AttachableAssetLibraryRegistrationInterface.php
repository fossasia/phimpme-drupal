<?php

namespace Drupal\libraries\ExternalLibrary\Asset;

use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;

/**
 * An interface for library types that want to react to library instantiation.
 */
interface AttachableAssetLibraryRegistrationInterface {

  /**
   * Reacts to the instantiation of a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\LibraryInterface $external_library
   *   The library that is being instantiated.
   * @param \Drupal\libraries\ExternalLibrary\LibraryManagerInterface $library_manager
   */
  public function getAttachableAssetLibraries(LibraryInterface $external_library, LibraryManagerInterface $library_manager);

}
