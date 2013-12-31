<?php

/**
 * @todo Move this code to the Media module's media.pages.inc file when it is
 *   ready to be committed to CVS.
 */

/**
 * Menu callback; prompt the browser to download the file.
 *
 * This function is very similar to file_download(). Its primary difference is
 * to force the Content-Disposition header to 'attachment', even for images and
 * other file types that could normally be displayed by the browser. This is
 * useful for "Download" links.
 *
 * By invoking hook_file_download() and checking access as file_download() does,
 * this function is compatible with public and private files as well as other
 * custom URI schemes that may be in use.
 *
 * @see file_download()
 */
function media_download($file) {
  $uri = $file->uri;
  $scheme = file_uri_scheme($uri);
  if (file_stream_wrapper_valid_scheme($scheme) && file_exists($uri)) {
    // Let other modules provide headers and controls access to the file.
    // module_invoke_all() uses array_merge_recursive() which merges header
    // values into a new array. To avoid that and allow modules to override
    // headers instead, use array_merge() to merge the returned arrays.
    $headers = array();
    foreach (module_implements('file_download') as $module) {
      $function = $module . '_file_download';
      $result = $function($uri);
      if ($result == -1) {
        return drupal_access_denied();
      }
      if (isset($result) && is_array($result)) {
        $headers = array_merge($headers, $result);
      }
    }
    // Default Content-Type and Content-Length headers, in case no other
    // hook_file_download() implementation set them.
    $name = mime_header_encode($file->filename);
    $type = mime_header_encode($file->filemime);
    $headers += array(
      'Content-Type' => $type . '; name="' . $name . '"',
      'Content-Length' => $file->filesize,
    );
    // Force the Content-Disposition header, regardless of what other
    // hook_file_download() implementations normally set.
    $headers['Content-Disposition'] = 'attachment; filename="' . $name . '"';
    file_transfer($uri, $headers);
  }
}
