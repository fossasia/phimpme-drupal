<?php

/**
 * Class for tab content of type "node" - this is for rendering a node as tab
 * content.
 */
class QuickNodeContent extends QuickContent {
  
  public static function getType() {
    return 'node';
  }

  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $form['node']['nid'] = array(
      '#type' => 'textfield',
      '#title' => t('Node'),
      '#description' => t('The node ID of the node.'),
      '#maxlength' => 10,
      '#size' => 20,
      '#default_value' => isset($tab['nid']) ? $tab['nid'] : '',
    );
    $form['node']['teaser'] = array(
      '#type' => 'checkbox',
      '#title' => t('Teaser view'),
      '#default_value' => isset($tab['teaser']) ? $tab['teaser'] : 0,
    );
    $form['node']['hide_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the title of this node'),
      '#default_value' => isset($tab['hide_title']) ? $tab['hide_title'] : 1,
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
      list($item['nid'], $item['teaser'], $item['hide_title']) = $args;
    }
    $output = array();
    if (isset($item['nid'])) {
      $node = node_load($item['nid']);
      if (!empty($node)) {
        if (node_access('view', $node)) {
          $buildmode = $item['teaser'] ? 'teaser' : 'full';
          $nstruct = node_view($node, $buildmode);
          if ($item['hide_title']) {
            $nstruct['#node']->title = NULL;
          }
          $output = $nstruct;
        }
        elseif (!$hide_empty) {
          $output = array('#markup' => theme('quicktabs_tab_access_denied', array('tab' => $item)));
        }
      }
    }
    return $output;
  }
  
  public function getAjaxKeys() {
    return array('nid', 'teaser', 'hide_title');
  }
}