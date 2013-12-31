<?php

/**
 * @file
 *   Rules integration default configuration.
 */

/**
 * Implementation of hook_rules_defaults().
 */
function statuses_rules_defaults() {
  $config = array(
    'rules' => array(
      'statuses_rules_update' => array(
        '#type' => 'rule',
        '#set' => 'event_statuses_save',
        '#label' => 'Submit Facebook-style Status',
        '#active' => 0,
        '#weight' => '0',
        '#categories' => array(
          0 => 'statuses',
        ),
        '#status' => 'default',
        '#conditions' => array(
        ),
        '#actions' => array(
          0 => array(
            '#type' => 'action',
            '#settings' => array(
              'severity' => '6',
              'type' => 'rules',
              'message' => '<?php echo check_plain($account->name); ?> submitted the status <?php echo $new_status; ?>.',
              'link' => '/user/<?php echo $account->uid; ?>',
              '#eval input' => array(
                'rules_input_evaluator_php' => array(
                  'message' => array(
                    0 => 'account',
                    1 => 'new_status',
                  ),
                  'link' => array(
                    0 => 'account',
                  ),
                ),
              ),
            ),
            '#name' => 'rules_action_watchdog',
            '#info' => array(
              'label' => 'Log to watchdog',
              'module' => 'System',
              'eval input' => array(
                0 => 'type',
                1 => 'message',
                2 => 'link',
              ),
            ),
            '#weight' => 0,
          ),
        ),
      ),
      'statuses_rules_delete' => array(
        '#type' => 'rule',
        '#set' => 'event_statuses_delete',
        '#label' => 'Delete Facebook-style Status',
        '#active' => 0,
        '#weight' => '0',
        '#categories' => array(
          0 => 'statuses',
        ),
        '#status' => 'default',
        '#conditions' => array(
        ),
        '#actions' => array(
          0 => array(
            '#type' => 'action',
            '#settings' => array(
              'message' => 'Status deleted.',
              'error' => 0,
            ),
            '#name' => 'rules_action_drupal_message',
            '#info' => array(
              'label' => 'Show a configurable message on the site',
              'module' => 'System',
              'eval input' => array(
                0 => 'message',
              ),
            ),
            '#weight' => 0,
          ),
        ),
      ),
    ),
  );
  return $config;
}
