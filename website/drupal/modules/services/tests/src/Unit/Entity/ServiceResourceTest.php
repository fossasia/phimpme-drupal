<?php

namespace Drupal\Tests\services\Unit\Entity;

use Drupal\services\Entity\ServiceResource;
use Drupal\Tests\UnitTestCase;

/**
 * @coversDefaultClass \Drupal\services\Entity\ServiceResource
 *
 * @group services
 */
class ServiceResourceTest extends UnitTestCase {

  public function testCanConstructDefaultResource() {
    /* @var ServiceResource $resource */
    $resource = new ServiceResource(
      [
        'service_plugin_id' => 'test:plugin:id',
        'service_endpoint_id' => 'test_endpoint_id',
        'formats' => ['json'],
        'authentication' => ['cookie'],
        'no_cache' => 0
      ],
      'service_endpoint_resource'
    );

    $this->assertEquals('test_endpoint_id.test.plugin.id', $resource->id(), 'ID constructed successfully.');
    $this->assertEquals(['json'], $resource->getFormats(), 'Formats found.');
    $this->assertEquals(['cookie'], $resource->getAuthentication(), 'Authentication found.');
    $this->assertEquals(0, $resource->getNoCache(), 'Cache setting found.');
  }

  public function testCanConstructResourceNoCacheFalse() {
    /* @var ServiceResource $resource */
    $resource = new ServiceResource(
      [
        'service_plugin_id' => 'test:plugin:id',
        'service_endpoint_id' => 'test_endpoint_id',
        'formats' => [],
        'authentication' => [],
        'no_cache' => false
      ],
      'service_endpoint_resource'
    );

    $this->assertEquals(0, $resource->getNoCache(), 'Cache setting found.');
  }

  public function testCanConstructResourceNoCacheTrue() {
    /* @var ServiceResource $resource */
    $resource = new ServiceResource(
      [
        'service_plugin_id' => 'test:plugin:id',
        'service_endpoint_id' => 'test_endpoint_id',
        'formats' => [],
        'authentication' => [],
        'no_cache' => true
      ],
      'service_endpoint_resource'
    );

    $this->assertEquals(1, $resource->getNoCache(), 'Cache setting found.');
  }

  public function testCanConstructResourceNoCache1() {
    /* @var ServiceResource $resource */
    $resource = new ServiceResource(
      [
        'service_plugin_id' => 'test:plugin:id',
        'service_endpoint_id' => 'test_endpoint_id',
        'formats' => [],
        'authentication' => [],
        'no_cache' => 1
      ],
      'service_endpoint_resource'
    );

    $this->assertEquals(1, $resource->getNoCache(), 'Cache setting found.');
  }

}
