<?php

/**
 * @file
 *   Handles admin pages for the Statuses module.
 */

/**
 * Settings form.
 */
function statuses_admin($form, $form_state) {
  $form['statuses_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum status length'),
    '#description' => t('Must be a positive integer, or zero for no maximum length.'),
    '#default_value' => variable_get('statuses_length', 140),
    '#size' => 3,
    '#maxlength' => 5,
    '#required' => TRUE,
    '#weight' => -40,
  );
  $formats = filter_formats($GLOBALS['user']);
  $options = array('none' => t('None (HTML escaped)'));
  foreach ($formats as $format) {
    $options[$format->format] = check_plain($format->name);
  }
  $form['statuses_filter'] = array(
    '#type' => 'select',
    '#title' => t('Input filter'),
    '#description' => '<strong>' . t('This input filter will run on all your statuses AND ACCESS TO IT IS NOT CHECKED, so make sure it is secure.') . '</strong>',
    '#default_value' => variable_get('statuses_filter', 'none'),
    '#options' => $options,
    '#weight' => -30,
  );
  $form['statuses_repost'] = array(
    '#type' => 'textfield',
    '#title' => t('Re-post format'),
    '#description' => t('The format of the default message when users click a link to re-post a status.') . ' ' .
      t("@name will be replaced with the poster's name and @message will be replaced with the status text."),
    '#default_value' => variable_get('statuses_repost', 'Re: @name @message '),
    '#weight' => -20,
  );
  $views = views_get_all_views();
  $list = array('' => t('None'));
  foreach ($views as $name => $view) {
    if (empty($view->disabled)) {
      $list[$name] = $name;
    }
  }
  $form['statuses_share_view'] = array(
    '#type' => 'select',
    '#title' => t('Show a view under the status update form at the "Share status" page at /statuses/share'),
    '#description' => t('The default statuses_stream view is recommended.'),
    '#default_value' => variable_get('statuses_share_view', $list['statuses_stream']),
    '#options' => $list,
    '#weight' => -10,
  );
  return system_settings_form($form);
}

/**
 * Validate function for the settings form.
 */
function statuses_admin_validate($form, &$form_state) {
  $size = $form_state['values']['statuses_length'];
  if (!is_numeric($size) || $size < 0 || $size != round($size)) {
    form_set_error('statuses_length', t('The maximum status length must be a positive integer, or zero for no maximum length.'));
  }
}

/**
 * Advanced settings form.
 */
function statuses_admin_advanced($form, $form_state) {
  $form['statuses_box_rows'] = array(
    '#type' => 'textfield',
    '#title' => t('Status update box rows'),
    '#description' => t('The default number of lines for the status update box.'),
    '#default_value' => variable_get('statuses_box_rows', 2),
    '#size' => 3,
    '#maxlength' => 3,
    '#required' => TRUE,
    '#weight' => -80,
  );
  $form['statuses_hide_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Show "Read more" after'),
    '#field_suffix' => t('characters'),
    '#description' => t('If a status message is longer than this number of characters and the viewing user has JavaScript enabled, the message will be truncated to this length and a "Read more" link will be appended.') . ' ' .
      t('When clicked, the "Read more" link will reveal the rest of the status message.') . ' ' .
      t('Must be a positive integer less than the maximum status message length, or zero to ignore this option.'),
    '#default_value' => variable_get('statuses_hide_length', 0),
    '#size' => 3,
    '#maxlength' => 5,
    '#required' => TRUE,
    '#weight' => -70,
  );
  $form['statuses_nl2br'] = array(
    '#type' => 'checkbox',
    '#title' => t('Convert line breaks'),
    '#description' => t('By default, line breaks in the status update form are ignored.') . ' ' .
      t('If you use an input filter to fix that, the status text will appear on a different line than the username and other information.') . ' ' .
      t('Enabling this setting allows line breaks to appear without the side-effect of the input filter.'),
    '#default_value' => variable_get('statuses_nl2br', 0),
    '#weight' => -60,
  );
  $form['statuses_refresh'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show AJAX "Refresh" link after status update views'),
    '#description' => t('Display a "Refresh" link under content that will be automatically refreshed when the status update form is submitted.') . ' ' .
      t('Clicking the link will similarly update the related content via JavaScript, without refreshing the page.') . ' ' .
      t('Obviously, the "Refresh" link will only appear when there is something that can be refreshed.') . ' ' .
      t('If you disable AHAH refreshing below or if you have no views attached to your status update forms, this setting is useless.'),
    '#default_value' => variable_get('statuses_refresh', 0),
    '#weight' => -50,
  );
  $form['statuses_profile'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show the status stream on the default user profiles'),
    '#description' => t('If enabled, the status update form and view will appear on default user profiles.') . ' ' .
      t('This setting should only matter if you have not overridden the default user profile.'),
    '#default_value' => variable_get('statuses_profile', 1),
    '#weight' => -40,
  );
  $form['statuses_enter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Submit the status update form with Shift+Enter'),
    '#description' => t('Enable this setting to allow submitting the status update form by pressing Shift+Enter in the status update box.'),
    '#default_value' => variable_get('statuses_enter', 0),
    '#weight' => -35,
  );
  $form['statuses_default_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Default text for personal status updates'),
    '#description' => t('The text that appears by default in the status update box for a personal status update.') . ' ' .
      t('This will be automatically cleared when a user clicks in (or focuses on) the status update box.'),
    '#default_value' => variable_get('statuses_default_text', ''),
    '#weight' => -30,
  );
  $form['statuses_default_text_other'] = array(
    '#type' => 'textfield',
    '#title' => t('Default text when sending status messages'),
    '#description' => t('The text that appears by default in the status update box when sending a status message to another entity.') . ' ' .
      t('This will be automatically cleared when a user clicks in (or focuses on) the status update box.'),
    '#default_value' => variable_get('statuses_default_text_other', ''),
    '#weight' => -29,
  );
  $form['statuses_reply_type'] = array(
    '#type' => 'select',
    '#title' => t('Reply method'),
    '#default_value' => variable_get('statuses_reply_type', 'at'),
    '#options' => array(
      'at' => t('Reply by @mention'),
      'convo' => t('Reply in conversation view'),
    ),
    '#weight' => -20,
  );
  if (module_exists('imagecache_profiles')) {
    $presets = image_styles();
    $opt = array('' => t('Default (let Imagecache Profiles choose)'));
    foreach ($presets as $name => $preset) {
      $opt[$name] = check_plain($name);
    }
    $form['statuses_imagecache_preset'] = array(
      '#type' => 'select',
      '#title' => t('Default image style for user pictures'),
      '#default_value' => variable_get('statuses_imagecache_preset', variable_get('user_picture_imagecache_profiles_default', '')),
      '#options' => $opt,
      '#weight' => -15,
    );
  }
  $form['statuses_ahah'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use AHAH to refresh the status update form without refreshing the page'),
    '#description' => t("You should leave this checked unless you know what you're doing."),
    '#default_value' => variable_get('statuses_ahah', 1),
    '#weight' => -10,
  );
  return system_settings_form($form);
}

/**
 * Validate function for the advanced settings form.
 */
function statuses_admin_advanced_validate($form, &$form_state) {
  $size = $form_state['values']['statuses_box_rows'];
  if (!is_numeric($size) || $size < 1 || $size != round($size)) {
    form_set_error('statuses_box_rows', t('The number of rows of the status update box must be a positive integer.'));
  }
  $length = $form_state['values']['statuses_hide_length'];
  if (!is_numeric($length) || $length < 0 || ($length >= variable_get('statuses_length', 140) && variable_get('statuses_length', 140) != 0) || $length != round($length)) {
    form_set_error('statuses_hide_length',
      t('The "Read more after" option requires that the number of characters specified is a positive integer less than the maximum status message length, or zero to ignore this option.'));
  }
}

/**
 * The draggable-table stream context configuration form.
 */
function statuses_admin_contexts($form, &$form_state) {
  $data = statuses_all_contexts();
  $form = array();
  $form['components'] = array();
  $form['components']['#tree'] = TRUE;
  $count = count($data);
  $views = views_get_all_views();
  $options = array('' => t('None'));
  foreach ($views as $name => $view) {
    if (empty($view->disabled)) {
      $options[$name] = $name;
    }
  }
  foreach ($data as $type => $values) {
    $form['components'][$type]['title'] = array(
      '#prefix' => '<div title="' . (isset($values['description']) ? $values['description'] : '') . '">',
      '#markup' => $values['title'],
      '#suffix' => '</div>',
    );
    $form['components'][$type]['view'] = array(
      '#type' => 'select',
      '#default_value' => isset($values['view']) ? $values['view'] : '',
      '#options' => $options,
    );
    $form['components'][$type]['link'] = array(
      '#markup' => l(
        t('Configure'),
        'admin/config/statuses/contexts/' . $type,
        array('query' => array('destination' => 'admin/config/statuses/contexts'))
      ),
    );
    $form['components'][$type]['weight'] = array(
      '#type' => 'weight',
      '#delta' => $count,
      '#default_value' => isset($values['weight']) ? $values['weight'] : 0,
    );
    $form['components'][$type]['type'] = array(
      '#type' => 'hidden',
      '#value' => $type,
    );
    $form['components'][$type]['already_saved'] = array(
      '#type' => 'hidden',
      '#value' => isset($values['in_db']) ? $values['in_db'] : FALSE,
    );
  }
  // The user context has 2 views.
  $self = $options[isset($data['user']['view']) ? $data['user']['view'] : ''];
  $other = variable_get('statuses_user_other_view', 'statuses_stream');
  if ($self != $other) {
    $form['components']['user']['view'] = array(
      '#markup' => t('%self and %other - view configuration to change', array(
        '%self' => $self,
        '%other' => $other,
      )),
    );
  }
  $form['components']['user']['view_value'] = array(
    '#type' => 'hidden',
    '#value' => $self,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * The submit callback for the stream context configuration form.
 */
function statuses_admin_contexts_submit($form, &$form_state) {
  $self = $form_state['values']['components']['user']['view_value'];
  $other = variable_get('statuses_user_other_view', 'statuses_stream');
  // If we are using the same view for the user "self" and "other" cases, save any changes to both places.
  if ($self == $other) {
    variable_set('statuses_user_other_view', $form_state['values']['components']['user']['view']);
  }
  // If not, we didn't expose the view option in the form, so make sure nothing changes.
  else {
    $form_state['values']['components']['user']['view'] = $self;
  }
  // Save the usual data.
  foreach ($form_state['values']['components'] as $type => $item) {
    $record = array(
      'type' => $item['type'],
      'weight' => intval($item['weight']),
      'view' => $item['view'],
    );
    if ($item['already_saved']) {
      drupal_write_record('statuses_contexts', $record, 'type');
    }
    else {
      drupal_write_record('statuses_contexts', $record);
    }
  }
}

/**
 * Themes the stream context configuration form in a draggable table.
 */
function theme_statuses_admin_contexts($variables) {
  $form = $variables['form'];
  drupal_add_tabledrag('statuses-admin-contexts-table', 'order', 'sibling', 'weight-group');
  $header = array(t('Title'), t('View'), t('Actions'), t('Weight'), '', '');
  $rows = array();
  foreach (element_children($form['components']) as $key) {
    $element = &$form['components'][$key];
    $element['title']['#attributes']['class']         = array('title-group');
    $element['view']['#attributes']['class']          = array('view-group');
    $element['link']['#attributes']['class']          = array('link-group');
    $element['weight']['#attributes']['class']        = array('weight-group');
    $element['type']['#attributes']['class']          = array('type');
    $element['already_saved']['#attributes']['class'] = array('already-saved');
    $row = array();
    $row[] = drupal_render($element['title']);
    $row[] = drupal_render($element['view']);
    $row[] = drupal_render($element['link']);
    $row[] = drupal_render($element['weight']);
    $row[] = drupal_render($element['type']);
    $row[] = drupal_render($element['already_saved']);
    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }
  $output = '<p>' . t('The view assigned to the "User profiles" context does not affect the view shown on conversation pages.') . '</p>';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'statuses-admin-contexts-table')));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * The individual context settings form.
 */
function statuses_admin_context_settings($form, $form_state, $context) {
  drupal_set_title(t('Context settings: !type', array('!type' => $context['title'])));
  $type = $context['handler']->type();
  $form = array();

  if ($type == 'user') {
    $form['notice'] = array(
      '#markup' => '<p>' . t('The view assigned to the "User profiles" context does not affect the view shown on conversation pages.') . '</p>',
    );
  }

  $views = views_get_all_views();
  $options = array('' => t('None'));
  foreach ($views as $name => $view) {
    if (empty($view->disabled)) {
      $options[$name] = $name;
    }
  }
  $form['view'] = array(
    '#type' => 'select',
    '#title' => t('View'),
    '#description' => t('Show this view for !type streams.', array('@type' => $context['title'])) . ' ' . t('The "default" display will be used.'),
    '#default_value' => isset($context['view']) ? $context['view'] : '',
    '#options' => $options,
  );
  if ($type == 'user') {
    $form['view']['#title'] = t('View for "my stream"');
    $form['view']['#description'] = t('Show this view for user streams when a user is looking at their own stream.') . ' ' . t('The "default" display will be used.');
    $form['other_view'] = array(
      '#type' => 'select',
      '#title' => t("View for others users' streams"),
      '#description' => t("Show this view for user streams when a user is looking at another user's stream.") . ' ' . t('The "default" display will be used.'),
      '#default_value' => variable_get('statuses_user_other_view', 'statuses_stream'),
      '#options' => $options,
    );
  }

  $form['selectors'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => isset($context['selectors']) ? $context['selectors'] : '',
    '#title' => t('Refreshable DOM selectors'),
    '#description' => '<p>' .
      t('Enter CSS selectors that specify sections of the page that should be automatically refreshed via AJAX when a status of that type is submitted.') . ' ' .
      t('For example, to automatically update all &lt;div&gt; elements with class "myclass", enter "div.myclass" in the box (without the quotes).') . ' ' .
      t('Do not include selectors that include the status update form.') . ' ' .
      t('Enter each selector on a separate line.') .
      '</p>',
  );

  // adapted from block_admin_configure()
  $form['page_vis_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced: change where this context applies'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $access = user_access('use PHP for context visibility');
  if ($context['visibility'] == 2 && !$access) {
    $form['page_vis_settings'] = array();
    $form['page_vis_settings']['visibility'] = array(
      '#type' => 'value',
      '#value' => 2,
    );
    $form['page_vis_settings']['pages'] = array(
      '#type' => 'value',
      '#value' => $context['pages'],
    );
  }
  else {
    $form['page_vis_settings']['warning'] = array(
      '#markup' => '<p><strong>' . t('Changing these values could break your site. Only change these settings if you know what you are doing.') . '</strong></p>
        <p>' . t('Specifically, most contexts make assumptions about what pages they are enabled on in order to identify the correct recipient of status messages.') . ' ' .
          t('Enabling a context on an unsupported page will cause that context to fail.') . ' ' .
          t('Additionally, the User context should apply on all pages as a fallback. If no context applies on a given page, errors will occur.') . '</p>',
    );
    $options = array(
      -1 => t('Use module default settings.'),
      BLOCK_VISIBILITY_NOTLISTED => t('Show on every page except the listed pages.'),
      BLOCK_VISIBILITY_LISTED => t('Show on only the listed pages.'),
    );
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));
    if ($access) {
      $options[BLOCK_VISIBILITY_PHP] = t('Show if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= ' ' . t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    if (module_exists('context')) {
      $options[3] = t('Use the conditions from a Context from the Context module');
      drupal_add_js(drupal_get_path('module', 'statuses') . '/resources/statuses_admin.js');
    }
    $form['page_vis_settings']['visibility'] = array(
      '#type' => 'radios',
      '#title' => t('Apply context on specific pages'),
      '#options' => $options,
      '#default_value' => isset($context['visibility']) ? ($context['visibility'] == 3 && !module_exists('context') ? -1 : $context['visibility']) : -1,
    );
    if (module_exists('context')) {
      $form['page_vis_settings']['context'] = array(
        '#type' => 'select',
        '#title' => t('Context'),
        '#description' => t('Choose a context provided by the Context module whose conditions should be evaluated to determine whether the stream context applies.'),
        '#default_value' => isset($context['context']) ? $context['context'] : '',
        '#options' => drupal_map_assoc(array_keys(context_enabled_contexts())),
        '#states' => array(
          'visible' => array(
            'input:radio[name="visibility"]' => array('value' => 3),
          ),
        ),
      );
    }
    else {
      $form['page_vis_settings']['context'] = array(
        '#type' => 'value',
        '#value' => isset($context['context']) ? $context['context'] : '',
      );
    }
    $form['page_vis_settings']['pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => isset($context['pages']) ? $context['pages'] : '',
      '#description' => $description,
      '#states' => array(
        'invisible' => array(
          'input:radio[name="visibility"]' => array('value' => 3),
        ),
      ),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['type'] = array(
    '#type' => 'hidden',
    '#value' => $type,
  );
  $form['already_saved'] = array(
    '#type' => 'hidden',
    '#value' => isset($context['in_db']) ? $context['in_db'] : FALSE,
  );
  $form['weight'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($context['weight']) ? $context['weight'] : 0,
  );
  return $form;
}

/**
 * The submit callback for the individual stream context configuration form.
 */
function statuses_admin_context_settings_submit($form, &$form_state) {
  $v = $form_state['values'];
  $record = array(
    'type' => $v['type'],
    'weight' => intval($v['weight']),
    'view' => $v['view'],
    'selectors' => $v['selectors'],
    'visibility' => intval($v['visibility']),
    'pages' => $v['pages'],
    'context' => $v['context'],
  );
  if ($v['already_saved']) {
    drupal_write_record('statuses_contexts', $record, 'type');
  }
  else {
    drupal_write_record('statuses_contexts', $record);
  }
  if (isset($v['other_view'])) {
    variable_set('statuses_user_other_view', $v['other_view']);
  }
  drupal_set_message(t('The configuration options have been saved.'));
}
