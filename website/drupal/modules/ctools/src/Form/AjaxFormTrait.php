<?php

namespace Drupal\ctools\Form;

use Drupal\Component\Serialization\Json;
use Drupal\Component\Utility\NestedArray;

/**
 * Provides helper methods for using an AJAX modal.
 */
trait AjaxFormTrait {

  /**
   * Gets attributes for use with an AJAX modal.
   *
   * @return array
   */
   public static function getAjaxAttributes() {
    return [
      'class' => ['use-ajax'],
      'data-dialog-type' => 'modal',
      'data-dialog-options' => Json::encode([
        'width' => 'auto',
      ]),
    ];
  }

  /**
   * Gets attributes for use with an add button AJAX modal.
   *
   * @return array
   */
   public static function getAjaxButtonAttributes() {
    return NestedArray::mergeDeep(AjaxFormTrait::getAjaxAttributes(), [
      'class' => [
        'button',
        'button--small',
        'button-action',
      ],
    ]);
  }

}
