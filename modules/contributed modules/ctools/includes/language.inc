<?php

/**
 * Returns array of language names.
 *
 * This is a one to one copy of locale_language_list because we can't rely on enabled locale module.
 *
 * @param $field
 *   'name' => names in current language, localized
 *   'native' => native names
 * @param $all
 *   Boolean to return all languages or only enabled ones
 *
 * @see locale_language_list
 */
function ctools_language_list($field = 'name', $all = FALSE) {
  if ($all) {
    $languages = language_list();
  }
  else {
    $languages = language_list('enabled');
    $languages = $languages[1];
  }
  $list = array();
  foreach ($languages as $language) {
    $list[$language->language] = ($field == 'name') ? t($language->name) : $language->$field;
  }
  return $list;
}

/**
 * Returns an array of language names similar to ctools_language_list() except
 * that additional choices have been added for ease of use.
 */

function ctools_language_list_all() {
  $languages = array(
    '***CURRENT_LANGUAGE***' => t("Current user's language"),
    '***DEFAULT_LANGUAGE***' => t("Default site language"),
    LANGUAGE_NONE => t('Language neutral'),
  );
  $languages = array_merge($languages, ctools_language_list());
  return $languages;
}