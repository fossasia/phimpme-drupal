
Drupal module: Variable API
===========================

Variable module will provide a registry for meta-data about Drupal variables.

Module Developers: Please declare your variables.

Why?
====
- So other modules can know about your module's variables and they can be translated, exported, etc.
- You'll get automatic variable edit forms, tokens, access control and uninstall for free. 

How?
====
Easy: Implement hook_variable_info();

/**
 * Implements hook_variable_info().
 */
function mymodule_variable_info($options) {

  $variable['mymodule_number'] = array(
    'title' => t('Magic number', array(), $options),
    'description' => t('Magic number, array(), $options),
    'type' => 'number',
    'access' => 'administer menus',
  );
 
  $variable['mymodule_name'] = array(
    'title' => t('Name', array(), $options),
    'description' => t('Enter your name, please.', array(), $options),
    'type' => 'string',
    'default' => t('Drupal user', array(), $options),
  );
  
  $variable['mymodule_mail'] = array(
    'title' => t('Mail'),
    'type' => 'mail_text',
    // This type will spawn into two real variables: mymodule_mail_subject, mymodule_mail_body
    // Everything, included the form elements, will be handled automatically
  );

  return $variable;
}  

Note: You can have your variables declared in a separate file that just will be loaded when needed.

      yourmodule.variable.inc

FAQ
===
  
- Will I need to add a dependency on the variable.module?

  Not neccessarily. Just if you want to enjoy some of the module's features advanced features like:
  - Getting variable values or defaults in different languages. Use variable_get_value().
  - Let other modules alter my variable defaults. Implement hook_variable_info_alter().
  - Let other modules know when variables are changed. Use variable_set_value(). Implement hook_variable_update().
  - Getting automatic forms for all the module's variables, a group of variables, etc..
  - Having variables with multiple values handled automatically like mail body and subject or variables for node types.
  
  Otherwise you can just provide the meta-data for other modules to use. You still get:
  - Tokens for your variables like [variable:myvariable_name]
  - Variables deleted automatically when the module is uninstalled
  - Localizable texts for your variables when using the Internationalization module.
  
- How do I get a form with all of my module's variables?
  
  drupal_get_form('variable_module_form', 'mymodule');
  
- Once I have declared a default for my variable, how can I benefit from it?
  
  variable_get_value('variable_name');
 
- What if I don't want to provide any administration form for my variables?

  That's ok, people will still be able to see and edit them by enabling the 'Variable Admin' module included.

  

