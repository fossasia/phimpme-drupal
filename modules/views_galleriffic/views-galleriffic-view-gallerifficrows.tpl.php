<?php
/**
 * @file
 * template for views galleriffic row
 */
?>
<?php if ($fields['slide']->content): ?>
  <li>
    <a class="thumb" href="<?php print $fields['slide']->content; ?>" title="<?php  if($fields['title']) { print $fields['title']->stripped;} ?>" name="<?php if($fields['title']) { print $fields['title']->stripped; }?>"><img src="<?php print $fields['thumbnail']->content; ?>" alt="<?php  if($fields['title']) { print $fields['title']->stripped; } ?>" /></a>
    <div class="caption">
      <?php if($fields['title']): ?><div class="image-title"><?php print $fields['title']->content; ?></div> <?php endif;?>
      <?php if($fields['desc']): ?><div class="image-desc"><?php print $fields['desc']->content; ?></div><?php endif;?>

    </div>
  </li>
<?php endif; ?>
