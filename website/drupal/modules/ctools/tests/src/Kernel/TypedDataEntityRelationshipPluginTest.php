<?php

namespace Drupal\Tests\ctools\Kernel;

use Drupal\node\Entity\NodeType;
use Drupal\user\Entity\User;

/**
 * @coversDefaultClass \Drupal\ctools\Plugin\Relationship\TypedDataEntityRelationship
 * @group CTools
 */
class TypedDataEntityRelationshipPluginTest extends RelationshipsTestBase {

    /**
   * @covers ::getName
   */
  public function testRelationshipName() {
    /** @var \Drupal\ctools\Plugin\RelationshipInterface $nid_plugin */
    $type_plugin = $this->relationshipManager->createInstance('typed_data_entity_relationship:entity:node:type');
    $this->assertSame('type', $type_plugin->getName());

    /** @var \Drupal\ctools\Plugin\RelationshipInterface $uuid_plugin */
    $uid_plugin = $this->relationshipManager->createInstance('typed_data_entity_relationship:entity:node:uid');
    $this->assertSame('uid', $uid_plugin->getName());
  }

  /**
   * @covers ::getRelationship
   */
  public function testRelationship() {
    /** @var \Drupal\ctools\Plugin\RelationshipInterface $type_plugin */
    $type_plugin = $this->relationshipManager->createInstance('typed_data_entity_relationship:entity:node:type');
    $type_plugin->setContextValue('base', $this->entities['node1']);
    $relationship = $type_plugin->getRelationship();
    $this->assertTrue($relationship->getContextValue() instanceof NodeType);
    $this->assertSame('entity:node_type', $relationship->getContextDefinition()->getDataType());

    /** @var \Drupal\ctools\Plugin\RelationshipInterface $uid_plugin */
    $uid_plugin = $this->relationshipManager->createInstance('typed_data_entity_relationship:entity:node:uid');
    $uid_plugin->setContextValue('base', $this->entities['node3']);
    $relationship = $uid_plugin->getRelationship();
    $this->assertTrue($relationship->getContextValue() instanceof User);
    $this->assertSame('entity:user', $relationship->getContextDefinition()->getDataType());
  }

}
