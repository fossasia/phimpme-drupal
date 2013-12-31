<?php
/**
 * @file content_migrate.admin.inc
 * Code to process field data migration, moved into a separate file for efficiency.
 */

/**
 * Determine which fields can be migrated, have already been migrated, and are
 * unable to be migrated due to missing modules.
 */
function content_migrate_get_options() {
  $options = array('available' => array(), 'converted' => array(), 'missing' => array());
  
  $field_values = content_migrate_get_field_values();

  if (empty($field_values)) {
    drupal_set_message(t('There is no D6 field information in this database.'));
    return FALSE;
  }

  $type_names = node_type_get_names();
  $new_fields = array_keys(field_info_fields());
  
  // Figure out which field and widget modules are available.
  $available_modules = array_unique(array_merge(module_implements('field_info'), module_implements('field_widget_info')));
  
  foreach ($field_values as $field_name => $field_value) {
    $messages = array();
    $bundles = array();
    $missing_module = !in_array($field_value['module'], $available_modules);
    if ($missing_module) {
      $messages[] = '<span class="error">' . t("Missing field module: '@field'. This field cannot be migrated.", array('@field' => $field_value['module'])) . '</span>';
    }
    if (isset($field_value['messages'])) {
      $messages = array_merge($messages, $field_value['messages']);
      unset($field_value['messages']);
    }

    $instance_values = content_migrate_get_instance_values(NULL, $field_name);


    // Debug
    //dsm($field_value);
    //dsm($instance_values);
    
    foreach ($instance_values as $bundle => $instance_value) {
      if (isset($instance_value['messages'])) { 
        $messages = array_merge($messages, $instance_value['messages']);    
        unset($instance_values[$bundle]['messages']);
      }
      $bundles[] = $type_names[$bundle];
      $label = $instance_value['label'];
    }
    $data = array(
      0 => $field_name,
      1 => $field_value['type'],
      2 => theme('item_list', array('items' => $bundles)),
      3 => theme('item_list', array('items' => $messages)),
    );
    if (in_array($field_name, $new_fields)) {
      $options['converted'][$field_name] = $data;
    }
    // TODO, do we need to check for more than the mere presence of a module?
    elseif ($missing_module) {
      $options['missing'][$field_name] = $data;
    }
    else {
      $options['available'][$field_name] = $data;
    }
  }
  return $options;
}

/**
 * Form generator for the migration selection form.
 * 
 * @todo Make this into a nice table where you have 
 * an option to check all available fields to migrate
 * them all at once.
 */
function content_migrate_select($form, &$form_state) {
  $form = array();
  $options = content_migrate_get_options();
  if (!$options) {
    return $form;
  }

  $header = array(t('Field'), t('Field type'), t('Content type(s)'), t('Other information'));
  $form['#tree'] = TRUE;
  $form['available'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['available']) < 1,
    '#title' => t('Available fields'),
    '#description' => t('Fields that have not yet been migrated but are available for migration. <strong>Please carefully read the messages next to each field before migrating it to understand changes that might be made.</strong>'),
  );
  $form['available']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['available'],
    '#empty' => t('No fields are available to be migrated.'),
  );  
  $form['available']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Migrate selected fields'),
    '#submit' => array('content_migrate_select_submit'),
  );  

  $form['converted'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['converted']) < 1,
    '#title' => t('Converted fields'),
    '#description' => '<p>'. t('Fields that have already been converted. You can choose to roll them back if the conversion did not work correctly. Note that rolling fields back will completely destroy the new field tables.') . ' <span class="error"><strong>' . t('This operation cannot be undone!') . '</strong></span>',
  );    
  $form['converted']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['converted'],
    '#empty' => t('No fields are already converted.'),
  );
  $form['converted']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Roll back selected fields'),
    '#submit' => array('content_migrate_rollback_submit'),
  );     
  
  $form['missing'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['missing']) < 1,
    '#title' => t('Unavailable fields'),
    '#description' => t('Fields that cannot be migrated because some modules are missing.'),
  );  
  $form['missing']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['missing'],
    '#empty' => t('No fields have missing modules.'),
  ); 
      
  return $form;
}

/**
 * Submit handler.
 * 
 * @TODO add a confirmation on the rollback submission.
 */
function content_migrate_rollback_submit($form, &$form_state) {
  $field_names = array_filter($form_state['values']['converted']['data']);
  content_migrate_rollback($field_names);
}

/**
 * Helper function to perform rollback.
 */
function content_migrate_rollback($field_names) {
  foreach ($field_names as $field_name) {
    $field = field_info_field($field_name);
    
    // Deleting the field only marks it for deletion.
    field_delete_field($field_name);
    
    // We are bypassing the field batch processing
    // and simply deleting all the data.
    // The assumption is that the migration was
    // unsuccessful and will be re-attempted
    // and we need to remove all traces of the 
    // new field for later migrations to work.
    $new_table = content_migrate_new_table($field);
    db_drop_table($new_table);
    
    $instances = field_read_instances(array('field_id' => $field['id']), array('include_deleted' => 1));
    foreach ($instances as $instance) {
      field_purge_instance($instance);
    }
    field_purge_field($field);
    drupal_set_message(t('Rolling back @field_name.', array('@field_name' => $field_name)));
  }
}

/**
 * Submit handler.
 */
function content_migrate_select_submit($form, &$form_state) {
  $field_names = array_filter($form_state['values']['available']['data']);
  _content_migrate_batch($field_names);
}

/**
 * Helper function to create a batch.
 */
function _content_migrate_batch($field_names) {
  $batch = array(
    'title' => t('Migrating data'),
    'file'  => drupal_get_path('module', 'content_migrate') . '/includes/content_migrate.admin.inc',
    'operations' => array(),
    'finished' => "Field migration is finished",
    'init_message' => t("Fields migration is starting."),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Field migration has encountered an error.'),
  );
  // Migrate field data one field at a time.
  foreach ($field_names as $field_name) {
    $batch['operations'][] = array('_content_migrate_batch_process_create_fields', array($field_name));
    $batch['operations'][] = array('_content_migrate_batch_process_migrate_data', array($field_name));
  }
  batch_set($batch);
}

/**
 * Batch operation callback to create fields.
 */
function _content_migrate_batch_process_create_fields($field_name, &$context) {
  $messages = array();
  $errors = array();
  $type_names = node_type_get_names();
  $allowed_fields = field_info_field_types();
  $allowed_widgets = field_info_widget_types();
  $allowed_formatters = field_info_formatter_types();
  
  $context['message'] = t('"Creating field: %field', array('%field' => $field_name));
  $field_value = content_migrate_get_field_values($field_name);
  if (isset($field_value['messages'])) {
    $messages = $field_value['messages'];
    unset($field_value['messages']);
  }
  $instance_info = field_info_instances('node');

  // Create the field and store the new field 
  // definition in $context so we can retrieve it later. 
  try {
    // A shared field may already have been created, check first.
    $field = field_info_field($field_value['field_name']);
    if (empty($field)) {
      unset($field_value['columns']);
      unset($field_value['db_storage']);   
      $field = field_create_field($field_value);
      $context['fields'][$field_name] = $field;
      $messages[] = t("Created field @field_name", array('@field_name' => $field_name));
    }
 
    // Create each of the new instances and store 
    // the new instance definitions in $context.
    $instance_values = content_migrate_get_instance_values(NULL, $field_name);

    foreach ($instance_values as $bundle => $instance_value) {
      try {

        if (isset($instance_value['messages'])) { 
          $messages = array_merge($messages, $instance_value['messages']);    
          unset($instance_value['messages']);
        }

        if (!isset($instance_info[$bundle][$field_name])) {
          $instance = field_create_instance($instance_value); 
          $context['instances'][$field_name][$bundle] = $instance;
          $messages[] = t("Created instance of @field_name in bundle @bundle.", array(
            '@field_name' => $field_name, '@bundle' => $type_names[$bundle]));
        }
        
      }
      catch (Exception $e) {
        $errors[] = t('Error creating instance of @field_name in bundle @bundle.', array(
          '@field_name' => $field_name, '@bundle' => $type_names[$bundle]));
        $errors[] = $e;
      }
    }
  }
  catch (Exception $e) {
    $errors[] = t("Error creating field @field_name", array('@field_name' => $field_name)); 
    $errors[] = $e;
  }
  field_info_cache_clear();

  foreach ($messages as $message) {
    drupal_set_message($message, 'warning');
  }
  foreach ($errors as $error) {
    drupal_set_message($error, 'error');
  }

  $context['finished'] = TRUE;
} 

/**
 * Batch operation callback to migrate data.
 * Copy old table data to new field table.
 */
function _content_migrate_batch_process_migrate_data($field_name, &$context) {

  // The first time through, find all the nodes that have this field.
  if (!isset($context['sandbox']['progress'])) {

    $field_value = content_migrate_get_field_values($field_name);
    if (isset($field_value['messages'])) {
      unset($field_value['messages']);
    }
    $instance_values = content_migrate_get_instance_values(NULL, $field_name);
    if (isset($instance_values['messages'])) {
      unset($instance_values['messages']);
    }
    $types = array();
    foreach ($instance_values as $bundle => $instance_value) {
      $types[] = $bundle;
    }
    $field = field_info_field($field_name);  
    $old_table          = content_migrate_old_table($field_value, $instance_value);
    $old_cols           = content_migrate_old_columns($field_value, $instance_value);
    $new_table          = content_migrate_new_table($field);
    $new_revision_table = content_migrate_new_revision($field);
    $new_columns        = content_migrate_new_columns($field);
    // Shared, non-multiple fields do not have a delta but are still in per-field tables.
    $add_delta          = $field_value['cardinality'] != 1 && content_migrate_storage_type($field_value, $instance_value) == CONTENT_DB_STORAGE_PER_FIELD;

    $query = db_select($old_table, 'old_table', array('fetch' => PDO::FETCH_ASSOC));
    $node_alias = $query->join('node', 'n', 'old_table.nid=n.nid');
    $result = $query
      ->fields($node_alias, array('title', 'type', 'vid', 'language'))
      ->fields('old_table', array('nid'))
      ->orderBy('nid', 'ASC')
      ->distinct()
      ->execute();
    
    $nodes = array();
    foreach ($result as $row) {
      $nodes[] = array('nid' => $row['nid'], 'title' => $row['title'], 'type' => $row['type'], 'vid' => $row['vid'], 'language' => $row['language']);
    }
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($nodes);
    $context['sandbox']['nodes'] = $nodes;
    $context['sandbox']['old_table'] = $old_table;
    $context['sandbox']['new_table'] = $new_table;
    $context['sandbox']['new_revision_table'] = $new_revision_table;
    $context['sandbox']['old_cols'] = $old_cols;
    $context['sandbox']['new_cols'] = $new_columns;
    $context['sandbox']['types'] = $types;
    $context['sandbox']['field'] = $field;
    $context['sandbox']['add_delta'] = $add_delta;
   
  }

  // Process one node in each batch.

  $node = array_shift($context['sandbox']['nodes']);
  if (!$node) {
    return;
  }

  $field = field_info_field($field_name);
  $instance = field_info_instance('node', $field_name, $node['type']);

  // Construct an record to insert into the new field table
  // from the data in the old table.

  $query = db_select($context['sandbox']['old_table'], 'old_table', array('fetch' => PDO::FETCH_ASSOC));
    
  // We need new columns for bundle name, entity type, and language.
  // See the debate going on at http://drupal.org/node/1164852.
  // Reverting back to setting all nodes as untranslated.
  $language = LANGUAGE_NONE;
  if ($field['translatable']) {
    //$language = $node['language'];
  }
  $query->addExpression("'". $node['type'] ."'", 'bundle');
  $query->addExpression("'node'", 'entity_type');
  $query->addExpression("'". $language ."'", 'language');
    
  // There are new names for what were the nid and vid columns.
  $query->addField('old_table', 'nid', 'entity_id');
  $query->addField('old_table', 'vid', 'revision_id');
    
  // Add the field columns to the select query.
  // Use the new column names as aliases in case the 
  // name changed, hopefully none did.
  foreach ($context['sandbox']['old_cols'] as $column_name => $db_column_name) {
    $query->addField('old_table', $db_column_name, $context['sandbox']['new_cols'][$column_name]);
  }
    
  // Add delta, or construct it if missing.
  if ($context['sandbox']['add_delta']) {
    $query->addField('old_table', 'delta', 'delta');
  }
  else {
    $query->addExpression(0, 'delta');
  }
  $query->condition('nid', $node['nid']);
  $result = $query->execute();

  foreach ($result as $record) {
  
    // Let modules alter this before the insert.
    drupal_alter('content_migrate_data_record', $record, $field, $instance);

    // Don't save empty values.
    if (!empty($record)) {
      $function = $field['module'] . '_field_is_empty';
      if (function_exists($function)) {
        // The $record array has the database columns as keys, which drupal_write_record() will need,
        // but the _field_is_empty() function will be looking for the short, normalized column name.
        $item = array();
        foreach ($context['sandbox']['new_cols'] as $column_name => $db_column_name) {
          if (array_key_exists($db_column_name, $record)) {
            $item[$column_name] = $record[$db_column_name];
          }
        }
        if ($function($item, $field)) {
          $record = NULL;
        }
      }
    }

    if (!empty($record)) {
      if ($record['revision_id'] == $node['vid']) {
        drupal_write_record($context['sandbox']['new_table'], $record);
      }
      drupal_write_record($context['sandbox']['new_revision_table'], $record);
    }
  }

  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['message'] = t('Processing %nid : %title', array('%title' => $node['title'], '%nid' => $node['nid']));

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}


