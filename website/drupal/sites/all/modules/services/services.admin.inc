<?php
function theme_services_resource_table($variables) {
  $table = $variables['table'];

  drupal_add_css(drupal_get_path('module', 'services') . '/css/services.admin.css');
  drupal_add_js(drupal_get_path('module', 'services') . '/js/services.admin.js');
  drupal_add_js('misc/tableselect.js');

  // Create header for resource selection table.
  $header = array(
    array('class' => array('select-all')),
    array('data' => t('Resource'), 'class' => array('resource_method')),
    array('data' => t('Description'), 'class' => array('resource_description')),
    array('data' => t('Alias'), 'class' => array('resource_alias')),
  );

  // Define the images used to expand/collapse the method groups.
  $js = array(
    'images' => array(
      theme('image', array('path' => 'misc/menu-collapsed.png', 'alt' => t('Expand'), 'title' => t('Expand'))) . ' <a href="#" class="resource-collapse">(' . t('Expand') . ')</a>',
      theme('image', array('path' => 'misc/menu-expanded.png', 'alt' => t('Collapse'), 'title' => t('Collapse'))) . ' <a href="#" class="resource-collapse">(' . t('Collapse') . ')</a>',
    ),
  );

  // Cycle through each method group and create a row.
  $rows = array();
  foreach (element_children($table) as $key) {
    $element = &$table[$key];
    $row = array();

    // Make the class name safe for output on the page by replacing all
    // non-word/decimal characters with a dash (-).
    $method_class = strtolower(trim(preg_replace("/[^\w\d]/", "-", $key)));

    // Select the right "expand"/"collapse" image, depending on whether the
    // category is expanded (at least one method selected) or not.
    $collapsed = !empty($element['#collapsed']);
    $image_index = $collapsed ? 0 : 1;

    // Place-holder for checkboxes to select group of methods.
    $row[] = array('id' => $method_class, 'class' => array('resource-select-all'));

    // Expand/collapse image and group title.
    $row[] = array(
      'data' => '<div class="resource-image" id="resource-method-group-' . $method_class . '"></div>' .
        '<label for="' . $method_class . '-select-all" class="resource-group-label">' . $key . '</label>',
      'class' => array('resource-group-label'),
    );

    $row[] = array(
      'data' => '&nbsp;',
      'class' => array('resource-group-description'),
    );
    $row[] = array(
      'data' => drupal_render($element['alias']),
      'class' => array('resource-group-alias'),
    );
    $rows[] = array('data' => $row, 'class' => array('resource-group'));

    // Add individual methods to group.
    $current_js = array(
      'methodClass' => $method_class . '-method',
      'methodNames' => array(),
      'imageDirection' => $image_index,
      'clickActive' => FALSE,
    );

    // Cycle through each method within the current group.
    foreach (element_children($element) as $method_name) {
      if($method_name != 'alias') {
        $method = $element[$method_name];
        $row = array();

        $current_js['methodNames'][] = $method['#id'];

        // Store method title and description so that checkbox won't render them.
        $title = $method['#title'];
        $description = $method['#description'];

        $method['#title_display'] = 'invisible';
        unset($method['#description']);

        // Test name is used to determine what methods to run.
        $method['#name'] = $method_name;

        $row[] = array(
          'data' => drupal_render($method),
          'class' => array('resource-method-select'),
        );
        $row[] = array(
          'data' => '<label for="' . $method['#id'] . '">' . $title . '</label>',
          'class' => array('resource-method-label'),
        );
        $row[] = array(
          'data' => '<div class="description">' . $description . '</div>',
          'class' => array('resource-method-description'),
        );
        $row[] = array(
          'data' => '<div class="alias">&nbsp;</div>',
          'class' => array('resource-method-alias'),
        );
        $rows[] = array('data' => $row, 'class' => array($method_class . '-method', ($collapsed ? 'js-hide' : '')));
      }

    }
    $js['resource-method-group-' . $method_class] = $current_js;
    unset($table[$key]);
  }

  // Add js array of settings.
  drupal_add_js(array('resource' => $js), 'setting');

  if (empty($rows)) {
    return '<strong>' . t('No resourcess to display.') . '</strong>';
  }
  else {
    return theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'resource-form-table')));
  }
}