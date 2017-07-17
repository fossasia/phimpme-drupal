<?php

namespace Drupal\ctools_block\Plugin\Block;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Access\AccessResult;
use Drupal\Core\Block\BlockBase;
use Drupal\Core\Cache\CacheableMetadata;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\FieldableEntityInterface;
use Drupal\Core\Field\FieldTypePluginManagerInterface;
use Drupal\Core\Field\FormatterPluginManager;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Plugin\ContextAwarePluginInterface;
use Drupal\Core\Render\Element;
use Drupal\Core\Session\AccountInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a block to a field on an entity.
 *
 * @Block(
 *   id = "entity_field",
 *   deriver = "Drupal\ctools_block\Plugin\Deriver\EntityFieldDeriver",
 * )
 */
class EntityField extends BlockBase implements ContextAwarePluginInterface, ContainerFactoryPluginInterface {

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The entity field manager.
   *
   * @var \Drupal\Core\Entity\EntityFieldManagerInterface
   */
  protected $entityFieldManager;

  /**
   * The field type manager.
   *
   * @var \Drupal\Core\Field\FieldTypePluginManagerInterface
   */
  protected $fieldTypeManager;

  /**
   * The formatter manager.
   *
   * @var \Drupal\Core\Field\FormatterPluginManager
   */
  protected $formatterManager;

  /**
   * The entity type id.
   *
   * @var string
   */
  protected $entityTypeId;

  /**
   * The field name.
   *
   * @var string
   */
  protected $fieldName;

  /**
   * The field definition.
   *
   * @var \Drupal\Core\Field\FieldDefinitionInterface
   */
  protected $fieldDefinition;

  /**
   * The field storage definition.
   *
   * @var \Drupal\Core\Field\FieldStorageDefinitionInterface
   */
  protected $fieldStorageDefinition;

  /**
   * Constructs a new EntityField.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Drupal\Core\Entity\EntityFieldManagerInterface $entity_field_manager
   *   The entity field manager.
   * @param \Drupal\Core\Field\FormatterPluginManager $formatter_manager
   *   The formatter manager.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityTypeManagerInterface $entity_type_manager, EntityFieldManagerInterface $entity_field_manager, FieldTypePluginManagerInterface $field_type_manager, FormatterPluginManager $formatter_manager) {
    $this->entityTypeManager = $entity_type_manager;
    $this->entityFieldManager = $entity_field_manager;
    $this->fieldTypeManager = $field_type_manager;
    $this->formatterManager = $formatter_manager;

    // Get the entity type and field name from the plugin id.
    list (, $entity_type_id, $field_name) = explode(':', $plugin_id);
    $this->entityTypeId = $entity_type_id;
    $this->fieldName = $field_name;

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
      $container->get('entity_type.manager'),
      $container->get('entity_field.manager'),
      $container->get('plugin.manager.field.field_type'),
      $container->get('plugin.manager.field.formatter')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function build() {
    /** @var \Drupal\Core\Entity\FieldableEntityInterface $entity */
    $entity = $this->getContextValue('entity');
    $build = [];
    /** @var \Drupal\Core\Field\FieldItemListInterface $field */
    $field = $entity->{$this->fieldName};
    $display_settings = $this->getConfiguration()['formatter'];
    $build['field'] = $field->view($display_settings);

    // Set the cache data appropriately.
    $build['#cache']['contexts'] = $this->getCacheContexts();
    $build['#cache']['tags'] = $this->getCacheTags();
    $build['#cache']['max-age'] = $this->getCacheMaxAge();

    return $build;
  }

  /**
   * {@inheritdoc}
   */
  protected function blockAccess(AccountInterface $account) {
    /** @var \Drupal\Core\Entity\EntityInterface $entity */
    $entity = $this->getContextValue('entity');
    // Make sure we have access to the entity.
    $access = $entity->access('view', $account, TRUE);
    if ($access->isAllowed()) {
      // Check that the entity in question has this field.
      if ($entity instanceof FieldableEntityInterface && $entity->hasField($this->fieldName)) {
        // Check field access.
        $field_access = $this->entityTypeManager
          ->getAccessControlHandler($this->entityTypeId)
          ->fieldAccess('view', $this->getFieldDefinition(), $account);

        if ($field_access) {
          // Build a renderable array for the field.
          $build = $entity->get($this->fieldName)->view($this->configuration['formatter']);
          // If there are actual renderable children, grant access.
          if (Element::children($build)) {
            return AccessResult::allowed();
          }
        }
      }
      // Entity doesn't have this field, so access is denied.
      return AccessResult::forbidden();
    }
    // If we don't have access to the entity, return the forbidden result.
    return $access;
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    $field_type_definition = $this->getFieldTypeDefinition();
    return [
      'formatter' => [
        'label' => 'above',
        'type' => $field_type_definition['default_formatter'] ?: '',
        'settings' => [],
        'third_party_settings' => [],
        'weight' => 0,
      ],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $config = $this->getConfiguration();

    $form['formatter_label'] = [
      '#type' => 'select',
      '#title' => $this->t('Label'),
      '#options' => [
        'above' => $this->t('Above'),
        'inline' => $this->t('Inline'),
        'hidden' => '- ' . $this->t('Hidden') . ' -',
        'visually_hidden' => '- ' . $this->t('Visually Hidden') . ' -',
      ],
      '#default_value' => $config['formatter']['label'],
    ];

    $form['formatter_type'] = [
      '#type' => 'select',
      '#title' => $this->t('Formatter'),
      '#options' => $this->getFormatterOptions(),
      '#default_value' => $config['formatter']['type'],
      '#ajax' => [
        'callback' => [static::class, 'formatterSettingsAjaxCallback'],
        'wrapper' => 'formatter-settings-wrapper',
        'effect' => 'fade',
      ],
    ];

    // Add the formatter settings to the form via AJAX.
    $form['#process'][] = [$this, 'formatterSettingsProcessCallback'];
    $form['formatter_settings_wrapper'] = [
      '#prefix' => '<div id="formatter-settings-wrapper">',
      '#suffix' => '</div>',
    ];
    $form['formatter_settings_wrapper']['formatter_settings'] = [
      '#tree' => TRUE,
      // The settings from the formatter plugin will be added in the
      // ::formatterSettingsProcessCallback method.
    ];

    return $form;
  }

  /**
   * Render API callback: builds the formatter settings elements.
   */
  public function formatterSettingsProcessCallback(array &$element, FormStateInterface $form_state, array &$complete_form) {
    $config = $this->getConfiguration();
    $parents_base = $element['#parents'];
    $formatter_parent = array_merge($parents_base, ['formatter_type']);
    $formatter_settings_parent = array_merge($parents_base, ['formatter_settings']);

    $settings_element = &$element['formatter_settings_wrapper']['formatter_settings'];

    // Set the #parents on the formatter_settings so they end up as a peer to
    // formatter_type.
    $settings_element['#parents'] = $formatter_settings_parent;

    // Get the formatter name in a way that works regardless of whether we're
    // getting the value via AJAX or not.
    $formatter_name = NestedArray::getValue($form_state->getUserInput(), $formatter_parent) ?: $element['formatter_type']['#default_value'];

    // Place the formatter settings on the form if a formatter is selected.
    $formatter = $this->getFormatter($formatter_name, $form_state->getValue('formatter_label'), $form_state->getValue($formatter_settings_parent, $config['formatter']['settings']), $config['formatter']['third_party_settings']);
    $settings_element = array_merge($formatter->settingsForm($settings_element, $form_state), $settings_element);

    // Store the array parents for our element so that we can use it to pull out
    // the formatter settings in our AJAX callback.
    $complete_form['#formatter_array_parents'] = $element['#array_parents'];

    return $element;
  }

  /**
   * Render API callback: gets the layout settings elements.
   */
  public static function formatterSettingsAjaxCallback(array $form, FormStateInterface $form_state) {
    $formatter_array_parents = $form['#formatter_array_parents'];
    return NestedArray::getValue($form, array_merge($formatter_array_parents, ['formatter_settings_wrapper']));
  }

  /**
   * {@inheritdoc}
   */
  public function blockSubmit($form, FormStateInterface $form_state) {
    $this->configuration['formatter']['label'] = $form_state->getValue('formatter_label');
    $this->configuration['formatter']['type'] = $form_state->getValue('formatter_type');
    // @todo Remove this manual cast after https://www.drupal.org/node/2635236
    //   is resolved.
    $this->configuration['formatter']['settings'] = (array) $form_state->getValue('formatter_settings');
  }

  /**
   * Gets the field definition.
   *
   * @return \Drupal\Core\Field\FieldDefinitionInterface
   */
  protected function getFieldDefinition() {
    if (empty($this->fieldDefinition)) {
      $field_map = $this->entityFieldManager->getFieldMap();
      $bundle = reset($field_map[$this->entityTypeId][$this->fieldName]['bundles']);
      $field_definitions = $this->entityFieldManager->getFieldDefinitions($this->entityTypeId, $bundle);
      $this->fieldDefinition = $field_definitions[$this->fieldName];
    }
    return $this->fieldDefinition;
  }

  /**
   * Gets the field storage definition.
   *
   * @return \Drupal\Core\Field\FieldStorageDefinitionInterface
   */
  protected function getFieldStorageDefinition() {
    if (empty($this->fieldStorageDefinition)) {
      $field_definitions = $this->entityFieldManager->getFieldStorageDefinitions($this->entityTypeId);
      $this->fieldStorageDefinition = $field_definitions[$this->fieldName];
    }
    return $this->fieldStorageDefinition;
  }

  /**
   * Gets field type definition.
   *
   * @return array
   *   The field type definition.
   */
  protected function getFieldTypeDefinition() {
    return $this->fieldTypeManager->getDefinition($this->getFieldStorageDefinition()->getType());
  }

  /**
   * Gets the formatter options for this field type.
   *
   * @return array
   *   The formatter options.
   */
  protected function getFormatterOptions() {
    return $this->formatterManager->getOptions($this->getFieldStorageDefinition()->getType());
  }

  /**
   * Gets the formatter object.
   *
   * @param string $type
   *   The formatter name.
   * @param string $label
   *   The label option for the formatter.
   * @param array $settings
   *   The formatter settings.
   * @param array $third_party_settings
   *   The formatter third party settings.
   *
   * @return \Drupal\Core\Field\FormatterInterface
   *   The formatter object.
   */
  protected function getFormatter($type, $label, array $settings, array $third_party_settings) {
     return $this->formatterManager->createInstance($type, [
      'field_definition' => $this->getFieldDefinition(),
      'view_mode' => 'default',
      'prepare' => TRUE,
      'label' => $label,
      'settings' => $settings,
      'third_party_settings' => $third_party_settings,
    ]);
  }

  public function __wakeup() {
    parent::__wakeup();
    // @todo figure out why this happens.
    // prevent $fieldStorageDefinition being erroneously set to $this.
    $this->fieldStorageDefinition = NULL;
  }


}
