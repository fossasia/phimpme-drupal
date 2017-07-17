<?php

namespace Drupal\libraries\ExternalLibrary\Asset;

/**
 * A trait for asset libraries that serve local and remote files.
 *
 * If the library files are available locally, they are served locally.
 * Otherwise, the remote files are served, assuming a remote URL is specified.
 *
 * This trait should only be used in classes implementing LocalLibraryInterface
 * and RemoteLibraryInterface.
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface
 * @see \Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface
 */
trait LocalRemoteAssetTrait {

  /**
   * Checks whether this library can be attached.
   *
   * @return bool
   *   TRUE if the library can be attached; FALSE otherwise.
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\SingleAssetLibraryTrait::canBeAttached()
   */
  protected function canBeAttached() {
    /** @var \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface|\Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface $this */
    return ($this->isInstalled() || $this->hasRemoteUrl());
  }

  /**
   * Gets the prefix to prepend to file paths.
   *
   * For local libraries this is the library path, for remote libraries this is
   * the remote URL.
   *
   * @return string
   *   The path prefix.
   */
  protected function getPathPrefix() {
    /** @var \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface|\Drupal\libraries\ExternalLibrary\Remote\RemoteLibraryInterface $this */
    if ($this->isInstalled()) {
      // LocalLibraryInterface::getLocalPath() returns the path relative to the
      // app root. In order for the core core asset system to register the path
      // as relative to the app root, a leading slash is required.
      /** @see \Drupal\Core\Asset\LibraryDiscoveryParser::buildByExtension() */
      return '/' . $this->getLocalPath();
    }
    elseif ($this->hasRemoteUrl()) {
      return $this->getRemoteUrl();
    }
    else {
      // @todo Throw an exception.
    }
  }

  /**
   * Gets the CSS assets attached to this library.
   *
   * @param array $assets
   *
   * @return array
   *   An array of CSS assets of the library following the core library CSS
   *   structure. The keys of the array must be among the SMACSS categories
   *   'base', 'layout, 'component', 'state', and 'theme'. The value of each
   *   category is in turn an array where the keys are the file paths of the CSS
   *   files and values are CSS options.
   *
   * @see https://smacss.com/
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\SingleAssetLibraryTrait::getCssAssets()
   */
  protected function processCssAssets(array $assets) {
    // @todo Consider somehow caching the processed information.
    $processed_assets = [];
    foreach ($assets as $category => $category_assets) {
      // @todo Somehow consolidate this with getJsAssets().
      foreach ($category_assets as $filename => $options) {
        $processed_assets[$category][$this->getPathPrefix() . '/' . $filename] = $options;
      }
    }
    return $processed_assets;
  }

  /**
   * Gets the JavaScript assets attached to this library.
   *
   * @param array $assets
   *
   * @return array
   *   An array of JavaScript assets of the library. The keys of the array are
   *   the file paths of the JavaScript files and the values are JavaScript
   *   options.
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\SingleAssetLibraryTrait::getJsAssets()
   */
  protected function processJsAssets(array $assets) {
    // @todo Consider somehow caching the processed information.
    $processed_assets = [];
    // @todo Somehow consolidate this with getCssAssets().
    foreach ($assets as $filename => $options) {
      $processed_assets[$this->getPathPrefix() . '/' . $filename] = $options;
    }
    return $processed_assets;
  }

}
