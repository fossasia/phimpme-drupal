<?php

/**
 * @file
 * Plugin to provide an relationship handler for an entity from a field.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Entity'),
  'description' => t('Creates an entity context from a foreign key on a field.'),
  'context' => 'ctools_entity_from_field_context',
  'edit form' => 'ctools_entity_from_field_edit_form',
  'get child' => 'ctools_entity_from_field_get_child',
  'get children' => 'ctools_entity_from_field_get_children',
  'defaults' => array('delta' => 0),
);

function ctools_entity_from_field_get_child($plugin, $parent, $child) {
  $plugins = ctools_entity_from_field_get_children($plugin, $parent);
  return $plugins[$parent . ':' . $child];
}

function ctools_entity_from_field_get_children($parent_plugin, $parent) {
  $cid = $parent_plugin['name'] . ':' . $parent;
  $cache = &drupal_static(__FUNCTION__);
  if (!empty($cache[$cid])) {
    return $cache[$cid];
  }

  ctools_include('fields');
  $entities = entity_get_info();
  $plugins = array();
  $context_types = array();

  // Get the schema information for every field.
  $fields_info = field_info_fields();
  foreach ($fields_info as $field_name => $field) {
    foreach ($field['bundles'] as $from_entity => $bundles) {
      foreach ($bundles as $bundle) {
        // There might be fields attached to bundles that are disabled (e.g. a
        // module that declared a node's content type, is now disabled), but the
        // field instance is still shown.
        if (!empty($entities[$from_entity]['bundles'][$bundle])) {
          $foreign_keys = ctools_field_foreign_keys($field_name);

          foreach ($foreign_keys as $key => $info) {
            if (isset($info['table'])) {
              foreach ($entities as $to_entity => $to_entity_info) {
                $from_entity_info = $entities[$from_entity];
                // If somehow the bundle doesn't exist on the to-entity,
                // skip.
                if (!isset($from_entity_info['bundles'][$bundle])) {
                  continue;
                }

                if (isset($to_entity_info['base table']) && $to_entity_info['base table'] == $info['table'] && array_keys($info['columns'], $to_entity_info['entity keys']['id'])) {
                  $name = $field_name . '-' . $from_entity . '-' . $to_entity;
                  $plugin_id = $parent . ':' . $name;

                  // Record the bundle for later.
                  $context_types[$plugin_id]['types'][$bundle] = $from_entity_info['bundles'][$bundle]['label'];

                  // We check for every bundle; this plugin may already have
                  // been created, so don't recreate it.
                  if (!isset($plugins[$plugin_id])) {
                    $plugin = $parent_plugin;
                    $replacements = array(
                      '@to_entity' => $to_entity_info['label'],
                      '@from_entity' => $from_entity_info['label'],
                      '@field_name' => $field_name,
                      '@field_label' => ctools_field_label($field_name),
                    );
                    $plugin['title'] = t('@to_entity from @from_entity (on @from_entity: @field_label [@field_name])', $replacements);
                    $plugin['keyword'] = $to_entity;
                    $plugin['context name'] = $name;
                    $plugin['name'] = $plugin_id;
                    $plugin['description'] = t('Creates a @to_entity context from @from_entity using the @field_name field on @from_entity.', $replacements);
                    $plugin['from entity'] = $from_entity;
                    $plugin['to entity'] = $to_entity;
                    $plugin['field name'] = $field_name;
                    $plugin['join key'] = $key;
                    $plugin['source key'] = current(array_keys($info['columns']));
                    $plugin['parent'] = $parent;

                    $plugins[$plugin_id] = $plugin;

/*
-- commented out until I figure out how to actually load the reverse properly.
                    // Build the reverse
                    $plugin = $parent_plugin;
                    $name = $field_name . '-' . $from_entity . '-' . $to_entity . '-reverse';
                    $plugin_id = $parent . ':' . $name;

                    $plugin['title'] = t('@from_entity from @to_entity (on @from_entity: @field_name)', $replacements);
                    $plugin['keyword'] = $to_entity;
                    $plugin['context name'] = $name;
                    $plugin['name'] = $plugin_id;
                    $plugin['description'] = t('Creates a @from_entity context from @to_entity using the @field_name field on @from_entity.', $replacements);

                    $plugin['from entity'] = $from_entity;
                    $plugin['to entity'] = $to_entity;
                    $plugin['field name'] = $field_name;
                    $plugin['reverse'] = TRUE;
                    $plugin['parent'] = $parent;

                    // Since we can't apply restrictions on the reverse relationship
                    // we just add the required context here.
                    $plugin['required context'] = new ctools_context_required($to_entity_info['label'], $to_entity);

                    $plugin_entities = array(
                      'to' => array($from_entity => $from_entity_info),
                      'from' => array($to_entity => $to_entity_info)
                    );
                    drupal_alter('ctools_entity_context', $plugin, $plugin_entities, $plugin_id);

                    $plugins[$plugin_id] = $plugin;
*/
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  foreach ($context_types as $key => $context) {
    list($parent, $plugin_name) = explode(':', $key);
    list($field_name, $from_entity, $to_entity) = explode('-', $plugin_name);

    $from_entity_info = $entities[$from_entity];
    $to_entity_info = $entities[$to_entity];

    $plugins[$key]['required context'] = new ctools_context_required($from_entity_info['label'], $from_entity, array('type' => array_keys($context['types'])));

    $plugin_entities = array(
      'to' => array($to_entity => $to_entity_info),
      'from' => array($from_entity => $from_entity_info)
    );
    drupal_alter('ctools_entity_context', $plugins[$key], $plugin_entities, $key);
  }
  drupal_alter('ctools_entity_contexts', $plugins);

  $cache[$cid] = $plugins;
  return $plugins;
}

/**
 * Return a new context based on an existing context.
 */
function ctools_entity_from_field_context($context, $conf) {
  $delta = !empty($conf['delta']) ? intval($conf['delta']) : 0;
  $plugin = $conf['name'];
  list($plugin, $plugin_name) = explode(':', $plugin);
  list($field_name, $from_entity, $to_entity) = explode('-', $plugin_name);
  // If unset it wants a generic, unfilled context, which is just NULL.
  $entity_info = entity_get_info($from_entity);
  if (empty($context->data) || !isset($context->data->{$entity_info['entity keys']['id']})) {
    return ctools_context_create_empty('entity:' . $to_entity, NULL);
  }

  if (isset($context->data->{$entity_info['entity keys']['id']})) {
    // Load the entity.
    $id = $context->data->{$entity_info['entity keys']['id']};
    $entity = entity_load($from_entity, array($id));
    $entity = $entity[$id];
    if ($items = field_get_items($from_entity, $entity, $field_name)) {
      if (isset($items[$delta])) {
        ctools_include('fields');
        $to_entity_info = entity_get_info($to_entity);
        $plugin_info = ctools_get_relationship($conf['name']);
        $to_entity_id = $items[$delta][$plugin_info['source key']];

        // Send it to ctools.
        return ctools_context_create('entity:' . $to_entity, $to_entity_id);
      }
      else {
        // In case that delta was empty.
        return ctools_context_create_empty('entity:' . $to_entity, NULL);
      }
    }
  }
}

function ctools_entity_from_field_edit_form($form, &$form_state) {
  $field = field_info_field($form_state['plugin']['field name']);
  $conf = $form_state['conf'];

  if ($field && $field['cardinality'] != 1) {
    if ($field['cardinality'] == -1) {
      $form['delta'] = array(
        '#type' => 'textfield',
        '#title' => t('Delta'),
        '#description' => t('The relationship can only create one context, but multiple items can be related. Please select which one. Since this can have unlimited items, type in the number you want. The first one will be 0.'),
        '#default_value' => !empty($conf['delta']) ? $conf['delta'] : 0,
      );
    }
    else {
      $form['delta'] = array(
        '#type' => 'select',
        '#title' => t('Delta'),
        '#description' => t('The relationship can only create one context, but multiple items can be related. Please select which one.'),
        '#options' => range(1, $field['cardinality']),
        '#default_value' => !empty($conf['delta']) ? $conf['delta'] : 0,
      );
    }
  }

  return $form;
}
