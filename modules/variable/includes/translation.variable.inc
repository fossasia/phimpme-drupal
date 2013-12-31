<?php
/**
 * @file
 * Variable API module. Definition for Drupal core variables
 */

/**
 * Implements hook_variable_info_alter().
 */
function translation_variable_info_alter(&$variables, $options) {
  $variables['language_content_type_[node_type]']['repeat']['options'][TRANSLATION_ENABLED] = t('Enabled, with translation', array(), $options);
  $variables['language_content_type_[node_type]']['description'] = t('Enable multilingual support for this content type. If enabled, a language selection field will be added to the editing form, allowing you to select from one of the <a href="!languages">enabled languages</a>. You can also turn on translation for this content type, which lets you have content translated to any of the enabled languages. If disabled, new posts are saved with the default language. Existing content will not be affected by changing this option.', array('!languages' => url('admin/config/regional/language', $options)), $options);
}