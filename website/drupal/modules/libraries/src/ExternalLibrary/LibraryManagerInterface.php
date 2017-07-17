<?php

namespace Drupal\libraries\ExternalLibrary;

/**
 * Provides an interface for external library managers.
 */
interface LibraryManagerInterface {

  /**
   * Gets a library by its ID.
   *
   * @param string $id
   *   The library ID.
   *
   * @return \Drupal\libraries\ExternalLibrary\LibraryInterface
   *   The library object.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryTypeNotFoundException
   * @throws \Drupal\Component\Plugin\Exception\PluginException
   */
  public function getLibrary($id);

  /**
   * Gets the list of libraries that are required by enabled extensions.
   *
   * Modules, themes, and installation profiles can declare library dependencies
   * by specifying a 'library_dependencies' key in their info files.
   *
   * @return string[]
   *   An array of library IDs.
   */
  public function getRequiredLibraryIds();

  /**
   * Loads library files for a library.
   *
   * Note that not all library types support explicit loading. Asset libraries,
   * in particular, are declared to Drupal core's library system and are then
   * loaded using that.
   *
   * @param string $id
   *   The ID of the library.
   *
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryDefinitionNotFoundException
   * @throws \Drupal\libraries\ExternalLibrary\Exception\LibraryNotInstalledException
   */
  public function load($id);

}
