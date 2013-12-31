<?php

/**
 * @file
 * Provides info about system-wide entities.
 */

/**
 * Implements hook_entity_property_info() on top of system module.
 *
 * @see entity_entity_property_info()
 * @see entity_metadata_site_wrapper()
 */
function entity_metadata_system_entity_property_info() {
  $info = array();

  // There is no site entity, but still add metadata for global site properties
  // here. That way modules can alter and add further properties at this place.
  // In order to make use of this metadata modules may use the wrapper returned
  // by entity_metadata_site_wrapper().
  $properties = &$info['site']['properties'];
  $properties['name'] = array(
    'label' => t("Name"),
    'description' => t("The name of the site."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'sanitize' => 'check_plain',
  );
  $properties['slogan'] = array(
    'label' => t("Slogan"),
    'description' => t("The slogan of the site."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'sanitize' => 'check_plain',
  );
  $properties['mail'] = array(
    'label' => t("Email"),
    'description' => t("The administrative email address for the site."),
    'getter callback' => 'entity_metadata_system_get_properties',
  );
  $properties['url'] = array(
    'label' => t("URL"),
    'description' => t("The URL of the site's front page."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'type' => 'uri',
  );
  $properties['login_url'] = array(
    'label' => t("Login page"),
    'description' => t("The URL of the site's login page."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'type' => 'uri',
  );
  $properties['current_user'] = array(
    'label' => t("Logged in user"),
    'description' => t("The currently logged in user."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'type' => 'user',
  );
  $properties['current_date'] = array(
    'label' => t("Current date"),
    'description' => t("The current date and time."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'type' => 'date',
  );
  $properties['current_page'] = array(
    'label' => t("Current page"),
    'description' => t("Information related to the current page request."),
    'getter callback' => 'entity_metadata_system_get_properties',
    'type' => 'struct',
    'property info' => array(
      'path' => array(
        'label' => t("Path"),
        'description' => t("The internal Drupal path of the current page request."),
        'getter callback' => 'current_path',
        'type' => 'text',
      ),
      'url' => array(
        'label' => t("URL"),
        'description' => t("The full URL of the current page request."),
        'getter callback' => 'entity_metadata_system_get_page_properties',
        'type' => 'uri',
      ),
    ),
  );

  // Files.
  $properties = &$info['file']['properties'];
  $properties['fid'] = array(
    'label' => t("File ID"),
    'description' => t("The unique ID of the uploaded file."),
    'type' => 'integer',
    'validation callback' => 'entity_metadata_validate_integer_positive',
    'schema field' => 'fid',
  );
  $properties['name'] = array(
    'label' => t("File name"),
    'description' => t("The name of the file on disk."),
    'getter callback' => 'entity_metadata_system_get_file_properties',
    'schema field' => 'filename',
  );
  $properties['mime'] = array(
    'label' => t("MIME type"),
    'description' => t("The MIME type of the file."),
    'getter callback' => 'entity_metadata_system_get_file_properties',
    'sanitize' => 'filter_xss',
    'schema field' => 'filemime',
  );
  $properties['size'] = array(
    'label' => t("File size"),
    'description' => t("The size of the file, in kilobytes."),
    'getter callback' => 'entity_metadata_system_get_file_properties',
    'type' => 'integer',
    'schema field' => 'filesize',
  );
  $properties['url'] = array(
    'label' => t("URL"),
    'description' => t("The web-accessible URL for the file."),
    'getter callback' => 'entity_metadata_system_get_file_properties',
  );
  $properties['timestamp'] = array(
    'label' => t("Timestamp"),
    'description' => t("The date the file was most recently changed."),
    'type' => 'date',
    'schema field' => 'timestamp',
  );
  $properties['owner'] = array(
    'label' => t("Owner"),
    'description' => t("The user who originally uploaded the file."),
    'type' => 'user',
    'getter callback' => 'entity_metadata_system_get_file_properties',
    'schema field' => 'uid',
  );
  return $info;
}
