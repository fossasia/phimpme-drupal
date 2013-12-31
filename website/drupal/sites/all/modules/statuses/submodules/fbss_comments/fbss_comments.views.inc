<?php

/**
 * @file
 *   Provide views data and handlers for the Statuses module.
 */

/**
 * Implements hook_views_data().
 */
function fbss_comments_views_data() {
  // Basic table information.
  $data['fbss_comments']['table']['group']  = t('Statuses Comments');
  $data['fbss_comments']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
      'type' => 'INNER',
    ),
    'statuses' => array(
      'left_field' => 'sid',
      'field' => 'sid',
      'type' => 'INNER',
    ),
  );

  // Advertise this table as a possible base table.
  $data['fbss_comments']['table']['base'] = array(
    'field' => 'cid',
    'title' => t('Statuses Comments'),
    'help' => t('Stores status comments.'),
    'weight' => 10,
  );

  // Alias for the FBSS Status ID to extend its use.
  $data['statuses']['comment-box'] = array(
    'title' => t('Status comment box'),
    'help' => t('Shows the list of comments for the relevant status and the form to submit a new status comment.'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'fbss_comments_views_handler_field_comment_box',
      'click sortable' => FALSE,
    ),
  );


  // Declares the Comment ID column.
  $data['fbss_comments']['cid'] = array(
    'title' => t('Status Comment ID'),
    'help' => t('The ID of the status comment.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  $data['fbss_comments']['edit'] = array(
    'title' => t('Edit'),
    'help' => t('Shows a link to edit the status comment to users with permission to see it.'),
    'field' => array(
      'field' => 'cid',
      'handler' => 'fbss_comments_views_handler_field_edit',
      'click sortable' => FALSE,
    ),
  );
  $data['fbss_comments']['delete'] = array(
    'title' => t('Delete'),
    'help' => t('Shows a link to delete the status comment to users with permission to see it.'),
    'field' => array(
      'field' => 'cid',
      'handler' => 'fbss_comments_views_handler_field_delete',
      'click sortable' => FALSE,
    ),
  );

  // Declares the Status ID column.
  $data['fbss_comments']['sid'] = array(
    'title' => t('Status ID'),
    'help' => t('The ID of the status.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // "Comment count."
  $data['statuses']['comment-count'] = array(
    'title' => t('Comment count'),
    'help' => t('The number of comments on the status.'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'fbss_comments_views_handler_field_cc',
      'click sortable' => TRUE,
    ),
  );

  // "Comment count" with plural-sensitive string.
  $data['statuses']['plural-comment-count'] = array(
    'title' => t('Pluralized comment count'),
    'help' => t('The number of comments on the status in words ("1 comment" or "N comments").'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'fbss_comments_views_handler_field_cc2',
      'click sortable' => TRUE,
    ),
  );

  // Declares the commenter's User ID column.
  $data['fbss_comments']['uid'] = array(
    'title' => t('User ID'),
    'help' => t('The User ID of the status comment author.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Declares the status comment timestamp column.
  $data['fbss_comments']['created'] = array(
    'title' => t('Created time'),
    'help' => t('The time the status comment was posted.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Declares the status comment text column.
  $data['fbss_comments']['comment'] = array(
    'title' => t('Comment text'),
    'help' => t('The text of the status comment.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function fbss_comments_views_data_alter(&$data) {
  $data['fbss_comments']['users'] = array(
    'group' => t('Statuses Comments'),
    'relationship' => array(
      'title' => t('Users'),
      'label' => t('Users'),
      'help' => t('Add a relationship to gain access to information related to the users who submitted the relevant status comments.'),
      'relationship table' => 'fbss_comments',
      'relationship field' => 'uid',
      'base' => 'users',
      'field' => 'uid',
      'type' => 'INNER',
      'handler' => 'views_handler_relationship',
    ),
  );
  $data['fbss_comments']['statuses'] = array(
    'group' => t('Statuses Comments'),
    'relationship' => array(
      'title' => t('Statuses'),
      'label' => t('Statuses'),
      'help' => t('Add a relationship to gain access to information related to the statuses themselves.'),
      'relationship table' => 'fbss_comments',
      'relationship field' => 'sid',
      'base' => 'statuses',
      'field' => 'sid',
      'type' => 'INNER',
      'handler' => 'views_handler_relationship',
    ),
  );
}

/**
 * Implements hook_views_handlers().
 */
function fbss_comments_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'fbss_comments'),
    ),
    'handlers' => array(
      'fbss_comments_views_handler_field_comment_box' => array(
        'parent' => 'views_handler_field',
      ),
      'fbss_comments_views_handler_field_cc' => array(
        'parent' => 'views_handler_field',
      ),
      'fbss_comments_views_handler_field_cc2' => array(
        'parent' => 'views_handler_field',
      ),
      'fbss_comments_views_handler_field_delete' => array(
        'parent' => 'views_handler_field',
      ),
      'fbss_comments_views_handler_field_edit' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
