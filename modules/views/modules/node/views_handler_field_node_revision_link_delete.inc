<?php
/**
 * Field handler to present delete a node revision.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_node_revision_link_delete extends views_handler_field_node_link {
  function construct() {
    parent::construct();
    $this->additional_fields['node_vid'] = array('table' => 'node', 'field' => 'vid');
  }

  function access() {
    return user_access('delete revisions') || user_access('administer nodes');
  }

  function render_link($data, $values) {
    // ensure user has access to delete this node.
    $node = $this->get_value($values);
    $node->status = 1; // unpublished nodes ignore access control
    if (!node_access('delete', $node)) {
      return;
    }

    // Current revision cannot be deleted.
    if ($node->vid == $this->get_value($values, 'node_vid')) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "node/$node->nid/revisions/$node->vid/delete";
    $this->options['alter']['query'] = drupal_get_destination();

    return $text;
  }
}
