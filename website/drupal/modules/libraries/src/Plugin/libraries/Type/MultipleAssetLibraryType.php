<?php

namespace Drupal\libraries\Plugin\libraries\Type;

use Drupal\libraries\ExternalLibrary\Asset\AttachableAssetLibraryRegistrationInterface;
use Drupal\libraries\ExternalLibrary\Asset\MultipleAssetLibrary;
use Drupal\libraries\ExternalLibrary\Asset\MultipleAssetLibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeBase;

/**
 * @LibraryType("asset_multiple")
 */
class MultipleAssetLibraryType extends LibraryTypeBase implements AttachableAssetLibraryRegistrationInterface {

  /**
   * {@inheritdoc}
   */
  public function getLibraryClass() {
    return MultipleAssetLibrary::class;
  }

  /**
   * {@inheritdoc}
   */
  public function getAttachableAssetLibraries(LibraryInterface $external_library, LibraryManagerInterface $library_manager) {
    assert('$external_library instanceof \Drupal\libraries\ExternalLibrary\Asset\MultipleAssetLibraryInterface');
    /** @var \Drupal\libraries\ExternalLibrary\Asset\MultipleAssetLibraryInterface $external_library */
    $attachable_libraries = [];
    foreach ($external_library->getAttachableAssetLibraries($library_manager) as $component_name => $attachable_library) {
      $attachable_library_id = $this->getAttachableLibraryId($external_library, $component_name);
      $attachable_libraries[$attachable_library_id] = $attachable_library;
    }
    return $attachable_libraries;
  }

  /**
   * @param \Drupal\libraries\ExternalLibrary\LibraryInterface $external_library
   * @param string $component_name
   *
   * @return string
   */
  protected function getAttachableLibraryId(LibraryInterface $external_library, $component_name) {
    return $external_library->getId() . MultipleAssetLibraryInterface::SEPARATOR . $component_name;
  }

}
