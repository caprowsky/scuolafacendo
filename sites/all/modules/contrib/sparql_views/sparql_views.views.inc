<?php

function sparql_views_views_data() {
  // For each registered endpoint, create a new base table.
  foreach (sparql_views_get_endpoints() as $endpoint) {
    $data[$endpoint->table_id]['table']['group'] = t('SPARQL Views');
  
    $data[$endpoint->table_id]['table']['base'] = array(
      'field' =>'vsid',
      'title' => t('SPARQL Views: ' . $endpoint->title),
      'query class' => 'sparql_views_query',
      'help' => t('Views SPARQL query builder'),
    );

    $data[$endpoint->table_id]['table']['endpoint'] = $endpoint;
  }

  if (!isset($data)) {
    drupal_set_message(t('There are no SPARQL Views view types available. To make one available, !register.', array('!register' => l(t('register a SPARQL endpoint'), 'admin/structure/sparql_registry'))), 'warning');
    return;
  }

  return $data;
}

/*
 * Implements hook_views_data_alter().
 *
 * Each endpoint gets its own set of fields based on which SPARQL Views
 * resource types are enabled for it.
 */
function sparql_views_views_data_alter(&$data) {
  $entity_info = entity_get_info('sparql_views_resource');
  // Create the data definition for each endpoint.
  foreach (sparql_views_get_endpoints() as $endpoint) {
    $select = db_select('sparql_views_resource_type', 'svrt')
      ->fields('svrt', array('name'));
    $select->join('sparql_views_resource_type_endpoint', 'e', 'svrt.id = e.svid AND e.srid = :endpoint', array(':endpoint' => $endpoint->srid));
    $bundles = $select->execute()->fetchCol();
    foreach($bundles as $bundle_type) {
      $rdf_mapping = $entity_info['bundles'][$bundle_type]['rdf_mapping'];
      $group = $bundle_type;  
      $new_fields = array();
      // Attach handlers for the subject URI.
      $new_fields[$bundle_type] = array(
        'title' => t('URI'),
        'help' => isset($rdf_mapping['rdftype']) ? t('RDF type: %type', array('%type' => $rdf_mapping['rdftype'])) : 'RDF resource',
        'group' => t($group),
        'field' => array(
          'handler' => 'sparql_views_handler_field_subject',
          'field_name' => 'subject',
          'bundle' => $bundle_type,
          'rdf_mapping' => isset($rdf_mapping['rdftype']) ? $rdf_mapping['rdftype'] : NULL,
          'click sortable' => TRUE,
        ),
        'sort' => array(
          'help' => t('Order the results of the query. This can make your query very slow and will time out on some endpoints, such as DBpedia.'),
          'handler' => 'sparql_views_handler_sort',
        ),
        'argument' => array(
          'help' => t('Replace part of the query'),
          'group' => t($group),
          'base' => $endpoint->table_id,
          'base field' => $bundle_type,
          'handler' => 'sparql_views_handler_argument',
          'label' => t('Test Field'),
        ),
      );
      // Attach handlers for the individual field.
      foreach ($rdf_mapping as $field_name => $field ) {
        if($field_name == 'rdftype')
          continue;
        $field_info = field_info_field($field_name);
        $new_field = array(
          'title' => t($field_name),
          'help' => t('RDF mapping: %rdf-mapping', array('%rdf-mapping' => implode(', ', $field['predicates']))),
          'group' => t($group),
          'field' => array(
            'handler' => 'sparql_views_handler_field',
            'field_info' => $field_info,
            'field_name' => $field_name,
            'bundle' => $bundle_type,
            'rdf_mapping' => $field['predicates'],
            'subject' => $bundle_type,
            'click sortable' => TRUE,
          ),
          'relationship' => array(
            'help' => t('The last comment of a node.'),
            'group' => t($group),
            'base' => $endpoint->table_id,
            'base field' => $field_name,
            'handler' => 'sparql_views_handler_relationship',
            'label' => t('Test Field'),
          ),
          'filter' => array(
            'help' => t('Compare values.'),
            'group' => t($group),
            'base' => $endpoint->table_id,
            'base field' => $field_name,
            'handler' => 'sparql_views_handler_filter_field_value',
            'label' => t('Test Field'),
          ),
          'argument' => array(
            'help' => t('Replace part of the query'),
            'group' => t($group),
            'base' => $endpoint->table_id,
            'base field' => $field_name,
            'handler' => 'sparql_views_handler_argument',
            'label' => t('Test Field'),
          ),
          'sort' => array(
            'help' => t('Order the results of the query. This can make your query very slow and will time out on some endpoints, such as DBpedia.'),
            'handler' => 'sparql_views_handler_sort',
          ),
        );
       
        $new_fields[$bundle_type.'_'.$field_name] = $new_field;
        $new_fields[$bundle_type.'_'.$field_name . '_language']['filter'] = array(
          'title' => t($field_name . ' language'),
            'help' => t('Choose the language for return values.'),
            'group' => t($group),
            'base' => $endpoint->table_id,
            'base field' => $bundle_type.'_'.$field_name,
            'handler' => 'sparql_views_handler_filter_language',
            'label' => t('Language'),
          );
      }
      $data[$endpoint->table_id] = array_merge($data[$endpoint->table_id], $new_fields);
    }
  }
}

/**
 * Implementation of hook_views_plugins().
 */
function sparql_views_views_plugins() {
  return array(
    'query' => array(
      'sparql_views_query' => array(
        'title' => t('SPARQL Query'),
        'help' => t('SPARQL Query'),
        'handler' => 'sparql_views_plugin_query_sparql'
      ),
    ),
    'argument default' => array(
      'sparql_views_argument_default_field' => array(
        'title' => t('Field Value (Node)'),
        'help' => t('Replace a variable in the query with a node\'s field value.'),
        'handler' => 'sparql_views_plugin_argument_default_field',
      ),
    ),
  );
}

/*
 * Helper function to get base tables with hashes.
 */
function sparql_views_get_endpoints() {
  $endpoints = sparql_registry_load_multiple();
  foreach ($endpoints as $key => $endpoint) {
    // We create a hash of the endpoint URI so that we can have a short base
    // table name while still ensuring exportability of Views between sites.
    $hash = crc32(str_replace('http://', '', $endpoint->endpoint));
    $endpoints[$key]->table_id = 'sparql_views_' . $hash;
  }
  return $endpoints;
}