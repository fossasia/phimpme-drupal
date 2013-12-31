<?php

/**
 * @file flag.rules.inc
 * Rules integration for the Flag module.
 */

/**
 * Implements hook_rules_data_info().
 * @ingroup rules
 */
function flag_rules_data_info() {
  return array(
    'flag' => array(
      'label' => t('flag'),
      'ui class' => 'FlagRulesUIClass',
      'wrapper class' => 'FlagRulesDataWrapper',
      'wrap' => TRUE,
    ),
  );
}

/**
 * A custom wrapper class for flags to be used with Rules.
 * @ingroup rules
 */
class FlagRulesDataWrapper extends RulesIdentifiableDataWrapper implements RulesDataWrapperSavableInterface {

  protected function extractIdentifier($flag) {
    return $flag->name;
  }

  protected function load($name) {
    return flag_get_flag($name);
  }

  public function save() {
    $flag = $this->value();
    $flag->save();
  }

  public function validate($value) {
    if (isset($value) && is_string($value)) {
      return TRUE;
    }
    elseif (isset($value) && is_object($value) && $value instanceof flag_flag) {
      return TRUE;
    }
    return parent::validate($value);
  }
}

/**
 * UI for inputing flags.
 * @ingroup rules
 */
class FlagRulesUIClass extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $options = _flag_rules_flags_options(isset($info['flag_type']) ? $info['flag_type'] : NULL);
    $header  = array(
      'title' => t('Flag:'),
      'type' => t('The flag type'),
      'global' => t('Is the flag global?'),
    );
    $settings += array($name => isset($info['default value']) ? $info['default value'] : '');

    $form[$name] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#required' => empty($info['optional']),
      '#multiple' => FALSE,
      '#default_value' => $settings[$name],
      '#emtpy' => t('There is no suiting flag available.')
    );
    return $form;
  }

  public static function render($value) {
    $flag = flag_get_flag($value);
    return array(
      'content' => array('#markup' => check_plain($flag->get_title())),
      '#attributes' => array('class' => array('rules-parameter-flag')),
    );
  }
}

function _flag_rules_flags_options($flag_type = NULL) {
  $flags = flag_get_flags();
  $options = array();
  foreach ($flags as $flag) {
    if (!isset($flag_type) || $flag->content_type == $flag_type) {
      $options[$flag->name] = array(
        'title' => $flag->get_title(),
        'type' => $flag->content_type,
        'global' => $flag->global ? t('Yes') : t('No'),
      );
    }
  }
  return $options;
}

/**
 * Implements hook_rules_event_info().
 */
function flag_rules_event_info() {
  $items = array();

  $flags = flag_get_flags();
  foreach ($flags as $flag) {
    // We only support flags on entities.
    if ($info = entity_get_info($flag->content_type)) {
      $variables = array(
        'flag' => array(
          'type' => 'flag',
          'label' => t('flag'),
          'flag_type' => $flag->content_type,
        ),
        'flagged_' . $flag->content_type => array(
          'type' => $flag->content_type,
          'label' => $info['label'],
        ),
        'flagging_user' => array(
          'type' => 'user',
          'label' => t('flagging user'),
        ),
      );

      // For each flag we define two events.
      $items['flag_flagged_' . $flag->name] = array(
        'group' => t('Flag'),
        'label' => t('A @flag-type has been flagged, under "@flag-title"', array('@flag-title' => $flag->get_title(), '@flag-type' => t($flag->content_type))),
        'variables' => $variables,
        'access callback' => 'flag_rules_integration_access',
      );
      $items['flag_unflagged_' . $flag->name] = array(
        'group' => t('Flag'),
        'label' => t('A @flag-type has been unflagged, under "@flag-title"', array('@flag-title' => $flag->get_title(), '@flag-type' => t($flag->content_type))),
        'variables' => $variables,
        'access callback' => 'flag_rules_integration_access',
      );
    }
  }
  return $items;
}

/**
 * Implements hook_rules_action_info().
 */
function flag_rules_action_info() {
  $items = array(
    'flag_trim' => array(
      'label' => t('Trim a flag'),
      'base' => 'flag_rules_action_trim',
      'parameter' => array(
        'flag' => array(
          'type' => 'flag',
          'label' => t('Flag'),
        ),
        'flagging_user' => array(
          'type' => 'user',
          'label' => t('User whose flag to trim'),
          'description' => t('For non-global flags, this is the user whose flag to trim. (For global flags, this argument is ignored.)'),
        ),
        'cutoff_size' => array(
          'type' => 'integer',
          'label' => t('Flag queue size'),
          'description' => t('The maximum number of objects to keep in the queue. Newly flagged objects will be kept; older ones will be removed. Tip: by typing "1" here you implement a singleton.'),
        ),
      ),
      'group' => t('Flag'),
      'access callback' => 'flag_rules_integration_access',
    ),
  );
  $param_defaults = array(
    'flagging_user' => array(
      'type' => 'user',
      'label' => t('User on whose behalf to flag'),
      'description' => t('For non-global flags, this is the user on whose behalf to flag the object. In addition, if checked below, the access permissions to the flag are checked against this user.'),
    ),
    'permission_check' => array(
      'type' => 'boolean',
      'label' => t('Permission check'),
      'description' => t('Whether to check access permissions against the user on whose behalf to flag.'),
      'restriction' => 'input',
    ),
  );
  foreach (flag_get_types() as $type) {
    $flag = flag_create_handler($type);
    $entity_info = entity_get_info($type);
    $items += array(
      'flag_flag'. $type => array(
        'label' => t('Flag a @type', array('@type' => $type)),
        'base' => 'flag_rules_action_flag',
        'parameter' => array(
          'flag' => array(
            'type' => 'flag',
            'label' => t('Flag'),
            'flag_type' => $type,
            'description' => t('The flag to check for.')
          ),
          $type => array(
            'type' => $type,
            'label' => isset($entity_info[$type]['label']) ? $entity_info[$type]['label'] : $type,
          ),
        ) + $param_defaults,
        'group' => t('Flag'),
        'access callback' => 'flag_rules_integration_access',
      ),
      'flag_unflag'. $type => array(
        'label' => t('Unflag a @type', array('@type' => $type)),
        'base' => 'flag_rules_action_unflag',
        'parameter' => array(
          'flag' => array(
            'type' => 'flag',
            'label' => t('Flag'),
            'flag_type' => $type,
            'description' => t('The flag to check for.')
          ),
          $type => array(
            'type' => $type,
            'label' => isset($entity_info[$type]['label']) ? $entity_info[$type]['label'] : $type,
          ),
        ) + $param_defaults,
        'group' => t('Flag'),
        'access callback' => 'flag_rules_integration_access',
      ),
    );
    $items['flag_fetch_users_' . $type] = array(
      'label' => t('Fetch users who have flagged a @type', array('@type' => $type)),
      'base' => 'flag_rules_action_fetch_users',
      'parameter' => array(
        'flag' => array(
          'type' => 'flag',
          'label' => t('Flag'),
          'flag_type' => $type,
          'description' => t('Choose the flag for which to fetch the users.'),
        ),
        $type => array(
          'type' => $type,
          'label' => isset($entity_info[$type]['label']) ? $entity_info[$type]['label'] : $type,
        ),
      ),
      'provides' => array(
        'users' => array(
          'label' => t('Users who flagged'),
          'type' => 'list<user>',
        ),
      ),
      'group' => t('Flag'),
      'access callback' => 'flag_rules_integration_access',
    );
  }
  return $items;
}

/**
 * Base action implementation: Flag.
 */
function flag_rules_action_flag($flag, $entity, $flagging_user, $permissions_check) {
  $flag->flag('flag', $flag->get_content_id($entity), $flagging_user, $permissions_check);
}

/**
 * Base action implementation: Unflag.
 */
function flag_rules_action_unflag($flag, $entity, $flagging_user, $permissions_check) {
  $flag->flag('unflag', $flag->get_content_id($entity), $flagging_user, $permissions_check);
}

/**
 * Base action implementation: Trim flag.
 */
function flag_rules_action_trim($flag, $flagging_user, $cutoff_size) {
  flag_trim_flag($flag, $flagging_user, $cutoff_size);
}

/**
 * Base action implementation: Fetch users who flagged an entity.
 */
function flag_rules_action_fetch_users($flag, $entity) {
  $result = db_select('flag_content', 'fc')
    ->fields('fc', array('uid'))
    ->condition('content_type', $flag->content_type)
    ->condition('content_id', $flag->get_content_id($entity))
    ->condition('fid', $flag->fid)
    ->execute();
  $uids = $result->fetchCol();
  // Filter out anonymous users.
  return array('users' => array_filter($uids));
}

/**
 * Implements hook_rules_condition_info().
 */
function flag_rules_condition_info() {
  $items = array();
  foreach (flag_get_types() as $type) {
    $flag = flag_create_handler($type);
    $entity_info = entity_get_info($type);
    $label = isset($entity_info[$type]['label']) ? $entity_info[$type]['label'] : $type;
    $items += array(
      'flag_threshold_'. $type => array(
        'label' => drupal_ucfirst(t('@type has flagging count', array('@type' => $label))),
        'base' => 'flag_rules_condition_threshold',
        'parameter' => array(
          'flag' => array(
            'type' => 'flag',
            'label' => t('Flag'),
            'flag_type' => $type,
            'description' => t('The flag to check for.')
          ),
           $type => array(
             'type' => $type,
             'label' => $label,
           ),
           'number' => array(
             'type' => 'integer',
             'label' => t('Number'),
             'description' => t('The number against which to test the number of times the object is flagged. For example, if you type "3" here, and choose "Greater than" for the operator, then this condition will return TRUE if the object is flagged more than three times.'),
           ),
           'operator' => array(
             'type' => 'text',
             'label' => t('Comparison operator'),
             'options list' => 'flag_rules_condition_threshold_operator_options',
             'restriction' => 'input',
             'default value' => '=',
             'optional' => TRUE,
           ),
        ),
        'group' => t('Flag'),
        'access callback' => 'flag_rules_integration_access',
      ),
      'flag_flagged_'. $type => array(
        'label' => drupal_ucfirst(t('@type is flagged', array('@type' => $label))),
        'base' => 'flag_rules_condition_flagged',
        'parameter' => array(
          'flag' => array(
            'type' => 'flag',
            'label' => t('Flag'),
            'flag_type' => $type,
            'description' => t('The flag to check for.')
          ),
           $type => array(
             'type' => $type,
             'label' => $label,
           ),
          'user' => array(
            'type' => 'user',
            'label' => t('User on whose behalf to check'),
            'description' => t('For non-global flags, this is the user on whose behalf the flag is checked.'),
          ),
        ),
        'group' => t('Flag'),
        'access callback' => 'flag_rules_integration_access',
      ),
    );
  }
  return $items;
}

/**
 * Options list callback for the operator parameter of the flagging threshold condition.
 */
function flag_rules_condition_threshold_operator_options() {
  return array(
    '>'  => t('Greater than'),
    '>=' => t('Greater than or equal'),
    '='  => t('Equal to'),
    '<=' => t('Less than or equal'),
    '<'  => t('Less than'),
  );
}

/**
 * Condition: Check flagging count.
 */
function flag_rules_condition_threshold($flag, $entity, $number, $operator = '=') {
  $count = $flag->get_count($flag->get_content_id($entity));

  switch ($operator) {
    case '>' : return $count >  $number;
    case '>=': return $count >= $number;
    case '=' : return $count == $number;
    case '<' : return $count <  $number;
    case '<=': return $count <= $number;
  }
}

/**
 * Condition: Flag is flagged.
 */
function flag_rules_condition_flagged($flag, $entity, $account) {
  return $flag->is_flagged($flag->get_content_id($entity), $account->uid);
}

/**
 * Rules integration access callback.
 */
function flag_rules_integration_access($type, $name) {
  return user_access('administer flags');
}
