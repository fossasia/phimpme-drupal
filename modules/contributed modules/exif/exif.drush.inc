<?php
/**
 * Implementation of hook_drush_command().
 */
function exif_drush_command() {
	$items['exif-list'] = array(
		'description' => 'list content type where exif is enabled.',
	);
	$items['exif-update'] = array(
		'description' => 'Update all nodes where exif is enabled.',
		'arguments' => array(
			'type' => 'Optional. The content-type to update (all other content-type will be ignored).',
	),
	);

	return $items;
};


/**
 * Implementation of hook_drush_help().
 */
function exif_drush_help($section) {
	switch ($section) {
		case 'drush:exif-list':
			return dt('list content type where exif is enabled.');
		case 'drush:exif-update':
			return dt('Update all nodes where exif is enabled.');
	}
}
/**
 * Drush callback;
 */
function drush_exif_list() {
	$types = variable_get('exif_nodetypes', array());
	drush_log(t('listing %count content types.', array('%count' =>  count($types))),'ok');
	foreach ( $types as $type) {
		drush_log(t('- %type.', array('%type' => $type)), 'ok');
	}
}

/**
 * Drush callback;
 */
function drush_exif_update($type = '') {
	$types = variable_get('exif_nodetypes', array());
	if (!empty($type)) {
		if (in_array($type, $types)) {
			$types = array ($type);
		} else {
			drupal_set_error(dt("%type is not handle by exif. run exif-list to see all content types where exif is enabled.",array('%type' => $type)));
		}
	}
	drush_log(dt('Need to update %count content type.', array('%count' =>  count($types))));
	foreach ( $types as $type) {
		$query = "SELECT n.nid FROM {node} n WHERE n.type = :type";
		$result =  db_query($query, array(':type' => $type));
		$count = 0;
		foreach ($result as $record) {
			// Load the node object from the database
			$node = node_load($record->nid);
			// Resave the node to make exif changes.
			node_save($node);
			$count++;
		}
		drush_log(dt('Updated %count %type nodes.', array('%count' => $count, '%type' => $type)),"ok");
	}
}