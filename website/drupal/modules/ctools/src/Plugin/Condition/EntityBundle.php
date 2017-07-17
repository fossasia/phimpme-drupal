<?php

namespace Drupal\ctools\Plugin\Condition;

use Drupal\Core\Condition\ConditionPluginBase;
use Drupal\Core\Entity\EntityTypeBundleInfoInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\ctools\ConstraintConditionInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a 'Entity Bundle' condition.
 *
 * @Condition(
 *   id = "entity_bundle",
 *   deriver = "\Drupal\ctools\Plugin\Deriver\EntityBundle"
 * )
 *
 */
class EntityBundle extends ConditionPluginBase implements ConstraintConditionInterface, ContainerFactoryPluginInterface {

  /**
   * The entity type bundle info service.
   *
   * @var \Drupal\Core\Entity\EntityTypeBundleInfoInterface
   */
  protected $entityTypeBundleInfo;

  /**
   * @var \Drupal\Core\Entity\EntityTypeInterface|null
   */
  protected $bundleOf;

  /**
   * Creates a new EntityBundle instance.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Drupal\Core\Entity\EntityTypeBundleInfoInterface $entity_type_bundle_info
   *   The entity type bundle info service.
   * @param array $configuration
   *   The plugin configuration, i.e. an array with configuration values keyed
   *   by configuration option name. The special key 'context' may be used to
   *   initialize the defined contexts by setting it to an array of context
   *   values keyed by context names.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, EntityTypeBundleInfoInterface $entity_type_bundle_info, array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->entityTypeBundleInfo = $entity_type_bundle_info;
    $this->bundleOf = $entity_type_manager->getDefinition($this->getDerivativeId());
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $container->get('entity_type.manager'),
      $container->get('entity_type.bundle.info'),
      $configuration,
      $plugin_id,
      $plugin_definition
    );
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $options = array();
    $bundles = $this->entityTypeBundleInfo->getBundleInfo($this->bundleOf->id());
    foreach ($bundles as $id => $info) {
      $options[$id] = $info['label'];
    }
    $form['bundles'] = array(
      '#title' => $this->pluginDefinition['label'],
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $this->configuration['bundles'],
    );
    return parent::buildConfigurationForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->configuration['bundles'] = array_filter($form_state->getValue('bundles'));
    parent::submitConfigurationForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function evaluate() {
    if (empty($this->configuration['bundles']) && !$this->isNegated()) {
      return TRUE;
    }
    /** @var \Drupal\Core\Entity\ContentEntityInterface $entity */
    $entity = $this->getContextValue($this->bundleOf->id());
    return !empty($this->configuration['bundles'][$entity->bundle()]);
  }

  /**
   * {@inheritdoc}
   */
  public function summary() {
    if (count($this->configuration['bundles']) > 1) {
      $bundles = $this->configuration['bundles'];
      $last = array_pop($bundles);
      $bundles = implode(', ', $bundles);
      return $this->t('@bundle_type is @bundles or @last', array('@bundle_type' => $this->bundleOf->getBundleLabel(), '@bundles' => $bundles, '@last' => $last));
    }
    $bundle = reset($this->configuration['bundles']);
    return $this->t('@bundle_type is @bundle', array('@bundle_type' => $this->bundleOf->getBundleLabel(), '@bundle' => $bundle));
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('bundles' => array()) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   *
   * @param \Drupal\Core\Plugin\Context\ContextInterface[] $contexts
   */
  public function applyConstraints(array $contexts = array()) {
    // Nullify any bundle constraints on contexts we care about.
    $this->removeConstraints($contexts);
    $bundle = array_values($this->configuration['bundles']);
    // There's only one expected context for this plugint type.
    foreach ($this->getContextMapping() as $definition_id => $context_id) {
      $contexts[$context_id]->getContextDefinition()->addConstraint('Bundle', ['value' => $bundle]);
    }
  }

  /**
   * {@inheritdoc}
   *
   * @param \Drupal\Core\Plugin\Context\ContextInterface[] $contexts
   */
  public function removeConstraints(array $contexts = array()) {
    // Reset the bundle constraint for any context we've mapped.
    foreach ($this->getContextMapping() as $definition_id => $context_id) {
      $constraints = $contexts[$context_id]->getContextDefinition()->getConstraints();
      unset($constraints['Bundle']);
      $contexts[$context_id]->getContextDefinition()->setConstraints($constraints);
    }
  }

}
