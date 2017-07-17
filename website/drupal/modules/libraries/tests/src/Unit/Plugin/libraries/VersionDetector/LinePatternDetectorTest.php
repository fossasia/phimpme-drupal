<?php

namespace Drupal\Tests\libraries\Unit\Plugin\libraries\VersionDetector;

use Drupal\libraries\ExternalLibrary\Local\LocalLibraryInterface;
use Drupal\libraries\ExternalLibrary\Version\VersionedLibraryInterface;
use Drupal\libraries\Plugin\libraries\VersionDetector\LinePatternDetector;
use Drupal\Tests\UnitTestCase;
use org\bovigo\vfs\vfsStream;

/**
 * Tests the line pattern version detector.
 *
 * @group libraries
 *
 * @coversDefaultClass \Drupal\libraries\Plugin\libraries\VersionDetector\LinePatternDetector
 */
class LinePatternDetectorTest extends UnitTestCase {

  protected $libraryId = 'test_library';

  /**
   * Tests that version detection fails for a non-local library.
   *
   * @expectedException \Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException
   *
   * @covers ::detectVersion
   */
  public function testDetectVersionNonLocal() {
    $library = $this->prophesize(VersionedLibraryInterface::class);
    $detector = $this->setupDetector();
    $detector->detectVersion($library->reveal());
  }

  /**
   * Tests that version detection fails for a missing file.
   *
   * @expectedException \Drupal\libraries\ExternalLibrary\Exception\UnknownLibraryVersionException
   *
   * @covers ::detectVersion
   */
  public function testDetectVersionMissingFile() {
    $library = $this->setupLibrary();

    $detector = $this->setupDetector(['file' => 'CHANGELOG.txt']);
    $detector->detectVersion($library->reveal());
  }

  /**
   * Tests that version detection fails without a version in the file.
   *
   * @dataProvider providerTestDetectVersionNoVersion
   *
   * @covers ::detectVersion
   */
  public function testDetectVersionNoVersion($configuration, $file_contents) {
    $library = $this->setupLibrary();

    $detector = $this->setupDetector($configuration);
    $this->setupFile($configuration['file'], $file_contents);

    $library->setVersion()->shouldNotBeCalled();
    $detector->detectVersion($library->reveal());
  }

  /**
   * @return array
   */
  public function providerTestDetectVersionNoVersion() {
    $test_cases = [];

    $configuration = [
      'file' => 'CHANGELOG.txt',
      'pattern' => '/@version (\d+\.\d+\.\d+)/'
    ];

    $test_cases['empty_file'] = [$configuration, ''];

    $test_cases['no_version'] = [$configuration, <<<EOF
This is a file with
multiple lines that does
not contain a version.
EOF
    ];

    $configuration['lines'] = 3;
    $test_cases['long_file'] = [$configuration, <<<EOF
This is a file that
contains the version after
the maximum number of lines
to test has been surpassed.

@version 1.2.3
EOF
    ];

    $configuration['columns'] = 10;
    // @todo Document why this is necessary.
    $configuration['lines'] = 2;
    $test_cases['long_column'] = [$configuration, <<<EOF
This is a file that contains the version after
the maximum number of columns to test has been surpassed. @version 1.2.3
EOF
    ];

    return $test_cases;
  }

  /**
   * Tests that version detection succeeds with a version in the file.
   *
   * @dataProvider providerTestDetectVersion
   *
   * @covers ::detectVersion
   */
  public function testDetectVersion($configuration, $file_contents, $version) {
    $library = $this->setupLibrary();

    $detector = $this->setupDetector($configuration);
    $this->setupFile($configuration['file'], $file_contents);

    $library->setVersion($version)->shouldBeCalled();
    $detector->detectVersion($library->reveal());
  }

  /**
   * @return array
   */
  public function providerTestDetectVersion() {
    $test_cases = [];

    $configuration = [
      'file' => 'CHANGELOG.txt',
      'pattern' => '/@version (\d+\.\d+\.\d+)/'
    ];
    $version = '1.2.3';

    $test_cases['version'] = [$configuration, <<<EOF
This a file with a version

@version $version
EOF
    , $version];

    return $test_cases;
  }

  /**
   * Sets up the library prophecy and returns it.
   *
   * @return \Prophecy\Prophecy\ObjectProphecy
   */
  protected function setupLibrary() {
    $library = $this->prophesize(VersionedLibraryInterface::class);
    $library->willImplement(LocalLibraryInterface::class);
    $library->getId()->willReturn($this->libraryId);
    $library->getLocalPath()->willReturn('libraries/' . $this->libraryId);
    return $library;
  }

  /**
   * Sets up the version detector for testing and returns it.
   *
   * @param array $configuration
   *   The plugin configuration to set the version detector up with.
   *
   * @return \Drupal\libraries\Plugin\libraries\VersionDetector\LinePatternDetector
   *   The line pattern version detector to test.
   */
  protected function setupDetector(array $configuration = []) {
    $app_root = 'root';
    vfsStream::setup($app_root);

    $plugin_id = 'line_pattern';
    $plugin_definition = [
      'id' => $plugin_id,
      'class' => LinePatternDetector::class,
      'provider' => 'libraries',
    ];
    return new LinePatternDetector($configuration, $plugin_id, $plugin_definition, 'vfs://' . $app_root);
  }

  /**
   * @param $file
   * @param $file_contents
   */
  protected function setupFile($file, $file_contents) {
    vfsStream::create([
      'libraries' => [
        $this->libraryId => [
          $file => $file_contents,
        ],
      ],
    ]);
  }

}
