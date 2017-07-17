<?php

namespace Drupal\libraries\ExternalLibrary\PhpFile;

/**
 * Provides a PHP file loader using PHP's require_once.
 *
 * @todo Provide a separate PhpIncludeOnceLoader.
 */
class PhpRequireLoader implements PhpFileLoaderInterface {

  /**
   * {@inheritdoc}
   */
  public function load($file) {
    // @todo Because libraries cannot be loaded twice it should be possible to
    //   use 'require' instead of 'require_once'.
    /** @noinspection PhpIncludeInspection */
    require_once $file;
  }

}
