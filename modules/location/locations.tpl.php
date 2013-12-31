<?php if (!empty($locations)): ?>
  <h3 class="location-locations-header"><?php print format_plural(count($locations), 'Location', 'Locations'); ?></h3>
  <div class="location-locations-wrapper">
    <?php foreach ($locations as $location): ?>
      <?php print $location; ?>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
