<?php

/**
 * Alter the Quicktabs instance before it gets rendered.
 *
 * @param &$quicktabs
 *   A loaded Quicktabs object, either from the database or from code.
 */
function hook_quicktabs_alter(&$quicktabs) {
}

/**
 * This hook allows other modules to create additional tab styles for
 * the quicktabs module.
 *
 * @return array
 *   An array of key => value pairs suitable for inclusion as the #options in a
 *   select or radios form element. Each key must be the location of a css
 *   file for a quick tabs style. Each value should be the name of the style.
 */
function hook_quicktabs_tabstyles() {
}

