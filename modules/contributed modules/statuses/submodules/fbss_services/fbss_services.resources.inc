<?php

/**
 * @file
 *   Specifies the Services resources for Statuses.
 */

/**
 * Standard CRUD operations per the Services module specification.
 */
function fbss_services_operations_resources() {
  return array(
    'create' => array(
      'help' => 'Creates a status message.',
      'callback' => 'fbss_services_create',
      'access callback' => '_fbss_services_access',
      'access arguments' => array('create'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'recipient',
          'type' => 'int',
          'description' => 'The ID of the recipient of the status message.',
          'optional' => FALSE,
          'source' => 'data',
        ),
        array(
          'name' => 'type',
          'type' => 'string',
          'description' => 'The type of the recipient of the status message.',
          'optional' => FALSE,
          'source' => 'data',
        ),
        array(
          'name' => 'message',
          'type' => 'string',
          'description' => 'The status message.',
          'optional' => FALSE,
          'source' => 'data',
          'default value' => '',
        ),
      ),
    ),
    'retrieve' => array(
      'help' => 'Retrieves a status message.',
      'callback' => 'fbss_services_retrieve',
      'access callback' => '_fbss_services_access',
      'access arguments' => array('retrieve'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'sid',
          'type' => 'int',
          'description' => 'The status ID.',
          'optional' => FALSE,
          'source' => array('path' => 0),
        ),
      ),
    ),
    'update' => array(
      'help' => 'Updates a status message.',
      'callback' => 'fbss_services_update',
      'access callback' => '_fbss_services_access',
      'access arguments' => array('update'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'sid',
          'type' => 'int',
          'description' => 'The status ID.',
          'optional' => FALSE,
          'source' => array('path' => 0),
        ),
        array(
          'name' => 'message',
          'type' => 'string',
          'description' => 'The new status message.',
          'optional' => FALSE,
          'source' => 'data',
          'default value' => '',
        ),
      ),
    ),
    'delete' => array(
      'help' => 'Deletes a status message.',
      'callback' => 'fbss_services_delete',
      'access callback' => '_fbss_services_access',
      'access arguments' => array('delete'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'sid',
          'type' => 'int',
          'description' => 'The status ID.',
          'optional' => FALSE,
          'source' => array('path' => 0),
        ),
      ),
    ),
    'index' => array(
      'help' => 'Lists status messages in pages of 20.',
      'callback' => 'fbss_services_index',
      'access callback' => '_fbss_services_access',
      'access arguments' => array('index'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'page',
          'type' => 'int',
          'description' => 'The zero-based index of the page to get (defaults to 0).',
          'optional' => TRUE,
          'source' => array('param' => 'page'),
          'default value' => 0,
        ),
        array(
          'name' => 'parameters',
          'type' => 'array',
          'description' => 'An array of options by which to filter the results. Valid parameters include recipient, type, and sender.',
          'optional' => TRUE,
          'source' => array('param' => 'parameters'),
          'default value' => array(),
        ),
      ),
    ),
  );
}

/**
 * Provides extended actions.
 */
function fbss_services_actions_resources() {
  return array(
    'all' => array(
      'help' => 'Returns the 20 most recent non-private statuses.',
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'fbss_services.twitter'),
      'method' => 'GET',
      'callback' => 'fbss_services_all',
      'access callback' => 'fbss_services_extended_access',
      'access arguments' => array('fbss_services_all_access'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'trim_user',
          'type' => 'string',
          'description' => "When set to true, t, or 1, each status returned in a timeline will include a user object including only the status author's numerical ID. Omit this parameter to receive the complete user object.",
          'source' => array('param' => 'trim_user'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'include_entities',
          'type' => 'string',
          'description' => 'When set to true, t, or 1, each status object will include an entry called "entities." This entry offers a variety of metadata about the tweet in a discreet structure, including @mentions and #hashtags.',
          'source' => array('param' => 'include_entities'),
          'optional' => TRUE,
          'default value' => 1,
        ),
      ),
    ),
    // TODO
    'home_timeline' => array(
      'help' => 'Returns the most recent non-private statuses relevant to the authenticated user. This is supposed to be the "main" timeline.',
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'fbss_services.twitter'),
      'callback' => 'fbss_services_home_timeline',
      'access callback' => 'fbss_services_twitter_access',
      'access arguments' => array('fbss_services_home_timeline_access'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'since_id',
          'type' => 'int',
          'description' => 'Returns results with an ID greater than (that is, more recent than) the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'max_id',
          'type' => 'int',
          'description' => 'Returns results with an ID less than (that is, older than) or equal to the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'count',
          'type' => 'int',
          'description' => 'Specifies the number of statuses to try to retrieve, up to a maximum of 200. The actual count may be less if there are fewer applicable statuses than this. The default is 20.',
          'source' => array('param' => 'count'),
          'optional' => TRUE,
          'default value' => 20,
        ),
        array(
          'name' => 'page',
          'type' => 'int',
          'description' => 'Specifies the page of results to retrieve. Each page contains statuses between since_id and max_id, but no more than the number of statuses specified in the count parameter. The pager starts at 1, not 0.',
          'source' => array('param' => 'page'),
          'optional' => TRUE,
          'default value' => 1,
        ),
        array(
          'name' => 'include_entities',
          'type' => 'string',
          'description' => 'When set to true, t, or 1, each status object will include an entry called "entities." This entry offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags.',
          'source' => array('param' => 'include_entities'),
          'optional' => TRUE,
          'default value' => 1,
        ),
      ),
    ),
    // http://developer.twitter.com/doc/get/statuses/mentions
    'mentions' => array(
      'help' => 'Returns the 20 most recent mentions (statuses containing @username) for the authenticated user.',
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'fbss_services.twitter'),
      'callback' => 'fbss_services_mentions',
      'access callback' => 'fbss_services_extended_access',
      'access arguments' => array('fbss_services_mentions_access'),
      'access arguments append' => TRUE,
      'args' => array(// No include_rts parameter because FBSS doesn't support that kind of retweeting.
        array(
          'name' => 'since_id',
          'type' => 'int',
          'description' => 'Returns results with an ID greater than (that is, more recent than) the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'max_id',
          'type' => 'int',
          'description' => 'Returns results with an ID less than (that is, older than) or equal to the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'count',
          'type' => 'int',
          'description' => 'Specifies the number of statuses to try to retrieve, up to a maximum of 200. The actual count may be less if there are fewer applicable statuses than this. The default is 20.',
          'source' => array('param' => 'count'),
          'optional' => TRUE,
          'default value' => 20,
        ),
        array(
          'name' => 'page',
          'type' => 'int',
          'description' => 'Specifies the page of results to retrieve. Each page contains statuses between since_id and max_id, but no more than the number of statuses specified in the count parameter. The pager starts at 1, not 0.',
          'source' => array('param' => 'page'),
          'optional' => TRUE,
          'default value' => 1,
        ),
        array(
          'name' => 'trim_user',
          'type' => 'string',
          'description' => "When set to true, t, or 1, each status returned in a timeline will include a user object including only the status author's numerical ID. Omit this parameter to receive the complete user object.",
          'source' => array('param' => 'trim_user'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'include_entities',
          'type' => 'string',
          'description' => 'When set to true, t, or 1, each status object will include an entry called "entities." This entry offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags.',
          'source' => array('param' => 'include_entities'),
          'optional' => TRUE,
          'default value' => 1,
        ),
      ),
    ),
    // http://developer.twitter.com/doc/get/statuses/user_timeline
    'user_timeline' => array(
      'help' => 'Returns the 20 most recent personal statuses posted by the specified user (or the authenticated user if unspecified) to their own stream.',
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'fbss_services.twitter'),
      'callback' => 'fbss_services_user_timeline',
      'access callback' => 'fbss_services_extended_access',
      'access arguments' => array('fbss_services_user_timeline_access'),
      'access arguments append' => TRUE,
      'args' => array(// No include_rts parameter because FBSS doesn't support that kind of retweeting.
        array(
          'name' => 'user_id',
          'type' => 'int',
          'description' => 'The ID of the user whose results should be returned.',
          'source' => array('param' => 'user_id'),
          'optional' => TRUE,
          'default value' => $GLOBALS['user']->uid,
        ),
        array(
          'name' => 'screen_name',
          'type' => 'string',
          'description' => 'The screen name of the user whose results should be returned.',
          'source' => array('param' => 'screen_name'),
          'optional' => TRUE,
          'default value' => $GLOBALS['user']->name,
        ),
        array(
          'name' => 'since_id',
          'type' => 'int',
          'description' => 'Returns results with an ID greater than (that is, more recent than) the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'max_id',
          'type' => 'int',
          'description' => 'Returns results with an ID less than (that is, older than) or equal to the sepcified ID.',
          'source' => array('param' => 'since_id'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'count',
          'type' => 'int',
          'description' => 'Specifies the number of statuses to try to retrieve, up to a maximum of 200. The actual count may be less if there are fewer applicable statuses than this. The default is 20.',
          'source' => array('param' => 'count'),
          'optional' => TRUE,
          'default value' => 20,
        ),
        array(
          'name' => 'page',
          'type' => 'int',
          'description' => 'Specifies the page of results to retrieve. Each page contains statuses between since_id and max_id, but no more than the number of statuses specified in the count parameter. The pager starts at 1, not 0.',
          'source' => array('param' => 'page'),
          'optional' => TRUE,
          'default value' => 1,
        ),
        array(
          'name' => 'trim_user',
          'type' => 'string',
          'description' => "When set to true, t, or 1, each status returned in a timeline will include a user object including only the status author's numerical ID. Omit this parameter to receive the complete user object.",
          'source' => array('param' => 'trim_user'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'include_entities',
          'type' => 'string',
          'description' => 'When set to true, t, or 1, each status object will include an entry called "entities." This entry offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags.',
          'source' => array('param' => 'include_entities'),
          'optional' => TRUE,
          'default value' => 1,
        ),
      ),
    ),
    // http://developer.twitter.com/doc/get/statuses/show/:id
    'show' => array(
      'help' => 'Returns the 20 most recent non-private statuses.',
      'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'fbss_services.twitter'),
      'callback' => 'fbss_services_public_timeline',
      'access callback' => 'fbss_services_extended_access',
      'access arguments' => array('fbss_services_public_timeline_access'),
      'access arguments append' => TRUE,
      'args' => array(
        array(
          'name' => 'trim_user',
          'type' => 'string',
          'description' => "When set to true, t, or 1, each status returned in a timeline will include a user object including only the status author's numerical ID. Omit this parameter to receive the complete user object.",
          'source' => array('param' => 'trim_user'),
          'optional' => TRUE,
          'default value' => 0,
        ),
        array(
          'name' => 'include_entities',
          'type' => 'string',
          'description' => 'When set to true, t, or 1, each status object will include an entry called "entities." This entry offers a variety of metadata about the tweet in a discreet structure, including: user_mentions, urls, and hashtags.',
          'source' => array('param' => 'include_entities'),
          'optional' => TRUE,
          'default value' => 1,
        ),
      ),
    ),
  );
}
