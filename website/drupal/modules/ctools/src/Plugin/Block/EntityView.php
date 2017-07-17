<?php

namespace Drupal\ctools\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Cache\CacheableMetadata;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Plugin\ContextAwarePluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a block to view a specific entity.
 *
 * @Block(
 *   id = "entity_view",
 *   deriver = "Drupal\ctools\Plugin\Deriver\EntityViewDeriver",
 * )
 */
class EntityView extends BlockBase implements ContextAwarePluginInterface, ContainerFactoryPluginInterface {

  /**
   * The entity manager.
   *
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $entityManager;

  /**
   * Constructs a new EntityView.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Entity\EntityManagerInterface $entity_manager
   *   The entity manager.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityManagerInterface $entity_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->entityManager = $entity_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity.manager')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'view_mode' => 'default',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form['view_mode'] = [
      '#type' => 'select',
      '#options' => $this->entityManager->getViewModeOptions($this->getDerivativeId()),
      '#title' => $this->t('View mode'),
      '#default_value' => $this->configuration['view_mode'],
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function blockSubmit($form, FormStateInterface $form_state) {
    $this->configuration['view_mode'] = $form_state->getValue('view_mode');
  }

  /**
   * {@inheritdoc}
   */
  public function build() {
    /** @var $entity \Drupal\Core\Entity\EntityInterface */
    $entity = $this->getContextValue('entity');

    $view_builder = $this->entityManager->getViewBuilder($entity->getEntityTypeId());
    $build = $view_builder->view($entity, $this->configuration['view_mode']);

    CacheableMetadata::createFromObject($this->getContext('entity'))
      ->applyTo($build);

    return $build;
  }

}
