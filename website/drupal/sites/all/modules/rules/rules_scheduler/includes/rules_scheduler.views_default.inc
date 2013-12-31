<?php

/**
 * @file
 * Views integration for the rules scheduler module.
 */

/**
 * Implements hook_views_default_views().
 */
function rules_scheduler_views_default_views() {
  $view = new view;
  $view->name = 'rules_scheduler';
  $view->description = 'Scheduled Rules components';
  $view->tag = '';
  $view->base_table = 'rules_scheduler';
  $view->api_version = '3.0-alpha1';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer rules';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '30';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'tid' => 'tid',
    'config' => 'config',
    'date' => 'date',
    'identifier' => 'identifier',
    'nothing' => 'nothing',
  );
  $handler->display->display_options['style_options']['default'] = 'date';
  $handler->display->display_options['style_options']['info'] = array(
    'tid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'config' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'date' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'identifier' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  /* Empty text: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No tasks have been scheduled.';
  $handler->display->display_options['empty']['area']['format'] = 'plain_text';
  /* Field: Rules scheduler: Tid */
  $handler->display->display_options['fields']['tid']['id'] = 'tid';
  $handler->display->display_options['fields']['tid']['table'] = 'rules_scheduler';
  $handler->display->display_options['fields']['tid']['field'] = 'tid';
  /* Field: Rules scheduler: Component name */
  $handler->display->display_options['fields']['config']['id'] = 'config';
  $handler->display->display_options['fields']['config']['table'] = 'rules_scheduler';
  $handler->display->display_options['fields']['config']['field'] = 'config';
  $handler->display->display_options['fields']['config']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['config']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['config']['alter']['path'] = 'admin/config/workflow/rules/config/[config]';
  $handler->display->display_options['fields']['config']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['config']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['config']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['config']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['config']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['config']['alter']['html'] = 0;
  $handler->display->display_options['fields']['config']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['config']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['config']['hide_empty'] = 0;
  $handler->display->display_options['fields']['config']['empty_zero'] = 0;
  /* Field: Rules scheduler: Scheduled date */
  $handler->display->display_options['fields']['date']['id'] = 'date';
  $handler->display->display_options['fields']['date']['table'] = 'rules_scheduler';
  $handler->display->display_options['fields']['date']['field'] = 'date';
  /* Field: Rules scheduler: User provided identifier */
  $handler->display->display_options['fields']['identifier']['id'] = 'identifier';
  $handler->display->display_options['fields']['identifier']['table'] = 'rules_scheduler';
  $handler->display->display_options['fields']['identifier']['field'] = 'identifier';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'admin/config/workflow/rules/schedule/[tid]/delete';
  $handler->display->display_options['fields']['nothing']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['nothing']['alter']['alt'] = 'Delete this scheduled task';
  $handler->display->display_options['fields']['nothing']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['nothing']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['nothing']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['nothing']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['nothing']['alter']['html'] = 0;
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['nothing']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['nothing']['hide_empty'] = 0;
  $handler->display->display_options['fields']['nothing']['empty_zero'] = 0;
  /* Sort criterion: Rules scheduler: Scheduled date */
  $handler->display->display_options['sorts']['date']['id'] = 'date';
  $handler->display->display_options['sorts']['date']['table'] = 'rules_scheduler';
  $handler->display->display_options['sorts']['date']['field'] = 'date';
  /* Argument: Rules scheduler: Component name */
  $handler->display->display_options['arguments']['config']['id'] = 'config';
  $handler->display->display_options['arguments']['config']['table'] = 'rules_scheduler';
  $handler->display->display_options['arguments']['config']['field'] = 'config';
  $handler->display->display_options['arguments']['config']['style_plugin'] = 'default_summary';
  $handler->display->display_options['arguments']['config']['wildcard'] = '0';
  $handler->display->display_options['arguments']['config']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['config']['glossary'] = 0;
  $handler->display->display_options['arguments']['config']['limit'] = '0';
  $handler->display->display_options['arguments']['config']['transform_dash'] = 0;
  /* Filter: Rules scheduler: Component name */
  $handler->display->display_options['filters']['config']['id'] = 'config';
  $handler->display->display_options['filters']['config']['table'] = 'rules_scheduler';
  $handler->display->display_options['filters']['config']['field'] = 'config';
  $handler->display->display_options['filters']['config']['exposed'] = TRUE;
  $handler->display->display_options['filters']['config']['expose']['operator'] = 'config_op';
  $handler->display->display_options['filters']['config']['expose']['label'] = 'Component filter';
  $handler->display->display_options['filters']['config']['expose']['identifier'] = 'config';
  $handler->display->display_options['filters']['config']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['config']['expose']['use_operator'] = 0;
  $handler->display->display_options['filters']['config']['expose']['reduce'] = 0;
  $translatables['rules_scheduler'] = array(
    t('Defaults'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort By'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('No tasks have been scheduled.'),
    t('Tid'),
    t('Component name'),
    t('admin/config/workflow/rules/config/[config]'),
    t('Scheduled date'),
    t('User provided identifier'),
    t('Operations'),
    t('delete'),
    t('admin/config/workflow/rules/schedule/[tid]/delete'),
    t('Delete this scheduled task'),
    t('All'),
    t('Component filter'),
  );

  $views = array();
  $views[$view->name] = $view;
  return $views;
}
