<?php
// $Id: flog.admin.inc,v 1.3 2011/01/04 20:47:16 dwightaspinwall Exp $

/**
 * @file
 * Admin page callbacks for the flog module.
 */

/**
 * Form for configuring file logger settings.
 *
 * @see system_settings_form()
 */
function flog_settings() {

  $form['flog_enabled'] = array(
    '#type' => 'radios',
    '#title' => t('File logging is'),
    '#default_value' => variable_get('flog_enabled', 0),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t('Enable or disable logging to file.'),
  );
  $form['flog_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Log file directory'),
    '#default_value' => variable_get('flog_path', '/var/log'),
    '#size' => 40,
    '#description' => t('Directory where your log file(s) live (e.g. /var/log). Don\'t include a trailing slash.'),
  );
  $form['flog_file'] = array(
    '#type' => 'textfield',
    '#title' => t('Default log file name'),
    '#default_value' => variable_get('flog_file', 'drupal.log'),
    '#size' => 40,
    '#description' => t('Default log file to receive print_r() dumps. Can be overridden by specifying another file name as the third argument to flog_it().'),
  );
  $form['flog_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Date string'),
    '#default_value' => variable_get('flog_date', 'Y-m-d H:i:s'),
    '#size' => 20,
    '#description' => t('Format of date string (see php !date docs).  Make this blank for no date/time output.', array('!date' => l(t('Date function'), 'http://php.net/manual/en/function.date.php'))),
  );

  return system_settings_form($form);
}

