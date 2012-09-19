<?php

class NodeResourceFeedModel implements ResourceTimeFeedModel {
  protected $nodes = NULL;
  protected $mode = 'raw';
  protected $item_length = 'fulltext';

  public function __construct($data, $arguments) {
    $this->nodes = $data;
    if (isset($arguments['mode'])) {
      $this->mode = $arguments['mode'];
    }
    if (isset($arguments['item_length'])) {
      $this->item_length = $arguments['item_length'];
    }
  }

  public function current() {
    $node = current($this->nodes);
    if ($node !== FALSE) {
      return new NodeResourceFeedModelItem($node, $this->mode, $this->item_length);
    }
    return FALSE;
  }

  public function key() {
    return key($this->nodes);
  }

  public function next() {
    next($this->nodes);
  }

  public function rewind() {
    reset($this->nodes);
  }

  public function valid() {
    // It's safe to use current as there never should be a boolean
    // in the node array.
    return current($this->nodes) !== FALSE;
  }
}

class NodeResourceFeedModelItem implements ResourceTimeFeedModelItem {
  protected $node = NULL;
  protected $mode = 'raw';
  protected $item_length = 'fulltext';

  public function __construct($data, $mode = 'raw', $item_length = 'fulltext') {
    $this->node = $data;
    $this->mode = $mode;
    $this->item_length = $item_length;
  }

  /**
   * Returns the raw node title
   *
   * @return string
   *  The title of the node
   */
  public function getName() {
    return $this->node->title;
  }

  /**
   * Calls node_build_content to create a teaser
   */
  public function getDescription() {
    $description = '';
    if ($this->mode == 'render') {
      // Don't bother with the rendering process if only
      // the title was requested.
      if ($this->item_length != 'title') {
        $description = $this->renderAsNodeFeed($this->node->nid, $this->item_length);
      }
    }
    else {
      // Serve the raw teaser or body
      switch ($this->item_length) {
        case 'teaser':
          if (isset($this->node->teaser)) {
            $description =  $this->node->teaser;
          }
        break;
        case 'fulltext':
          if (isset($this->node->body)) {
            $description =  $this->node->body;
          }
        break;
      }
    }
    return $description;
  }

  /**
   * This function consists entirely of code copy pasted from node_feed()
   * to make the node resource feed model consistent with normal feeds.
   *
   * @param int $nid
   *  The id of the node that should be rendered
   * @param string $item_length
   *  The length of the content to generate: title, teaser or fulltext
   * @return string
   *  The text that should be used for the feed item description
   */
  protected function renderAsNodeFeed($nid, $item_length) {
    // Load the specified node:
    $item = node_load($nid);
    $item->build_mode = NODE_BUILD_RSS;
    $item->link = url("node/$nid", array('absolute' => TRUE));

    if ($item_length != 'title') {
      $teaser = ($item_length == 'teaser') ? TRUE : FALSE;

      // Filter and prepare node teaser
      if (node_hook($item, 'view')) {
        $item = node_invoke($item, 'view', $teaser, FALSE);
      }
      else {
        $item = node_prepare($item, $teaser);
      }

      // Allow modules to change $node->content before the node is rendered.
      node_invoke_nodeapi($item, 'view', $teaser, FALSE);

      // Set the proper node property, then unset unused $node property so that a
      // bad theme can not open a security hole.
      $content = drupal_render($item->content);
      if ($teaser) {
        $item->teaser = $content;
        unset($item->body);
      }
      else {
        $item->body = $content;
        unset($item->teaser);
      }

      // Allow modules to modify the fully-built node.
      node_invoke_nodeapi($item, 'alter', $teaser, FALSE);
    }

    // Allow modules to add additional item fields and/or modify $item
    $extra = node_invoke_nodeapi($item, 'rss item');
    $extra = array_merge($extra, array(
      array('key' => 'pubDate', 'value' => gmdate('r', $item->created)),
      array('key' => 'dc:creator', 'value' => $item->name),
      array('key' => 'guid', 'value' => $item->nid . ' at ' . $base_url, 'attributes' => array('isPermaLink' => 'false')))
    );
    foreach ($extra as $element) {
      if (isset($element['namespace'])) {
        $namespaces = array_merge($namespaces, $element['namespace']);
      }
    }

    // Prepare the item description
    switch ($item_length) {
      case 'fulltext':
        $item_text = $item->body;
        break;
      case 'teaser':
        $item_text = $item->teaser;
        if (!empty($item->readmore)) {
          $item_text .= '<p>' . l(t('read more'), 'node/' . $item->nid, array('absolute' => TRUE, 'attributes' => array('target' => '_blank'))) . '</p>';
        }
        break;
      case 'title':
        $item_text = '';
        break;
    }

    return $item_text;
  }

  /**
   * Returns the absolute url to the node
   *
   * @return string
   *  The node url
   */
  public function getUrl() {
    return url('node/' . $this->node->nid, array('absolute' => TRUE));
  }

  /**
   * Gets the created time for the node
   *
   * @return int
   *  The created time of the node as a timestamp
   */
  public function getCreated() {
    return $this->node->created;
  }

  /**
   * Gets the created time for the node
   *
   * @return int
   *  The created time of the node as a timestamp
   */
  public function getStarts() {
    return $this->node->created;
  }

  /**
   * Gets the created time for the node
   *
   * @return int
   *  The created time of the node as a timestamp
   */
  public function getEnds() {
    return $this->node->created;
  }

  /**
   * Gets a associative array containing extra properties for the item.
   *
   * @return array
   *  The extra properties of the item as an array
   */
  public function getProperties() {
    return get_object_vars($this->node);
  }
}
