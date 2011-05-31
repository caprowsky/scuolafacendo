<?php
/**
 * @file
 * poi_import.feeds_importer_default.inc
 */

/**
 * Implementation of hook_feeds_importer_default().
 */
function poi_import_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass;
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'importer_spiagge';
  $feeds_importer->config = array(
    'name' => 'importer spiagge',
    'description' => 'Importa le spiagge da sardegnaturismo',
    'fetcher' => array(
      'plugin_key' => 'FeedsFileFetcher',
      'config' => array(
        'allowed_extensions' => 'txt csv tsv xml opml html htm',
        'direct' => FALSE,
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsXPathParserXML',
      'config' => array(
        'sources' => array(
          'xpathparser:0' => 'nome',
          'xpathparser:1' => 'geo/envelope//@minx',
          'xpathparser:2' => 'geo/envelope//@miny',
          'xpathparser:3' => 'geo/envelope//@maxx',
          'xpathparser:4' => 'geo/envelope//@maxy',
          'xpathparser:5' => 'geo/pos//@x',
          'xpathparser:6' => 'geo/pos//@y',
          'xpathparser:8' => 'gallery/image//@src',
          'xpathparser:9' => 'uno_sguardo_in_anteprima',
          'xpathparser:10' => 'come_arrivare',
          'xpathparser:11' => 'informazioni_utili_e_servizi',
          'xpathparser:12' => 'strutture/servizio',
          'xpathparser:13' => 'photo/immagine//@digital',
          'xpathparser:14' => 'photo/immagine//@digitalLink',
        ),
        'rawXML' => array(
          'xpathparser:0' => 0,
          'xpathparser:1' => 0,
          'xpathparser:2' => 0,
          'xpathparser:3' => 0,
          'xpathparser:4' => 0,
          'xpathparser:5' => 0,
          'xpathparser:6' => 0,
          'xpathparser:8' => 0,
          'xpathparser:9' => 0,
          'xpathparser:10' => 0,
          'xpathparser:11' => 0,
          'xpathparser:12' => 0,
          'xpathparser:13' => 0,
          'xpathparser:14' => 0,
        ),
        'context' => '/drupal/item',
        'exp' => array(
          'errors' => 1,
          'debug' => array(
            'context' => 'context',
            'xpathparser:0' => 'xpathparser:0',
            'xpathparser:1' => 0,
            'xpathparser:2' => 0,
            'xpathparser:3' => 0,
            'xpathparser:4' => 0,
            'xpathparser:5' => 0,
            'xpathparser:6' => 0,
            'xpathparser:8' => 0,
            'xpathparser:9' => 0,
            'xpathparser:10' => 0,
            'xpathparser:11' => 0,
            'xpathparser:12' => 0,
            'xpathparser:13' => 0,
            'xpathparser:14' => 0,
          ),
        ),
      ),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsNodeProcessor',
      'config' => array(
        'content_type' => 'poi',
        'expire' => '-1',
        'author' => '1',
        'mappings' => array(
          0 => array(
            'source' => 'xpathparser:0',
            'target' => 'title',
            'unique' => FALSE,
          ),
          1 => array(
            'source' => 'xpathparser:1',
            'target' => 'field_map_minx',
            'unique' => FALSE,
          ),
          2 => array(
            'source' => 'xpathparser:2',
            'target' => 'field_map_miny',
            'unique' => FALSE,
          ),
          3 => array(
            'source' => 'xpathparser:3',
            'target' => 'field_map_maxx',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'xpathparser:4',
            'target' => 'field_map_maxy',
            'unique' => FALSE,
          ),
          5 => array(
            'source' => 'xpathparser:5',
            'target' => 'field_map_posx',
            'unique' => FALSE,
          ),
          6 => array(
            'source' => 'xpathparser:6',
            'target' => 'field_map_posy',
            'unique' => FALSE,
          ),
          7 => array(
            'source' => 'xpathparser:8',
            'target' => 'field_gallery_img',
            'unique' => FALSE,
          ),
          8 => array(
            'source' => 'xpathparser:9',
            'target' => 'field_description',
            'unique' => FALSE,
          ),
          9 => array(
            'source' => 'xpathparser:10',
            'target' => 'field_how_to_arrive',
            'unique' => FALSE,
          ),
          10 => array(
            'source' => 'xpathparser:11',
            'target' => 'field_info',
            'unique' => FALSE,
          ),
          11 => array(
            'source' => 'xpathparser:12',
            'target' => 'field_service',
            'unique' => FALSE,
          ),
          12 => array(
            'source' => 'xpathparser:13',
            'target' => 'field_dl_id',
            'unique' => FALSE,
          ),
          13 => array(
            'source' => 'xpathparser:14',
            'target' => 'field_dl_img:url',
            'unique' => FALSE,
          ),
        ),
        'update_existing' => '2',
        'input_format' => 'full_html',
      ),
    ),
    'content_type' => '',
    'update' => 0,
    'import_period' => '-1',
    'expire_period' => 3600,
    'import_on_create' => 1,
    'process_in_background' => 0,
  );
  $export['importer_spiagge'] = $feeds_importer;

  return $export;
}
