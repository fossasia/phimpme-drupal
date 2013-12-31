<?php
// $Id$

/**
 * @file
 * Provide formatter class for all standard response formats.
 *
 */

/**
 * Base class for all response format views.
 */
abstract class RESTServerView {
  protected $model;
  protected $arguments;

  function __construct($model, $arguments=array()) {
    $this->model = $model;
    $this->arguments = $arguments;
  }

  public abstract function render();
}

class RESTServerViewBuiltIn extends RESTServerView {
  public function render() {
    switch ($this->arguments['format']) {
      case 'json':
        return $this->render_json($this->model);
      case 'jsonp':
        return $this->render_json($this->model, TRUE);
      case 'php':
        return $this->render_php($this->model);
      case 'xml':
        return $this->render_xml($this->model);
      case 'yaml':
        return $this->render_yaml($this->model);
      case 'bencode':
        return $this->render_bencode($this->model);
    }
    return '';
  }

  private function render_json($data, $jsonp = FALSE) {
    $json = str_replace('\\/', '/', json_encode($data));
    if ($jsonp && isset($_GET['callback'])) {
      return sprintf('%s(%s);', $_GET['callback'], $json);
    }
    return $json;
  }

  private function render_php($data) {
    return serialize($data);
  }

  private function render_yaml($data) {
    module_load_include('php', 'rest_server', 'lib/spyc');
    return Spyc::YAMLDump($data, 4, 60);
  }

  private function render_bencode($data) {
    module_load_include('php', 'rest_server', 'lib/bencode');
    return bencode($data);
  }

  private function render_xml($data) {
    $doc = new DOMDocument('1.0', 'utf-8');
    $root = $doc->createElement('result');
    $doc->appendChild($root);

    $this->xml_recurse($doc, $root, $data);

    return $doc->saveXML();
  }

  private function xml_recurse(&$doc, &$parent, $data) {
    if (is_object($data)) {
      $data = get_object_vars($data);
    }

    if (is_array($data)) {
      $assoc = FALSE || empty($data);
      foreach ($data as $key => $value) {
        if (is_numeric($key)) {
          $key = 'item';
        }
        else {
          $assoc = TRUE;
          $key = preg_replace('/[^A-Za-z0-9_]/', '_', $key);
          $key = preg_replace('/^([0-9]+)/', '_$1', $key);
        }
        $element = $doc->createElement($key);
        $parent->appendChild($element);
        $this->xml_recurse($doc, $element, $value);
      }

      if (!$assoc) {
        $parent->setAttribute('is_array', 'true');
      }
    }
    elseif ($data !== NULL) {
      $parent->appendChild($doc->createTextNode($data));
    }
  }
}