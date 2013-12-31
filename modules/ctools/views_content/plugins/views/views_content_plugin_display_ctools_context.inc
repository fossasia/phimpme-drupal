<?php
/**
 * @file
 * Contains the block display plugin.
 */

/**
 * The plugin that handles a block.
 *
 * @ingroup views_display_plugins
 */
class views_content_plugin_display_ctools_context extends views_plugin_display {
  /**
   * If this variable is true, this display counts as a context. We use this
   * variable so that we can easily build plugins against this display type.
   */
  var $context_display = TRUE;

  function get_style_type() { return 'context'; }

  function defaultable_sections($section = NULL) {
    if (in_array($section, array('style_options', 'style_plugin', 'row_options', 'row_plugin',))) {
      return FALSE;
    }

    return parent::defaultable_sections($section);
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['admin_title'] = array('default' => '', 'translatable' => TRUE);

    // Overrides for standard stuff:
    $options['style_plugin']['default'] = 'ctools_context';
    $options['row_plugin']['default'] = 'fields';
    $options['defaults']['default']['style_plugin'] = FALSE;
    $options['defaults']['default']['style_options'] = FALSE;
    $options['defaults']['default']['row_plugin'] = FALSE;
    $options['defaults']['default']['row_options'] = FALSE;
    $options['inherit_panels_path'] = array('default' => 0);
    $options['argument_input'] = array('default' => array());

    return $options;
  }

  /**
   * The display block handler returns the structure necessary for a block.
   */
  function execute() {
    $this->executing = TRUE;
    return $this->view->render();
  }

  function preview() {
    $this->previewing = TRUE;
    return $this->view->render();
  }

  /**
   * Render this display.
   */
  function render() {
    if (!empty($this->previewing)) {
      return theme($this->theme_functions(), array('view' => $this->view));
    }
    else {
      // We want to process the view like we're theming it, but not actually
      // use the template part. Therefore we run through all the preprocess
      // functions which will populate the variables array.
      $hooks = theme_get_registry();
      $info = $hooks[$this->definition['theme']];
      if (!empty($info['file'])) {
        @include_once('./' . $info['path'] . '/' . $info['file']);
      }
      $this->variables = array('view' => &$this->view);

      if (isset($info['preprocess functions']) && is_array($info['preprocess functions'])) {
        foreach ($info['preprocess functions'] as $preprocess_function) {
          if (function_exists($preprocess_function)) {
            $preprocess_function($this->variables, $this->definition['theme']);
          }
        }
      }
    }

    return $this->variables;
  }

  /**
   * Provide the summary for page options in the views UI.
   *
   * This output is returned as an array.
   */
  function options_summary(&$categories, &$options) {
    // It is very important to call the parent function here:
    parent::options_summary($categories, $options);

    $categories['context'] = array(
      'title' => t('Context settings'),
      'column' => 'second',
      'build' => array(
        '#weight' => -10,
      ),
    );

    $admin_title = $this->get_option('admin_title');
    if (empty($admin_title)) {
      $admin_title = t('Use view name');
    }

    if (drupal_strlen($admin_title) > 16) {
      $admin_title = drupal_substr($admin_title, 0, 16) . '...';
    }

    $options['admin_title'] = array(
      'category' => 'context',
      'title' => t('Admin title'),
      'value' => $admin_title,
    );

    $options['inherit_panels_path'] = array(
      'category' => 'context',
      'title' => t('Use Panel path'),
      'value' => $this->get_option('inherit_panels_path') ? t('Yes') : t('No'),
    );

    $options['argument_input'] = array(
      'category' => 'context',
      'title' => t('Argument input'),
      'value' => t('Edit'),
    );
  }

  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);
    switch ($form_state['section']) {
      case 'row_plugin':
        // This just overwrites the existing row_plugin which is using the wrong options.
        $form['row_plugin']['#options'] = views_fetch_plugin_names('row', 'normal', array($this->view->base_table));
        break;
      case 'admin_title':
        $form['#title'] .= t('Administrative title');

        $form['admin_title'] = array(
          '#type' => 'textfield',
          '#default_value' => $this->get_option('admin_title'),
          '#description' => t('This is the title that will appear for this view context in the configure context dialog. If left blank, the view name will be used.'),
        );
        break;
      case 'inherit_panels_path':
        $form['#title'] .= t('Inherit path from panel display');

        $form['inherit_panels_path'] = array(
          '#type' => 'select',
          '#options' => array(1 => t('Yes'), 0 => t('No')),
          '#default_value' => $this->get_option('inherit_panels_path'),
          '#description' => t('If yes, all links generated by Views, such as more links, summary links, and exposed input links will go to the panels display path, not the view, if the display has a path.'),
        );
        break;
      case 'argument_input':
        $form['#title'] .= t('Choose the data source for view arguments');
        $argument_input = $this->get_argument_input();
        ctools_include('context');
        ctools_include('dependent');
        $form['argument_input']['#tree'] = TRUE;

        $converters = ctools_context_get_all_converters();
        ksort($converters);

        foreach ($argument_input as $id => $argument) {
          $form['argument_input'][$id] = array(
            '#tree' => TRUE,
          );

          $safe = str_replace(array('][', '_', ' ', ':'), '-', $id);
          $type_id = 'edit-argument-input-' . $safe;

          $form['argument_input'][$id]['type'] = array(
            '#type' => 'select',
            '#options' => array(
              'none' => t('No argument'),
              'context' => t('From context'),
            ),
            '#id' => $type_id,
            '#title' => t('@arg source', array('@arg' => $argument['name'])),
            '#default_value' => $argument['type'],
          );

          $form['argument_input'][$id]['context'] = array(
            '#type' => 'select',
            '#title' => t('Required context'),
            '#description' => t('If "From context" is selected, which type of context to use.'),
            '#default_value' => $argument['context'],
            '#options' => $converters,
            '#dependency' => array($type_id => array('context')),
          );

          $form['argument_input'][$id]['context_optional'] = array(
            '#type' => 'checkbox',
            '#title' => t('Context is optional'),
            '#description' => t('This context need not be present for the pane to function. If you plan to use this, ensure that the argument handler can handle empty values gracefully.'),
            '#default_value' => $argument['context_optional'],
            '#dependency' => array($type_id => array('context')),
          );
        }
        break;
    }
  }

  /**
   * Perform any necessary changes to the form values prior to storage.
   * There is no need for this function to actually store the data.
   */
  function options_submit(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_submit($form, $form_state);
    switch ($form_state['section']) {
      case 'admin_title':
      case 'argument_input':
      case 'inherit_panels_path':
        $this->set_option($form_state['section'], $form_state['values'][$form_state['section']]);
        break;
    }
  }

  /**
   * Adjust the array of argument input to match the current list of
   * arguments available for this display. This ensures that changing
   * the arguments doesn't cause the argument input field to just
   * break.
   */
  function get_argument_input() {
    $arguments = $this->get_option('argument_input');
    $handlers = $this->get_handlers('argument');

    // We use a separate output so as to seamlessly discard info for
    // arguments that no longer exist.
    $output = array();

    foreach ($handlers as $id => $handler) {
      if (empty($arguments[$id])) {
        $output[$id] = array(
          'type' => 'none',
          'context' => 'any',
          'context_optional' => FALSE,
          'name' => $handler->ui_name(),
        );
      }
      else {
        $output[$id] = $arguments[$id];
        $output[$id]['name'] = $handler->ui_name();
      }
    }

    return $output;
  }

  function get_path() {
    if ($this->get_option('link_display') == 'custom_url' && $override_path = $this->get_option('link_url')) {
      return $override_path;
    }
    if ($this->get_option('inherit_panels_path')) {
      return $_GET['q'];
    }
    return parent::get_path();
  }
}
