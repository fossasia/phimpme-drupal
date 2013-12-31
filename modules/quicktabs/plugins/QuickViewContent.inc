<?php

/**
 * Class for tab content of type "view" - this is for rendering a view as tab
 * content.
 */
class QuickViewContent extends QuickContent {

  // Each view that we render, whether via ajax or not, will need a unique DOM
  // id. Unfortunately we can only control the ones that Quicktabs is responsible
  // for, so if there are other views on the page, there may be duplicate ids.
  static $view_dom_id = 1;
  
  public static function getType() {
    return 'view';
  }
  
  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $views = quicktabs_get_views();
    $views_keys = array_keys($views);

    $selected_view = (isset($tab['vid']) ? $tab['vid'] : (isset($views_keys[0]) ? $views_keys[0] : ''));
    $form['view']['vid'] = array(
      '#type' => 'select',
      '#options' => $views,
      '#default_value' => $selected_view,
      '#title' => t('Select a view'),
      '#ajax' => array(
        'callback' => '_quicktabs_replace_view_displays_callback',
      ),
    );
    $form['view']['display'] = array(
      '#type' => 'select',
      '#title' => 'display',
      '#options' => _quicktabs_get_views_displays($selected_view),
      '#default_value' => isset($tab['display']) ? $tab['display'] : '',
      '#prefix' => '<div id="view-display-dropdown-' . $delta . '">',
      '#suffix' => '</div>'
    );
    $form['view']['args'] = array(
      '#type' => 'textfield',
      '#title' => 'arguments',
      '#size' => '40',
      '#required' => FALSE,
      '#default_value' => isset($tab['args']) ? $tab['args'] : '',
      '#description' => t('Additional arguments to send to the view as if they were part of the URL in the form of arg1/arg2/arg3. You may use %0, %1, ..., %N to grab arguments from the URL.'),
    );
    return $form;
  }
  
  public function __construct($item) {
    parent::__construct($item);
    if (module_exists('views')) views_add_js('ajax_view');
    $this->settings['view_path'] = rawurlencode($_GET['q']);
    $this->settings['view_dom_id'] = self::$view_dom_id++;
    if (isset($item['args'])) {
      $url_args = arg();
      $args = $item['args'];
  
      foreach ($url_args as $id => $arg) {
        $args = str_replace("%$id", $arg, $args);
      }
      $args = preg_replace(',/?(%\d),', '', $args);
      if (!empty($args)) {
        $this->settings['ajax_args'] = rawurlencode($args);
        $args_array = explode('/', $args);
      }
      else {
        $this->settings['ajax_args'] = '';
        $args_array = array();
      }
      $this->settings['actual_args'] = $args_array;
    }
  }

  public function render($hide_empty = FALSE, $args = array()) {
    if (!empty($args)) {
      // The args have been passed in from an ajax request. We use Views' own
      // ajax functionality to get the view.
      // The first element of the args array is the qt_name, which we don't need
      // for this content type.
      array_shift($args);
      // The order of these arguments corresponds to the array returned in
      // $this->getAjaxKeys().
      $_REQUEST['view_name'] = array_shift($args);
      $_REQUEST['view_display_id'] = array_shift($args);
      $_REQUEST['view_dom_id'] = array_shift($args);
      $view_path = array_shift($args);
      $_REQUEST['view_path'] = rawurldecode($view_path);
      if (!empty($args)) {
        $view_args = array_shift($args);
        $_REQUEST['view_args'] = rawurldecode($view_args);
      }

      module_load_include('inc', 'views', 'includes/ajax');
      $view = views_ajax();
      foreach ($view['#commands'] as $command) {
        if ($command['command'] == 'insert') {
          return array('#markup' => trim($command['data']));
        }
      }
      return array();
    }

    // Non-ajax rendering of a view.
    if ($this->rendered_content) {
      return $this->rendered_content;
    }
    $item = $this->settings;
    $output = array();
    if (isset($item['vid'])) {
      if (module_exists('views')) {
        if ($view = views_get_view($item['vid'])) {
          if ($view->access($item['display'])) {
            $view->set_display($item['display']);
            $view->set_arguments($item['actual_args']);
            $view_output = $view->preview();
            if (!empty($view->result) || $view->display_handler->get_option('empty') || !empty($view->style_plugin->definition['even empty'])) {
              $output['#markup'] = $view_output;
            }
          }
          elseif (!$hide_empty) {
            $output['#markup'] = theme('quicktabs_tab_access_denied', array('tab' => $item));
          }
          $view->destroy();
        }
      }
      elseif (!$hide_empty) {
        $output['#markup'] = t('Views module is not enabled, cannot display content.');
      }
    }
    $this->rendered_content = $output;
    return $output;
  }

  public function getAjaxKeys() {
    return array('vid', 'display', 'view_dom_id', 'view_path', 'ajax_args');
  }
}