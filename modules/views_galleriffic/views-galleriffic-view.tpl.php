<?php
/**
 * @file
 *  Views Galleriffic theme wrapper.
 *
 * @ingroup views_templates
 */

?>
<div id="galleriffic" class="clearfix">
  <div id="controls" class="controls"></div>
  <div id="gallery" class="content">
    <div id="loading" class="loader"></div>
    <div id="slideshow"></div>
    <div id="caption" class="caption-container"></div>
  </div>
  <div id="thumbs" class="navigation">
    <ul class="thumbs noscript">
      <?php foreach ($rows as $row): ?>
        <?php print $row?>
      <?php endforeach; ?>
    </ul>
  </div>
</div>
