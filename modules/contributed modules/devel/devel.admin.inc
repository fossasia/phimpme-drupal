<?php

function devel_admin_settings() {
  $form['queries'] = array('#type' => 'fieldset', '#title' => t('Query log'));

  $description = t('Display a log of the database queries needed to generate the current page, and the execution time for each. Also, queries which are repeated during a single page view are summed in the # column, and printed in red since they are candidates for caching.');
  if (!devel_is_compatible_optimizer()) {
    $description = t('You must disable or upgrade the php Zend Optimizer extension in order to enable this feature. The minimum required version is 3.2.8. Earlier versions of Zend Optimizer are <a href="!url">horribly buggy and segfault your Apache</a> ... ', array('!url' => url('http://drupal.org/node/126098'))) . $description;
  }
  $form['queries']['devel_query_display'] = array('#type' => 'checkbox',
    '#title' => t('Display query log'),
    '#default_value' => variable_get('devel_query_display', 0),
    '#description' => $description,
    '#disabled' => !devel_is_compatible_optimizer(),
  );
  $form['queries']['settings'] = array(
    '#type' => 'container',
      '#states' => array(
        // Hide the query log settings when not displaying query log.
        'invisible' => array(
          'input[name="devel_query_display"]' => array('checked' => FALSE),
        ),
      ),
  );
  $form['queries']['settings']['devel_query_sort'] = array('#type' => 'radios',
    '#title' => t('Sort query log'),
    '#default_value' =>   variable_get('devel_query_sort', DEVEL_QUERY_SORT_BY_SOURCE),
    '#options' => array(t('by source'), t('by duration')),
    '#description' => t('The query table can be sorted in the order that the queries were executed or by descending duration.'),
  );
  $form['queries']['settings']['devel_execution'] = array('#type' => 'textfield',
    '#title' => t('Slow query highlighting'),
    '#default_value' => variable_get('devel_execution', 5),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('Enter an integer in milliseconds. Any query which takes longer than this many milliseconds will be highlighted in the query log. This indicates a possibly inefficient query, or a candidate for caching.'),
  );

  $form['xhprof'] = array(
    '#type' => 'fieldset',
    '#title' => 'XHProf',
    '#description' => t('XHProf is a php extension which is essential for profiling your Drupal site. It pinpoints slow functions, and also memory hogging functions.'),
  );
  $description = extension_loaded('xhprof') ? t('Profile requests with the xhprof php extension.') : '<span class="warning">' . t('You must enable the <a href="!url">xhprof php extension</a> to use this feature.', array('!url' => url('http://techportal.ibuildings.com/2009/12/01/profiling-with-xhprof/'))) . '</span>';
  $form['xhprof']['devel_xhprof_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable profiling of all page views and <a href="!drush">drush</a> requests.', array('!drush' => url('http://drush.ws'))),
    '#default_value' => variable_get('devel_xhprof_enabled', FALSE),
    '#description' => $description,
    '#disabled' => !extension_loaded('xhprof'),
  );
  $form['xhprof']['settings'] = array(
    '#type' => 'container',
    '#states' => array(
      'invisible' => array(
        'input[name="devel_xhprof_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['xhprof']['settings']['devel_xhprof_directory'] = array(
    '#type' => 'textfield',
    '#title' => 'xhprof directory',
    '#description' => t('Location of the xhprof source code on your system, usually somewhere in /usr/local/share or /usr/share, include the leading forward slash.'),
    '#default_value' => variable_get('devel_xhprof_directory', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="devel_xhprof_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['xhprof']['settings']['devel_xhprof_url'] = array(
    '#type' => 'textfield',
    '#title' => 'XHProf URL',
    '#description' => t('Path to the publically accessible xhprof_html - required to display profiler reports. You will need to set this up outside Drupal, for example at http://xhprof.localhost/xhprof_html'),
    '#default_value' => variable_get('devel_xhprof_url', ''),
    '#states' => array(
      'invisible' => array(
        'input[name="devel_xhprof_enabled"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['devel_api_url'] = array('#type' => 'textfield',
    '#title' => t('API Site'),
    '#default_value' => variable_get('devel_api_url', 'api.drupal.org'),
    '#description' => t('The base URL for your developer documentation links. You might change this if you run <a href="!url">api.module</a> locally.', array('!url' => url('http://drupal.org/project/api'))));
  $form['dev_timer'] = array('#type' => 'checkbox',
    '#title' => t('Display page timer'),
    '#default_value' => variable_get('dev_timer', 0),
    '#description' => t('Display page execution time in the query log box.'),
  );

  $form['dev_mem'] = array('#type' => 'checkbox',
    '#title' => t('Display memory usage'),
    '#default_value' => variable_get('dev_mem', 0),
    '#description' => t('Display how much memory is used to generate the current page. This will show memory usage when devel_init() is called and when devel_exit() is called.'),
  );
  $form['devel_redirect_page'] = array('#type' => 'checkbox',
    '#title' => t('Display redirection page'),
    '#default_value' => variable_get('devel_redirect_page', 0),
    '#description' => t('When a module executes drupal_goto(), the query log and other developer information is lost. Enabling this setting presents an intermediate page to developers so that the log can be examined before continuing to the destination page.'),
  );
  $form['devel_page_alter'] = array('#type' => 'checkbox',
    '#title' => t('Display $page array'),
    '#default_value' => variable_get('devel_page_alter', FALSE),
    '#description' => t('Display $page array from <a href="http://api.drupal.org/api/function/hook_page_alter/7">hook_page_alter()</a> in the messages area of each page.'),
  );
  $form['devel_error_handler'] = array('#type' => 'radios',
    '#title' => t('Error handler'),
    '#default_value' => variable_get('devel_error_handler', DEVEL_ERROR_HANDLER_STANDARD),
    '#options' => array(DEVEL_ERROR_HANDLER_NONE => t('None'), DEVEL_ERROR_HANDLER_STANDARD => t('Standard drupal')),
    '#description' => t('Choose an error handler for your site. <em>Backtrace</em> prints nice debug information when an error is noticed, and you <a href="@choose">choose to show errors on screen</a>. <strong>Backtrace requires the <a href="@krumo">krumo library</a></strong>. <em>None</em> is a good option when stepping through the site in your debugger.', array('@krumo' => url('http://krumo.sourceforge.net'), '@choose' => url('admin/config/development/logging'))),
  );
  if (has_krumo()) {
    $form['devel_error_handler']['#options'][DEVEL_ERROR_HANDLER_BACKTRACE] = t('Backtrace');
  }

  $options = drupal_map_assoc(array('default', 'blue', 'green', 'orange', 'white', 'disabled'));
  $form['devel_krumo_skin'] = array(
    '#type' => 'radios',
    '#title' => t('Krumo display'),
    '#description' => t('Select a skin for your debug messages or select <em>disabled</em> to display object and array output in standard PHP format.'),
    '#options' => $options,
    '#default_value' => variable_get('devel_krumo_skin', 'default'),
  );

  $form['devel_rebuild_theme_registry'] = array(
   '#type' => 'checkbox',
   '#title' => t('Rebuild the theme registry on every page load'),
   '#description' => t('While creating new templates and theme_ overrides the theme registry needs to be rebuilt.'),
   '#default_value' => variable_get('devel_rebuild_theme_registry', FALSE),
  );

  $form['devel_use_uncompressed_jquery'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use uncompressed jQuery'),
    '#default_value' => variable_get('devel_use_uncompressed_jquery', FALSE),
    '#description' => t("Use a human-readable version of jQuery instead of the minified version that ships with Drupal, to make JavaScript debugging easier."),
  );

  return system_settings_form($form);
}
