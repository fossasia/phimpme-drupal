<?php

/**
 * @file
 * Hooks provided by Services for the definition of servers.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Triggered when the REST server request a list of available request parsers.
 *
 * @param array $parsers
 *  An associative array of parser callbacks keyed by mime-type.
 * @return void
 */
function hook_rest_server_request_parsers_alter(&$parsers) {
  $parsers['application/json'] = 'RESTServer::parseJSON';
  unset($parsers['application/x-www-form-urlencoded']);
}

/**
 * Triggered when the REST server request a list of supported response formats.
 *
 * @param array $formatters
 *  An associative array of formatter info arrays keyed by type extension. The
 *  formatter info specifies an array of 'mime types' that corresponds to the
 *  output format; a 'view' class that is a subclass of RESTServerView; and
 *  'view arguments' that should be passed to the view when it is created;
 *  a 'model' can also be specified which the controller then must declare
 *  support for to be able to serve data in that format.
 * @return void
 */
function hook_rest_server_response_formatters_alter(&$formatters) {
  /*
   * Sample modifications of the formatters array. Both yaml and
   * rss are formats that already are supported, so the changes are
   * nonsensical but illustrates the proper use of this hook.
   */

  // Add a Yaml response format.
  $formatters['yaml'] = array(
    'mime types' => array('text/plain', 'application/x-yaml', 'text/yaml'),
    'view' => 'RESTServerViewBuiltIn',
    'view arguments' => array('format' => 'yaml'),
  );

  // Add a Rss response format.
  $formatters['rss'] = array(
    'model' => 'ResourceFeedModel',
    'mime types' => array('text/xml'),
    'view' => 'RssFormatView',
  );

  // Remove the jsonp response format.
  unset($formatters['jsonp']);
}

