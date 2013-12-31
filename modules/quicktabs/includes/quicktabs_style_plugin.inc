<?php
// Id:$

/**
 * @file Add Quicktabs style plugins to Views.
 */

/**
 * Style plugin to display Quicktabs.
 */
class quicktabs_style_plugin extends views_plugin_style {

  // Allow some options for the style.
  function option_definition() {
    $options = parent::option_definition();
    $options['tab_style'] = array('default' => 'default');
    $options['tab_title_field'] = array('default' => NULL);

    return $options;
  }

  // Create the options form.
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $options = array();
    $styles = module_invoke_all('quicktabs_tabstyles');
    // The keys used for options must be valid html id-s.
    // Removing the css file path, because that can't be used.
    foreach ($styles as $style) {
      $options[$style] = $style;
    }
    ksort($options);
    $form['tab_style'] = array(
      '#type' => 'select',
      '#title' => t('Tab style'),
      '#options' => array('nostyle' => t('No style'), 'default' => t('Default style')) + $options,
      '#default_value' => $this->options['tab_style'],
      '#description' => t('The tab style that will be applied to this set of tabs. Note that this style may not show in the live preview.'),
      '#weight' => -5,
    );

    if (isset($form['grouping'])) {
      $options = $form['grouping']['#options'];
      unset($options['']);
      $form['tab_title_field'] = array(
        '#type' => 'select',
        '#title' => t('Title field'),
        '#options' => $options,
        '#required' => TRUE,
        '#default_value' => $this->options['tab_title_field'],
        '#description' => t('Select the field that will be used as the tab title.'),
        '#weight' => -3,
      );
    }
  }

  // Ensure we have all the settings necessary to render into tabs.
  function validate() {
    $errors = parent::validate();

    // Ensure that we're using the field row style.
    if (!$this->row_plugin->uses_fields()) {
      $errors[] = t('Display "@display" uses the "@style" row style, but the Quicktabs display style requires use of the "Fields" row style.', array('@display' => $this->display->display_title, '@style' => $this->row_plugin->definition['title']));
    }

    // Ensure that a valid tab title field is selected.
    $fields = $this->display->handler->get_handlers('field');
    if (empty($this->options['tab_title_field']) || !isset($fields[$this->options['tab_title_field']])) {
      $errors[] = t('The Quicktabs display style requires that a field be configured to be used as the tab title.');
    }

    return $errors;
  }

  // Override the render functionality.
  function render() {
    if (empty($this->row_plugin)) {
      vpr('views_plugin_style_default: Missing row plugin');
      return;
    }

    $view = $this->view;
    $qt_name = 'view__' . $view->name .'__'. $view->current_display;

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    $tabs = array();

    foreach ($sets as $title => $records) {
      if ($this->uses_row_plugin()) {
        $rows = array();
        foreach ($records as $row_index => $row) {
          $this->view->row_index = $row_index;
          $rows[] = $this->row_plugin->render($row);
        }
      }
      else {
        $rows = $records;
      }

      // If grouped, we'll be using the group title for each tab.
      if ($this->options['grouping']) {
        // Remove labels from titles.
        $grouping_field = $this->options['grouping'];
        if ($this->view->field[$grouping_field]->options['label']) {
          $title = str_replace($this->view->field[$grouping_field]->options['label'] . ': ', '', $title);
        }
        $contents = '';
        foreach ($rows as $row) {
          $contents .= '<div class="quicktabs-views-group">' . $row . '</div>';
        }
        $tabs[] = array(
          'title' => $title,
          'contents' => array('#markup' => $contents),
        );
      }

      // If not grouped, there's just one set of rows that we loop through.
      else {
        foreach ($rows as $index => $row) {
          $title = $this->get_field($index, $this->options['tab_title_field']);
          $tabs[] = array(
            'title' => $title,
            'contents' => array('#markup' => $row),
          );
        }
      }
    }

    $overrides = array('style' => $view->style_options['tab_style'], 'sorted' => TRUE);
    $quicktabs = quicktabs_build_quicktabs($qt_name, $overrides, $tabs);

    $output = drupal_render($quicktabs);

    // If doing a live preview, add the JavaScript directly to the output.
    if (isset($view->live_preview) && $view->live_preview) {
      $js = drupal_add_js();
      $qtsettings = array();
      foreach ($js['settings']['data'] as $settings) {
        if (isset($settings['quicktabs']['qt_'. $qt_name])) {
          $qtsettings = $settings['quicktabs']['qt_'. $qt_name];
          break;
        }
      }

      $output .= "<script type=\"text/javascript\">\n";
      $output .= "Drupal.settings.quicktabs = Drupal.settings.quicktabs || {};\n";
      $output .= "jQuery.extend(Drupal.settings.quicktabs, ". json_encode(array('qt_'. $qt_name => $qtsettings)) .");\n";
      $output .= "</script>\n";
    }

    unset($view->row_index);
    
    return $output;
  }

}
