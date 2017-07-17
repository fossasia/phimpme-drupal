<?php

namespace Drupal\Tests\ctools_block\Functional;

use Drupal\Tests\BrowserTestBase;

/**
 * Tests the entity field block.
 *
 * @group ctools_block
 */
class EntityFieldBlockTest extends BrowserTestBase {

  /**
   * {@inheritdoc}
   */
  public static $modules = ['block', 'ctools_block', 'ctools_block_field_test'];

  /**
   * Tests using the node body field in a block.
   */
  public function testBodyField() {
    $block = $this->drupalPlaceBlock('entity_field:node:body', [
      'formatter' => [
        'type' => 'text_default',
      ],
      'context_mapping' => [
        'entity' => '@node.node_route_context:node',
      ],
    ]);
    $node = $this->drupalCreateNode(['type' => 'ctools_block_field_test']);
    $this->drupalGet('node/' . $node->id());
    $assert = $this->assertSession();
    $assert->pageTextContains($block->label());
    $assert->pageTextContains($node->body->value);

    $node->set('body', NULL)->save();
    $this->getSession()->reload();
    // The block should not appear if there is no value in the field.
    $assert->pageTextNotContains($block->label());
  }

  /**
   * Tests that empty image fields will still render their default value.
   */
  public function testEmptyImageField() {
    $source = \Drupal::moduleHandler()->getModule('image')->getPath() . '/sample.png';
    file_unmanaged_copy($source, 'public://sample.png');

    /** @var \Drupal\file\FileInterface $file */
    $file = \Drupal::entityTypeManager()
      ->getStorage('file')
      ->create([
        'uri' => 'public://sample.png',
      ]);
    $file->save();

    /** @var \Drupal\field\FieldConfigInterface $field */
    $field = \Drupal::entityTypeManager()
      ->getStorage('field_config')
      ->load('node.ctools_block_field_test.field_image');
    $settings = $field->getSettings();
    $settings['default_image']['uuid'] = $file->uuid();
    $field->set('settings', $settings)->save();

    $this->drupalPlaceBlock('entity_field:node:field_image', [
      'formatter' => [
        'type' => 'image_image',
      ],
      'context_mapping' => [
        'entity' => '@node.node_route_context:node',
      ],
    ]);

    $node = $this->drupalCreateNode(['type' => 'ctools_block_field_test']);
    $this->drupalGet('node/' . $node->id());

    $url = $file->getFileUri();
    $url = file_create_url($url);
    $url = file_url_transform_relative($url);
    $this->assertSession()->responseContains('src="' . $url . '"');
  }

  /**
   * Tests using the node uid base field in a block.
   */
  public function testNodeBaseFields() {
    $block = $this->drupalPlaceBlock('entity_field:node:title', [
      'formatter' => [
        'type' => 'string',
      ],
      'context_mapping' => [
        'entity' => '@node.node_route_context:node',
      ],
    ]);
    $node = $this->drupalCreateNode(['type' => 'ctools_block_field_test', 'uid' => 1]);
    $this->drupalGet('node/' . $node->id());
    $assert = $this->assertSession();
    $assert->pageTextContains($block->label());
    $assert->pageTextContains($node->getTitle());
  }

  /**
   * Tests that we are setting the render cache metadata correctly.
   */
  public function testRenderCache() {
    $this->drupalPlaceBlock('entity_field:node:body', [
      'formatter' => [
        'type' => 'text_default',
      ],
      'context_mapping' => [
        'entity' => '@node.node_route_context:node',
      ],
    ]);
    $a = $this->drupalCreateNode(['type' => 'ctools_block_field_test']);
    $b = $this->drupalCreateNode(['type' => 'ctools_block_field_test']);

    $assert = $this->assertSession();
    $this->drupalGet('node/' . $a->id());
    $assert->pageTextContains($a->body->value);
    $this->drupalGet('node/' . $b->id());
    $assert->pageTextNotContains($a->body->value);
    $assert->pageTextContains($b->body->value);

    $text = 'This is my text. Are you not entertained?';
    $a->body->value = $text;
    $a->save();
    $this->drupalGet('node/' . $a->id());
    $assert->pageTextContains($text);
  }

}
