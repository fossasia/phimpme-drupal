<?php

namespace Drupal\ctools_wizard_test\Wizard;


class EntityAddWizardTest extends EntityEditWizardTest {

  /**
   * {@inheritdoc}
   */
  public function getRouteName() {
    return 'entity.ctools_wizard_test_config_entity.add_step_form';
  }

}
