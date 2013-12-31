<?php
/**
 * @file
 * Admin settings page and checked nodes summary table.
 */

/**
 * Page callback for settings page.
 */
function disable_breadcrumbs_settings_page() {
  $output = drupal_get_form('disable_breadcrumbs_settings_form');
  return $output;
}

/**
 * Settings/configuration form.
 */
function disable_breadcrumbs_settings_form() {
  
  $disable_breadcrumbs_all = variable_get('disable_breadcrumbs_all', NULL);
  
  if ($disable_breadcrumbs_all) {
    drupal_set_message(t('All breadcrumbs are currently disabled'), 'warning');
  }

  $content_types = array_map('check_plain', node_type_get_names());

  $form['node_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content type settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['node_types']['disable_breadcrumbs_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types to allow breadcrumbs to be disabled on'),
    '#description' => t('Any content types selected here will have a breadcrumb tab on node edit forms. They can also be administered at %path.', 
        array('%path' => 'admin/content/node')
      ),
    '#options' => $content_types,
    '#default_value' => variable_get('disable_breadcrumbs_node_types', array()),
    '#multiple' => TRUE,
  );
  $form['node_types']['disable_breadcrumbs_node_types_all'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types to disable ALL breadcrumbs for'),
    '#description' => t('For any content types checked here, breadcrumbs will be disabled for all nodes of this type.'),
    '#options' => $content_types,
    '#default_value' => variable_get('disable_breadcrumbs_node_types_all', array()),
    '#multiple' => TRUE,
  );
  $form['node_paths'] = array(
    '#type' => 'fieldset',
    '#title' => t('Path settings'),
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('disable_breadcrumbs_node_paths', "") ? FALSE : TRUE,
  );
  $form['node_paths']['disable_breadcrumbs_node_paths'] = array(
    '#type' => 'textarea',
    '#title' => t('Disable breadcrumbs by path'),
    '#description' => t('Specify pages by using their paths. Enter one path per line. 
        The \'*\' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog.'
      ),
    '#default_value' => variable_get('disable_breadcrumbs_node_paths', ""),
  );
  $form['disable_all'] = array(
    '#type' => 'fieldset',
    '#title' => t('ALL Breadcrumbs'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['disable_all']['disable_breadcrumbs_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable') . ' <b>' . t('ALL') . '</b> ' . t('breadcrumbs'),
    '#description' => t('This will disable all breadcrumbs on your site, regardless of entity type or page callback.'),
    '#default_value' => variable_get('disable_breadcrumbs_all', NULL),
  );
  $form['reset_breadcrumbs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reset'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['reset_breadcrumbs']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Reset disable breadcrumbs'),
    '#prefix' => '<div id="reset-breadcrumbs">',
    '#suffix' => '</div>',
    '#submit' => array('_disable_breadcrumbs_settings_form_delete_all_submit'),
    '#attributes' => array(
      'id' => 'reset-breadcrumbs',
      'onclick' => 'return confirm("' . t('Are you sure you want to clear the disable_breadcrumbs database table? This action cannot be undone.') . '")',
    ),
  );
  $form['reset_breadcrumbs']['markup'] = array(
    '#markup' => '<em>' . t('Reset disable breadcrumbs database table - disabled breadcrumb settings on all nodes will be removed.') . '</em>',
  );
  $form['checked_nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currently checked nodes'),
    '#weight' => 100,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['checked_nodes']['table'] = array(
    '#markup' => theme('disable_breadcrumbs_checked_nodes'),
  );
  $form['#redirect'] = FALSE;

  return system_settings_form($form);
}

/**
 * Removes all entries in disable_breadcrumbs table.
 */
function _disable_breadcrumbs_settings_form_delete_all_submit() {
  db_delete('disable_breadcrumbs')
    ->execute();
  drupal_set_message(t('The disable_breadcrumbs database table has been cleared.'));
}

/**
 * Produce sortable table of currently checked nodes
 */
function theme_disable_breadcrumbs_checked_nodes() {

  $header = array(
    array(
      'data' => 'Node (nid)',
      'field' => 'n.nid',
      'sort' => 'desc',
      ), 
    array(
      'data' => 'Type',
      'field' => 'n.type',
      ),
    array(
      'data' => 'Title',
      'field' => 'n.title',
      ),
    'Operations',
  );

  $query = db_select('node', 'n');
  $query->fields('n', array('nid', 'type', 'title'));
  $query->innerJoin('disable_breadcrumbs', 'db', 'n.nid = db.nid');
  $query
    ->extend('tableSort')
    ->orderByHeader($header);
  $query->addTag('node_access');

  $result = $query->execute();

  $rows = array();
  foreach ($result as $row) {
    $rows[$row->nid] = array(
      $row->nid,
      $row->type,
      l($row->title, 'node/' . $row->nid),
      '<a href="' . base_path() . 'node/' . $row->nid . '/edit' . '">Edit</a>',
    );
  }
  
  if ($rows) {
    $table = array(
      'header' => $header,
      'rows' => $rows,
    );

    $output = theme('table', $table);
  }
  else {
    $output = '<p>' . t('There are currently no checked nodes.') . '</p>';
  }

  return $output;
}
