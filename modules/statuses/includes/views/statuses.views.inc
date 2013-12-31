<?php

/**
 * @file
 * Provide views data and handlers for the Statuses module.
 */

/**
 * Implementation of hook_views_data().
 */
function statuses_views_data() {
  // Basic table information.
  $data['statuses']['table']['group']  = t('Statuses');
  $data['statuses']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'sender',
      'type' => 'INNER',
    ),
  );
  $data['users']['table']['join'] = array(
    'statuses' => array(
      'left_field' => 'sender',
      'field' => 'uid',
      'type' => 'INNER',
    ),
  );

  // Advertise this table as a possible base table.
  $data['statuses']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('Statuses Updates'),
    'help' => t('Stores status updates.'),
    'weight' => 10,
  );

  // Make it possible to add a relationship to the Users table.
  // Commented out because Views automatically JOINs on the {users} table when
  // using User data without a Relationship, so adding the Relationship will
  // cause a duplicate JOIN.
  /*
  $data['statuses']['users'] = array(
    'group' => t('Statuses'),
    'relationship' => array(
      'title' => t('Users'),
      'label' => t('Users'),
      'help' => t('Add a relationship to gain access to information related to the users who submitted the relevant statuses.'),
      'relationship table' => 'statuses',
      'relationship field' => 'sender',
      'base' => 'users',
      'field' => 'uid',
      'type' => 'INNER',
      'handler' => 'views_handler_relationship',
    ),
  );
   */

  // Declares the Status ID column.
  $data['statuses']['sid'] = array(
    'title' => t('Status ID'),
    'help' => t('The ID of the status update.'),
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

  // Alias for Status ID to extend its use.
  $data['statuses']['last_per_user'] = array(
    'title' => t('Last Status per User'),
    'help' => t('Show only the last status update for each user.'),
    'filter' => array(
      'field' => 'sid',
      'handler' => 'statuses_views_handler_filter_latest_only',
      'label' => t('Last Status per User'),
    ),
  );
  $data['statuses']['edit'] = array(
    'title' => t('Edit'),
    'help' => t('Shows a link to edit the status to users with permission to see it.'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'statuses_views_handler_field_edit',
      'click sortable' => FALSE,
    ),
  );
  $data['statuses']['delete'] = array(
    'title' => t('Delete'),
    'help' => t('Shows a link to delete the status to users with permission to see it.'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'statuses_views_handler_field_delete',
      'click sortable' => FALSE,
    ),
  );
  $data['statuses']['repost'] = array(
    'title' => t('Re-post'),
    'help' => t('Shows a link to re-post a status.'),
    'field' => array(
      'field' => 'sid',
      'handler' => 'statuses_views_handler_field_repost',
      'click sortable' => FALSE,
    ),
  );

  // Declares the Type column.
  $data['statuses']['type'] = array(
    'title' => t('Recipient type'),
    'help' => t('The type of the recipient of the status message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'statuses_views_handler_filter_type',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['statuses']['auto-type'] = array(
    'title' => t('Use current context'),
    'help' => t('Only show status updates that match the current context (i.e. that appear in the stream that belongs to the entity the user is viewing).'),
    'filter' => array(
      'field' => 'type',
      'handler' => 'statuses_views_handler_filter_autotype',
    ),
  );

  // Declares the Recipient ID column.
  $data['statuses']['recipient'] = array(
    'title' => t('Recipient ID'),
    'help' => t('The ID of the recipient of the status.'),
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

  // Alias for Recipient ID to use the name.
  $data['statuses']['recipient_name'] = array(
    'title' => t('Recipient Name'),
    'help' => t('The name of the entity on whose stream the status message was posted.'),
    'field' => array(
      'field' => 'recipient',
      'handler' => 'statuses_views_handler_field_recipient',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'field' => 'recipient',
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'field' => 'recipient',
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'field' => 'recipient',
      'handler' => 'views_handler_argument_user_uid',
    ),
  );
  // Alias for poster picture.
  $data['statuses']['recipient_pic'] = array(
    'title' => t('Recipient picture'),
    'help' => t('The picture of the user who received the status message, if applicable.'),
    'field' => array(
      'field' => 'recipient',
      'handler' => 'statuses_views_handler_field_recipient_pic',
      'click sortable' => FALSE,
    ),
  );

  // Declares the Sender ID column.
  $data['statuses']['sender'] = array(
    'title' => t('Sender ID'),
    'help' => t('The User ID of the creator of the status.'),
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

  // Alias for extras for the sender.
  $data['statuses']['user_contextual'] = array(
    'title' => t('Usernames (context-aware)'),
    'help' => t('The themed username(s) of either the owner or the sender and recipient of the status, depending on the context.'),
    'field' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_field_cross',
      'click sortable' => FALSE,
      'label' => t('Users'),
    ),
  );
  $data['statuses']['not_own'] = array(
    'title' => t('Not own statuses'),
    'help' => t("Do not show statuses posted to one's own profile."),
    'filter' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_filter_not_own',
      'label' => t("Exclude own status updates"),
    ),
  );
  // This is slightly different than, but could probably be combined with, not_own.
  $data['statuses']['only_own'] = array(
    'title' => t('Only own statuses'),
    'help' => t("Only show statuses posted to one's own profile."),
    'filter' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_filter_own',
      'label' => t('Restrict to own status updates'),
    ),
  );
  $data['statuses']['sender_pic'] = array(
    'title' => t('Sender picture'),
    'help' => t('The picture of the user who sent the status message.'),
    'field' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_field_sender_pic',
      'click sortable' => FALSE,
    ),
  );
  $data['statuses']['user_contextual_pic'] = array(
    'title' => t('Users with Pictures'),
    'help' => t('The themed username(s) and pictures of either the owner or the sender and recipient of the status, depending on the context.'),
    'field' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_field_cross_pic',
      'click sortable' => FALSE,
      'label' => t('Users with Pictures'),
    ),
  );
  $data['statuses']['respond'] = array(
    'title' => t('Respond link'),
    'help' => t('Links to view the conversation or respond to it when appropriate.'),
    'field' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_field_respond',
      'click sortable' => FALSE,
      'label' => t('Respond to conversation'),
    ),
  );

  // Adds an argument and filter for "is participant."
  $data['statuses']['participant'] = array(
    'title' => t('User is sender or recipient'),
    'help' => t('Limits status messages to those where the specified user ID belongs to either the sender or recipient.'),
    'filter' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_filter_participant',
      'label' => t('User is participant'),
    ),
    'argument' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_argument_participant',
      'label' => t('User is participant'),
    ),
  );

  // Declares the status message timestamp column.
  $data['statuses']['created'] = array(
    'title' => t('Created time'),
    'help' => t('The time the status message was posted.'),
    'field' => array(
      'handler' => 'statuses_views_handler_field_created',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Declares the status text column.
  $data['statuses']['message'] = array(
    'title' => t('Status message'),
    'help' => t('The text of the status update.'),
    'field' => array(
      'handler' => 'statuses_views_handler_field_message',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  $data['statuses']['classes'] = array(
    'title' => t('HTML classes'),
    'help' => t('The HTML classes that should be associated with a status.'),
    'field' => array(
      'field' => 'sender',
      'handler' => 'statuses_views_handler_field_classes',
      'click sortable' => FALSE,
    ),
  );

  // Limits users by who they have communicated with.
  $data['users']['communicated'] = array(
    'title' => t('Users have communicated with current user'),
    'help' => t('Shows only users who have sent status messages to or received status messages from the current user.'),
    'filter' => array(
      'field' => 'uid',
      'handler' => 'statuses_views_handler_filter_communicated',
      'label' => t('Users communicated'),
    ),
  );
  $data['users']['communicate'] = array(
    'title' => t('Users have communicated with argument user'),
    'help' => t('Shows only users who have sent status messages to or received status messages from the argument user.'),
    'argument' => array(
      'field' => 'uid',
      'handler' => 'statuses_views_handler_argument_communicated',
      'label' => t('Users communciated'),
    ),
  );

  // Last status to current user from user.
  $data['users']['last_status'] = array(
    'title' => t('Last status to current user'),
    'help' => t('Shows the last status each user sent to the current user, if any.'),
    'field' => array(
      'field' => 'uid',
      'handler' => 'statuses_views_handler_field_last',
    ),
  );

  // Current user's UID.
  $data['users']['current_uid'] = array(
    'title' => t("Current user's User ID"),
    'help' => t('Shows the User ID of the current user.'),
    'field' => array(
      'field' => 'uid',
      'handler' => 'statuses_views_handler_field_current_uid',
    ),
  );

  if (module_exists('user_relationships_api')) {
    // Expose data from a user's friends AND that user.
    $data['user_relationships']['rels_and_me'] = array(
      'title' => t('Requestee or Requester or Argument user'),
      'help' => t('Filters results to show content for the requestee, requester, or argument user.'),
      'argument' => array(
        'field' => 'requestee_id',
        'handler' => 'statuses_views_handler_argument_rels_and_me',
      ),
    );
  }

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function statuses_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'statuses') . '/includes/views/handlers',
    ),
    'handlers' => array(
      'statuses_views_handler_argument_communicated' => array(
        'parent' => 'views_handler_argument',
      ),
      'statuses_views_handler_argument_participant' => array(
        'parent' => 'views_handler_argument',
      ),
      'statuses_views_handler_argument_rels_and_me' => array(
        'parent' => 'views_handler_argument',
      ),
      'statuses_views_handler_field_classes' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_created' => array(
        'parent' => 'views_handler_field_date',
      ),
      'statuses_views_handler_field_cross' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_cross_pic' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_current_uid' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_delete' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_edit' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_last' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_message' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_recipient' => array(
        'parent' => 'views_handler_field_user',
      ),
      'statuses_views_handler_field_recipient_pic' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_repost' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_respond' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_field_sender_pic' => array(
        'parent' => 'views_handler_field',
      ),
      'statuses_views_handler_filter_autotype' => array(
        'parent' => 'views_handler_filter',
      ),
      'statuses_views_handler_filter_communicated' => array(
        'parent' => 'views_handler_filter',
      ),
      'statuses_views_handler_filter_latest_only' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'statuses_views_handler_filter_participant' => array(
        'parent' => 'views_handler_filter',
      ),
      'statuses_views_handler_filter_not_own' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'statuses_views_handler_filter_own' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'statuses_views_handler_filter_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function statuses_views_plugins() {
  return array(
    'argument validator' => array(
      'status' => array(
        'title' => t('Status'),
        'handler' => 'statuses_plugin_argument_validate',
      ),
    ),
  );
}
