<?php

/**
 * @file
 *   Callbacks for Services integration with Statuses.
 */

/**
 * Determines permissions to CRUD resources.
 *
 * @param $action
 *   One of "create," "retrieve," "update," "delete," "index."
 *   Depending on what $action is, other parameters will follow:
 *   - create: $recipient_id, $type, $message
 *   - retrieve, delete: $sid
 *   - update: $sid, $message
 *   - index: $page, $parameters
 * @return
 *   TRUE if access to the given action is permitted; FALSE otherwise.
 */
function fbss_services_node_access($action) {
  $args = func_get_args();
  $args = $args[1];
  if ($action == 'create') {
    $context = statuses_determine_context($args[1]);
    if (empty($context)) {
      return services_error('Invalid context stream type', 406);
    }
    $recipient = $context['handler']->load_recipient($args[0]);
    if (empty($recipient)) {
      return services_error('Recipient not found', 404);
    }
    return statuses_user_access('add', $recipient, $args[1]);
  }
  elseif ($action == 'index') {
    if (!empty($args[1]['recipient']) && !empty($args[1]['type'])) {
      $context = statuses_determine_context($args[1]['type']);
      if (empty($context)) {
        return services_error('Invalid context stream type', 406);
      }
      $recipient = $context['handler']->load_recipient($args[1]['recipient']);
      if (empty($recipient)) {
        return services_error('Recipient not found', 404);
      }
      return statuses_user_access('view_stream', $recipient, $args[1]['type']);
    }
    return user_access('view all statuses');
  }
  else {
    if ($action == 'retrieve') {
      $action = 'view';
    }
    elseif ($action == 'update') {
      $action = 'edit';
    }
    $status = statuses_load($args[0]);
    if (empty($status)) {
      return services_error('Status sid ' . $sid . ' not found', 404);
    }
    return statuses_user_access($action, $status);
  }
}

/**
 * Creates a new status message based on submitted values.
 *
 * @param $recipient_id
 *   The ID of the recipient of the status message.
 * @param $type
 *   The type of the recipient of the status message.
 * @param $message
 *   The status message.
 * @return
 *   The newly saved status object. If applicable, has an additional "uri"
 *   parameter containing the fully qualified URI to this resource.
 */
function fbss_services_create($recipient_id, $type, $message) {
  $maxlen = variable_get('statuses_length', 140);
  if (drupal_strlen($message) > $maxlen && $maxlen != 0) {
    return services_error('The status must be no longer than ' . $maxlen . ' characters', 406);
  }
  $context = statuses_determine_context($type);
  if (empty($context)) {
    return services_error('Invalid context stream type', 406);
  }
  $recipient = $context['handler']->load_recipient($recipient_id);
  if (empty($recipient)) {
    return services_error('Recipient not found', 404);
  }
  $status = statuses_save_status($recipient, $type, $message);
  if ($uri = services_resource_uri(array('status', $status->sid))) {
    $status->uri = $uri;
  }
  return $status;
}

/**
 * Loads and retrieves a status object.
 *
 * @param $sid
 *   The ID of the status to return.
 * @return
 *   The status object.
 */
function fbss_services_retrieve($sid) {
  $status = statuses_load($sid);
  if (!empty($status)) {
    $status->uri = services_resource_uri(array('status', $status->sid));
    return $status;
  }
  return services_error('Status sid ' . $sid . ' not found', 404);
}

/**
 * Updates a status message based on submitted values.
 *
 * @param $sid
 *   The ID of the status to edit.
 * @param $message
 *   The new message text.
 * @return
 *   The modified status object. If applicable, has an additional "uri"
 *   parameter containing the fully qualified URI to this resource.
 */
function fbss_services_update($sid, $message) {
  $maxlen = variable_get('statuses_length', 140);
  if (drupal_strlen($message) > $maxlen && $maxlen != 0) {
    return services_error('The status must be no longer than ' . $maxlen . ' characters', 406);
  }
  $status = statuses_load($sid);
  if (empty($status)) {
    return services_error('Status sid ' . $sid . ' not found', 404);
  }
  $status = statuses_edit_status($status, $message);
  if ($uri = services_resource_uri(array('status', $sid))) {
    $status->uri = $uri;
  }
  return $status;
}

/**
 * Delete a status given its SID.
 *
 * @param $sid
 *   The ID of the status to delete.
 */
function fbss_services_delete($sid) {
  statuses_delete_status($sid);
  return TRUE;
}

/**
 * Return a paged set of statuses based on a set of parameters.
 *
 * An example request might look like this:
 *
 * ...endpoint/status?page=0&parameters['recipient']=7&parameters['type']=user
 *
 * This would return an array of the last 20 status messages sent to the user
 * with UID 7.
 *
 * @param $page
 *   Page number of results to return (in pages of 20). Optional.
 * @param $parameters
 *   An optional array of parameters by which to filter. Valid parameters
 *   include:
 *   - recipient: The ID of the recipient of the status messages.
 *   - type: The type of the recipient of the status messages. Required if
 *     recipient is specified.
 *   - sender: The user ID of the sender of the status messages.
 */
function fbss_services_index($page, $parameters) {
  if ($page < 0 || !is_numeric($page)) {
    return services_error('Invalid page', 404);
  }
  $statuses = array();
  $args = array();
  $query = "SELECT * FROM {statuses} WHERE created <> 0";
  if (!empty($parameters['type'])) {
    $query .= " AND type = '%s'";
    $args[] = $parameters['type'];
  }
  if (!empty($parameters['recipient'])) {
    if (empty($parameters['type'])) {
      return services_error('Invalid context stream type', 406);
    }
    $query .= " AND recipient = %d";
    $args[] = $parameters['recipient'];
  }
  if (!empty($parameters['sender'])) {
    $query .= " AND sender = %d";
    $args[] = $parameters['sender'];
  }
  $query .= " ORDER BY created DESC, sid DESC";
  $result = db_query_range($query, $args);
  while ($status = db_fetch_object($result)) {
    if ($uri = services_resource_uri(array('status', $status->sid))) {
      $status->uri = $uri;
    }
    $statuses[] = $status;
  }
  return $statuses;
}
