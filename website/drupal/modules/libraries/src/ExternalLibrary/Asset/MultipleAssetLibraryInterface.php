<?php

namespace Drupal\libraries\ExternalLibrary\Asset;

use Drupal\libraries\ExternalLibrary\LibraryInterface;
use Drupal\libraries\ExternalLibrary\LibraryManagerInterface;

/**
 * Provides an interface for external asset libraries with multiple libraries.
 *
 * See SingleAssetLibraryInterface for more information on external asset
 * libraries in general.
 *
 * In case an external asset library contains multiple components that should
 * be loadable independently from each other, Libraries API registers each
 * library component as a separate library in the core asset library system. The
 * resulting core library identifier is
 * 'libraries/[machine_name].[component_name]' where '[machine_name]' is the
 * Libraries API machine name of the external library and '[component_name]' is
 * the component name specified by the library definition.
 *
 * Thus, assuming that the external library 'bootstrap' has been declared as a
 * dependency, for example, and it has 'button' and 'form' components, they can
 * be attached to a render array in the $build variable with the following code:
 * @code
 *   $build['#attached']['library'] = [
 *     'libraries/bootstrap.button',
 *     'libraries/bootstrap.form',
 *   ];
 * @endcode
 *
 * @see \Drupal\libraries\ExternalLibrary\Asset\AssetLibraryInterface
 *
 * @todo Support loading of source or minified assets.
 * @todo Document how library dependencies work.
 */
interface MultipleAssetLibraryInterface extends LibraryInterface {

  /**
   * Separates the library machine name from its component name.
   *
   * The period is chosen in alignment with core asset libraries, which are
   * named, for example, 'core/jquery.once'.
   */
  const SEPARATOR = '.';

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
  public function getAttachableAssetLibraries(LibraryManagerInterface $library_manager);

}
