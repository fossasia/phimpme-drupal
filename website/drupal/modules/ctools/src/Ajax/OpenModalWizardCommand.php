<?php

namespace Drupal\ctools\Ajax;

use Drupal\Core\Ajax\OpenModalDialogCommand;

class OpenModalWizardCommand extends OpenModalDialogCommand {

  public function __construct($object, $tempstore_id, array $parameters = array(), array $dialog_options = array(), $settings = NULL) {
    // Instantiate the wizard class properly.
    $parameters += [
      'tempstore_id' => $tempstore_id,
      'machine_name' => NULL,
      'step' => NULL,
    ];
    $form = \Drupal::service('ctools.wizard.factory')->getWizardForm($object, $parameters, TRUE);
    $title = isset($form['#title']) ? $form['#title'] : '';
    $content = $form;

    parent::__construct($title, $content, $dialog_options, $settings);
  }

}
