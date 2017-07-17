<?php

namespace Drupal\libraries\Plugin\libraries\Type;

use Drupal\libraries\ExternalLibrary\Asset\AssetLibrary;
use Drupal\libraries\ExternalLibrary\Asset\AttachableAssetLibraryRegistrationInterface;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;
use Drupal\libraries\ExternalLibrary\Type\LibraryTypeBase;

/**
 * @LibraryType("asset")
 */
class AssetLibraryType extends LibraryTypeBase implements AttachableAssetLibraryRegistrationInterface {

  /**
   * {@inheritdoc}
   */
  public function getLibraryClass() {
    return AssetLibrary::class;
  }

  /**
   * {@inheritdoc}
   */
  public function getAttachableAssetLibraries(LibraryInterface $library, LibraryManagerInterface $library_manager) {
    assert('$library instanceof \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryInterface');
    /** @var \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryInterface $library */
    return [$library->getId() => $library->getAttachableAssetLibrary($library_manager)];
  }

}
