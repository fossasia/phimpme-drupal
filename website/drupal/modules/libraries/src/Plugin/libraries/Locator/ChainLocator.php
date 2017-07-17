<?php

namespace Drupal\libraries\Plugin\libraries\Locator;

use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Local\LocatorInterface;

/**
 * Provides a locator utilizing a chain of other individual locators.
 *
 * @Locator("chain")
 *
 * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface
 */
class ChainLocator implements LocatorInterface {

  /**
   * The locators to check.
   *
   * @var \Drupal\libraries\ExternalLibrary\Local\LocatorInterface[]
   */
  protected $locators = [];

  /**
   * Add a locator to the chain.
   *
   * @param \Drupal\libraries\ExternalLibrary\Local\LocatorInterface $locator
   *   A locator to add to the chain.
   */
  public function addLocator(LocatorInterface $locator) {
    $this->locators[] = $locator;
    return $this;
  }

  /**
   * Locates a library.
   *
   * @param \Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface $library
   *   The library to locate.
   *
   * @see \Drupal\libraries\ExternalLibrary\Local\LocatorInterface::locate()
   */
  public function locate(LocalLibraryInterface $library) {
    foreach ($this->locators as $locator) {
      $locator->locate($library);
      if ($library->isInstalled()) {
        return;
      }
    }
    $library->setUninstalled();
  }

}
