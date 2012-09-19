<?php

/**
 * @file
 * Provide dependent checkboxes that can be easily used in forms.
 *
 * This system will ensure that form items are invisible if the dependency is
 * not met. What this means is that you set the #dependency of an item to a
 * list of form ids that must be set, and the list of values that qualify.
 *
 * For a simple use, setting an item to be dependent upon a select box, if
 * any of the listed values are selected, the item will be visible. Otherwise,
 * the item will be invisible.
 *
 * If dependent upon multiple items, use #dependency_count = X to set the
 * number of items that must be set in order to make this item visible. This
 * defaults to 1. If set to 2, then at least 2 form items in the list must
 * have their items set for the item to become visible.
 *
 * When hiding checkboxes and radios you need to add their id in a div
 * manually via #prefix and #suffix since they don't have their own id. You
 * actually need to add TWO divs because it's the parent that gets hidden.
 *
 * Fieldsets can not be hidden by default. Adding '#input' => TRUE to the
 * fieldset works around that.
 *
 * For radios, because they are selected a little bit differently, instead of
 * using the CSS id, use: radio:NAME where NAME is the #name of the property.
 * This can be quickly found by looking at the HTML of the generated form, but
 * it is usually derived from the array which contains the item. For example,
 * $form['menu']['type'] would have a name of menu[type]. This name is the same
 * field that is used to determine where in $form_state['values'] you will find
 * the value of the form.
 *
 * The item that is dependent on, should be set to #tree = TRUE.
 *
 * Usage:
 *
 * First, ensure this tool is loaded:
 * @code { ctools_include('dependent'); }
 *
 * On any form item, add
 * - @code '#dependency' => array('id-of-form-without-the-#' => array(list, of, values, that, make, this, gadget, visible)), @endcode
 *
 * A fuller example, that hides the menu title when no menu is selected:
 * @code
 *function ctools_dependent_example() {
 *  $form = array();
 *  $form['menu'] = array(
 *    '#type' => 'fieldset',
 *    '#title' => t('Menu settings'),
 *    '#tree' => TRUE,
 *  );
 *  $form['menu']['type'] = array(
 *    '#title' => t('Menu type'),
 *    '#type' => 'radios',
 *    '#options' => array(
 *      'none' => t('No menu entry'),
 *      'normal' => t('Normal menu entry'),
 *      'tab' => t('Menu tab'),
 *      'default tab' => t('Default menu tab'),
 *    ),
 *    '#default_value' => 'none',
 *  );
 *
 *  $form['menu']['title'] = array(
 *    '#title' => t('Title'),
 *    '#type' => 'textfield',
 *    '#default_value' => '',
 *    '#description' => t('If set to normal or tab, enter the text to use for the menu item.'),
 *    '#dependency' => array('radio:menu[type]' => array('normal', 'tab', 'default tab')),
 *   );
 *
 *  return system_settings_form($form);
 *}
 * @endcode
 *
 * An example for hiding checkboxes using #prefix and #suffix:
 * @code
 *function ctools_dependent_example_checkbox() {
 *  $form = array();
 *  $form['object'] = array(
 *    '#type' => 'fieldset',
 *    '#title' => t('Select object type'),
 *    '#tree' => TRUE,
 *  );
 *  $form['object']['type'] = array(
 *    '#title' => t('Object type'),
 *    '#type' => 'radios',
 *    '#options' => array(
 *      'view' => t('View'),
 *      'node' => t('Node'),
 *      'field' => t('Field'),
 *      'term' => t('Term'),
 *    ),
 *    '#default_value' => 'view',
 *  );
 *
 *  $form['object']['elements'] = array(
 *    '#title' => t('Select the elements to load from the node.'),
 *    '#type' => 'checkboxes',
 *    '#prefix' => '<div id="edit-elements-wrapper"><div id="edit-elements">',
 *    '#suffix' => '</div></div>',
 *    '#dependency' => array('radio:menu[type]' => array('node')),
 *    '#options' => array(
 *      'body' => t('Body'),
 *      'fields' => t('Fields'),
 *      'taxonomy' => t('Taxonomy'),
 *    ),
 *    '#default_value' => array('body', 'fields'),
 *   );
 *
 *  return system_settings_form($form);
 *}
 * @endcode
 *
 * Deprecated:
 *
 * You no longer use ctools_dependent_process(), and it should be removed
 * completely.
 *
 * If you have a form element which isn't listed in ctools_dependent_element_info_alter
 * you have to add [#pre_render'][] => 'ctools_dependent_pre_render' to your form.
 */

/**
 * Process callback to add dependency to form items.
 *
 */
function ctools_dependent_process($element, &$form_state, &$form) {
  return $element;
}

function ctools_dependent_pre_render($element) {
  // Preprocess only items with #dependency set.
  if (isset($element['#dependency'])) {
    if (!isset($element['#dependency_count'])) {
      $element['#dependency_count'] = 1;
    }
    if (!isset($element['#dependency_type'])) {
      $element['#dependency_type'] = 'hide';
    }

    $js = array(
      'values' => $element['#dependency'],
      'num' => $element['#dependency_count'],
      'type' => $element['#dependency_type'],
    );

    // Add a additional wrapper id around fieldsets, textareas to support depedency on it.
    if (in_array($element['#type'], array('textarea', 'fieldset', 'text_format'))) {
      $element['#theme_wrappers'][] = 'container';
      $element['#attributes']['id'] = $element['#id'] . '-wrapper';
    }

    // Text formats need to unset the dependency on the textarea
    // or it gets applied twice.
    if ($element['#type'] == 'text_format') {
      unset($element['value']['#dependency']);
    }

    $element['#attached']['js'][] = ctools_attach_js('dependent');
    $options['CTools']['dependent'][$element['#id']] = $js;
    $element['#attached']['js'][] = array('type' => 'setting', 'data' => $options);

  }

  return $element;
}

/**
 * CTools alters the element_info to be able to add #process functions to
 * every major form element to make it much more handy to use #dependency,
 * because you don't have to add #process.
 */
function ctools_dependent_element_info_alter(&$type) {
  $form_elements = array('checkbox', 'checkboxes', 'date', 'fieldset', 'item', 'machine_name', 'markup', 'radio', 'radios', 'select', 'textarea', 'textfield', 'text_format');
  foreach ($form_elements as $element) {
    $type[$element]['#pre_render'][] = 'ctools_dependent_pre_render';
  }
}
