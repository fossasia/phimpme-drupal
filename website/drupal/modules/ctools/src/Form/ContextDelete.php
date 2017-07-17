<?php

namespace Drupal\ctools\Form;


use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\ConfirmFormBase;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a form for deleting an contexts and relationships.
 */
abstract class ContextDelete extends ConfirmFormBase {

  /**
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * @var string
   */
  protected $tempstore_id;

  /**
   * @var string
   */
  protected $machine_name;

  /**
   * The static context's machine name.
   *
   * @var array
   */
  protected $context_id;

  public static function create(ContainerInterface $container) {
    return new static($container->get('user.shared_tempstore'));
  }

  public function __construct(SharedTempStoreFactory $tempstore) {
    $this->tempstore = $tempstore;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_context_delete_form';
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $tempstore_id = NULL, $machine_name = NULL, $context_id = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $this->context_id = $context_id;
    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $form_state->setRedirectUrl($this->getCancelUrl());
  }

  protected function getTempstore() {
    return $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
  }

  protected function setTempstore($cached_values) {
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
  }

}
