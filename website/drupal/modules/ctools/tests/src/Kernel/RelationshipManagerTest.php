<?php

namespace Drupal\Tests\ctools\Kernel;

use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;

/**
 * @coversDefaultClass \Drupal\ctools\Plugin\RelationshipManagerInterface
 * @group CTools
 */
class RelationshipManagerTest extends RelationshipsTestBase {

  /**
   * @covers ::getDefinitions
   */
  public function testRelationshipConstraints() {
    $definitions = $this->relationshipManager->getDefinitions();
    $expected = [
      'Bundle' => [
        0 => "page",
        1 => "foo"
      ]
    ];
    $this->assertSame($expected, $definitions['typed_data_relationship:entity:node:body']['context']['base']->getConstraints());

    // Check that typed data primitive labels are formatted properly.
    $this->assertSame('Body from Page and Foo', (string) $definitions['typed_data_relationship:entity:node:body']['label']);

    // Check that entity relationship labels are formatted properly.
    $this->assertSame('Authored by Entity from Content', (string) $definitions['typed_data_entity_relationship:entity:node:uid']['label']);

    // Check that language relationship labels are formatted properly.
    $this->assertSame('Language Language from Content', (string) $definitions['typed_data_language_relationship:entity:node:langcode']['label']);
  }

  /**
   * @covers ::getDefinitionsForContexts
   */
  public function testRelationshipPluginAvailability() {
    $context_definition = new ContextDefinition('entity:node');
    $contexts = [
      'node' => new Context($context_definition, $this->entities['node1']),
    ];
    $definitions = $this->relationshipManager->getDefinitionsForContexts($contexts);
    //$this->assertTrue(isset($definitions['typed_data_relationship:entity:node:body']));

    $context_definition = new ContextDefinition('entity:node');
    $contexts = [
      'node' => new Context($context_definition, $this->entities['node2']),
    ];
    $definitions = $this->relationshipManager->getDefinitionsForContexts($contexts);
    $this->assertFalse(isset($definitions['typed_data_relationship:entity:node:body']));

    $context_definition = new ContextDefinition('entity:node');
    $contexts = [
      'node' => new Context($context_definition, $this->entities['node3']),
    ];
    $definitions = $this->relationshipManager->getDefinitionsForContexts($contexts);
    //$this->assertTrue(isset($definitions['typed_data_relationship:entity:node:body']));
  }

}
