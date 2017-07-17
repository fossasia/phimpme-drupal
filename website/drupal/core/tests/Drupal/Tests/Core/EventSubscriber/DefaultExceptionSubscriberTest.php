<?php

namespace Drupal\Tests\Core\EventSubscriber;

use Drupal\Core\EventSubscriber\DefaultExceptionSubscriber;
use Drupal\Tests\UnitTestCase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpKernel\Event\GetResponseForExceptionEvent;
use Symfony\Component\HttpKernel\Exception\MethodNotAllowedHttpException;
use Symfony\Component\HttpKernel\HttpKernelInterface;

/**
 * @coversDefaultClass \Drupal\Core\EventSubscriber\DefaultExceptionSubscriber
 * @group EventSubscriber
 */
class DefaultExceptionSubscriberTest extends UnitTestCase {

  /**
   * @covers ::onException
   * @covers ::onFormatUnknown
   */
  public function testOnExceptionWithUnknownFormat() {
    $config_factory = $this->getConfigFactoryStub();

    $kernel = $this->prophesize(HttpKernelInterface::class);
    $request = Request::create('/test?_format=bananas');
    $e = new MethodNotAllowedHttpException(['POST', 'PUT'], 'test message');
    $event = new GetResponseForExceptionEvent($kernel->reveal(), $request, 'GET', $e);
    $subscriber = new DefaultExceptionSubscriber($config_factory);
    $subscriber->onException($event);
    $response = $event->getResponse();

    $this->assertInstanceOf(Response::class, $response);
    $this->assertEquals('test message', $response->getContent());
    $this->assertEquals(405, $response->getStatusCode());
    $this->assertEquals('POST, PUT', $response->headers->get('Allow'));
    // Also check that that text/plain content type was added.
    $this->assertEquals('text/plain', $response->headers->get('Content-Type'));
  }

}
