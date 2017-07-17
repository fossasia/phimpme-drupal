<?php

namespace Drupal\Tests\ctools\Unit;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Component\Uuid\UuidInterface;
use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Display\VariantInterface;
use Drupal\ctools\Plugin\VariantCollectionTrait;
use Drupal\ctools\Plugin\VariantPluginCollection;
use Drupal\Tests\UnitTestCase;
use Prophecy\Argument;

/**
 * Tests the methods of a variant-aware class.
 *
 * @coversDefaultClass \Drupal\ctools\Plugin\VariantCollectionTrait
 *
 * @group Ctools
 */
class VariantCollectionTraitTest extends UnitTestCase {

  /**
   * @var \Drupal\Component\Plugin\PluginManagerInterface|\PHPUnit_Framework_MockObject_MockObject
   */
  protected $manager;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    $container = new ContainerBuilder();
    $this->manager = $this->prophesize(PluginManagerInterface::class);
    $container->set('plugin.manager.display_variant', $this->manager->reveal());
    \Drupal::setContainer($container);
  }

  /**
   * @covers ::getVariants
   */
  public function testGetVariantsEmpty() {
    $trait_object = new TestVariantCollectionTrait();
    $this->manager->createInstance()->shouldNotBeCalled();

    $variants = $trait_object->getVariants();
    $this->assertInstanceOf(VariantPluginCollection::class, $variants);
    $this->assertSame(0, count($variants));
  }

  /**
   * @covers ::getVariants
   */
  public function testGetVariants() {
    $trait_object = new TestVariantCollectionTrait();
    $config = [
      'foo' => ['id' => 'foo_plugin'],
      'bar' => ['id' => 'bar_plugin'],
    ];
    foreach ($config as $value) {
      $plugin = $this->prophesize(VariantInterface::class);
      $this->manager->createInstance($value['id'], $value)->willReturn($plugin->reveal());
    }
    $trait_object->setVariantConfig($config);

    $variants = $trait_object->getVariants();
    $this->assertInstanceOf(VariantPluginCollection::class, $variants);
    $this->assertSame(2, count($variants));
    return $variants;
  }

  /**
   * @covers ::getVariants
   *
   * @depends testGetVariants
   */
  public function testGetVariantsSort(VariantPluginCollection $variants) {
    $this->assertSame(['bar' => 'bar', 'foo' => 'foo'], $variants->getInstanceIds());
  }

  /**
   * @covers ::addVariant
   */
  public function testAddVariant() {
    $config = ['id' => 'foo'];
    $uuid = 'test-uuid';
    $expected_config = $config + ['uuid' => $uuid];

    $uuid_generator = $this->prophesize(UuidInterface::class);
    $uuid_generator->generate()
      ->willReturn($uuid)
      ->shouldBeCalledTimes(1);
    $trait_object = new TestVariantCollectionTrait();
    $trait_object->setUuidGenerator($uuid_generator->reveal());

    $plugin_prophecy = $this->prophesize(VariantInterface::class);
    $plugin_prophecy->getConfiguration()
      ->willReturn($expected_config)
      ->shouldBeCalled();
    $plugin_prophecy->setConfiguration($expected_config)
      ->willReturn($expected_config)
      ->shouldBeCalled();

    $this->manager->createInstance('foo', $expected_config)
      ->willReturn($plugin_prophecy->reveal());

    $resulting_uuid = $trait_object->addVariant($config);
    $this->assertSame($uuid, $resulting_uuid);

    $variants = $trait_object->getVariants();
    $this->assertSame([$uuid => $uuid], $variants->getInstanceIds());
    $this->assertSame([$uuid => $expected_config], $variants->getConfiguration());
    $this->assertSame($plugin_prophecy->reveal(), $variants->get($uuid));
    return [$trait_object, $uuid, $plugin_prophecy->reveal()];
  }

  /**
   * @covers ::getVariant
   *
   * @depends testAddVariant
   */
  public function testGetVariant($data) {
    list($trait_object, $uuid, $plugin) = $data;
    $this->manager->createInstance()->shouldNotBeCalled();

    $this->assertSame($plugin, $trait_object->getVariant($uuid));
    return [$trait_object, $uuid];
  }

  /**
   * @covers ::removeVariant
   *
   * @depends testGetVariant
   */
  public function testRemoveVariant($data) {
    list($trait_object, $uuid) = $data;

    $this->assertSame($trait_object, $trait_object->removeVariant($uuid));
    $this->assertFalse($trait_object->getVariants()->has($uuid));
    return [$trait_object, $uuid];
  }

  /**
   * @covers ::getVariant
   *
   * @depends testRemoveVariant
   *
   * @expectedException \Drupal\Component\Plugin\Exception\PluginNotFoundException
   * @expectedExceptionMessage Plugin ID 'test-uuid' was not found.
   */
  public function testGetVariantException($data) {
    list($trait_object, $uuid) = $data;
    // Attempt to retrieve a variant that has been removed.
    $this->assertNull($trait_object->getVariant($uuid));
  }

}

class TestVariantCollectionTrait {
  use VariantCollectionTrait;

  /**
   * @var array
   */
  protected $variantConfig = [];

  /**
   * @var \Drupal\Component\Uuid\UuidInterface
   */
  protected $uuidGenerator;

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
   * Sets the variant configuration.
   *
   * @param array $config
   *   The variant configuration.
   *
   * @return $this
   */
  public function setVariantConfig(array $config) {
    $this->variantConfig = $config;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  protected function getVariantConfig() {
    return $this->variantConfig;
  }

}
