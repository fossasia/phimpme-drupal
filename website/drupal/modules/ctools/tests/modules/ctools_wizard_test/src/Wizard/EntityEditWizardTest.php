<?php

namespace Drupal\ctools_wizard_test\Wizard;

use Drupal\ctools\Wizard\EntityFormWizardBase;

class EntityEditWizardTest extends EntityFormWizardBase {

  /**
   * {@inheritdoc}
   */
  public function getWizardLabel() {
    return $this->t('Example entity');
  }

  /**
   * {@inheritdoc}
   */
  public function getMachineLabel() {
    return $this->t('Label');
  }

  /**
   * {@inheritdoc}
   */
  public function getEntityType() {
    return 'ctools_wizard_test_config_entity';
  }

  /**
   * {@inheritdoc}
   */
  public function exists() {
    return '\Drupal\ctools_wizard_test\Entity\ExampleConfigEntity::load';
  }

  /**
   * {@inheritdoc}
   */
  public function getOperations($cached_values) {
    /** @var $page \Drupal\ctools_wizard_test\Entity\ExampleConfigEntity */
    $config_entity = $cached_values['ctools_wizard_test_config_entity'];

    $steps = [
      'general' => [
        'form' => 'Drupal\ctools_wizard_test\Form\ExampleConfigEntityGeneralForm',
        'title' => $this->t('General'),
      ],
      'one' => [
        'form' => 'Drupal\ctools_wizard_test\Form\ExampleConfigEntityOneForm',
        'title' => $this->t('Form One'),
      ],
      'two' => [
        'form' => 'Drupal\ctools_wizard_test\Form\ExampleConfigEntityTwoForm',
        'title' => $this->t('Form Two'),
      ],
    ];

    // To test that we can get the config entity and add/remove steps
    // based on it's values, we'll add a special step only when the entity
    // is pre-existing.
    if (!empty($config_entity) && !$config_entity->isNew()) {
      $steps['existing'] = [
        'form' => 'Drupal\ctools_wizard_test\Form\ExampleConfigEntityExistingForm',
        'title' => $this->t('Existing entity'),
      ];
    }

    return $steps;
  }
}
