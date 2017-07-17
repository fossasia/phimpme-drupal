<?php

namespace Drupal\services\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\services\ServiceResourceOptionsTrait;

/**
 * Class \Drupal\services\Form\ServiceSettingsForm.
 */
class ServiceSettingsForm extends ConfigFormBase {
  use ServiceResourceOptionsTrait;

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'services_admin_settings';
  }

  /**
   * {@inheritdoc}
   */
  public function getEditableConfigNames() {
    return [$this->configName()];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->getConfig();

    $form['default_formats'] = [
      '#type' => 'checkboxes',
      '#title' => $this->t('Default formats'),
      '#description' => $this->t('Check all HTTP response formats you want
        enabled by default.'),
      '#options' => $this->getFormatOptions(),
      '#default_value' => $config->get('default_formats'),
    ];
    $form['default_authentication'] = [
      '#type' => 'checkboxes',
      '#title' => $this->t('Default authentication'),
      '#description' => $this->t('Check all authentication providers you want
        enabled by default'),
      '#options' => $this->getAuthOptions(),
      '#default_value' => $config->get('default_authentication'),
    ];
    $form['default_no_cache'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Disable cache'),
      '#description' => $this->t('Do not cache the response of the resources by default.'),
      '#default_value' => $config->get('default_no_cache')
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->getConfig()
      ->setData($form_state->cleanValues()->getValues())
      ->save();

    parent::submitForm($form, $form_state);
  }

  /**
   * Define the configuration name.
   *
   * @return string
   *   A string to use as the config name.
   */
  protected function configName() {
    return 'services.settings';
  }

  /**
   * Get service setting config object.
   *
   * @return \Drupal\Core\Config\Config
   *   An instantiated configuration object.
   */
  protected function getConfig() {
    return $this->config($this->configName());
  }

}
