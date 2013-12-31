<?php

/**
 * @file
 * Template file for displaying a media item (entity) as a thumbnail on the
 * gallery page.
 *
 * The template-specific available variables are:
 *
 * - $media_gallery_item: The rendered media gallery item.
 * - $media_gallery_meta: The rendered metadata about the item.
 *
 * @see template_preprocess_media_gallery_media_item_thumbnail()
 */
?>
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php print render($title_prefix); ?>
  <?php if (!empty($title)): ?>
    <h3<?php print $title_attributes; ?>><?php print $title ?></h3>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <?php print $media_gallery_item; ?>

  <?php print $media_gallery_meta; ?>
</div>
