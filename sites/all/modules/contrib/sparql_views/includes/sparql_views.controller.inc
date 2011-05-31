<?php

/**
 * @file
 * Provides a controller building upon the core controller but providing more
 * features like full CRUD functionality.
 */

/**
 * A controller implementing EntityAPIControllerInterface for the database 
 * which can deal with exportable SPARQL Views resources.
 */
class SparqlViewsController extends EntityAPIController implements EntityAPIControllerInterface {

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();
    $entity_record = parent::save($entity, $transaction);
    field_info_cache_clear();
    return $entity_record;
  }

}
