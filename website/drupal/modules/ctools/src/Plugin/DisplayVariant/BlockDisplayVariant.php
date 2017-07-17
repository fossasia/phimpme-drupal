<?php

namespace Drupal\ctools\Plugin\DisplayVariant;

use Drupal\Component\Uuid\UuidInterface;
use Drupal\Core\Block\BlockManager;
use Drupal\Core\Cache\RefinableCacheableDependencyInterface;
use Drupal\Core\Condition\ConditionManager;
use Drupal\Core\Display\VariantBase;
use Drupal\Core\Display\ContextAwareVariantInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Plugin\Context\ContextHandlerInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Utility\Token;
use Drupal\ctools\Form\AjaxFormTrait;
use Drupal\ctools\Plugin\BlockVariantInterface;
use Drupal\ctools\Plugin\BlockVariantTrait;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a base class for a display variant that simply contains blocks.
 */
abstract class BlockDisplayVariant extends VariantBase implements ContextAwareVariantInterface, ContainerFactoryPluginInterface, BlockVariantInterface, RefinableCacheableDependencyInterface {

  use AjaxFormTrait;
  use BlockVariantTrait;

  /**
   * The context handler.
   *
   * @var \Drupal\Core\Plugin\Context\ContextHandlerInterface
   */
  protected $contextHandler;

  /**
   * The UUID generator.
   *
   * @var \Drupal\Component\Uuid\UuidInterface
   */
  protected $uuidGenerator;

  /**
   * The current user.
   *
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected $account;

  /**
   * The token service.
   *
   * @var \Drupal\Core\Utility\Token
   */
  protected $token;

  /**
   * An array of collected contexts.
   *
   * This is only used on runtime, and is not stored.
   *
   * @var \Drupal\Component\Plugin\Context\ContextInterface[]
   */
  protected $contexts = [];

  /**
   * Constructs a new BlockDisplayVariant.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Plugin\Context\ContextHandlerInterface $context_handler
   *   The context handler.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The current user.
   * @param \Drupal\Component\Uuid\UuidInterface $uuid_generator
   *   The UUID generator.
   * @param \Drupal\Core\Utility\Token $token
   *   The token service.
   * @param \Drupal\Core\Block\BlockManager $block_manager
   *   The block manager.
   * @param \Drupal\Core\Condition\ConditionManager $condition_manager
   *   The condition manager.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ContextHandlerInterface $context_handler, AccountInterface $account, UuidInterface $uuid_generator, Token $token, BlockManager $block_manager, ConditionManager $condition_manager) {
    // Inject dependencies as early as possible, so they can be used in
    // configuration.
    $this->contextHandler = $context_handler;
    $this->account = $account;
    $this->uuidGenerator = $uuid_generator;
    $this->token = $token;
    $this->blockManager = $block_manager;
    $this->conditionManager = $condition_manager;

    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('context.handler'),
      $container->get('current_user'),
      $container->get('uuid'),
      $container->get('token'),
      $container->get('plugin.manager.block'),
      $container->get('plugin.manager.condition')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return parent::defaultConfiguration() + [
      'blocks' => []
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    foreach ($this->getBlockCollection() as $instance) {
      $this->calculatePluginDependencies($instance);
    }
    return $this->dependencies;
  }

  /**
   * {@inheritdoc}
   */
  public function getConfiguration() {
    return [
      'blocks' => $this->getBlockCollection()->getConfiguration(),
    ] + parent::getConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function setConfiguration(array $configuration) {
    // preserve the uuid.
    if ($this->configuration && !empty($this->configuration['uuid'])) {
      $configuration['uuid'] = $this->configuration['uuid'];
    }
    parent::setConfiguration($configuration);
    $this->getBlockCollection()->setConfiguration($this->configuration['blocks']);
    return $this;
  }

  /**
   * Gets the contexts.
   *
   * @return \Drupal\Component\Plugin\Context\ContextInterface[]
   *   An array of set contexts, keyed by context name.
   */
  public function getContexts() {
    return $this->contexts;
  }

  /**
   * Sets the contexts.
   *
   * @param \Drupal\Component\Plugin\Context\ContextInterface[] $contexts
   *   An array of contexts, keyed by context name.
   *
   * @return $this
   */
  public function setContexts(array $contexts) {
    $this->contexts = $contexts;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  protected function contextHandler() {
    return $this->contextHandler;
  }

  /**
   * {@inheritdoc}
   */
  protected function getBlockConfig() {
    return $this->configuration['blocks'];
  }

  /**
   * {@inheritdoc}
   */
  protected function uuidGenerator() {
    return $this->uuidGenerator;
  }

  /**
   * {@inheritdoc}
   */
  public function __sleep() {
    $vars = parent::__sleep();

    // Gathered contexts objects should not be serialized.
    if (($key = array_search('contexts', $vars)) !== FALSE) {
      unset($vars[$key]);
    }

    // The block plugin collection should also not be serialized, ensure that
    // configuration is synced back.
    if (($key = array_search('blockPluginCollection', $vars)) !== FALSE) {
      if ($this->blockPluginCollection) {
        $this->configuration['blocks'] = $this->blockPluginCollection->getConfiguration();
      }
      unset($vars[$key]);
    }

    return $vars;
  }

}
