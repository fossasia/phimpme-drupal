<?php

namespace Drupal\ctools;

use Drupal\Core\DependencyInjection\DependencySerializationTrait;
use Drupal\user\SharedTempStore;

/**
 * An extension of the SharedTempStore system for serialized data.
 */
class SerializableTempstore extends SharedTempStore {
  use DependencySerializationTrait;
}
