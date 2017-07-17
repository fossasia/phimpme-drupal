<?php

namespace Drupal\ctools\Access;

use Drupal\Core\Session\AccountInterface;

interface AccessInterface {
  public function access(AccountInterface $account);
}
