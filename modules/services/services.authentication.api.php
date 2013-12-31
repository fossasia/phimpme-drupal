<?php

/**
 * @file
 * Hooks provided by Services for the definition of authentication plugins.
 */

/**
 * @addtogroup hooks
 * @{
 */

 /**
  * Supplies information about a given authentication method to Services.
  *
  * @return
  *   An associative array with information about the authentication method
  *   and its callbacks. The possible keys are as follows (all keys are
  *   optional unless noted).
  *
  *   - title (required): The display name for this authentication method.
  *   - description (required): Longer text describing this authentciation
  *     method.
  *   - authenticate_call (required): The name of a function to be called
  *     to perform the actual authentication. <details of params/return>
  *   - security_settings: A callback function which returns an associative
  *     array of Form API elements for a settings form.
  *   - _services_security_settings_validate: The name of a standard form
  *     validation callback for the form defined in 'security_settings'.
  *   - _services_security_settings_submit: The name of a standard form
  *     submit callback for the form defined in 'security_settings'.
  *   - alter_methods: The name of a callback function which will alter a
  *     services method signature in order to add required arguments.
  *   - file: An include file which contains the authentication callbacks.
  */
function hook_services_authentication_info() {

}
