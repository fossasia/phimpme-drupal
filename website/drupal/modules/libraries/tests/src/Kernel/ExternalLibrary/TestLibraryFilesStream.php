<?php

namespace Drupal\Tests\libraries\Kernel\ExternalLibrary;

use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\StreamWrapper\LocalStream;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\StringTranslation\TranslationInterface;
use Drupal\libraries\StreamWrapper\LocalHiddenStreamTrait;
use Drupal\libraries\StreamWrapper\PrivateStreamTrait;

/**
 * Provides a stream wrapper for accessing test library files.
 */
class TestLibraryFilesStream extends LocalStream {

  use LocalHiddenStreamTrait;
  use PrivateStreamTrait;
  use StringTranslationTrait;

  /**
   * The module handler.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * The test directory.
   *
   * @var string
   */
  protected $directory;

  /**
   * Constructs a stream wrapper for test library files.
   *
   * Dependency injection is generally not possible to implement for stream
   * wrappers, because stream wrappers are initialized before the container is
   * booted, but this stream wrapper is only registered explicitly from tests
   * so it is possible here.
   *
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param \Drupal\Core\StringTranslation\TranslationInterface $string_translation
   *   The string translation handler.
   * @param string $directory
   *   The directory within the Libraries API's tests directory that is to be
   *   searched for test library files.
   */
  public function __construct(ModuleHandlerInterface $module_handler, TranslationInterface $string_translation, $directory) {
    $this->moduleHandler = $module_handler;
    $this->directory = (string) $directory;

    $this->setStringTranslation($string_translation);
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    $this->t('Test library files');
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    $this->t('Provides access to test library files.');
  }

  /**
   * {@inheritdoc}
   */
  public function getDirectoryPath() {
    $module_path = $this->moduleHandler->getModule('libraries')->getPath();
    return $module_path . '/tests/' . $this->directory;
  }

}
