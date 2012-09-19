<?php
/**
 * @file
 * Provides the Quicktabs administrative interface.
 */

/**
 * Page callback to list quicktabs in the system.
 */
function quicktabs_list() {
  $header = array(
    array('data' => t('QuickSet')),
    array('data' => t('Storage')),
    array('data' => t('Operations'), 'colspan' => 4),
  );
  $rows = array();

  foreach (quicktabs_load_multiple() as $qt) {
    // Determine storage
    switch ($qt->export_type) {
      case EXPORT_IN_DATABASE | EXPORT_IN_CODE:
        $storage = t('Overridden');
        $delete = l(t('Revert'), 'admin/structure/quicktabs/manage/'. $qt->machine_name .'/delete');
        break;
      case EXPORT_IN_DATABASE:
        $storage = t('Normal');
        $delete = l(t('Delete'), 'admin/structure/quicktabs/manage/'. $qt->machine_name .'/delete');
        break;
      case EXPORT_IN_CODE:
        $storage = t('Default');
        $delete = '';
        break;
    }

    $tablerow = array(
      array('data' => check_plain($qt->title)),
      array('data' => $storage),
      array('data' => l(t('Edit'), 'admin/structure/quicktabs/manage/'. $qt->machine_name .'/edit')),
      array('data' => l(t('Export'), 'admin/structure/quicktabs/manage/'. $qt->machine_name .'/export')),
      array('data' => l(t('Clone'), 'admin/structure/quicktabs/manage/'. $qt->machine_name .'/clone')),
      array('data' => $delete),
    );
    $rows[] = $tablerow;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No quicktabs instances available.'), 'colspan' => 6));
  }

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'quicktabs'),
  );
  return $build;
}

/**
 * Clone QuickTabs.
 */
function quicktabs_clone($qt) {
  unset($qt->machine_name);
  $qt->title = '';
  return drupal_get_form('quicktabs_form', 'clone', $qt);
}

/**
 * Build the quicktab creation and edit form.
 */
function quicktabs_form($form, $form_state, $formtype, $qt = NULL) {
  if (!isset($qt)) {
    $qt = new stdClass;
  }
  $form = _quicktabs_admin_main_form($form_state, $qt);

  // If creating a new Quicktabs instance, start off with 2 empty tabs.
  if (empty($qt->tabs)) {
    $qt->tabs = array(
      0 => array(),
      1 => array(),
    );
  }

  // If the "Add another" button was clicked, we need to increment the number of
  // tabs by one.
  if (isset($form_state['num_tabs']) && $form_state['num_tabs'] > count($qt->tabs)) {
    $qt->tabs[] = array();
  }
  $form_state['num_tabs'] = count($qt->tabs);
  
  // If the "Remove" button was clicked for a tab, we need to remove that tab
  // from the form.
  if (isset($form_state['to_remove'])) {
    unset($qt->tabs[$form_state['to_remove']]);
    unset($form_state['to_remove']);
    $form_state['num_tabs']--;
  }

  $tab_titles = array();
  // Add current tabs to the form.
  foreach ($qt->tabs as $delta => $tab) {
    $tab['delta'] = $delta;
    $form['qt_wrapper']['tabs'][$delta] = _quicktabs_form($tab, $qt);
    if (isset($tab['title'])) {
      $tab_titles[$delta] = $tab['title'];
    }
  }
  // If there's only one tab, it shouldn't be removeable.
  if (count($qt->tabs) == 1) $form['qt_wrapper']['tabs'][$delta]['remove']['#access'] = FALSE;

  $form['default_tab'] = array(
    '#type' => 'select',
    '#title' => t('Default tab'),
    '#options' => $tab_titles,
    '#default_value' => isset($qt->default_tab) ? $qt->default_tab : 0,
    '#access' => !empty($tab_titles),
    '#weight' => -5,
  );

  return $form;
}

/**
 * The main section of admin page.
 */
function _quicktabs_admin_main_form($form_state, &$qt) {

  // The contents of $qt will either come from the db or from $form_state.
  if (isset($form_state['values']['title'])) {
    $qt = _quicktabs_convert_form_to_quicktabs($form_state);
  }
  
  $form['title'] = array(
    '#title' => t('Title'),
    '#description' => t('This will appear as the block title.'),
    '#type' => 'textfield',
    '#default_value' => isset($qt->title) ? $qt->title : '',
    '#weight' => -9,
    '#required' => TRUE,
  );
  
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'quicktabs_machine_name_exists',
      'source' => array('title'),
    ),
    '#description' => t('A unique machine-readable name for this Quicktabs instance. It must only contain lowercase letters, numbers, and underscores. The machine name will be used internally by Quicktabs and will be used in the CSS ID of your Quicktabs block.'),
    '#weight' => -8,
  );

  if (!empty($qt->machine_name)) {
    $form['machine_name']['#default_value'] = $qt->machine_name;
    $form['machine_name']['#disabled'] = TRUE;
    $form['machine_name']['#value'] = $qt->machine_name;
  }

  ctools_include('plugins');
  $renderers = ctools_get_plugins('quicktabs', 'renderers');
  $renderer_options = array();
  foreach ($renderers as $name => $info) {
    if ($class = ctools_plugin_load_class('quicktabs', 'renderers', $name, 'handler')) {
      // Add the renderer to the dropdown list of renderers
      $renderer_options[$name] = $name;
      // Get the renderer's options form elements
      
      // PHP 5.2 doesn't support $class::staticMethod() syntax, so we have to
      // use call_user_func_array() until PHP 5.3 is required.
      $renderer_form_options[$name] = call_user_func_array(array($class, 'optionsForm'), array($qt));
    }
  }
  ksort($renderer_options);
  $form['renderer'] = array(
    '#type' => 'select',
    '#title' => t('Renderer'),
    '#options' => $renderer_options,
    '#default_value' => isset($qt->renderer) ? $qt->renderer : 'quicktabs',
    '#description' => t('Choose how to render the content.'),
    '#weight' => -7,
  );
  // Add the renderer options form elements to the form, to be shown only if the
  // renderer in question is selected.
  $form['options'] = array('#tree' => TRUE, '#weight' => -6);
  foreach ($renderer_form_options as $renderer => $options) {
    foreach ($options as &$option) {
      $option['#states'] = array('visible' => array(':input[name="renderer"]' => array('value' => $renderer)));
    }
    $form['options'][$renderer] = $options;
  }
  
  $styles = module_invoke_all('quicktabs_tabstyles');
  if (count($styles)) {
    $style_options = array();
    // The keys used for options must be valid html IDs.
    foreach ($styles as $style) {
      $style_options[$style] = $style;
    }
    ksort($style_options);
    $form['style'] = array(
      '#type' => 'select',
      '#title' => t('Style'),
      '#options' => array('nostyle' => t('No style')) + array('default' => t('Default style')) + $style_options,
      '#default_value' => isset($qt->style) ? $qt->style : 'default',
      '#description' => t('Choose the quicktab style.'),
      '#states' => array('visible' => array(':input[name="renderer"]' => array('value' => 'quicktabs'))),
      '#weight' => -6,
    );
  }
  else {
    $form['style'] = array(
      '#type' => 'value',
      '#value' => 'nostyle',
    );
  }

  $form['ajax'] = array(
    '#type' => 'radios',
    '#title' => t('Ajax'),
    '#options' => array(
      TRUE => t('Yes') . ': ' . t('Load only the first tab on page view'),
      FALSE => t('No') . ': ' . t('Load all tabs on page view.'),
    ),
    '#default_value' => isset($qt->ajax) ? $qt->ajax : 0,
    '#description' => t('Choose how the content of tabs should be loaded.<p>By choosing "Yes", only the first tab will be loaded when the page first viewed. Content for other tabs will be loaded only when the user clicks the other tab. This will provide faster initial page loading, but subsequent tab clicks will be slower. This can place less load on a server.</p><p>By choosing "No", all tabs will be loaded when the page is first viewed. This will provide slower initial page loading, and more server load, but subsequent tab clicks will be faster for the user. Use with care if you have heavy views.</p><p>Warning: if you enable Ajax, any block you add to this quicktabs block will be accessible to anonymous users, even if you place role restrictions on the quicktabs block. Do not enable Ajax if the quicktabs block includes any blocks with potentially sensitive information.</p>'),
    '#states' => array('visible' => array(':input[name="renderer"]' => array('value' => 'quicktabs'))),
    '#weight' => -5,
  );

  $form['hide_empty_tabs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide empty tabs'),
    '#default_value' => isset($qt->hide_empty_tabs) ? $qt->hide_empty_tabs : 0,
    '#description' => t('Empty and restricted tabs will not be displayed. Could be useful when the tab content is not accessible.<br />This option does not work in ajax mode.'),
    '#weight' => -4,
  );

  // Add a wrapper for the tabs and Add Another Tab button.
  $form['qt_wrapper'] = array(
    '#tree' => FALSE,
    '#weight' => -3,
    '#prefix' => '<div class="clear-block" id="quicktabs-tabs-wrapper">',
    '#suffix' => '</div>',
  );

  $form['qt_wrapper']['tabs'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="quicktab-tabs">',
    '#suffix' => '</div>',
    '#theme' => 'quicktabs_admin_form_tabs',
  );

  $form['qt_wrapper']['tabs_more'] = array(
    '#type' => 'submit',
    '#prefix' => '<div id="add-more-tabs-button">',
    '#suffix' => '<label for="edit-tabs-more">' . t('Add tab') . '</label></div>',
    '#value' => t('More tabs'),
    '#attributes' => array('class' => array('add-tab'), 'title' => t('Click here to add more tabs.')),
    '#weight' => 1,
    '#submit' => array('quicktabs_more_tabs_submit'),
    '#ajax' => array(
      'callback' => 'quicktabs_ajax_callback',
      'wrapper' => 'quicktab-tabs',
      'effect' => 'fade',
    ),
    '#limit_validation_errors' => array(),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit_form'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/*
 * Build one row (one tabpage) on the QT admin form.
 * 
 * @param array $tab
 *  An array containing the details of this particular tabpage.
 * 
 * @param object $qt
 *  An object representing the Quicktabs instance that the tabs are
 *  being built for. 
 */
function _quicktabs_form(array $tab, $qt) {
  $form['#tree'] = TRUE;
  $delta = $tab['delta'];

  $form['weight'] = array(
    '#type' => 'weight',
    '#default_value' => isset($tab['weight']) ? $tab['weight'] : $delta-100,
    '#delta' => 100,
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#size' => '10',
    '#default_value' => isset($tab['title']) ? $tab['title'] : '',
  );

  // Load all "contents" plugins to display a choice of content types.
  ctools_include('plugins');
  $contents = ctools_get_plugins('quicktabs', 'contents');
  foreach ($contents as $name => $info) {
    if (isset($info['dependencies'])) {
      foreach ($info['dependencies'] as $dep) {
        // Do not load the options form for any plugin that is missing dependencies.
        if (!module_exists($dep)) continue 2;
      }
    }
    $tabtypes[$name] = $name;
    $content_provider = quick_content_factory($name, $tab);
    $form = array_merge_recursive($form, $content_provider->optionsForm($delta, $qt));
  }

  $form['type'] = array(
    '#type' => 'radios',
    '#options' => $tabtypes,
    '#default_value' => isset($tab['type']) ? $tab['type'] : key($tabtypes),
  );

  $form['remove'] = array(
    '#type' => 'submit',
    '#prefix' => '<div>',
    '#suffix' => '<label for="edit-remove">' . t('Delete') . '</label></div>',
    '#value' => 'remove_' . $delta,
    '#attributes' => array('class' => array('delete-tab'), 'title' => t('Click here to delete this tab.')),
    '#submit' => array('quicktabs_remove_tab_submit'),
    '#ajax' => array(
      'callback' => 'quicktabs_ajax_callback',
      'wrapper' => 'quicktab-tabs',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#limit_validation_errors' => array(),
  );

  return $form;
}

/**
 * Theme function for quicktabs admin page.
 * Theme the form elements for the tabs as draggable table rows.
 *
 * @ingroup themeable
 */
function theme_quicktabs_admin_form_tabs($variables) {
  $tabs = $variables['tabs'];

  drupal_add_tabledrag('qt-tablist-table', 'order', 'sibling', 'qt-tabs-weight');
  $rows = array();
  $header = array(
    t('Tab title'),
    t('Tab weight'),
    t('Tab type'),
    t('Tab content'),
    t('Operations'),
  );

  foreach (element_children($tabs) as $key) {
    $tab = &$tabs[$key];
    $tab['weight']['#attributes']['class'] = array('qt-tabs-weight');

    // tab settings fields
    $tab_fields = array(
      array('data' => drupal_render($tab['title']), 'class' => array('qt-tab-title')),
      array('data' => drupal_render($tab['weight']), 'class' => array('qt-tab-weight')),
      array('data' => drupal_render($tab['type']), 'class' => array('qt-tab-type')),
    );

    // content plugins
    $content_plugins = '';
    foreach ($tab['type']['#options'] as $content_provider ) {
      $tab[$content_provider]['#prefix'] = '<div class="qt-tab-options-form qt-tab-'. $content_provider .'-options-form">';
      $tab[$content_provider]['#suffix'] = '</div>';
      $content_plugins .= drupal_render($tab[$content_provider]);
    }
    $tab_fields[] = array('data' => $content_plugins);
    $tab_fields[] = array('data' => drupal_render($tab['remove']), 'class' => array('qt-tab-remove'));

    // Build the table row.
    $row = array(
      'data' => $tab_fields,
      'class' => array('draggable'),
    );

    // Add additional attributes to the row, such as a class for this row.
    if (isset($tab['#attributes'])) {
      $row = array_merge($row, $tab['#attributes']);
    }
    $rows[] = $row;
  }

  $build['quicktab'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'qt-tablist-table'),
    '#weight' => -1,
  );
  $build['#attached']['css'][] = drupal_get_path('module', 'quicktabs') . '/css/quicktabs-admin.css';
  $build['#attached']['js'][] = drupal_get_path('module', 'quicktabs') . '/js/quicktabs_form.js';

  $output = drupal_render($build);
  return $output;
}

/**
 * Ajax callback for the add tab and remove tab buttons.
 */
function quicktabs_ajax_callback($form, $form_state) {
  $form_tabs = $form['qt_wrapper']['tabs'];
  return $form_tabs;
}

/**
 * Submit handler for the "Add Tab" button.
 */
function quicktabs_more_tabs_submit($form, &$form_state) {
  // Increment the number of tabs to be rendered.
  $form_state['num_tabs']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the "Remove Tab" button.
 */
function quicktabs_remove_tab_submit($form, &$form_state) {
  // Get the tab delta for the clicked button.
  $delta = $form_state['clicked_button']['#parents'][1];
  $form_state['to_remove'] = $delta;
  $form_state['rebuild'] = TRUE;
}

/**
 * Validation handler for quicktabs admin page.
 */
function quicktabs_form_validate($form, &$form_state) {
  if (empty($form_state['values']['machine_name'])) {
    form_set_error('machine_name', t('The quicktabs machine name is required.'));
  }
  elseif (!preg_match('!^[a-z0-9_]+$!', $form_state['values']['machine_name'])) {
    form_set_error('machine_name', t('The quicktabs machine name must contain only lowercase letters, numbers, and underscores.'));
  }

  if (!isset($form_state['values']['tabs'])) {
    form_set_error('', t('At least one tab should be created.'));
  }
  else {
    foreach ($form_state['values']['tabs'] as $j => $tab) {
      if (empty($tab['title'])) {
        form_set_error('tabs][' . $j . '][title', t('Title is required for each tab.'));
      }
    }
  }
}

function quicktabs_callback_element_validate($element, &$form_state, $form) {
  // We can tell which tab delta the element is for from the element's #parents
  // property, which is an array based on the tree structure.
  $delta = $element['#parents'][1];
  if ($form_state['values']['tabs'][$delta]['type'] == 'callback') {
    if (empty($element['#value']) || url_is_external($element['#value'])) {
      form_error($element, t('You must specify a valid path.'));
    }
    if (strpos($element['#value'], '%') === 0) {
      form_error($element, t('"%" may not be used for the first segment of a path.'));
    }
    // automatically remove '/' from path.
    $form_state['values']['tabs'][$delta]['callback']['path'] = trim($form_state['values']['tabs'][$delta]['callback']['path'], '/');
  }
}


/**
 * Submit handler for quicktabs admin page.
 */
function quicktabs_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] == 'edit-submit-form') {
    $qt = _quicktabs_convert_form_to_quicktabs($form_state);

    $exists = quicktabs_load($qt->machine_name);
    if ($exists && empty($exists->in_code_only)) {
      $ret = drupal_write_record('quicktabs', $qt, 'machine_name');
      if ($ret == SAVED_UPDATED) {
        drupal_set_message(t('The Quicktabs instance has been updated.'));
      }
    }
    else {
      $ret = drupal_write_record('quicktabs', $qt);
      if ($ret == SAVED_NEW) {
        drupal_set_message(t('The Quicktabs instance has been created.'));
      }
    }
    quicktabs_i18n_update_strings(array($qt->machine_name));
    drupal_goto('admin/structure/quicktabs');
  }
}

/**
 * Deletion of quicktab block.
 */
function quicktabs_block_delete($form, $form_state, $qt) {
  $form['machine_name'] = array('#type' => 'hidden', '#value' => $qt->machine_name);
  $form['title'] = array('#type' => 'hidden', '#value' => $qt->title);
  return confirm_form($form, t('Are you sure you want to delete the quicktab block %title?', array('%title' => $qt->title)), 'admin/structure/quicktabs', '', t('Delete'), t('Cancel'));
}

/**
 * Submit handler for quicktab block deletion.
 */
function quicktabs_block_delete_submit($form, &$form_state) {
  db_query('DELETE FROM {quicktabs} WHERE machine_name = :machine_name', array(':machine_name' => $form_state['values']['machine_name']));
  drupal_set_message(t('The Quicktabs instance %name has been removed.', array('%name' => $form_state['values']['title'])));
  cache_clear_all();
  $form_state['redirect'] = 'admin/structure/quicktabs';
};

/**
 * Export form for quicktabs.
 */
function quicktabs_export_form($form, &$form_state, $qt) {
  ctools_include('export');
  // Generate export code
  $code = '$items = array();' ."\n";
  $code .= ctools_export_object('quicktabs', $qt, '');
  $code .= '$items["'. $qt->machine_name .'"] = $quicktabs;' ."\n";
  $code .= 'return $items;';

  // Create form
  $form = array();
  $form['export'] = array(
    '#type' => 'textarea',
    '#default_value' => $code,
    '#rows' => substr_count($code, "\n") + 1,
    '#resizable' => FALSE,
    '#description' => t('Place this code in your module\'s implementation of <code>hook_quicktabs_default_quicktabs()</code> to provide it as a default quicktab.'),
  );
  $form['done'] = array(
    '#type' => 'submit',
    '#value' => t('Done'),
  );
  $form['#redirect'] = 'admin/structure/quicktabs';

  return $form;
}

/**
 * Helper function to get all blocks.
 */
function quicktabs_get_blocks() {
  $blocksarray = &drupal_static(__FUNCTION__, array());
  if (empty($blocksarray)) {
    $blocks = _block_rehash();
    $blocksarray = array();
    foreach ($blocks as $block) {
      if ($block['module'] != 'quicktabs') {
        $key = $block['module'] . '_delta_' . $block['delta'];
        $blocksarray[$key] = $block['info'] . ' (' . $block['module'] . ':' . $block['delta'] . ')';
      }
    }
  }
  return $blocksarray;
}

/**
 * Ajax callback, triggered when view is changed.
 */
function _quicktabs_replace_view_displays_callback($form, $form_state) {
  $view_name = $form_state['triggering_element']['#value'];
  $delta = $form_state['triggering_element']['#parents'][1];
  $display_options = _quicktabs_get_views_displays($view_name);
  $form['qt_wrapper']['tabs'][$delta]['view']['display']['#options'] = $display_options;

  $commands = array();
  // Replace the view display dropdown.
  $commands[] = ajax_command_replace("#view-display-dropdown-$delta", drupal_render($form['qt_wrapper']['tabs'][$delta]['view']['display']));
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Helper function to get all views.
 */
function quicktabs_get_views() {
  $enabled_views = array();
  $views = views_get_all_views();

  foreach ($views as $view) {
    // Skip disabled views.
    if (!empty($views[$view->name]->disabled)) {
      continue;
    }
    $enabled_views[$view->name] = $view->name;
  }
  ksort($enabled_views);
  return $enabled_views;
}

/**
 * Helper function to get all view displays.
 */
function _quicktabs_get_views_displays($view_name) {
  $displays = array();
  if (empty($view_name)) {
    // No view.
    return $displays;
  }

  $views = views_get_all_views();
  $view = $views[$view_name];

  if (empty($view->display)) {
    // This view is broken.
    return $displays;
  }

  foreach ($view->display as $id => $display) {
    $displays[$id] = $id .': '. $display->display_title;
  }
  return $displays;
}

/**
 * Helper function to convert the data on admin form into quicktab presentation.
 */
function _quicktabs_convert_form_to_quicktabs($form_state) {
  $formvalues_tabs = array();
  if (!empty($form_state['values']['tabs'])) {
    foreach ($form_state['values']['tabs'] as $j => $tab) {
      $formvalues_tabs[$j] = $tab[$tab['type']];
      $formvalues_tabs[$j]['title'] = $tab['title'];
      $formvalues_tabs[$j]['weight'] = $tab['weight'];
      $formvalues_tabs[$j]['type'] = $tab['type'];
      $weight[$j] = $tab['weight'];
    }
    array_multisort($weight, SORT_ASC, $formvalues_tabs);
  }
  $renderer = $form_state['values']['renderer'];
  $qt = new stdClass();
  $qt->title = $form_state['values']['title'];
  $qt->ajax = $form_state['values']['ajax'];
  $qt->default_tab = isset($form_state['values']['default_tab']) ? $form_state['values']['default_tab'] : 0;
  $qt->hide_empty_tabs = $form_state['values']['hide_empty_tabs'];
  $qt->renderer = $renderer;
  $qt->style = $form_state['values']['style'];
  $qt->tabs = $formvalues_tabs;
  $qt->options = isset($form_state['values']['options'][$renderer]) ? $form_state['values']['options'][$renderer] : array();

  if (isset($form_state['values']['machine_name'])) {
    $qt->machine_name = $form_state['values']['machine_name'];
  }

  return $qt;
}
