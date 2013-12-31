<?php
/**
 * @file
 * Contains the base row style plugin.
 */

/**
 * The basic 'fields' row plugin
 *
 * This displays fields one after another, giving options for inline
 * or not.
 *
 * @ingroup views_row_plugins
 */
class views_galleriffic_plugin_row_gallerifficrows extends views_plugin_row {
  function option_definition() {
    $options = parent::option_definition();
    $options['slide_field'] = array('default' => '');
    $options['thumbnail_field'] = array('default' => '');
    $options['title_field'] = array('default' => '');
    $options['description_field'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Pre-build all of our option lists for the dials and switches that follow.
    $fields = array('' => t('<none>'));
    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
      if ($label = $handler->label()) {
        $fields[$field] = $handler->label() .  ': ' . $handler->options['settings']['image_style'];
      }
      else {
        $fields[$field] = $handler->ui_name();
      }

    }
    $form['title_field'] = array(
      '#type' => 'select',
      '#title' => t('Title field'),
      '#options' => $fields,
      '#default_value' => $this->options['title_field'],
      '#description' => t('Select the field that will be used as the title field. Optional.'),
    );
    $form['description_field'] = array(
      '#type' => 'select',
      '#title' => t('Description field'),
      '#options' => $fields,
      '#default_value' => $this->options['description_field'],
      '#description' => t('Select the field that will be used as the description. Optional.'),
    );
    $form['slide_field'] = array(
      '#type' => 'select',
      '#title' => t('Slide field'),
      '#options' => $fields,
      '#default_value' => $this->options['slide_field'],
      '#description' => t('Select the field that will be used as the image slide. REQUIRED.'),
    );
    $form['thumbnail_field'] = array(
      '#type' => 'select',
      '#title' => t('Thumbnail field'),
      '#options' => $fields,
      '#default_value' => $this->options['thumbnail_field'],
      '#description' => t('Select the field that will be used as the thumbnail field. REQUIRED.'),
    );
  }
}
