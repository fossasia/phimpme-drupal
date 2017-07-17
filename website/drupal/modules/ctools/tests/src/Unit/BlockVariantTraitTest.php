<?php

namespace Drupal\Tests\ctools\Unit;

use Drupal\Component\Uuid\UuidInterface;
use Drupal\ctools\Plugin\BlockPluginCollection;
use Drupal\ctools\Plugin\BlockVariantTrait;
use Drupal\Tests\UnitTestCase;

/**
 * Tests the methods of a block-based variant.
 *
 * @coversDefaultClass \Drupal\ctools\Plugin\BlockVariantTrait
 *
 * @group CTools
 */
class BlockVariantTraitTest extends UnitTestCase {

  /**
   * Tests the getRegionAssignments() method.
   *
   * @covers ::getRegionAssignments
   *
   * @dataProvider providerTestGetRegionAssignments
   */
  public function testGetRegionAssignments($expected, $blocks = []) {
    $block_collection = $this->prophesize(BlockPluginCollection::class);
    $block_collection->getAllByRegion()
      ->willReturn($blocks)
      ->shouldBeCalled();

    $display_variant = new TestBlockVariantTrait();
    $display_variant->setBlockPluginCollection($block_collection->reveal());

    $this->assertSame($expected, $display_variant->getRegionAssignments());
  }

  public function providerTestGetRegionAssignments() {
    return [
      [
        [
          'top' => [],
          'bottom' => [],
        ],
      ],
      [
        [
          'top' => ['foo'],
          'bottom' => [],
        ],
        [
          'top' => ['foo'],
        ],
      ],
      [
        [
          'top' => [],
          'bottom' => [],
        ],
        [
          'invalid' => ['foo'],
        ],
      ],
      [
        [
          'top' => [],
          'bottom' => ['foo'],
        ],
        [
          'bottom' => ['foo'],
          'invalid' => ['bar'],
        ],
      ],
    ];
  }

}

class TestBlockVariantTrait {
  use BlockVariantTrait;

  /**
   * @var array
   */
  protected $blockConfig = [];

  /**
   * @var \Drupal\Component\Uuid\UuidInterface
   */
  protected $uuidGenerator;

  /**
   * @param BlockPluginCollection $block_plugin_collection
   *
   * @return $this
   */
  public function setBlockPluginCollection(BlockPluginCollection $block_plugin_collection) {
    $this->blockPluginCollection = $block_plugin_collection;
    return $this;
  }

  /**
   * @param \Drupal\Component\Uuid\UuidInterface $uuid_generator
   *
   * @return $this
   */
  public function setUuidGenerator(UuidInterface $uuid_generator) {
    $this->uuidGenerator = $uuid_generator;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  protected function uuidGenerator() {
    return $this->uuidGenerator;
  }

  /**
   * Sets the block configuration.
   *
   * @param array $config
   *   The block configuration.
   *
   * @return $this
   */
  public function setBlockConfig(array $config) {
    $this->blockConfig = $config;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  protected function getBlockConfig() {
    return $this->blockConfig;
  }

  /**
   * {@inheritdoc}
   */
  public function getRegionNames() {
    return [
      'top' => 'Top',
      'bottom' => 'Bottom',
    ];
  }

}
