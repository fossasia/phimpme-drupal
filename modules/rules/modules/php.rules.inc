<?php

/**
 * @file rules integration for the php module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the php module.
 */
function rules_php_file_info() {
  return array('modules/php.eval');
}

/**
 * Implements hook_rules_evaluator_info() on behalf of the php module.
 */
function rules_php_evaluator_info() {
  return array(
    'php' => array(
      'class' => 'RulesPHPEvaluator',
      'type' => array('text', 'uri'),
      'weight' => -10,
      'module' => 'php',
    ),
  );
}

/**
 * Implements hook_rules_data_processor_info() on behalf of the php module.
 */
function rules_php_data_processor_info() {
  return array(
    'php' => array(
      'class' => 'RulesPHPDataProcessor',
      'type' => array('text', 'token',  'decimal', 'integer', 'date', 'duration', 'boolean', 'uri'),
      'weight' => 10,
      'module' => 'php',
    ),
  );
}

/**
 * Implements hook_rules_action_info() on behalf of the php module.
 */
function rules_php_action_info() {
  return array(
    'php_eval' => array(
      'label' => t('Execute custom PHP code'),
      'group' => t('PHP'),
      'parameter' => array(
        'code' => array(
          'restriction' => 'input',
          'type' => 'text',
          'label' => t('PHP code'),
          'description' => t('Enter PHP code without &lt;?php ?&gt; delimiters.'),
        ),
      ),
      'base' => 'rules_execute_php_eval',
      'access callback' => 'rules_php_integration_access',
    ),
  );
}

/**
 * Alter the form for improved UX.
 */
function rules_execute_php_eval_form_alter(&$form, &$form_state) {
  // Remove the PHP evaluation help to avoid confusion whether <?php tags should
  // be used. But keep the help about available variables.
  $form['parameter']['code']['settings']['help']['php']['#type'] = 'container';
  $form['parameter']['code']['settings']['help']['php']['top']['#markup'] = t('The following variables are available and may be used by your PHP code:');
}

/**
 * Process the settings to prepare code execution.
 */
function rules_execute_php_eval_process(RulesAbstractPlugin $element) {
  $element->settings['used_vars'] = RulesPHPEvaluator::getUsedVars('<?' . $element->settings['code'], $element->availableVariables());
}

/**
 * Specify the php module as dependency.
 */
function rules_execute_php_eval_dependencies() {
  return array('php');
}

/**
 * PHP integration access callback.
 */
function rules_php_integration_access() {
  return user_access('use PHP for settings');
}

/**
 * Implements hook_rules_condition_info() on behalf of the PHP module.
 */
function rules_php_condition_info() {
  return array(
    'php_eval' => array(
      'label' => t('Execute custom PHP code'),
      'group' => t('PHP'),
      'parameter' => array(
        'code' => array(
          'restriction' => 'input',
          'type' => 'text',
          'label' => t('PHP code'),
          'description' => t('Enter PHP code without &lt;?php ?&gt; delimiters that returns a boolean value; e.g. <code>@code</code>.', array('@code' => "return arg(0) == 'node';")),
        ),
      ),
      'base' => 'rules_execute_php_eval',
      'access callback' => 'rules_php_integration_access',
    ),
  );
}

/**
 * Generates help for the PHP actions, conditions and input evaluator.
 */
function rules_php_evaluator_help($var_info, $action_help = FALSE) {
  $render['top'] = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#markup' => t('PHP code inside of &lt;?php ?&gt; delimiters will be evaluated and replaced by its output. E.g. &lt;? echo 1+1?&gt; will be replaced by 2.')
                 . ' ' . t('Furthermore you can make use of the following variables:'),
  );
  $render['vars'] = array(
    '#theme' => 'table',
    '#header' => array(t('Variable name'), t('Type'), t('Description')),
    '#attributes' => array('class' => array('rules-php-help')),
  );

  $cache = rules_get_cache();
  foreach ($var_info as $name => $info) {
    $row   = array();
    $row[] = '$'. check_plain($name);
    $label = isset($cache['data_info'][$info['type']]['label']) ? $cache['data_info'][$info['type']]['label'] : $info['type'];
    $row[] = check_plain(drupal_ucfirst($label));
    $row[] = check_plain($info['label']);
    $render['vars']['#rows'][] = $row;
  }

  if ($action_help) {
    $render['updated_help'] = array(
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t("If you want to change a variable just return an array of new variable values, e.g.: !code", array('!code' => '<pre>return array("node" => $node);</pre>')),
    );
  }
  return $render;
}

/**
 * @}
 */