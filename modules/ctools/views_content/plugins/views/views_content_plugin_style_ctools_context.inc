<?php
/**
 * @file
 * Contains the default style plugin.
 */

/**
 * Default style plugin to render rows one after another with no
 * decorations.
 *
 * @ingroup views_style_plugins
 */
class views_content_plugin_style_ctools_context extends views_plugin_style {
  var $rows = array();

  /**
   * Render the display in this style.
   */
  function render() {
    if (!empty($this->view->display_handler->previewing)) {
      return parent::render();
    }

    $this->rows = array();
    $this->groups = array();
    if ($this->uses_row_plugin() && empty($this->row_plugin)) {
      vpr('views_plugin_style_default: Missing row plugin');
      return;
    }

    // Some engines like solr key results on ids, but rendering really expects
    // things to be keyed exclusively by row index. Using array_values()
    // guarantees that.
    $this->view->result = array_values($this->view->result);

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);

    // Render each group separately and concatenate.  Plugins may override this
    // method if they wish some other way of handling grouping.
    $output = '';
    foreach ($sets as $title => $records) {
      foreach ($records as $row_index => $row) {
        $this->view->row_index = $row_index;
        $this->rows[$row_index] = $this->row_plugin->render($row);
        $this->groups[$row_index] = $title;
      }
    }
    unset($this->view->row_index);
    return $this->rows;
  }

}
