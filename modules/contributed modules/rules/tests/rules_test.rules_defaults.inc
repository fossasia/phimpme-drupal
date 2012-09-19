<?php

/**
 * @file Includes any rules integration provided by the module.
 */


/**
 * Implements hook_default_rules_configuration().
 */
function rules_test_default_rules_configuration() {
  $rule = rules_reaction_rule();
  $rule->label = 'example default rule';
  $rule->active = FALSE;
  $rule->event('node_update')
       ->condition(rules_condition('data_is', array('data:select' => 'node:status', 'value' => TRUE))->negate())
       ->condition('data_is', array('data:select' => 'node:type', 'value' => 'page'))
       ->action('drupal_message', array('message' => 'A node has been updated.'));

  $configs['rules_test_default_1'] = $rule;

  $action_set = rules_action_set(array('node' => array('type' => 'node', 'label' => 'Content')));
  $action_set->action('node_publish');
  $configs['rules_test_action_set'] = $action_set;

  // Test providing a rule using an export.
  $configs['rules_export_test'] = rules_import(_rules_export_get_test_export());

  // An action set used to test merging term parents.
  $configs['rules_retrieve_term_parents'] = rules_import('{ "rules_retrieve_term_parents" : {
    "LABEL" : "Retrieve term parents",
    "PLUGIN" : "action set",
    "REQUIRES" : [ "rules" ],
    "USES VARIABLES" : {
      "terms" : { "label" : "Terms", "type" : "list\u003ctaxonomy_term\u003e" },
      "term_parents" : {
        "label" : "Term parents",
        "type" : "list\u003ctaxonomy_term\u003e",
        "parameter" : false
      }
    },
    "ACTION SET" : [
      { "LOOP" : {
          "USING" : { "list" : [ "terms" ] },
          "ITEM" : { "current_term" : "Current term" },
          "DO" : [
            { "LOOP" : {
                "USING" : { "list" : [ "current-term:parent" ] },
                "ITEM" : { "current_parent" : "Current parent" },
                "DO" : [
                  { "list_add" : {
                      "list" : [ "term-parents" ],
                      "item" : [ "current-parent" ],
                      "unique" : 1
                    }
                  }
                ]
              }
            }
          ]
        }
      }
    ],
    "PROVIDES VARIABLES" : [ "term_parents" ]
  }
}');

  return $configs;
}

/**
 * Defines the export of rule for testing import/export.
 */
function _rules_export_get_test_export() {
  return '{ "rules_export_test" : {
    "LABEL" : "Test import rule2",
    "PLUGIN" : "reaction rule",
    "WEIGHT" : "-1",
    "ACTIVE" : false,
    "TAGS" : [ "bar", "baz", "foo" ],
    "REQUIRES" : [ "rules", "comment" ],
    "ON" : [ "comment_insert" ],
    "IF" : [
      { "OR" : [
          { "NOT node_is_sticky" : { "node" : [ "comment:node" ] } },
          { "node_is_of_type" : {
              "node" : [ "comment:node" ],
              "type" : { "value" : { "page" : "page" } }
            }
          },
          { "NOT AND" : [ { "OR" : [] } ] }
        ]
      }
    ],
    "DO" : [
      { "data_set" : {
          "data" : [ "comment:node:created" ],
          "value" : { "select" : "site:current-date", "date_offset" : { "value" : -604800 } }
        }
      },
      { "node_make_sticky" : { "node" : [ "comment:node" ] } },
      { "variable_add" : {
          "USING" : { "type" : "token", "value" : "error" },
          "PROVIDE" : { "variable_added" : { "level" : "Error level" } }
        }
      },
      { "drupal_message" : {
          "message" : "fein, [comment:node:title] has been made sticky!",
          "type" : [ "level" ]
        }
      },
      { "LOOP" : {
          "USING" : { "list" : [ "site:current-user:roles" ] },
          "ITEM" : { "current_role" : "Current role" },
          "DO" : [ { "drupal_message" : { "message" : [ "current-role" ] } } ]
        }
      }
    ]
  }
}';
}
