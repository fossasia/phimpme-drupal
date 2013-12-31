<?php
/**
 * @file
 * Advanced variables.
 */

/**
 * Implements hook_variable_group_info().
 */
function variable_advanced_variable_group_info() {
  $groups['advanced'] = array(
    'title' => t('Advanced options'),
    'description' => t('Advanced settings not usually exposed. Changing these variables may seriously break your site so make sure you know what you do.'),
  );
  return $groups;
}

/**
 * Implements hook_variable_info().
 */
function variable_advanced_variable_info($options) {
  // Bootstrap caching options
  $variables['page_cache_invoke_hooks'] = array(
    'title' => t('Cache invoke hooks'),
    'type' => 'enable',
    'default' => 1,
    'group' => 'advanced',
    'description' => T('Invoke <em>boot</em> and <em>exit</em> hooks when the page is served from cache.'),
  );
  $variables['actions_max_stack'] = array(
    'title' => t('Actions recursion level'),
    'type' => 'number',
    'default' => 35,
    'group' => 'advanced',
    'description' => t('Maximum recursion level for actions before the execution is aborted.', array(), $options),
  ); 
  // Bootstrap language variables.
  $variables['language_count'] = array(
    'title' => t('Language count'),
    'type' => 'number',
    'default' => 1,
    'group' => 'advanced',
    'description' => t('Number of enabled languages, used for quick bootstrap. Not to be changed manually.', array(), $options),
  );
  $variables['language_types'] = array(
    'title' => t('Language types'),
    'type' => 'array',
    'default callback' => 'drupal_language_types',
    'group' => 'advanced',
    'description' => t('Available language types.'),
  );  
  // Bootstrap proxy configuration
  $variables['reverse_proxy'] = array(
    'title' => t('Reverse proxy'),
    'type' => 'enable',
    'default' => 0,
    'group' => 'advanced',
    'description' => t('If Drupal is behind a reverse proxy, we use the X-Forwarded-For header instead of $_SERVER[\'REMOTE_ADDR\'], which would be the IP address of the proxy server, and not the client\'s. The actual header name can be configured by the reverse_proxy_header variable.', array(), $options),
  );
  $variables['reverse_proxy_header'] = array(
    'title' => t('Reverse proxy header'),
    'default' => 'HTTP_X_FORWARDED_FOR',
    'group' => 'advanced',
  );
  $variables['reverse_proxy_addresses'] = array(
    'title' => t('Reverse proxy addresses'),
    'type' => 'array',
    'group' => 'advanced',
    'default' => array(),
    'description' => t('If an array of known reverse proxy IPs is provided, then trust the XFF header if request really comes from one of them.', array(), $options),
  );  
  return $variables;
}
