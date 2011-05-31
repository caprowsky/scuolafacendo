<?php
/**
 * @file
 * poi_import.features.inc
 */

/**
 * Implementation of hook_ctools_plugin_api().
 */
function poi_import_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function poi_import_node_info() {
  $items = array(
    'poi' => array(
      'name' => t('poi'),
      'base' => 'node_content',
      'description' => t('Define Point of interest'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
