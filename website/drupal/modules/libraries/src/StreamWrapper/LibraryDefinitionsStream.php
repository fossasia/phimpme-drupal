<?php

namespace Drupal\libraries\StreamWrapper;

use Drupal\Core\StreamWrapper\LocalStream;

/**
 * Provides a stream wrapper for library definitions.
 *
 * Can be used with the 'library-definitions' scheme, for example
 * 'library-definitions://example.json' for a library ID of 'example'.
 *
 * By default this stream wrapper reads from a single directory that is
 * configurable and points to the 'library-definitions' directory within the
 * public files directory by default. This makes library definitions writable
 * by the webserver by default, which is in anticipation of a user interface
 * that fetches definitions from a remote repository and stores them locally.
 * For improved security the library definitions can be managed manually (or put
 * under version control) and placed in a directory that is not writable by the
 * webserver.
 *
 * The idea of using a stream wrapper for this as well as the default location
 * is taken from the 'translations' stream wrapper provided by the Interface
 * Translation module.
 *
 * @see \Drupal\locale\StreamWrapper\TranslationsStream
 *
 * @todo Use a setting instead of configuration for the directory.
 */
class LibraryDefinitionsStream extends LocalStream {

  use LocalHiddenStreamTrait;
  use PrivateStreamTrait;

  /**
   * The config factory
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * Constructs an external library registry.
   *
   * @todo Dependency injection.
   */
  public function __construct() {
    $this->configFactory = \Drupal::configFactory();
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    return t('Library definitions');
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return t('Provides access to library definition files.');
  }

  /**
   * {@inheritdoc}
   */
  public function getDirectoryPath() {
    return $this->getConfig('local.path');
  }

  /**
   * Fetches a configuration value from the library definitions configuration.
   * @param $key
   *   The configuration key to fetch.
   *
   * @return array|mixed|null
   *   The configuration value.
   */
  protected function getConfig($key) {
    return $this->configFactory
      ->get('libraries.settings')
      ->get("definitions.$key");
  }

}
