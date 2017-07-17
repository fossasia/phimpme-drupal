<?php

namespace Drupal\services\StackMiddleware;

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\HttpKernelInterface;

/**
 * Class which extracts accept headers and sets Request formats accordingly to
 * at least allow for accept header variation when needed. This class respects
 * core's usage of request format and attempts to communicate it generically.
 * It should leave the Request format NULL when an unrecognized header is
 * submitted. Drupal core will later set a default of html. If a recognized
 * format comes through, it's set at this point to allow the PageCache
 * http_middleware to properly cache by format.
 */
class FormatSetter implements HttpKernelInterface {

  /**
   * The wrapped HTTP kernel.
   *
   * @var \Symfony\Component\HttpKernel\HttpKernelInterface
   */
  protected $httpKernel;

  /**
   * Constructs a PageCache object.
   *
   * @param \Symfony\Component\HttpKernel\HttpKernelInterface $http_kernel
   *   The decorated kernel.
   */
  public function __construct(HttpKernelInterface $http_kernel) {
    $this->httpKernel = $http_kernel;
  }

  /**
   * {@inheritdoc}
   */
  public function handle(Request $request, $type = self::MASTER_REQUEST, $catch = TRUE) {
    if ($request->headers->has('Accept')) {
      $request->setRequestFormat($request->getFormat($request->headers->get('Accept')));
    }

    return $this->httpKernel->handle($request, $type, $catch);
  }

}
