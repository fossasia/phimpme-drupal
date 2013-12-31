<?php
/**
 * @file
 * Variable API module. Extended API.
 */

/**
 * Build generic variable information
 */
function variable_build_info($type, $options) {
  variable_module_include();
  switch ($type) {
    case 'variable':
      return variable_build_list_info($options);
    case 'group':
    case 'type':
      $info = variable_invoke_all('variable_' . $type . '_info');
      drupal_alter('variable_' . $type . '_info', $info);
      return $info;
  }  
}

/**
 * Build variable information
 */
function variable_build_list_info($options) {
  $variables = array();
  foreach (module_implements('variable_info') as $module) {
    $result = call_user_func($module . '_variable_info', $options);
    if (isset($result) && is_array($result)) {
      foreach ($result as $name => $variable) {
        // Support name => title declarations
        $variable = is_array($variable) ? $variable : array('title' => $variable);
        $variable += array('name' => $name, 'module' => $module);
        // Check variable name for multiple values
        $multiple = NULL;
        if (preg_match('/\[(\w+)\]/', $name, $matches)) {
          $multiple = $matches[1];
          $variable += array('type' => 'multiple');
        }  
        $variable += array('group' => 'default', 'type' => 'default');
        $variables[$name] = $variable + variable_get_type($variable['type']);
        // Add this at the end so it doesn't override type properties
        if (!empty($multiple)) {
          $variables[$name] += array('multiple' => $multiple);
        }
      }
    }
  }
  // Last chance for modules to alter variable infoa
  drupal_alter('variable_info', $variables, $options);
  return $variables;
}

/**
 * Invoke hook on all modules, adding 'module' property
 */
function variable_invoke_all() {
  $args = func_get_args();
  $hook = $args[0];
  unset($args[0]);
  $return = array();
  foreach (module_implements($hook) as $module) {
    $function = $module . '_' . $hook;
    if (function_exists($function)) {
      $result = call_user_func_array($function, $args);
      if (isset($result) && is_array($result)) {
        foreach ($result as $key => $value) {
          $return[$key] = $value + array('module' => $module);
        }
      }
    }
  }
  return $return;
}


/**
 * Build multiple variables
 */
function variable_build_multiple($variable, $options) {
  // Invoke variable callbacks
  if (!empty($variable['multiple callback'])) {
    $variable['multiple'] = variable_callback($variable['multiple callback'], $variable, $options);
  }
  if (isset($variable['multiple'])) {
    if (!is_array($variable['multiple'])) {
      $variable['multiple'] = variable_option_list($variable['multiple'], $variable, $options);
    }
    $variable += array('children' => array(), 'repeat' => array());
    // Build chilcren variables with the name => value array 
    foreach ($variable['multiple'] as $key => $title) {
      $name = preg_replace('/\[\w+\]/', $key, $variable['name']);
      // Be careful to respect previously set properties, add but do not override.
      $child = isset($variable['children'][$name]) ? $variable['children'][$name] : array();
      $child += $variable['repeat'];
      $child += array(
        'name' => $name, 'index' => $key, 'title' => $title, 
        'type' => 'default', 'parent' => $variable['name'], 'module' => $variable['module'],
      );
      $child += variable_get_type($child['type']);
      $variable['children'][$name] = $child;
    }
  }
  return $variable;
}

/**
 * Build variable with options
 */
function variable_build_options($variable, $options) {
  $variable = variable_build_variable($variable, $options);
  if (isset($variable['options callback'])) {
    $variable['options'] = variable_callback($variable['options callback'], $variable, $options);
  }
  if (!empty($variable['options']) && !is_array($variable['options'])) {
    $variable['options'] = variable_option_list($variable['options'], $variable, $options);
  }
  return $variable;
}

/**
 * Build single variable
 * 
 * Some variables may spawn into multiple ones
 */
function variable_build_variable($variable, $options = array()) {
  if (empty($variable['built'])) {
    variable_include($variable);
    // Mark as built so we don't build it again
    $variable['built'] = TRUE;
    $options = _variable_options($options);
    $variable = _variable_variable($variable, $options);
    // If the variable has a build callback, go for it
    if (isset($variable['build callback'])) {
      $variable = variable_callback($variable['build callback'], $variable, $options);
    }
  }
  return $variable;
}

/**
 * Invoke variable callback
 * 
 * @param $callback
 *   Function name to invoke or array with module and funcion in this order
 * @param $variable
 *   Array of variable information.
 * @param $options
 *   Options to pass to the callback
 * @param $module
 *   Optional module to include its '*.variable.inc' file if the function not found
 */
function variable_callback($callback, $variable, $options = array()) {
  if (is_array($callback)) {
    list($module, $function) = $callback;
  }
  else {
    $function = $callback;
  }
  if (!function_exists($function)) {
    if (isset($module)) {
      variable_module_include($module);
    }
    else {
      variable_include($variable);
    }
  }
  return call_user_func($function, $variable, $options);
}

/**
 * List variables for a group
 */
function variable_list_group($group) {
  $list = array();
  foreach (variable_get_info() as $name => $variable) {
    if ($variable['group'] == $group) {
      $list[$name] = $variable;
    }
  }
  return $list;
}

/**
 * List variables for a module
 */
function variable_list_module($module) {
  $list = array();
  foreach (variable_get_info() as $name => $variable) {
    if ($variable['module'] == $module) {
      $list[$name] = $variable;
    }
  }
  return $list;
}

/**
 * Fetch options for variable
 */
function variable_option_list($type, $variable, $options) {
  $cache = &drupal_static(__FUNCTION__);
  if (isset($cache[$type])) {
    return $cache[$type];
  }
  elseif ($info = variable_get_type($type)) {
    if (isset($info['options callback'])) {
      $info['options'] = variable_callback(array($info['module'], $info['options callback']), $variable, $options);
    }
    if (!empty($info['cache'])) {
      $cache[$type] = $info['options'];
    }
    return $info['options'];
  }
  else {
    return array();
  }
}

/**
 * General function to include variable definitions for all modules
 */
function variable_module_include($modules = NULL) {
  static $core_modules = array('locale', 'forum', 'menu', 'node', 'system', 'taxonomy', 'translation', 'user');
  static $included = array();
  $modules = $modules ? (is_array($modules) ? $modules : array($modules)) : $core_modules;
  foreach ($modules as $module) {
    if (!isset($included[$module])) {
      if (module_exists($module)) {
        if (in_array($module, $core_modules)) {
          $included[$module] = module_load_include('variable.inc', 'variable', 'includes/' . $module);
        }
        else {
          $included[$module] = module_load_include('variable.inc', $module);
        }
      }
    }
  }
}

/**
 * Uninstall variables for module
 * 
 * This will be called from variable_modules_uninstalled(), no need to implement it directly.
 */
function variable_module_uninstall($module) {
  foreach (variable_list_module($module) as $name => $variable) {
    variable_delete($variable);
  }
}


/**
 * Format multiple variable
 */
function variable_format_multiple($value, $variable, $options = array()) {
  return t('<Multiple>', array(), $options);
}

/**
 * Get value for multiple variable
 */
function variable_multiple_get_value($variable, $options = array()) {
  $variable = variable_build($variable, $options);
  $values = array();
  foreach ($variable['children'] as $child) {
    $values[$child['index']] = variable_get_value($child, $options);
  }
  return $values;
}

/**
 * Get defaults for multiple variable
 */
function variable_multiple_get_default($variable, $options = array()) {
  $variable = variable_build($variable, $options);
  $values = array();
  foreach ($variable['children'] as $child) {
    $values[$child['index']] = variable_get_default($child, $options);
  }  
  return $values;
}
