<?php

/**
 * @file
 * Contains an example for a Views query plugin that could use the data selection tables.
 */

/**
 * Describes the additional methods looked for on a query plugin if data selection based tables or fields are used.
 *
 * Only get_result_entities() needs to be present, so results can be retrieved.
 * The other methods are optional.
 *
 * If the table does not contain entities, however, the get_result_wrappers()
 * method is necessary, too. If this is the case and there are no relations to
 * entity tables, the get_result_entities() method is not needed.
 *
 * @see entity_views_table_definition()
 */
abstract class entity_views_example_query extends views_plugin_query {

  /**
   * Add a sort to the query.
   *
   * This is used to add a sort based on an Entity API data selector instead
   * of a field alias.
   *
   * This method has to be present if click-sorting on fields should be allowed
   * for some fields using the default Entity API field handlers.
   *
   * @param $selector
   *   The field to sort on, as an Entity API data selector.
   * @param $order
   *   The order to sort items in - either 'ASC' or 'DESC'. Defaults to 'ASC'.
   */
  public abstract function add_selector_orderby($selector, $order = 'ASC');

  /**
   * Returns the according entity objects for the given query results.
   *
   * This is compatible to the get_result_entities() method used by Views.
   *
   * The method is responsible for resolving the relationship and returning the
   * entity objects for that relationship. The helper methods
   * EntityFieldHandlerHelper::construct_property_selector() and
   * EntityFieldHandlerHelper::extract_property_multiple() can be used to do
   * this.
   *
   * @param $results
   *   The results of the query, as returned by this query plugin.
   * @param $relationship
   *   (optional) A relationship for which the entities should be returned.
   * @param $field
   *   (optional) The field for which the entity should be returned. This is
   *   only needed in case a field is derived via a referenced entity without
   *   using a relationship. For example, if the node's field "author:name" is
   *   used, the user entity would be returned instead of the node entity.
   *
   * @return
   *   A numerically indexed array containing two items: the entity type of
   *   entities returned by this method; and the array of entities, keyed by the
   *   same indexes as the results.
   *
   * @see EntityFieldHandlerHelper::extract_property_multiple()
   */
  public abstract function get_result_entities($results, $relationship = NULL, $field = NULL);

  /**
   * Returns the according metadata wrappers for the given query results.
   *
   * This can be used if no entities for the results can be given, but entity
   * metadata wrappers can be constructed for them.
   *
   * @param $results
   *   The results of the query, as returned by this query plugin.
   * @param $relationship
   *   (optional) A relationship for which the wrappers should be returned.
   * @param $field
   *   (optional) The field of which a wrapper should be returned.
   *
   * @return
   *   A numerically indexed array containing two items: the data type of
   *   the wrappers returned by this method; and the array of retrieved
   *   EntityMetadataWrapper objects, keyed by the same indexes as the results.
   */
  public abstract function get_result_wrappers($results, $relationship = NULL, $field = NULL);

}
