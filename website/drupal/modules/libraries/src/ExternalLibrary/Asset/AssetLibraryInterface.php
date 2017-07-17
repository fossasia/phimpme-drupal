<?php

namespace Drupal\libraries\ExternalLibrary\Asset;

use Drupal\libraries\ExternalLibrary\Dependency\DependentLibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;

/**
 * Provides an interface for external asset libraries with a single library.
 *
 * Asset is the generic term for CSS and JavaScript files.
 *
 * In order to load assets of external libraries as part of a page request the
 * assets must be registered with Drupal core's library system. Therefore,
 * Libraries API makes all libraries that are required by the installed
 * installation profile, modules, and themes available as core asset libraries
 * with the identifier 'libraries/[machine_name]' where '[machine_name]' is
 * the Libraries API machine name of the external library.
 *
 * Thus, assuming that the external library 'flexslider' has been declared as a
 * dependency, for example, it can be attached to a render array in the $build
 * variable with the following code:
 * @code
 *   $build['#attached']['library'] = ['libraries/flexslider'];
 * @endcode
 *
 * In some cases an external library may contain multiple components, that
 * should be loadable independently from each other. In this case, implement
 * MultipleAssetLibraryInterface instead.
 *
 * @see libraries_library_info_build()
 * @see \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryTrait
 * @see \Drupal\libraries\ExternalLibrary\Asset\MultipleAssetLibraryInterface
 *
 * @todo Support loading of source or minified assets.
 * @todo Document how library dependencies work.
 *
 * @ingroup libraries
 */
interface AssetLibraryInterface extends
  LibraryInterface,
  VersionedLibraryInterface,
  DependentLibraryInterface
{

  /**
   * Returns a core asset library array structure for this library.
   *
   * @param \Drupal\libraries\ExternalLibrary\LibraryManagerInterface $library_manager
   *   The library manager that can be used to fetch dependencies.
   *
   * @return array
   *
   * @see \Drupal\libraries\ExternalLibrary\Asset\SingleAssetLibraryTrait
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\InvalidLibraryDependencyException
   *
   * @todo Document the return value.
   * @todo Reconsider passing the library manager.
   */
  public function getAttachableAssetLibrary(LibraryManagerInterface $library_manager);

}
