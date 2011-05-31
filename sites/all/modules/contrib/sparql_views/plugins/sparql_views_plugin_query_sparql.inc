<?php
/**
 * Object used to create a SELECT query.
 */
class sparql_views_plugin_query_sparql extends views_plugin_query {
  /**
   * An array of fields which have been specified for selection. Fields may be
   * included in the triple patterns (i.e. in filters or for path traversal)
   * without being selected for retrieval.
   */
  var $select_fields = array();

  /**
   * An array of sections of the triple pattern. Each section is in itself
   * an array of pieces.
   */
  var $triples = array();

  /**
   * An array of URIs and resources that are used as replacements for variables.
   */
  var $replacements = array();

  /**
   * An array of filter functions and parameters.
   */
  var $filters = array();

  /**
   * An array of fields to order by and the ordering direction.
   */
  var $orderby = array();

  /**
   * A pager plugin that should be provided by the display.
   */
  var $pager = NULL;

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);
    $this->base_table = $base_table;
    $this->base_field = $base_field;
    $this->unpack_options($this->options, $options);

    // Check to see whether the necessary requirements are met in order to
    // create a valid query.
    $data = views_fetch_data();
    unset($data[$base_table]['table']);
    if (count($data[$base_table]) === 0) {
      drupal_set_message(t('No resources are available for this endpoint. Please edit the !config to indicate which resources can be used with this endpoint.', array('!config' => l(t('resource configuration', 'admin/structure/sparql-views')))), 'warning', FALSE);
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['consolidate'] = array(
      'default' => FALSE,
      'bool' => TRUE,
    );

    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    $form['consolidate'] = array(
      '#type' => 'checkbox',
      '#title' => t('Consolidate Results'),
      '#description' => t('This will combine all the properties for a given subject into one result. This will override any field-level consolidation.'),
      '#default_value' => !empty($this->options['consolidate']),
    );
  }

 /**
  * Generate a query and a countquery from all of the information supplied
  * to the object.
  *
  * @param $get_count
  *   Provide a countquery if this is true, otherwise provide a normal query.
  */
  function query($view, $get_count = FALSE) {
    $triple_pattern = '';
    $filters = '';
    $orderby = '';
    $ns = rdf_get_namespaces();
    $bindings = array();
    $prefixes = '';
    $mynamespaces = array();

    // Create SELECT statement.
    $select = 'SELECT';
    foreach ($this->select_fields as $key => $field) {
      // Do not add variable to SELECT if it will be replaced with a value.
      if (!isset($this->replacements[$field])) {
        // Add a question mark before each variable.
        $select .= " ?$field";
      }
    }
    $where = '';

    // Form the triple pattern.
    foreach($this->triples as $s => $pattern) {
      $statements = array();
      $triple_pattern .= isset($this->replacements[$s]) ? $this->replacements[$s]['replacement_formatted'] : "?$s";
      foreach($pattern['pred_obj'] as $pred_obj) {
        $p = isset($this->replacements[$pred_obj['p']]) ? $this->replacements[$pred_obj['p']]['replacement_formatted'] : $pred_obj['p'];
        $o = isset($this->replacements[$pred_obj['o']]) ? $this->replacements[$pred_obj['o']]['replacement_formatted'] : '?' . $pred_obj['o'];
        $statements[] = " $p $o";
        $mynamespaces[] = _sparql_views_predicate_ns($p); 
      }
      $triple_pattern .= implode(';', $statements) . ".\n";
    }
    $mynamespaces = array_unique($mynamespaces);
    foreach($mynamespaces as $prefix) {
      $namespace = $ns[$prefix];
      $prefixes .= "PREFIX $prefix: <$namespace>\n";
    }
    // If no triples were added, run a generic ?s ?p ?o query.
    if (empty($triple_pattern)) {
      $triple_pattern = "?$field ?p ?o";
    }

    foreach ($this->filters as $filter) {
      $filters .= "FILTER $filter\n";
    }

    if (!empty($this->orderby)) {
      $orderby = "ORDER BY";
      foreach ($this->orderby as $field => $order) {
        $orderby .= " $order(?$field)\n";
      }
    }
    $limit = isset($this->limit) ? "LIMIT $this->limit" : 'LIMIT 10';
    
    $query = $prefixes;
    $query .= $select . "\n";
    $query .= "WHERE {\n$triple_pattern $filters} \n";
    $query .= $orderby;
    $query .= $limit;
    return $query;
  }

  /**
   * Let modules modify the query just prior to finalizing it.
   */
  function alter(&$view) {  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    $view->init_pager();

    // Let the pager modify the query to add limits.
    $this->pager->query($view);

    $view->build_info['query'] = $this->query($view);
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  function execute(&$view) {
    $views_result = array();
    $query = $view->build_info['query'];
    $views_data = sparql_views_views_data();
    $endpoint = $views_data[$this->base_table]['table']['endpoint']->endpoint;
    $start = microtime(TRUE);

    $sparql_result = sparql_request($query, $endpoint);
    if (empty($sparql_result) || empty($sparql_result['result']['rows'])) {
      $view->result = array();
      vsm(t('No result returned. Please check your query and the endpoint status.'));
      return;
    }
    $rows = $sparql_result['result']['rows'];
    $fields = $sparql_result['result']['variables'];

    // Add the fields that were replaced by values so that we can render the
    // preset values as part of the result.
    $fields = array_merge($fields, array_keys($this->replacements));
    foreach ($rows as $row) {
      $views_row = array();
      foreach($fields as $field) {
        if (isset($row[$field])) {
          $views_row[$field] = $row[$field];
        }
        else if (isset($this->replacements[$field])) {
          $views_row[$field] = $this->replacements[$field]['replacement_raw'];
        }
      }
      $views_result[] = (object) $views_row;
    }
    if ($this->options['consolidate']) {
      $view->result = $this->consolidate_results($views_result);
    }
    else {
      $view->result = $views_result;
    }
    $view->total_rows = count($view->result);
    $view->execute_time = microtime(TRUE) - $start;
    $view->pager['current_page'] = 0;
  }

  function add_field($subject, $predicate, $object) {
    $i = 1;
    $alias = $object;
    while (in_array($alias, $this->select_fields)) {
      $alias = $object . '_' . $i;
      $i++;
    }
    $this->select_fields[] = $alias;
    $this->triples[$subject]['pred_obj'][] = array('p' => $predicate, 'o' => $alias);
    return $alias;
  }

  function add_replacement($field_name, $replacement, $value_type, $language = NULL) {
    switch($value_type){
      case 'uri':
        $replacement_formatted = "<$replacement>";
        break;
      case 'string':
        $replacement_formatted = '"' . $replacement . '"';
        if (!empty($language)) {
          $replacement_formatted .= "@$language";
        }
        break;
    }
    $this->replacements[$field_name] = array(
      'replacement_formatted' => $replacement_formatted,
      'replacement_raw' => $replacement
    );
  }

  function add_filter($function, $params) {
    $params = implode(', ',$params);
    $this->filters[] = "$function($params)";
  }

  function add_orderby($field_name, $order) {
    $this->orderby[$field_name] = $order;
  }

  /**
   * Set a LIMIT on the query, specifying a maximum number of results.
   */
  function set_limit($limit) {
    $this->limit = $limit;
  }

  function consolidate_results($views_result) {
    $results = array();
    $columns = array_keys((array) $views_result[0]);
    $triples = $this->triples;
    $subjects = array_keys($triples);
    $properties = array_diff($columns, $subjects);

    // Get all the rows that share the same subject
    foreach ($views_result as $key => $value) {
      foreach ($subjects as $subject) {
        $row_subjects[$key][$subject] = $value->{$subject};
        foreach ($triples[$subject]['pred_obj'] as $pred_obj) {
          $field = $pred_obj['o'];
          if (in_array($field, $properties)) {
            $subject_uris[$subject][$value->{$subject}][$field][$value->{$field}] = $value->{$field};
          }
        }
      }
    }
    $rows = _sparql_views_super_unique($row_subjects);
    foreach ($rows as $key => $row) {
      foreach ($row as $subject_field_name => $subject) {
        $results[$key][$subject_field_name][] = $subject;
        $results[$key] += $subject_uris[$subject_field_name][$subject];
      }
    }
    return $results;
  }
}

function _sparql_views_super_unique($array)
{
  $result = array_map("unserialize", array_unique(array_map("serialize", $array)));

  foreach ($result as $key => $value) {
    if (is_array($value)) {
      $result[$key] = _sparql_views_super_unique($value);
    }
  }

  return $result;
}