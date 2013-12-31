<?php

/**
 * @file
 * Hooks provided by the Variable module.
 */

/**
 * @defgroup variable_api_hooks Variable API Hooks
 * @{
 * Functions to define and modify information about variables.
 * 
 * These hooks and all the related callbacks may be defined in a separate file
 * named module.variable.inc
 * 
 * @}
 */

/**
 * Define variables used by a module.
 * 
 * Provides meta-information for each variable that includes at the very least some human readable title.
 * This information may be used by other modules to select variables from a list for translating, exporting, etc.
 * 
 * Though not required we can also provide some more information to be able to handle the variable in an effective
 * way, like which type of data and form element it uses, default value, etc.. There are multiple predefined 
 * variable types ('type' attribute) that will add a predefined set of properties. Some of them are:
 * 
 * - "string": Simple plain string variable. The form element will be a text field and it will be localizable.
 * - "number": Simple numeric value. The form element will be a text field.
 * - "boolean": Simple TRUE/FALSE value. It will be a checkbox.
 * - "enable": Enabled / Disabled selector. It will display as two radio buttons. 
 * - "select": Selectable list of options. Depending on the number of options, the element will be a list of
 *   radios or a drop down.
 * - "options": List of options with multiple choices. It will be a list of checkboxes.
 * ...
 * 
 * More variable types can be defined by modules using hook_variable_type_info().
 * 
 * For the case of variable names that depend on some other parameter (like variables per content-type),
 * there's some special type of variables: Multiple variables. These can be defined like this:
 * 
 * @code
 *   $variables['node_options_[node_type]'] = array(
 *     'type' => 'multiple',
 *     'title' => t('Default options', array(), $options),
 *     'repeat' => array(
 *       'type' => 'options',
 *       'default' => array('status', 'promote'),
 *       'options callback' => 'node_variable_option_list',
 *     ),
 *   );
 * @endcode
 * 
 * This multiple variable will spawn into one variable for each node type. Note the variable name that includes
 * the property [node_type]. Values for [node_type] will be defined on hook_variable_type_info().
 * 
 * The 'repeat' property defines the properties of children variables. In this case the 'type' property is optional
 * and will default to 'multiple'.
 * 
 * @param $options
 *   Array of options to build variable properties. Since variable properties are cached per language
 *   these options should be used at the very least for string translations, so titles and defaults are
 *   localized. Possible options: 
 *   - "language" => Language object for which strings and defaults must be returned. This one will be always defined.
 * 
 * @return
 *   An array of information defining the module's variables. The array
 *   contains a sub-array for each node variable, with the variable name
 *   as the key. Possible attributes:
 *   - "title": The human readable name of the variable, will be used in auto generated forms.
 *   - "type": Variable type, should be one of the defined on hook_variable_type_info().
 *   - "group": Group key, should be one of the defined on hook_variable_group_info().
 *   - "description": Variable description, will be used in auto generated forms.
 *   - "options": Array of selectable options, or option name as defined on hook_variable_option_info().
 *   - "options callback": Function to invoke to get the list of options.
 *   - "default": Default value.
 *   - "default callback": Function to invoke to get the default value.
 *   - "multiple": Array of multiple children variables to be created from this one.
 *   - "multiple callback": Function to invoke to get children variables.
 *   - "element": Form element properties to override the default ones for this variable type.
 *   - "element callback": Function to invoke to get a form element for this variable.
 *   - "module": Module to which this variable belongs. This property will be added automatically.
 *   - "repeat": Array of variable properties for children variables.
 *   - "localize": Boolean value, TRUE for variables that should be localized. This may be used by other modules.
 *   - "validate callback": Callback to validate the variable value, it will be added to form element #validate.
 */
function hook_variable_info($options) {
  $variables['site_name'] = array(
    'type' => 'string',
    'title' => t('Name', array(), $options),
    'default' => 'Drupal',
    'description' => t('The name of this website.', array(), $options),
    'required' => TRUE,
  );
  $variables['site_403'] = array(
    'type' => 'drupal_path',
    'title' => t('Default 403 (access denied) page', array(), $options),
    'default' => '',
    'description' => t('This page is displayed when the requested document is denied to the current user. Leave blank to display a generic "access denied" page.', array(), $options),
  );
  return $variables;  
}
 
/**
 * Define types of variables or list of values used by a module.
 * 
 * These subtypes can be used to provide defaults for all properties of variables of this type
 * or to provide a list of options either for variable options (selectable values) or for children
 * variables in the case of multiple variables.
 * 
 * Example, three usages of variable type:
 * @code
 *   // Use variable type 'weekday' to provide a selector for a day of the week
 *   $variables['date_first_day'] = array(
 *   	 'type' => 'weekday',
 *   	 'title' => t('First day of week'),
 *   	 'default' => 0,
 * 	 );
 * 
 *   // Use 'options' with value 'weekday' for any other variable that needs to provide a selectable
 *   // list of days of the week. In this example you can select one or more days.
 *   $variables['working_days'] = array(
 *   	'type' => 'options',
 *    'options' => 'weekday',
 *    'title' => t('Select working days from the list.'),
 *   );
 *   
 *   // Use 'multiple' with value 'weekday' to create a subset of variables, one for each day of the week.
 *   // In fact, using '[weekday]' in the variable name will set these properties ('type' and 'multiple') automatically.
 * 	 $variables['daily_greeting_[weekday]'] = array(
 *      'type' => 'multiple',
 * 			'multiple' => 'weekday',
 *      'repeat' => array('type' => 'string'),
 *      'title' => t('Greeting to display each day of the week'),
 * 	 );
 * @endcode
 * 
 * @return
 *   An array of information defining variable types. The array contains
 *   a sub-array for each variable type, with the variable type as the key.
 *   
 *   The possible attributes are the same as for hook_variable_info(), with the
 *   type attributes being added on top of the variable attributes.
 *   
 *   A special attribute:
 *   - "type": Variable subtype, the properties for the subtype will be added to these ones.
 */
function hook_variable_type_info() {
  $type['mail_address'] = array(
    'title' => t('E-mail address'),
    'element' => array('#type' => 'textfield'),
    'token' => TRUE,
  );
  $type['mail_text'] = array(
    'title' => t('Mail text'),
    'multiple' => array('subject' => t('Subject'), 'body' => t('Body')),
    'build callback' => 'variable_build_mail_text',
    'localize' => TRUE,
    'type' => 'multiple',
  );
  return $type;  
}

/**
 * Define groups of variables used by a module.
 * 
 * Variable groups are used for presentation only, to display and edit the variables
 * on manageable groups. Groups can define a subset of a module's variables and can
 * be reused accross modules to group related variables.
 * 
 * A form to edit all variables in a group can be generated with:
 * 
 *   drupal_get_form('variable_group_form', group_name);
 * 
 * @return
 *   An array of information defining variable types. The array contains
 *   a sub-array for each variable group, with the group as the key.
 *   Possible attributes:
 *   - "title": The human readable name of the group. Must be localized.
 *   - "description": The human readable description of the group. Must be localized.
 *   - "access": Permission required to edit group's variables. Will default to 'administer site configuration'.
 *   - "path": Array of administration paths where these variables can be accessed.
 */
function hook_variable_group_info() {
  $groups['system_site_information'] = array(
    'title' => t('Site information'),
    'description' => t('Site information and maintenance mode'),
    'access' => 'administer site configuration',
    'path' => array('admin/config/system/site-information', 'admin/config/development/maintenance'),
  );
  $groups['system_feed_settings'] = array(
    'title' => t('Feed settings'),
    'description' => t('Feed settings'),
    'access' => 'administer site configuration',
  );
  return $groups;
}
