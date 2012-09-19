<?php

/**
 * The Galleries settings page will just return the term edit form for the "all galleries" tid.
 *
 */
function media_gallery_admin_settings() {
  $edit = media_gallery_get_default_gallery_collection();
  $form_state['build_info']['args'] = array($edit);
  form_load_include($form_state, 'inc', 'taxonomy', 'taxonomy.admin');
  return drupal_build_form('taxonomy_form_term', $form_state);
}
