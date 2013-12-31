<?php

/**
 *  @file
 *  XML data retrieval and storage API for Media.
 */

/**
 *  A wrapper around simplexml to retrieve a given XML file.
 *
 *  @param $url
 *    The URL to the XML to retrieve.
 *  @param $display_errors
 *    Optional; if TRUE, then we'll display errors to the end user. They'll be
 *    logged to the watchdog in any case.
 *  @param $refresh
 *    Optional; if TRUE, then we'll force a new load of the XML. Otherwise,
 *    a cached version will be retrieved if possible.
 *  @return
 *    A fully populated object, or FALSE on an error.
 */
function _media_retrieve_xml($url, $display_errors = FALSE, $refresh = FALSE) {
  $xmls = &drupal_static(__FUNCTION__, array());

  // Return our cached XML if allowed, and it exists.
  if (!$refresh && isset($xmls[$url])) {
    return $xmls[$url];
  }
  elseif (!$refresh && $cache = cache_get('media:xml:' . $url, 'cache_media_xml')) {
    $xmls[$url] = $cache->data;
    return $xmls[$url];
  }

  // Enable user error handling.
  libxml_use_internal_errors(TRUE);

  // Load the document
  $xml = simplexml_load_file($url);

  if (!$xml) {
    foreach (libxml_get_errors() as $error) {
      $params = array('%url' => $url, '%error' => $error->message);
      // Handle errors here.
      if ($display_errors) {
        drupal_set_message(t('Error retrieving XML from %url: %error', $params), 'error');
      }
      watchdog('media', 'Error retrieving XML from %url: %error', $params, WATCHDOG_WARNING);
    }

    // Clear our error cache.
    libxml_clear_errors();

    // Set the static cache, but not Drupal's cache, so we can attempt to
    // retrieve the file another time if possible.
    $xmls[$url] = FALSE;
  }
  else {
    $xmls[$url] = _media_unserialize_xml($xml);
    cache_set('media:xml:' . $url, $xmls[$url], 'cache_media_xml', media_variable_get('xml_cache_expire', 3600));
  }

  return $xmls[$url];
}

/**
 *  Recursively converts a SimpleXMLElement object into an array.
 *  @param $xml
 *    The original XML object.
 */
function _media_unserialize_xml($xml) {
  if ($xml instanceof SimpleXMLElement) {
    $xml = (array) $xml;
  }
  if (is_array($xml)) {
    foreach ($xml as $key => $item) {
      $xml[$key] = _media_unserialize_xml($item);
    }
  }
  return $xml;
}
