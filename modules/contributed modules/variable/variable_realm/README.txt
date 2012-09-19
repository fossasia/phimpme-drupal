
Drupal module: Variable Realms
============================================
This is an API module that works as an arbitrator for multiple modules overriding global variables. It can
handle multiple realms defined by different modules. Examples: 'global', 'language', 'country', 

Each realm has a weight and a current status. Realms with higher weights will override realms with lower weight.

There's a special 'global/default' realm that is the one storing default global variables. It has a weight of 0
so realms with weights higher than that (default weight for new realms is 10) will override these.

Any number of realms can be defined by different modules. If two modules use the same realm, the last one's variables
and weight will override the previous one. Every time we switch a realm, the $conf global array will be rebuilt.

At any moment the $conf global array of variables will be a combination of the active realms.
If we've got these two reamls defined:
 - global/default, weight 0, which is defined by this module, will hold global default variables
 - mymodule/key, weight 10, which may be defined by any contrib module on hook_boot() or hook_init()
The resulting variable set will be a combination of these two, with the second overriding the first one,
because of a higher weight. This is how we calculate the resulting variables when using variable_realm_switch()

 $conf = $variables['global/default'] + $variables['mymodule/key']
 
API Example
-----------
This is an example of how realms work:

// We add a language realm with some variables and immediately switch to it

  variable_realm_switch('language', 'es', $spanish_variables);

// We add a country realm on top of it with some more variables but don't switch to it yet.
// Note the first time we add a domain we can set the weight for it.

  variable_realm_add('country', 'spain', $spain_variables, 100);

// We add another country realm, but don't switch to it.
// The same weight from previous 'country' realm will be used

  variable_realm_add('country', 'mexico', $mexico_variables);

// Now we can switch to the 'spanish/spain' set of variables

  variable_realm_switch('country', 'spain');

// Or we can use the 'spanish/mexico' set

  variable_realm_switch('country', 'mexico');
  
// Still we can add one more realm which will override some variables for the current node's content type
// These will override all the others because of its higher weight

  variable_realm_switch('nodetype', 'story', $story_variables, 200)

An example of a module using this API is Internationalization's i18n_variable module.