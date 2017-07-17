<?php

namespace Drupal\Tests\ctools\Kernel;

use Drupal\Core\Language\LanguageInterface;

/**
 * @coversDefaultClass \Drupal\ctools\Plugin\Relationship\TypedDataEntityRelationship
 * @group CTools
 */
class TypedDataLanguageRelationshipPluginTest extends RelationshipsTestBase {

  /**
   * @covers ::getName
   */
  public function testRelationshipName() {
    /** @var \Drupal\ctools\Plugin\RelationshipInterface $langcode_plugin */
    $langcode_plugin = $this->relationshipManager->createInstance('typed_data_language_relationship:entity:node:langcode');
    $this->assertSame('langcode', $langcode_plugin->getName());
  }

  /**
   * @covers ::getRelationship
   *
   * @todo expand to include a new language.
   */
  public function testRelationship() {
    /** @var \Drupal\ctools\Plugin\RelationshipInterface $langcode_plugin */
    $langcode_plugin = $this->relationshipManager->createInstance('typed_data_language_relationship:entity:node:langcode');
    $langcode_plugin->setContextValue('base', $this->entities['node1']);
    $relationship = $langcode_plugin->getRelationship();
    $this->assertTrue($relationship->getContextValue() instanceof LanguageInterface);
    $this->assertSame('en', $relationship->getContextValue()->getId());
  }

}
