<?php

/**
 * A base class for Resource Stream Wrappers.
 *
 * This class provides a complete stream wrapper implementation. It passes
 * incoming  URL's through an interpolation method then recursively calls
 * the invoking PHP filesystem function.
 *
 * MediaReadOnlyStreamWrapper implementations need to override at least the
 * interpolateUrl method to rewrite the URL before is it passed back into the
 * calling function.
 */
abstract class MediaReadOnlyStreamWrapper implements DrupalStreamWrapperInterface {
  protected $parameters = array();
  protected $base_url = NULL;
  private $_DEBUG_MODE = NULL;

  public function get_parameters() {
    return $this->parameters;
  }

  // As part of the inode protection mode returned by stat(), identifies the
  // file as a regular file, as opposed to a directory, symbolic link, or other
  // type of "file".
  // @see http://linux.die.net/man/2/stat
  const S_IFREG = 0100000;

  /**
   * "Template" for stat calls. All elements must be initialized.
   * @var array
   */
  protected $_stat = array(
    0 => 0, // device number
    'dev' => 0,
    1 => 0, // inode number
    'ino' => 0,
    // inode protection mode. file_unmanaged_delete() requires is_file() to
    // return TRUE.
    2 => self::S_IFREG,
    'mode' => self::S_IFREG,
    3 => 0, // number of links
    'nlink' => 0,
    4 => 0, // userid of owner
    'uid' => 0,
    5 => 0, // groupid of owner
    'gid' => 0,
    6 => -1, // device type, if inode device *
    'rdev' => -1,
    7 => 0, // size in bytes
    'size' => 0,
    8 => 0, // time of last access (Unix timestamp)
    'atime' => 0,
    9 => 0, // time of last modification (Unix timestamp)
    'mtime' => 0,
    10 => 0, // time of last inode change (Unix timestamp)
    'ctime' => 0,
    11 => -1, // blocksize of filesystem IO
    'blksize' => -1,
    12 => -1, // number of blocks allocated
    'blocks' => -1,
  );

  function interpolateUrl() {
    if ($parameters = $this->get_parameters()) {
      return $this->base_url . '?' . http_build_query($parameters);
    }
  }

  /**
   * Returns a web accessible URL for the resource.
   *
   * This function should return a URL that can be embedded in a web page
   * and accessed from a browser. For example, the external URL of
   * "youtube://xIpLd0WQKCY" might be
   * "http://www.youtube.com/watch?v=xIpLd0WQKCY".
   *
   * @return
   *   Returns a string containing a web accessible URL for the resource.
   */
  public function getExternalUrl() {
    return $this->interpolateUrl();
  }

  /**
   * Base implementation of getMimeType().
   */
  static function getMimeType($uri, $mapping = NULL) {
    return 'application/octet-stream';
  }

  /**
   * Base implementation of realpath().
   */
  function realpath() {
    return $this->getExternalUrl();
  }

   /**
   * Stream context resource.
   *
   * @var Resource
   */
  public $context;

  /**
   * A generic resource handle.
   *
   * @var Resource
   */
  public $handle = NULL;

  /**
   * Instance URI (stream).
   *
   * A stream is referenced as "scheme://target".
   *
   * @var String
   */
  protected $uri;

  /**
   * Base implementation of setUri().
   */
  function setUri($uri) {
    $this->uri = $uri;
    $this->parameters = $this->_parse_url($uri);
  }

  /**
   * Base implementation of getUri().
   */
  function getUri() {
    return $this->uri;
  }

  /**
   *  Report an error.
   *  @param $message
   *    The untranslated string to report.
   *  @param $options
   *    An optional array of options to send to t().
   *  @param $display
   *    If TRUE, then we display the error to the user.
   *  @return
   *    We return FALSE, since we sometimes pass that back from the reporting
   *    function.
   */
  private function _report_error($message, $options = array(), $display = FALSE) {
    watchdog('resource', $message, $options, WATCHDOG_ERROR);
    if ($display) {
      drupal_set_message(t($message, $options), 'error');
    }
    return FALSE;
  }

  private function _debug($message, $type = 'status') {
    if ($this->_DEBUG_MODE) {
      drupal_set_message($message, $type);
    }
  }

  /**
   *  Returns an array of any parameters stored in the URL's path.
   *  @param $url
   *    The URL to parse, such as youtube://v/[video-code]/t/[tags+more-tags].
   *  @return
   *    An associative array of all the parameters in the path,
   *    or FALSE if the $url is ill-formed.
   */
  protected function _parse_url($url) {
    $path = explode('://', $url);
    $parts = explode('/',  $path[1]);
    $params = array();
    $count = 0;
    $total = count($parts);
    if (!$total || ($total % 2)) {
      // If we have no parts, or an odd number of parts, it's malformed.
      return FALSE;
    }
    while ($count < $total) {
      // We iterate count for each step of the assignment to keep us honest.
      $params[$parts[$count++]] = $parts[$count++];
    }
    return $params;
  }

  /**
   * Support for fopen(), file_get_contents(), file_put_contents() etc.
   *
   * @param $path
   *   A string containing the path to the file to open.
   * @param $mode
   *   The file mode ("r", "wb" etc.).
   * @param $options
   *   A bit mask of STREAM_USE_PATH and STREAM_REPORT_ERRORS.
   * @param &$opened_path
   *   A string containing the path actually opened.
   * @return
   *  TRUE if file was opened successfully.
   */
  public function stream_open($url, $mode, $options, &$opened_url) {
    $this->_debug(t('Stream open: %url', array('%url' => $url)));

    // We only handle Read-Only mode by default.
    if ($mode != 'r' && $mode != 'rb') {
      return $this->_report_error('Attempted to open %url as mode: %mode.', array('%url' => $url, '%mode' => $mode), ($options & STREAM_REPORT_ERRORS));
    }

    // We parse a URL as youtube://v/dsyiufo34/t/cats+dogs to store
    // the relevant code(s) in our private array of parameters.
    $this->parameters = $this->_parse_url($url);

    if ($this->parameters === FALSE) {
      return $this->_report_error('Attempted to parse an ill-formed url: %url.', array('%url' => $url), ($options & STREAM_REPORT_ERRORS));
    }

    if ((bool)$this->parameters && ($options & STREAM_USE_PATH)) {
      $opened_url = $url;
    }

    $this->_debug(t('Stream opened: %parameters', array('%parameters' => print_r($this->parameters, TRUE))));

    return (bool)$this->parameters;
  }

  // Undocumented PHP stream wrapper method.
  function stream_lock($operation) {
    return FALSE;
  }

  /**
   * Support for fread(), file_get_contents() etc.
   *
   * @param $count
   *    Maximum number of bytes to be read.
   * @return
   *  The string that was read, or FALSE in case of an error.
   */
  public function stream_read($count) {
    return FALSE;
  }

  /**
   * Support for fwrite(), file_put_contents() etc.
   *
   * @param $data
   *   The string to be written.
   * @return
   *   The number of bytes written.
   */
  public function stream_write($data) {
    return FALSE;
  }

  /**
   * Support for feof().
   *
   * @return
   *   TRUE if end-of-file has been reached.
   */
  public function stream_eof() {
    return FALSE;
  }

  /**
   * Support for fseek().
   *
   * @param $offset
   *   The byte offset to got to.
   * @param $whence
   *   SEEK_SET, SEEK_CUR, or SEEK_END.
   * @return
   *   TRUE on success
   */
  public function stream_seek($offset, $whence) {
    return FALSE;
  }

  /**
   * Support for fflush().
   *
   * @return
   *   TRUE if data was successfully stored (or there was no data to store).
   */
  public function stream_flush() {
    return FALSE;
  }

  /**
   * Support for ftell().
   *
   * @return
   *   The current offset in bytes from the beginning of file.
   */
  public function stream_tell() {
    return FALSE;
  }

  /**
   * Support for fstat().
   *
   * @return
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   */
  public function stream_stat() {
    return $this->_stat;
  }

  /**
   * Support for fclose().
   *
   * @return
   *   TRUE if stream was successfully closed.
   */
  public function stream_close() {
    return TRUE;
  }


  /**
   * Support for unlink().
   *
   * @param $uri
   *   A string containing the uri to the resource to delete.
   * @return
   *   TRUE if resource was successfully deleted.
   * @see http://php.net/manual/en/streamwrapper.unlink.php
   */
//   public function unlink($uri) {
//     $this->uri = $uri;
//     return unlink($this->getLocalPath());
//   }

  /**
   * Support for rename().
   *
   * @param $from_uri,
   *   The uri to the file to rename.
   * @param $to_uri
   *   The new uri for file.
   * @return
   *   TRUE if file was successfully renamed.
   * @see http://php.net/manual/en/streamwrapper.rename.php
   */
//   public function rename($from_uri, $to_uri) {
//     return rename($this->getLocalPath($from_uri), $this->getLocalPath($to_uri));
//   }

  /**
   * Support for mkdir().
   *
   * @param $uri
   *   A string containing the URI to the directory to create.
   * @param $mode
   *   Permission flags - see mkdir().
   * @param $options
   *   A bit mask of STREAM_REPORT_ERRORS and STREAM_MKDIR_RECURSIVE.
   * @return
   *   TRUE if directory was successfully created.
   * @see http://php.net/manual/en/streamwrapper.mkdir.php
   */
//   public function mkdir($uri, $mode, $options) {
//     $this->uri = $uri;
//     $recursive = (bool)($options & STREAM_MKDIR_RECURSIVE);
//     if ($recursive) {
//       // $this->getLocalPath() fails if $uri has multiple levels of directories
//       // that do not yet exist.
//       $localpath = $this->getDirectoryPath() . '/' . file_uri_target($uri);
//     }
//     else {
//       $localpath = $this->getLocalPath($uri);
//     }
//     if ($options & STREAM_REPORT_ERRORS) {
//       return mkdir($localpath, $mode, $recursive);
//     }
//     else {
//       return @mkdir($localpath, $mode, $recursive);
//     }
//   }

  /**
   * Support for rmdir().
   *
   * @param $uri
   *   A string containing the URI to the directory to delete.
   * @param $options
   *   A bit mask of STREAM_REPORT_ERRORS.
   * @return
   *   TRUE if directory was successfully removed.
   * @see http://php.net/manual/en/streamwrapper.rmdir.php
   */
//   public function rmdir($uri, $options) {
//     $this->uri = $uri;
//     if ($options & STREAM_REPORT_ERRORS) {
//       return rmdir($this->getLocalPath());
//     }
//     else {
//       return @rmdir($this->getLocalPath());
//     }
//   }

  /**
   * Support for stat().
   *
   * @param $url
   *   A string containing the url to get information about.
   * @param $flags
   *   A bit mask of STREAM_URL_STAT_LINK and STREAM_URL_STAT_QUIET.
   * @return
   *   An array with file status, or FALSE in case of an error - see fstat()
   *   for a description of this array.
   */
  public function url_stat($url, $flags) {
    return $this->stream_stat();
  }

  /**
   * Support for opendir().
   *
   * @param $url
   *   A string containing the url to the directory to open.
   * @param $options
   *   Unknown (parameter is not documented in PHP Manual).
   * @return
   *   TRUE on success.
   */
  public function dir_opendir($url, $options) {
    return FALSE;
  }

  /**
   * Support for readdir().
   *
   * @return
   *   The next filename, or FALSE if there are no more files in the directory.
   */
  public function dir_readdir() {
    return FALSE;
  }

  /**
   * Support for rewinddir().
   *
   * @return
   *   TRUE on success.
   */
  public function dir_rewinddir() {
    return FALSE;
  }

  /**
   * Support for closedir().
   *
   * @return
   *   TRUE on success.
   */
  public function dir_closedir() {
    return FALSE;
  }

  public function getDirectoryPath() {
    return '';
  }

  /**
   * DrupalStreamWrapperInterface requires that these methods be implemented,
   * but none of them apply to a read-only stream wrapper. On failure they
   * are expected to return FALSE.
   */

  public function unlink($uri) {
    // Although the remote file itself can't be deleted, return TRUE so that
    // file_delete() can remove the file record from the database.
    return TRUE;
  }
  public function rename($from_uri, $to_uri) {
    return FALSE;
  }
  public function mkdir($uri, $mode, $options) {
    return FALSE;
  }
  public function rmdir($uri, $options) {
    return FALSE;
  }
  public function chmod($mode) {
    return FALSE;
  }
  public function dirname($uri = NULL) {
    return FALSE;
  }

}
