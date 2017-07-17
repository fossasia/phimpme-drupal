<?php

namespace Drupal\Tests\ctools\Unit;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Entity\EntityRepositoryInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\TypedData\DataDefinition;
use Drupal\Core\TypedData\Plugin\DataType\IntegerData;
use Drupal\Core\TypedData\TypedDataManager;
use Drupal\ctools\Context\EntityLazyLoadContext;
use Drupal\ctools\ContextMapper;
use Drupal\Tests\UnitTestCase;

/**
 * @coversDefaultClass \Drupal\ctools\ContextMapper
 *
 * @group CTools
 */
class ContextMapperTest extends UnitTestCase {

  /**
   * The typed data manager.
   *
   * @var \Drupal\Core\TypedData\TypedDataManager|\Prophecy\Prophecy\ProphecyInterface
   */
  protected $typedDataManager;

  /**
   * @var \Drupal\Core\Entity\EntityRepositoryInterface|\Prophecy\Prophecy\ProphecyInterface
   */
  protected $entityRepository;

  /**
   * @var \Drupal\page_manager\ContextMapper
   */
  protected $staticContext;

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    $this->typedDataManager = $this->prophesize(TypedDataManager::class);
    $this->entityRepository = $this->prophesize(EntityRepositoryInterface::class);
    $this->staticContext = new ContextMapper($this->entityRepository->reveal());

    $container = new ContainerBuilder();
    $container->set('typed_data_manager', $this->typedDataManager->reveal());
    \Drupal::setContainer($container);
  }

  /**
   * @covers ::getContextValues
   */
  public function testGetContextValues() {
    $input = [];
    $actual = $this->staticContext->getContextValues($input);
    $this->assertEquals([], $actual);
  }

  /**
   * @covers ::getContextValues
   */
  public function testGetContextValuesContext() {
    $data_definition = DataDefinition::createFromDataType('integer');
    $typed_data = IntegerData::createInstance($data_definition);
    $this->typedDataManager->createDataDefinition('integer')->willReturn($data_definition);
    $this->typedDataManager->getDefaultConstraints($data_definition)->willReturn([]);
    $this->typedDataManager->create($data_definition, 5)->willReturn($typed_data);

    $input = [
      'foo' => [
        'label' => 'Foo',
        'description' => NULL,
        'type' => 'integer',
        'value' => 5,
      ],
    ];
    $expected = new Context(new ContextDefinition('integer', 'Foo'), 5);
    $actual = $this->staticContext->getContextValues($input)['foo'];
    $this->assertEquals($expected, $actual);
  }

  /**
   * @covers ::getContextValues
   */
  public function testGetContextValuesEntityContext() {
    $input = [
      'foo' => [
        'label' => 'Foo',
        'description' => NULL,
        'type' => 'entity:node',
        'value' => 'the_node_uuid',
      ],
    ];
    $expected = new EntityLazyLoadContext(new ContextDefinition('entity:node', 'Foo'), $this->entityRepository->reveal(), 'the_node_uuid');
    $actual = $this->staticContext->getContextValues($input)['foo'];
    $this->assertEquals($expected, $actual);
  }

}
