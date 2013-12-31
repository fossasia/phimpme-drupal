<?php

/**
 * @file
 * Class for handling REST calls.
 */

class RESTServer {
  private $endpoint;

  /**
   * Handles the call to the REST server
   *
   * @param string $canonical_path
   * @param string $endpoint_path
   * @return void
   */
  public function handle($canonical_path, $endpoint_path) {
    $this->endpoint_path = $endpoint_path;
    services_set_server_info('resource_uri_formatter', array(&$this, 'uri_formatter'));

    // Determine the request method
    $method = $_SERVER['REQUEST_METHOD'];
    if ($method == 'POST' && isset($_SERVER['HTTP_X_HTTP_METHOD_OVERRIDE'])) {
      $method = $_SERVER['HTTP_X_HTTP_METHOD_OVERRIDE'];
    }
    if ($method == 'POST' && (isset($_GET['_method']) && $_GET['_method'])) {
      $method = $_GET['_method'];
    }
    $path = explode('/', $canonical_path);
    $leaf = array_pop($path);
    $resource_name = array_shift($path);
    // Extract response format info from the path
    $matches = array();
    if ($leaf && preg_match('/^(.+)\.([^\.]+)$/', $leaf, $matches)) {
      $leaf = $matches[1];
      $response_format = $matches[2];
    }
    // Response will vary with accept headers
    // if no format was supplied as path suffix
    if (empty($response_format)) {
      drupal_add_http_header('Vary', 'Accept');
    }

    // Return the leaf to the path array if it's not the resource name
    if (isset($leaf)) {
      if (!$resource_name) {
        $resource_name = $leaf;
      }
      else {
        array_push($path, $leaf);
      }
    }

    $endpoint = services_get_server_info('endpoint', '');
    $endpoint_definition = services_endpoint_load($endpoint);

    // Get the server settings from the endpoint.
    $this->settings = !empty($endpoint_definition->server_settings['rest_server']) ? $endpoint_definition->server_settings['rest_server'] : array();
    // Normalize the settings so that we get the expected structure
    // and sensible defaults.
    $this->settings = rest_server_setup_settings($this->settings);

    $resources = services_get_resources($endpoint);
    $controller = FALSE;
    if (!empty($resource_name) && isset($resources[$resource_name])) {
      $resource = $resources[$resource_name];

      // Get the operation and fill with default values
      $controller = $this->resolveController($resource, $method, $path);
    }
    else {
      //This will stop the 404 from happening when you request just the endpoint.
      if ($endpoint_definition->path == $resource_name) {
        $response = t('Services Endpoint "@name" has been setup successfully.', array('@name' => $endpoint));
        drupal_alter('services_endpoint_response', $response);
        return $response;
      }
      return services_error(t('Could not find resource @name.', array('@name' => $resource_name)), 404);
    }

    if (!$controller) {
      return services_error(t('Could not find the controller.'), 404);
    }

    // Parse the request data
    $data = array();
    $arguments = $this->getControllerArguments($controller, $path, $method, $data);

    $formats = $this->responseFormatters();

    // Negotiate response format based on accept-headers if we
    // don't have a response format
    if (empty($response_format)) {
      $mime_candidates = array();
      $mime_map = array();
      // Add all formatters that accepts raw data, or supports the format model
      foreach ($formats as $format => $formatter) {
        if (!isset($formatter['model']) || $this->supportedControllerModel($controller, $formatter)) {
          foreach ($formatter['mime types'] as $m) {
            $mime_candidates[] = $m;
            $mime_map[$m] = $format;
          }
        }
      }

      // Get the best matching format, default to json
      $response_format = 'json';
      if (isset($_SERVER['HTTP_ACCEPT'])) {
        $mime = $this->mimeParse();
        $mime_type = $mime->best_match($mime_candidates, $_SERVER['HTTP_ACCEPT']);
        $response_format = $mime_map[$mime_type];
      }
    }

    // Check if we support the response format and determine the mime type
    if (empty($mime_type) && isset($formats[$response_format])) {
      $formatter = $formats[$response_format];
      if (!isset($formatter['model']) || $this->supportedControllerModel($controller, $formatter)) {
        $mime_type = $formatter['mime types'][0];
      }
    }

    if (empty($response_format) || empty($mime_type)) {
      return services_error(t('Unknown or unsupported response format.'), 406);
    }

    // Give the model (if any) a opportunity to alter the arguments.
    // This might be needed for the model to ensure that all the required
    // information is requested.
    if (isset($formatter['model'])) {
      $cm = &$controller['models'][$formatter['model']];
      if (!isset($cm['arguments'])) {
        $cm['arguments'] = array();
      }

      // Check if any of the model arguments have been overridden
      if (isset($cm['allow_overrides'])) {
        foreach ($cm['allow_overrides'] as $arg) {
          if (isset($_GET[$formatter['model'] . ':' . $arg])) {
            $cm['arguments'][$arg] = $_GET[$formatter['model'] . ':' . $arg];
          }
        }
      }

      if (isset($cm['class']) && class_exists($cm['class'])) {
        if (method_exists($cm['class'], 'alterArguments')) {
          call_user_func_array($cm['class'] . '::alterArguments', array(&$arguments, $cm['arguments']));
        }
      }
    }

    try {
      $result = services_controller_execute($controller, $arguments);
    }
    catch (ServicesException $e) {
      $errors = $this->handleException($e);
      drupal_alter('rest_server_execute_errors', $errors, $controller, $arguments);
      $result = $errors;
    }
    $formatter = $formats[$response_format];

    // Set the content type and render output
    drupal_add_http_header('Content-type', $mime_type);
    return $this->renderFormatterView($controller, $formatter, $result);
  }

  /**
   * Formats a resource uri
   *
   * @param array $path
   *  An array of strings containing the component parts of the path to the resource.
   * @return string
   *  Returns the formatted resource uri
   */
  public function uri_formatter($path) {
    return url($this->endpoint_path . '/' . join($path, '/'), array(
      'absolute' => TRUE,
    ));
  }

  /**
   * Parses controller arguments from request
   *
   * @param array $controller
   *  The controller definition
   * @param string $path
   * @param string $method
   *  The method used to make the request
   * @param array $sources
   *  An array of the sources used for getting the arguments for the call
   * @return void
   */
  private function getControllerArguments($controller, $path, $method, &$sources = array()) {
    // Get argument sources
    $parameters = $_GET;
    if (isset($parameters['_method'])) {
      unset($parameters['_method']);
    }

    $data = $this->parseRequest($method, $controller);
    drupal_alter('rest_server_request_parsed', $data, $controller);

    $defaults = array(
      'path' => $path,
      'param' => $parameters,
      'data' => $data,
    );
    foreach ($defaults as $key => $data) {
      if (!isset($sources[$key])) {
        $sources[$key] = $data;
      }
    }
    // Map source data to arguments.
    $arguments = array();
    if (isset($controller['args'])) {
      foreach ($controller['args'] as $i => $info) {
        // Fill in argument from source
        if (isset($info['source'])) {
          if (is_array($info['source'])) {
            list($source) = array_keys($info['source']);
            $key = $info['source'][$source];
            if (isset($sources[$source][$key])) {
              $arguments[$i] = $sources[$source][$key];
            }
          }
          else {
            if (isset($sources[$info['source']])) {
              $arguments[$i] = $sources[$info['source']];
            }
          }
          // Convert to array if argument expected to be array.
          if (isset($info['type']) && $info['type'] == 'array' && isset($arguments[$i])) {
            $arguments[$i] = (array)$arguments[$i];
          }
        }
        // When argument isn't set, insert default value if provided or
        // throw a exception if the argument isn't optional.
        if (!isset($arguments[$i])) {
          if (isset($info['default value'])) {
            $arguments[$i] = $info['default value'];
          }
          elseif (!isset($info['optional']) || !$info['optional']) {
            return services_error(t('Missing required argument @arg', array('@arg' => $info['name'])), 401);
          }
          else {
            $arguments[$i] = NULL;
          }
        }
      }
    }
    return $arguments;
  }

  private function parseRequest($method, $controller) {
    switch ($method) {
      case 'POST':
      case 'PUT':
        $type = self::parseContentHeader($_SERVER['CONTENT_TYPE']);

        // Get the mime type for the request, default to form-urlencoded
        if (isset($type['value'])) {
          $mime = $type['value'];
        }
        else {
          $mime = 'application/x-www-form-urlencoded';
        }

        // Get the parser for the mime type
        $parser = $this->requestParser($mime, $controller);
        if (!$parser) {
          return services_error(t('Unsupported request content type @mime', array('@mime' => $mime)), 406);
        }
        // php://input is not available with enctype="multipart/form-data".
        // see http://php.net/manual/en/wrappers.php.php
        if ($mime == 'multipart/form-data') {
          $data = call_user_func($parser);
        }
        else {
          // Read the raw input stream
          if (module_exists('inputstream')) {
            $handle = fopen('drupal://input', 'r');
          }
          else {
            $handle = fopen('php://input', 'r');
          }

          if ($handle) {
            $data = call_user_func($parser, $handle);
            fclose($handle);
          }
        }
        return $data;

      default:
        return array();
    }
  }

  public static function parseContentHeader($value) {
    $ret_val = array();
    $value_pattern = '/^([^;]+)(;\s*(.+)\s*)?$/';
    $param_pattern = '/([a-z]+)=(([^\"][^;]+)|(\"(\\\"|[^"])+\"))/';
    $vm=array();

    if (preg_match($value_pattern, $value, $vm)) {
      $ret_val['value'] = $vm[1];
      if (count($vm)>2) {
        $pm = array();
        if (preg_match_all($param_pattern, $vm[3], $pm)) {
          $pcount = count($pm[0]);
          for ($i=0; $i<$pcount; $i++) {
            $value = $pm[2][$i];
            if (drupal_substr($value, 0, 1) == '"') {
              $value = stripcslashes(drupal_substr($value, 1, mb_strlen($value)-2));
            }
            $ret_val['param'][$pm[1][$i]] = $value;
          }
        }
      }
    }

    return $ret_val;
  }

  public static function contentFromStream($handle) {
    $content = '';
    while (!feof($handle)) {
      $content .= fread($handle, 8192);
    }
    return $content;
  }

  public static function fileRecieve($handle, $validators = array()) {
    $validators['file_validate_name_length'] = array();

    $type = RESTServer::parseContentHeader($_SERVER['CONTENT_TYPE']);
    $disposition = RESTServer::parseContentHeader($_SERVER['HTTP_CONTENT_DISPOSITION']);

    $filename = file_munge_filename(trim(basename(($disposition['params']['filename']))));

    // Rename potentially executable files, to help prevent exploits.
    if (preg_match('/\.(php|pl|py|cgi|asp|js)$/i', $filename) && (drupal_substr($filename, -4) != '.txt')) {
      $type['value'] = 'text/plain';
      $filename .= '.txt';
    }

    $filepath = file_destination(file_create_path(file_directory_temp() . '/' . $filename), FILE_EXISTS_RENAME);
    $file = (object)array(
      'uid' => 0,
      'filename' => $filename,
      'filepath' => $filepath,
      'filemime' => $type['value'],
      'status' => FILE_STATUS_TEMPORARY,
      'timestamp' => time(),
    );
    RESTServer::streamToFile($handle, $filepath);
    $file->filesize = filesize($filepath);

    // Call the validation functions.
    $errors = array();
    foreach ($validators as $function => $args) {
      array_unshift($args, $file);
      $errors = array_merge($errors, call_user_func_array($function, $args));
    }
    if (!empty($errors)) {
      return services_error(t('Errors while validating the file - @errors', array('@errors' => implode(" ", $errors))), 406);
    }

    drupal_write_record('files', $file);

    return $file;
  }

  public static function streamToFile($source, $file) {
    $fp = fopen($file, 'w');
    if ($fp) {
      self::streamCopy($source, $fp);
      fclose($fp);
      return TRUE;
    }
    return FALSE;
  }

  public static function streamCopy($source, $destination) {
    while (!feof($source)) {
      $content = fread($source, 8192);
      fwrite($destination, $content);
    }
  }

  private function renderFormatterView($controller, $formatter, $result) {
    // Wrap the results in a model class if required by the formatter
    if (isset($formatter['model'])) {
      $cm = $controller['models'][$formatter['model']];
      $model_arguments = isset($cm['arguments'])?$cm['arguments']:array();

      $model_class = new ReflectionClass($cm['class']);
      $result = $model_class->newInstanceArgs(array($result, $model_arguments));
    }

    $view_class = new ReflectionClass($formatter['view']);
    $view_arguments = isset($formatter['view arguments'])?$formatter['view arguments']:array();
    $view = $view_class->newInstanceArgs(array($result, $view_arguments));
    return $view->render();
  }

  /**
   * Get best match parser for $controller based on $mime type.
   */
  private function requestParser($mime, $controller = NULL) {
    // Check if the controller has declared support for parsing the mime type.
    if ($controller && !empty($controller['rest request parsers'])) {
      $parser = $this->matchParser($mime, $controller['rest request parsers']);
      if ($parser) {
        return $parser;
      }
    }

    $parsers = rest_server_request_parsers();

    // Remove parsers that have been disabled for this endpoint.
    foreach (array_keys($parsers) as $key) {
      if (!$this->settings['parsers'][$key]) {
        unset($parsers[$key]);
      }
    }

    return $this->matchParser($mime, $parsers);
  }

  /**
   * Create a instance of the Mimeparse utility class.
   *
   * @return Mimeparse
   */
  private function mimeParse() {
    static $mimeparse;
    if (!$mimeparse) {
      module_load_include('php', 'rest_server', 'lib/mimeparse');
      $mimeparse = new Mimeparse();
    }
    return $mimeparse;
  }

  /**
   * Matches a mime-type against a set of parsers.
   *
   * @param string $mime
   *  The mime-type of the request.
   * @param array $parsers
   *  An associative array of parser callbacks keyed by mime-type.
   * @return mixed
   *  Returns a parser callback or FALSE if no match was found.
   */
  private function matchParser($mime, $parsers) {
    $mimeparse = $this->mimeParse();
    $mime_type = $mimeparse->best_match(array_keys($parsers), $mime);

    return ($mime_type) ? $parsers[$mime_type] : FALSE;
  }

  public static function parseURLEncoded($handle) {
    parse_str(self::contentFromStream($handle), $data);
    return $data;
  }

  public static function parsePHP($handle) {
    return unserialize(self::contentFromStream($handle));
  }

  public static function parseJSON($handle) {
    return json_decode(self::contentFromStream($handle), TRUE);
  }

  public static function parseFile($handle) {
    return self::contentFromStream($handle);
  }

  public static function parseMultipart() {
    return $_POST;
  }

  public static function parseYAML($handle) {
    module_load_include('php', 'rest_server', 'lib/spyc');
    return Spyc::YAMLLoadString(self::contentFromStream($handle));
  }

  private function responseFormatters($format = NULL) {
    $formatters = rest_server_response_formatters();

    // Remove formatters that have been disabled for this endpoint.
    foreach (array_keys($formatters) as $key) {
      if (!$this->settings['formatters'][$key]) {
        unset($formatters[$key]);
      }
    }

    if ($format) {
      return isset($formatters[$format]) ? $formatters[$format] : FALSE;
    }

    return $formatters;
  }

  private function supportedControllerModel($controller, $format) {
    if (
      // The format uses models
      isset($format['model']) &&
      // The controller provides models
      isset($controller['models']) &&
      // The controller supports the model required by the format
      isset($controller['models'][$format['model']])) {
        return $controller['models'][$format['model']];
    }
  }

  private function resolveController($resource, $method, $path) {
    $pc = count($path);
    // Use the index handler for all empty path request, except on POST
    if (!$pc && $method!='POST') {
      return isset($resource['index']) ? $resource['index'] : NULL;
    }
    // Detect the standard crud operations
    elseif (($pc == 1 && ($method == 'GET' || $method == 'PUT' || $method == 'DELETE')) || ($pc == 0 && $method == 'POST')) {
      $action_mapping = array(
        'GET' => 'retrieve',
        'POST' => 'create',
        'PUT' => 'update',
        'DELETE' => 'delete',
      );
      if (isset($resource[$action_mapping[$method]])) {
        $controller = $resource[$action_mapping[$method]];
        if (isset($resource['file'])) {
          $controller['file'] = $resource['file'];
        }
        return $controller;
      }
    }
    // Detect relationship requests
    elseif ($pc >= 2 && $method == 'GET') {
      if (isset($resource['relationships']) && $resource['relationships'][$path[1]]) {
        $relationship = $resource['relationships'][$path[1]];
        return $relationship;
      }
    }
    // Detect action requests
    elseif ($pc == 1 && $method == 'POST') {
      return $resource['actions'][$path[0]];
    }
    // Detect action requests targeted at specific resources
    elseif ($pc >= 2 && $method == 'POST') {
      $action = $resource['targeted_actions'][$path[1]];
      return $action;
    }
  }
  function handleException($exception){
    $code = $exception->getCode();
    switch ($code) {
      case 204:
        drupal_add_http_header('HTTP/1.0 204 No Content', $exception->getMessage());
        break;
      case 304:
        drupal_add_http_header('HTTP/1.0 304 Not Modified', $exception->getMessage());
        break;
      case 401:
        drupal_add_http_header('HTTP/1.0 401 Unauthorized', $exception->getMessage());
        break;
      case 404:
        drupal_add_http_header('HTTP/1.0 404 Not found', $exception->getMessage());
        break;
      case 406:
        drupal_add_http_header('HTTP/1.0 406 Not Acceptable', $exception->getMessage());
        break;
      default:
        if ($code >= 400 && $code < 600) {
          drupal_add_http_header('HTTP/1.0 ' . $code, $exception->getMessage());
        }
        else {
          drupal_add_http_header('HTTP/1.0 500 An error occurred: (' . $code . ')',  $exception->getMessage());
        }
        break;
    }
    if(method_exists($exception,'getData')){
      return $exception->getData();
    }
  }
}
