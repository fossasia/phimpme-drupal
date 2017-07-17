<?php

namespace Drupal\Tests\ctools\Kernel;

use Drupal\ctools\SerializableTempstore;
use Drupal\KernelTests\KernelTestBase;
use Symfony\Component\HttpFoundation\Request;

/**
 * Tests the serializable tempstore service.
 *
 * @group ctools
 */
class SerializableTempstoreTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  public static $modules = ['ctools', 'system', 'user'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();
    $this->installSchema('system', ['key_value_expire']);
  }

  /**
   * Tests serializing a serializable temp store object.
   */
  public function testSerializableTempStore() {
    $store = $this->container
      ->get('ctools.serializable.tempstore.factory')
      ->get('foobar');

    // Add an unserializable request to the request stack. If the tempstore
    // didn't use DependencySerializationTrait, the exception would be thrown
    // when we try to serialize the tempstore.
    $request = $this->prophesize(Request::class);
    $request->willImplement('\Serializable');
    $request->serialize()->willThrow(new \LogicException('Not cool, bruh!'));
    $this->container->get('request_stack')->push($request->reveal());

    $this->assertInstanceOf(SerializableTempstore::class, $store);
    /** @var SerializableTempstore $store */

    $store = serialize($store);
    $this->assertInternalType('string', $store);
    $this->assertNotEmpty($store, 'The tempstore was serialized.');

    $store = unserialize($store);
    $this->assertInstanceOf(SerializableTempstore::class, $store, 'The tempstore was unserialized.');

    $request_stack = $this->getObjectAttribute($store, 'requestStack');
    $this->assertSame(
      $this->container->get('request_stack'),
      $request_stack,
      'The request stack was pulled from the container during unserialization.'
    );
    $this->assertSame($request->reveal(), $request_stack->pop());
  }

}
