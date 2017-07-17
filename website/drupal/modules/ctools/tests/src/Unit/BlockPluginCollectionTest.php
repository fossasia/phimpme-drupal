<?php

namespace Drupal\Tests\ctools\Unit;

use Drupal\Core\Block\BlockManagerInterface;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\ctools\Plugin\BlockPluginCollection;
use Drupal\Tests\UnitTestCase;
use Prophecy\Argument;

/**
 * Tests the block plugin collection.
 *
 * @coversDefaultClass \Drupal\ctools\Plugin\BlockPluginCollection
 *
 * @group CTools
 */
class BlockPluginCollectionTest extends UnitTestCase {

  /**
   * Tests the getAllByRegion() method.
   *
   * @covers ::getAllByRegion
   */
  public function testGetAllByRegion() {
    $blocks = [
      'foo' => [
        'id' => 'foo',
        'label' => 'Foo',
        'plugin' => 'system_powered_by_block',
        'region' => 'bottom',
      ],
      'bar' => [
        'id' => 'bar',
        'label' => 'Bar',
        'plugin' => 'system_powered_by_block',
        'region' => 'top',
      ],
      'bing' => [
        'id' => 'bing',
        'label' => 'Bing',
        'plugin' => 'system_powered_by_block',
        'region' => 'bottom',
        'weight' => -10,
      ],
      'baz' => [
        'id' => 'baz',
        'label' => 'Baz',
        'plugin' => 'system_powered_by_block',
        'region' => 'bottom',
      ],
    ];
    $block_manager = $this->prophesize(BlockManagerInterface::class);
    $plugins = [];
    foreach ($blocks as $block_id => $block) {
      $plugin = $this->prophesize(BlockPluginInterface::class);
      $plugin->label()->willReturn($block['label']);
      $plugin->getConfiguration()->willReturn($block);
      $plugins[$block_id] = $plugin->reveal();

      $block_manager->createInstance($block_id, $block)
        ->willReturn($plugin->reveal())
        ->shouldBeCalled();
    }


    $block_plugin_collection = new BlockPluginCollection($block_manager->reveal(), $blocks);
    $expected = [
      'bottom' => [
        'bing' => $plugins['bing'],
        'baz' => $plugins['baz'],
        'foo' => $plugins['foo'],
      ],
      'top' => [
        'bar' => $plugins['bar'],
      ],
    ];
    $this->assertSame($expected, $block_plugin_collection->getAllByRegion());
  }

}
