<?php

namespace Drupal\libraries\StreamWrapper;

/**
 * Provides a trait for local streams that are not publicly accessible.
 *
 * @see \Drupal\locale\StreamWrapper\TranslationsStream
 */
trait PrivateStreamTrait {

  /**
   * Returns a web accessible URL for the resource.
   *
   * This function should return a URL that can be embedded in a web page
   * and accessed from a browser. For example, the external URL of
   * "youtube://xIpLd0WQKCY" might be
   * "http://www.youtube.com/watch?v=xIpLd0WQKCY".
   *
   * @return string
   *   Returns a string containing a web accessible URL for the resource.
   *
   * @see \Drupal\Core\StreamWrapper\StreamWrapperInterface::getExternalUrl()
   */
  function getExternalUrl() {
    throw new \LogicException("{$this->getName()} should not be public.");
  }

  /**
   * Returns the name of the stream wrapper for use in the UI.
   *
   * @return string
   *   The stream wrapper name.
   *
   * @see \Drupal\Core\StreamWrapper\StreamWrapperInterface::getName()
   */
  abstract public function getName();

}
