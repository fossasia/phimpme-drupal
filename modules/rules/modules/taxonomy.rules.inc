<?php

/**
 * @file rules integration for the taxonomy_term module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_event_info().
 */
function rules_taxonomy_event_info() {
  $defaults_term = array(
    'group' => t('Taxonomy'),
    'access callback' => 'rules_taxonomy_term_integration_access',
    'module' => 'taxonomy',
  );
  $defaults_vocab = array(
    'group' => t('Taxonomy'),
    'access callback' => 'rules_taxonomy_vocabulary_integration_access',
    'module' => 'taxonomy',
  );
  return array(
    'taxonomy_term_insert' => $defaults_term + array(
      'label' => t('After saving a new term'),
      'variables' => array(
        'term' => array('type' => 'taxonomy_term', 'label' => t('created term')),
      ),
    ),
    'taxonomy_term_update' => $defaults_term + array(
      'label' => t('After updating an existing term'),
      'variables' => array(
        'term' => array('type' => 'taxonomy_term', 'label' => t('updated term')),
        'term_unchanged' => array('type' => 'taxonomy_term', 'label' => t('unchanged term'), 'handler' => 'rules_events_entity_unchanged'),
      ),
    ),
    'taxonomy_term_presave' => $defaults_term + array(
      'label' => t('Before saving a taxonomy term'),
      'variables' => array(
        'term' => array('type' => 'taxonomy_term', 'label' => t('saved term'), 'skip save' => TRUE),
        'term_unchanged' => array('type' => 'taxonomy_term', 'label' => t('unchanged term'), 'handler' => 'rules_events_entity_unchanged'),
      ),
    ),
    'taxonomy_term_delete' => $defaults_term + array(
      'label' => t('After deleting a term'),
      'variables' => array(
        'term' => array('type' => 'taxonomy_term', 'label' => t('deleted term')),
      ),
    ),
    'taxonomy_vocabulary_insert' => $defaults_vocab + array(
      'label' => t('After saving a new vocabulary'),
      'variables' => array(
        'vocabulary' => array('type' => 'taxonomy_vocabulary', 'label' => t('created vocabulary')),
      ),
    ),
    'taxonomy_vocabulary_update' => $defaults_vocab + array(
      'label' => t('After updating an existing vocabulary'),
      'variables' => array(
        'vocabulary' => array('type' => 'taxonomy_vocabulary', 'label' => t('updated vocabulary')),
        'vocabulary_unchanged' => array('type' => 'taxonomy_vocabulary', 'label' => t('unchanged vocabulary'), 'handler' => 'rules_events_entity_unchanged'),
      ),
    ),
    'taxonomy_vocabulary_presave' => $defaults_vocab + array(
      'label' => t('Before saving a vocabulary'),
      'variables' => array(
        'vocabulary' => array('type' => 'taxonomy_vocabulary', 'label' => t('saved vocabulary'), 'skip save' => TRUE),
        'vocabulary_unchanged' => array('type' => 'taxonomy_vocabulary', 'label' => t('unchanged vocabulary'), 'handler' => 'rules_events_entity_unchanged'),
      ),
    ),
    'taxonomy_vocabulary_delete' => $defaults_vocab + array(
      'label' => t('After deleting a vocabulary'),
      'variables' => array(
        'vocabulary' => array('type' => 'taxonomy_vocabulary', 'label' => t('deleted vocabulary')),
      ),
    ),
  );
}

/**
 * Taxonomy term integration access callback.
 */
function rules_taxonomy_term_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'taxonomy_term');
  }
}

/**
 * Taxonomy vocabulary integration access callback.
 */
function rules_taxonomy_vocabulary_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'taxonomy_vocabulary');
  }
}

/**
 * @}
 */
