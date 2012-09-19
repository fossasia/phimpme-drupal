<?php

/**
 * @file
 *  These are the base interfaces for resource models.
 */

/**
 * Interface that should be implemented by feed models.
 */
interface ResourceFeedModel extends Iterator {
  // /**
  //  * Implement this function to alter the arguments before the service method
  //  * is called. This makes it possible for the model to ensure that required
  //  * data is requested when the model is used.
  //  *
  //  * @param array &$arguments
  //  * @param array $model_arguments
  //  * @return void
  //  */
  // public static function alterArguments(&$arguments, $model_arguments);
}

/**
 * Interface that must be implemented by items returned
 * by a ResourceFeedModel.
 */
interface ResourceFeedModelItem {
  public function getName();
  public function getDescription();
  public function getUrl();
  public function getCreated();

  /**
   * Gets a associative array containing extra properties for the item.
   *
   * @return array
   *  The extra properties of the item as an array
   */
  public function getProperties();
}

/**
 * Interface that should be implemented by time feed models.
 */
interface ResourceTimeFeedModel extends ResourceFeedModel {
}

/**
 * Interface that must be implemented by items returned
 * by a ResourceTimeFeedModel.
 */
interface ResourceTimeFeedModelItem extends ResourceFeedModelItem {
  /**
   * Gets the starting time of the item
   *
   * @return int
   *  The starting time of the item as a timestamp
   */
  public function getStarts();

  /**
   * Gets the end time of the item
   *
   * @return int
   *  The end time of the item as a timestamp
   */
  public function getEnds();
}