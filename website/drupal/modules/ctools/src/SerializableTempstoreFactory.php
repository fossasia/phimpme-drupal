<?php

namespace Drupal\ctools;

use Drupal\user\SharedTempStoreFactory;

/**
 * A factory for creating SerializableTempStore objects.
 */
class SerializableTempstoreFactory extends SharedTempStoreFactory {

  /**
   * {@inheritdoc}
   */
  function get($collection, $owner = NULL) {
    // Use the currently authenticated user ID or the active user ID unless the
    // owner is overridden.
    if (!isset($owner)) {
      $owner = \Drupal::currentUser()->id() ?: session_id();
    }

    // Store the data for this collection in the database.
    $storage = $this->storageFactory->get("user.shared_tempstore.$collection");
    return new SerializableTempstore($storage, $this->lockBackend, $owner, $this->requestStack, $this->expire);
  }

}
