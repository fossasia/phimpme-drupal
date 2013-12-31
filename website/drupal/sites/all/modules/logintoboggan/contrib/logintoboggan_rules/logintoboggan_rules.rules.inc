<?php

/**
 * Implement hook_rules_event_info().
 * @ingroup rules
 */

function logintoboggan_rules_rules_event_info() {
  return array(
    'logintoboggan_validated' => array(
      'label' => t("When the user account is validated."),
      'group' => 'LoginToboggan',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t("The validated user's account"),
        ),
      ),
      'group' => t('User'),
    ),
  );
}
