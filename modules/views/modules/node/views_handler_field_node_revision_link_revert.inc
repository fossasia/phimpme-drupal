<?php
/**
 * Field handler to present a link to revert a node to a revision.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_node_revision_link_revert extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['node_vid'] = array('table' => 'node', 'field' => 'vid');
  }

  function access() {
    return user_access('revert revisions') || user_access('administer nodes');
  }

  function render_link($data, $values) {
    // ensure user has access to edit this node.
    $node = $this->get_value($values);
    $node->status = 1; // unpublished nodes ignore access control
    if (!node_access('update', $node)) {
      return;
    }

    // Current revision cannot be reverted.
    if ($node->vid ==  $this->get_value($values, 'node_vid')) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('revert');

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "node/$node->nid/revisions/$node->vid/revert";
    $this->options['alter']['query'] = drupal_get_destination();

    return $text;
  }
}
