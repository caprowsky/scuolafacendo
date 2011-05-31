<?php
/**
 * @file
 * SPARQL API query engine implementation.
 *
 * @author Arto Bendiken <http://bendiken.net/>
 * @copyright Copyright (c) 2007-2008 Arto Bendiken. All rights reserved.
 * @license GPL <http://creativecommons.org/licenses/GPL/2.0/>
 * @package sparql.module
 */

//////////////////////////////////////////////////////////////////////////////
// Module settings

define('SPARQL_ENDPOINT', variable_get('sparql_endpoint', FALSE));

//////////////////////////////////////////////////////////////////////////////
// SPARQL API query constructors (textual)

/**
 * Executes a SPARQL query in text form.
 */
function sparql_query($text, $options = array(), &$errors = NULL) {
  if (!isset($options['prefixes']) || $options['prefixes']) {
    $text = sparql_default_prefixes() . "\n" . $text;
  }

  if (!empty($options['endpoint'])) {
    require_once drupal_get_path('module', 'sparql') . '/sparql.client.inc';
    return sparql_request($options['endpoint'], $text, $options, $errors);
  }


}


function sparql_default_prefixes() {
  return preg_replace('/xmlns:(\w+)\=\"([^\"]*)\"/', "PREFIX $1: <$2>", drupal_get_rdf_namespaces());
}
