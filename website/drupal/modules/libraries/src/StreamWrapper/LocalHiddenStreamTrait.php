<?php

namespace Drupal\libraries\StreamWrapper;

use Drupal\Core\StreamWrapper\StreamWrapperInterface;

/**
 * Provides a trait for local hidden streams.
 */
trait LocalHiddenStreamTrait {

  /**
   * Returns the type of stream wrapper.
   *
   * @return int
   *
   * @see \Drupal\Core\StreamWrapper\StreamWrapperInterface::getType()
   */
  public static function getType() {
    return StreamWrapperInterface::LOCAL_HIDDEN;
  }

}
