<?php

/**
 * Class for tab content of type "qtabs" - this is for rendering a QuickSet instance
 * as the tab content of another QuickSet instance.
 */
class QuickQtabsContent extends QuickContent {
  
  public static function getType() {
    return 'qtabs';
  }
  
  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $tab_options = array();
    foreach (quicktabs_load_multiple() as $machine_name => $info) {
      // Do not offer the option to put a tab inside itself.
      if (!isset($qt->machine_name) || $machine_name != $qt->machine_name) {
        $tab_options[$machine_name] = $info->title;
      }
    }
    $form['qtabs']['machine_name'] = array(
      '#type' => 'select',
      '#title' => t('Quicktabs instance'),
      '#description' => t('The Quicktabs instance to put inside this tab.'),
      '#options' => $tab_options,
      '#default_value' => isset($tab['machine_name']) ? $tab['machine_name'] : '',
    );
    return $form;
  }

  public function render($hide_empty = FALSE, $args = array()) {
    if ($this->rendered_content) {
      return $this->rendered_content;
    }
    $item = $this->settings;
    if (!empty($args)) {
      // The args have been passed in from an ajax request.
      // The first element of the args array is the qt_name, which we don't need
      // for this content type.
      array_shift($args);
      $item['machine_name'] = $args[0];
    }

    $output = array();
    if (isset($item['machine_name'])) {
      if ($quicktabs = quicktabs_load($item['machine_name'])) {
        $contents = $quicktabs->tabs;
        $name = $quicktabs->machine_name;
        unset($quicktabs->tabs, $quicktabs->machine_name);
        $options = (array) $quicktabs;
        if ($qt = QuickSet::QuickSetRendererFactory($name, $contents, $quicktabs->renderer, $options)) {
          $output = $qt->render();
        }
      }
    }
    $this->rendered_content = $output;
    return $output;
  }

  public function getAjaxKeys() {
    return array('machine_name');
  }
}