<?php

namespace Drupal\ctools_wizard_test\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\ctools_wizard_test\ExampleConfigEntityInterface;

/**
 * Defines the Example config entity entity.
 *
 * @ConfigEntityType(
 *   id = "ctools_wizard_test_config_entity",
 *   label = @Translation("Example config entity"),
 *   handlers = {
 *     "list_builder" = "Drupal\ctools_wizard_test\ExampleConfigEntityListBuilder",
 *     "form" = {
 *       "delete" = "Drupal\ctools_wizard_test\Form\ExampleConfigEntityDeleteForm"
 *     },
 *     "wizard" = {
 *       "add" = "Drupal\ctools_wizard_test\Wizard\EntityAddWizardTest",
 *       "edit" = "Drupal\ctools_wizard_test\Wizard\EntityEditWizardTest"
 *     }
 *   },
 *   config_prefix = "ctools_wizard_test_config_entity",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/ctools_wizard_test_config_entity/{ctools_wizard_test_config_entity}",
 *     "edit-form" = "/admin/structure/ctools_wizard_test_config_entity/{machine_name}/{step}",
 *     "delete-form" = "/admin/structure/ctools_wizard_test_config_entity/{ctools_wizard_test_config_entity}/delete",
 *     "collection" = "/admin/structure/ctools_wizard_test_config_entity"
 *   }
 * )
 */
class ExampleConfigEntity extends ConfigEntityBase implements ExampleConfigEntityInterface {

  /**
   * The Example config entity ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Example config entity label.
   *
   * @var string
   */
  protected $label;

  /**
   * The first piece of information.
   *
   * @var string
   */
  protected $one;

  /**
   * The second piece of information.
   *
   * @var string
   */
  protected $two;

  /**
   * @inheritDoc
   */
  public function getOne() {
    return $this->one;
  }

  /**
   * @inheritDoc
   */
  public function getTwo() {
    return $this->two;
  }

}
