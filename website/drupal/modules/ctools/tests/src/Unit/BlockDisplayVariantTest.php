<?php

namespace Drupal\Tests\ctools\Unit;

use Drupal\Component\Uuid\UuidInterface;
use Drupal\Core\Block\BlockManager;
use Drupal\Core\Condition\ConditionManager;
use Drupal\Core\Plugin\Context\ContextHandlerInterface;
use Drupal\Core\Form\FormState;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Utility\Token;
use Drupal\ctools\Plugin\DisplayVariant\BlockDisplayVariant;
use Drupal\Tests\UnitTestCase;

/**
 * Tests the block display variant plugin.
 *
 * @coversDefaultClass \Drupal\ctools\Plugin\DisplayVariant\BlockDisplayVariant
 *
 * @group CTools
 */
class BlockDisplayVariantTest extends UnitTestCase {

  /**
   * Tests the submitConfigurationForm() method.
   *
   * @covers ::submitConfigurationForm
   *
   * @dataProvider providerTestSubmitConfigurationForm
   */
  public function testSubmitConfigurationForm($values) {
    $account = $this->prophesize(AccountInterface::class);
    $context_handler = $this->prophesize(ContextHandlerInterface::class);
    $uuid_generator = $this->prophesize(UuidInterface::class);
    $token = $this->prophesize(Token::class);
    $block_manager = $this->prophesize(BlockManager::class);
    $condition_manager = $this->prophesize(ConditionManager::class);

    $display_variant = new TestBlockDisplayVariant([], '', [], $context_handler->reveal(), $account->reveal(), $uuid_generator->reveal(), $token->reveal(), $block_manager->reveal(), $condition_manager->reveal());

    $form = [];
    $form_state = (new FormState())->setValues($values);
    $display_variant->submitConfigurationForm($form, $form_state);
    $this->assertSame($values['label'], $display_variant->label());
  }

  /**
   * Provides data for testSubmitConfigurationForm().
   */
  public function providerTestSubmitConfigurationForm() {
    $data = [];
    $data[] = [
      [
        'label' => 'test_label1',
      ],
    ];
    $data[] = [
      [
        'label' => 'test_label2',
        'blocks' => ['foo1' => []],
      ],
    ];
    $data[] = [
      [
        'label' => 'test_label3',
        'blocks' => ['foo1' => [], 'foo2' => []],
      ],
    ];
    return $data;
  }

}

class TestBlockDisplayVariant extends BlockDisplayVariant {

  /**
   * {@inheritdoc}
   */
  public function build() {
    return [];
  }

  public function getRegionNames() {
    return [
      'top' => 'Top',
      'bottom' => 'Bottom',
    ];
  }

}
