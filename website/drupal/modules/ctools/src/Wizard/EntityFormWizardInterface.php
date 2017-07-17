<?php

namespace Drupal\ctools\Wizard;

/**
 * Form wizard interface for use with entities.
 */
interface EntityFormWizardInterface extends FormWizardInterface {

  /**
   * The fieldset #title for your label & machine name elements.
   *
   * @return string
   */
  public function getWizardLabel();

  /**
   * The form element #title for your unique identifier label.
   *
   * @return string
   */
  public function getMachineLabel();

  /**
   * The machine name of the entity type.
   *
   * @return string
   */
  public function getEntityType();

  /**
   * A method for determining if this entity already exists.
   *
   * @return callable
   *   The callable to pass the id to via typical machine_name form element.
   */
  public function exists();

}
