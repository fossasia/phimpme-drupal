<?php

/**
 * @file Contains core ui functions.
 */

/**
 * Plugin UI Interface.
 */
interface RulesPluginUIInterface {

  /**
   * Adds the whole configuration form of this rules configuration. For rule
   * elements that are part of a configuration this method just adds the
   * elements configuration form.
   *
   * @param $form
   *   The form array where to add the form.
   * @param $form_state
   *   The current form state.
   * @param $options
   *   An optional array of options with the known keys:
   *    - 'show settings': Whether to include the 'settings' fieldset for
   *      editing configuration settings like the label or categories. Defaults
   *      to FALSE.
   *    - 'button': Whether a submit button should be added. Defaults to FALSE.
   *    - 'init': Whether the element is about to be configured the first time
   *      and the configuration is about to be initialized. Defaults to FALSE.
   *    - 'restrict plugins: May be used to restrict the list of rules plugins
   *      that may be added to this configuration. For that set an array of
   *      valid plugins. Note that conditions and actions are always valid, so
   *      just set an empty array for just allowing those.
   *    - 'restrict conditions': Optionally set an array of condition names to
   *      restrict the conditions that are available for adding.
   *    - 'restrict actions': Optionally set an array of action names to
   *      restrict the actions that are available to for adding.
   *    - 'restrict events': Optionally set an array of event names to restrict
   *      the events that are available for adding.
   *
   *  @todo
   *    Implement the 'restrict *' options.
   */
  public function form(&$form, &$form_state, $options = array());

  /**
   * Validate the configuration form of this rule element.
   *
   * @param $form
   * @param $form_state
   */
  public function form_validate($form, &$form_state);

  /**
   * Submit the configuration form of this rule element. This makes sure to
   * put the updated configuration in the form state. For saving changes
   * permanently, just call $config->save() afterwards.
   *
   * @param $form
   * @param $form_state
   */
  public function form_submit($form, &$form_state);

  /**
   * Returns a structured array for rendering this element in overviews.
   */
  public function buildContent();

  /**
   * Returns the help text for editing this plugin.
   */
  public function help();

  /**
   * Returns ui operations for this element.
   */
  public function operations();

}

/**
 * Helper object for mapping elements to ids.
 */
class RulesElementMap {

  /**
   * @var RulesPlugin
   */
  protected $configuration;
  protected $index = array();
  protected $counter = 0;

  public function __construct(RulesPlugin $config) {
    $this->configuration = $config->root();
  }

  /**
   * Makes sure each element has an assigned id.
   */
  public function index() {
    foreach ($this->getUnIndexedElements($this->configuration) as $element) {
      $id = &$element->property('elementId');
      $id = ++$this->counter;
      $this->index[$id] = $element;
    }
  }

  protected function getUnIndexedElements($element, &$unindexed = array()) {
    // Remember unindexed elements.
    $id = $element->property('elementId');
    if (!isset($id)) {
      $unindexed[] = $element;
    }
    else {
      // Make sure $this->counter refers to the highest id.
      if ($id > $this->counter) {
        $this->counter = $id;
      }
      $this->index[$id] = $element;
    }
    // Recurse down the tree.
    if ($element instanceof RulesContainerPlugin) {
      foreach ($element as $child) {
        $this->getUnIndexedElements($child, $unindexed);
      }
    }
    return $unindexed;
  }

  /**
   * Looks up the element with the given id.
   */
  public function lookup($id) {
    if (!$this->index) {
      $this->index();
    }
    return isset($this->index[$id]) ? $this->index[$id] : FALSE;
  }
}

/**
 * Faces UI extender for all kind of Rules plugins. Provides various useful
 * methods for any rules UI.
 */
class RulesPluginUI extends FacesExtender implements RulesPluginUIInterface {

  /**
   * @var RulesPlugin
   */
  protected $element;

  /**
   * The base path determines where a Rules overview UI lives. All forms that
   * want to display Rules (overview) forms need to set this variable. This is
   * necessary in order to get correct operation links, paths, redirects, bread
   * crumbs etc. for the form() and overviewTable() methods.
   *
   * @see RulesUIController
   * @see rules_admin_reaction_overview()
   * @see rules_admin_components_overview()
   */
  public static $basePath = NULL;

  /**
   * Provide $this->element to make the code more meaningful.
   */
  public function __construct(FacesExtendable $object) {
    parent::__construct($object);
    $this->element = $object;
  }

  /**
   * Returns the form values for the given form, possible being only a part of the whole form.
   *
   * In case the form is embedded somewhere, this function figures out the
   * location of its form values and returns them for further use.
   *
   * @param $form
   *   A form array, or an array of form elements to get the value for.
   * @param $form_state
   *   The form state as usual.
   */
  public static function &getFormStateValues($form, &$form_state) {
    $values = NULL;
    if (isset($form_state['values'])) {
      // Assume the top level if parents are not yet set.
      $form += array('#parents' => array());
      $values = &$form_state['values'];
      foreach ($form['#parents'] as $parent) {
        $values = &$values[$parent];
      }
    }
    return $values;
  }

  /**
   * Implements RulesPluginUIInterface.
   * Generates the element edit form.
   *
   * Note: Make sure that you set RulesPluginUI::$basePath before using this
   * method, otherwise paths, links, redirects etc. won't be correct.
   */
  public function form(&$form, &$form_state, $options = array()) {
    self::formDefaults($form, $form_state);
    $form_state += array('rules_element' => $this->element);

    // Add the help to the top of the form.
    $help = $this->element->help();
    $form['help'] = is_array($help) ? $help : array('#markup' => $help);

    // We use $form_state['element_settings'] to store the settings of both
    // parameter modes. That way one can switch between the parameter modes
    // without losing the settings of those.
    $form_state += array('element_settings' => $this->element->settings);
    $settings = $this->element->settings + $form_state['element_settings'];

    $form['parameter'] = array(
      '#tree' => TRUE,
    );

    foreach ($this->element->pluginParameterInfo() as $name => $parameter) {
      if ($parameter['type'] == 'hidden') {
        continue;
      }

      $form['parameter'][$name] = array(
        '#type' => 'fieldset',
        '#title' => check_plain($parameter['label']),
        '#description' => filter_xss(isset($parameter['description']) ? $parameter['description'] : ''),
      );

      // Init the parameter input mode.
      $form_state['parameter_mode'][$name] = !isset($form_state['parameter_mode'][$name]) ? NULL : $form_state['parameter_mode'][$name];
      $form['parameter'][$name] += $this->getParameterForm($name, $parameter, $settings, $form_state['parameter_mode'][$name]);
    }

    // Provide a form for editing the label and name of provided variables.
    $settings = $this->element->settings;
    foreach ($this->element->pluginProvidesVariables() as $var_name => $var_info) {
      $form['provides'][$var_name] = array(
        '#type' => 'fieldset',
        '#title' => check_plain($var_info['label']),
        '#description' => filter_xss(isset($var_info['description']) ? $var_info['description'] : ''),
      );
      $form['provides'][$var_name]['label'] = array(
        '#type' => 'textfield',
        '#title' => t('Variable label'),
        '#default_value' => isset($settings[$var_name . ':label']) ? $settings[$var_name . ':label'] : $var_info['label'],
        '#required' => TRUE,
      );
      $form['provides'][$var_name]['var'] = array(
        '#type' => 'textfield',
        '#title' => t('Variable name'),
        '#default_value' => isset($settings[$var_name . ':var']) ? $settings[$var_name . ':var'] : $var_name,
        '#description' => t('The variable name must contain only lowercase letters, numbers, and underscores and must be unique in the current scope.'),
        '#element_validate' => array('rules_ui_element_machine_name_validate'),
        '#required' => TRUE,
      );
    }
    if (!empty($form['provides'])) {
      $help = '<div class="description">' . t('Adjust the names and labels of provided variables, but note that renaming of already utilizied variables invalidates the existing uses.') . '</div>';
      $form['provides'] += array(
        '#tree' => TRUE,
        '#prefix' => '<h4 class="rules-form-heading">' . t('Provided variables') . '</h4>' . $help,
      );
    }

    // Add settings form, if specified.
    if (!empty($options['show settings'])) {
      $this->settingsForm($form, $form_state);
    }
    // Add submit button, if specified.
    if (!empty($options['button'])) {
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#weight' => 10,
      );
    }
  }

  /**
   * Actually generates the parameter form for the given data type.
   */
  protected function getParameterForm($name, $info, $settings, &$mode) {
    $class = $this->getDataTypeClass($info['type'], $info);
    $supports_input_mode = in_array('RulesDataDirectInputFormInterface', class_implements($class));

    // Init the mode.
    if (!isset($mode)) {
      if (isset($settings[$name . ':select'])) {
        $mode = 'selector';
      }
      elseif (isset($settings[$name]) && $supports_input_mode) {
        $mode = 'input';
      }
      elseif (isset($info['restriction'])) {
        $mode = $info['restriction'];
      }
      else {
        // Allow the parameter to define the 'default mode' and fallback to the
        // data type default.
        $mode = !empty($info['default mode']) ? $info['default mode'] : call_user_func(array($class, 'getDefaultMode'));
      }
    }

    // For translatable parameters, pre-populate an internal translation source
    // key so data type forms or input evaluators (i18n) may produce suiting
    // help.
    if (drupal_multilingual() && !empty($info['translatable'])) {
      $parameter = $this->element->pluginParameterInfo();
      $info['custom translation language'] = !empty($parameter['language']);
    }

    // Add the parameter form.
    if ($mode == 'input' && $supports_input_mode) {
      $form['settings'] = call_user_func(array($class, 'inputForm'), $name, $info, $settings, $this->element);
    }
    else {
      $form['settings'] = call_user_func(array($class, 'selectionForm'), $name, $info, $settings, $this->element);
    }

    // Add a link for switching the input mode when JS is enabled and a button
    // to switch it without javascript, in case switching is possible.
    if ($supports_input_mode && empty($info['restriction'])) {
      $value = $mode == 'selector' ? t('Switch to the direct input mode') : t('Switch to data selection');

      $form['switch_button'] = array(
        '#type' => 'submit',
        '#name' => 'param_' . $name,
        '#attributes' => array('class' => array('rules-switch-button')),
        '#parameter' => $name,
        '#value' => $value,
        '#submit' => array('rules_ui_parameter_replace_submit'),
        '#ajax' => rules_ui_form_default_ajax('none'),
        // Do not validate!
        '#limit_validation_errors' => array(),
      );
    }
    return $form;
  }

  /**
   * Implements RulesPluginUIInterface.
   */
  public function form_validate($form, &$form_state) {
    $this->form_extract_values($form, $form_state);
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);

    if (isset($form_values['provides'])) {
      $vars = $this->element->availableVariables();
      foreach ($form_values['provides'] as $name => $values) {
        if (isset($vars[$values['var']])) {
          form_error($form['provides'][$name]['var'], t('The variable name %name is already taken.', array('%name' => $values['var'])));
        }
      }
    }
    // Settings have been updated, so process them now.
    $this->element->processSettings(TRUE);

    // Make sure the current user really has access to configure this element
    // as well as the used input evaluators and data processors.
    if (!user_access('bypass rules access') && !$this->element->root()->access()) {
      form_set_error('', t('Access violation! You have insufficient access permissions to edit this configuration.'));
    }
    if (!empty($form['settings'])) {
      $this->settingsFormValidate($form, $form_state);
    }
  }

  /**
   * Applies the values of the form to the element.
   */
  public function form_extract_values($form, &$form_state) {
    $this->element->settings = array();
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);
    if (isset($form_values['parameter'])) {
      foreach ($form_values['parameter'] as $name => $values) {
        $this->element->settings += $values['settings'];
      }
    }
    if (isset($form_values['provides'])) {
      foreach ($form_values['provides'] as $name => $values) {
        $this->element->settings[$name . ':label'] = $values['label'];
        $this->element->settings[$name . ':var'] = $values['var'];
      }
    }
    if (!empty($form['settings'])) {
      $this->settingsFormExtractValues($form, $form_state);
    }
  }

  /**
   * Implements RulesPluginUIInterface.
   */
  public function form_submit($form, &$form_state) {
    if (!empty($form['settings'])) {
      $this->settingsFormSubmit($form, $form_state);
    }
    $this->element->save();
  }

  /**
   * Adds the configuration settings form (label, tags, description, ..).
   */
  public function settingsForm(&$form, &$form_state) {
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);
    // Add the settings in a separate fieldset below.
    $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => empty($form_values['settings']['vars']['more']),
      '#weight' => 5,
      '#tree' => TRUE,
    );
    $form['settings']['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $this->element->label(),
      '#required' => TRUE,
      '#weight' => -5,
    );
    if (!empty($this->element->module) && !empty($this->element->name) && $this->element->module == 'rules' && strpos($this->element->name, 'rules_') === 0) {
      // Remove the Rules module prefix from the machine name.
      $machine_name = substr($this->element->name, strlen($this->element->module) + 1);
    }
    else {
      $machine_name = $this->element->name;
    }
    $form['settings']['name'] = array(
      '#type' => 'machine_name',
      '#default_value' => isset($machine_name) ? $machine_name : '',
      '#maxlength' => 64,
      '#disabled' => entity_has_status('rules_config', $this->element, ENTITY_IN_CODE) && !(isset($form_state['op']) && $form_state['op'] == 'clone'),
      '#machine_name' => array(
        'exists' => 'rules_config_load',
        'source' => array('settings', 'label'),
      ),
      '#required' => TRUE,
      '#description' => t('The machine-readable name of this configuration is used by rules internally to identify the configuration. This name must contain only lowercase letters, numbers, and underscores and must be unique.'),
    );
    $form['settings']['tags'] = array(
      '#type' => 'textfield',
      '#title' => t('Tags'),
      '#default_value' => isset($this->element->tags) ? drupal_implode_tags($this->element->tags) : '',
      '#autocomplete_path' => 'admin/config/workflow/rules/autocomplete_tags',
      '#description' => t('Tags associated with this configuration, used for filtering in the admin interface. Separate multiple tags with commas.'),
    );

    // Show a form for editing variables for components.
    if (($plugin_info = $this->element->pluginInfo()) && !empty($plugin_info['component'])) {
      if ($this->element->hasStatus(ENTITY_IN_CODE)) {
        $description = t('The variables used by the component. They can not be edited for configurations that are provided in code.');
      }
      else {
        $description = t('Variables are normally input <em>parameters</em> for the component – data that should be available for the component to act on. Additionaly, action components may <em>provide</em> variables back to the caller. Each variable must have a specified data type, a label and a unique machine readable name containing only lowercase alphanumeric characters and underscores. See <a href="@url">the online documentation</a> for more information about variables.',
          array('@url' => rules_external_help('variables'))
        );
      }
      $form['settings']['vars'] = array(
        '#prefix' => '<div id="rules-component-variables">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
        '#element_validate' => array('rules_ui_element_variable_form_validate'),
        '#theme' => 'rules_ui_variable_form',
        '#title' => t('Variables'),
        '#description' => $description,
        // Variables can not be edited on configurations in code.
        '#disabled' => $this->element->hasStatus(ENTITY_IN_CODE),
      );

      $weight = 0;
      $provides = $this->element->providesVariables();
      foreach ($this->element->componentVariables() as $name => $var_info) {
        $form['settings']['vars']['items'][$name] = RulesPluginUI::getVariableForm($name, $var_info, isset($provides[$name]));
        $form['settings']['vars']['items'][$name]['weight']['#default_value'] = $weight++;
      }
      // Always add three empty forms.
      for ($i = 0; $i < 3; $i++) {
        $form['settings']['vars']['items'][$i] = RulesPluginUI::getVariableForm();
        $form['settings']['vars']['items'][$i]['weight']['#default_value'] = $weight++;
      }
      $form['settings']['vars']['more'] = array(
        '#type' => 'submit',
        '#value' => t('Add more'),
        // Enable AJAX once #756762 is fixed.
        // '#ajax' => rules_ui_form_default_ajax('none'),
        '#limit_validation_errors' => array(array('vars')),
        '#submit' => array('rules_form_submit_rebuild'),
      );
      if (!empty($this->element->id)) {
        // Display a setting to manage access.
        $form['settings']['access'] = array(
          '#weight' => 50,
        );
        $plugin_type = $this->element instanceof RulesActionInterface ? t('action') : t('condition');
        $form['settings']['access']['access_exposed'] = array(
          '#type' => 'checkbox',
          '#title' => t('Configure access for using this component with a permission.'),
          '#default_value' => !empty($this->element->access_exposed),
          '#description' => t('By default, the @plugin-type for using this component may be only used by users that have access to configure the component. If checked, access is determined by a permission instead.', array('@plugin-type' => $plugin_type))
        );
        $form['settings']['access']['permissions'] = array(
          '#type' => 'container',
          '#states' => array(
            'visible' => array(
              ':input[name="settings[access][access_exposed]"]' => array('checked' => TRUE),
            ),
          ),
        );
        $form['settings']['access']['permissions']['matrix'] = $this->settingsFormPermissionMatrix();
      }
    }

    // TODO: Attach field form thus description.
  }

  /**
   * Provides a matrix permission for the component based in the existing roles.
   *
   * @return
   *   Form elements with the matrix of permissions for a component.
   */
  protected function settingsFormPermissionMatrix() {
    $form['#theme'] = 'user_admin_permissions';
    $status = array();
    $options = array();

    $role_names = user_roles();
    $role_permissions = user_role_permissions($role_names);
    $component_permission = rules_permissions_by_component(array($this->element));
    $component_permission_name = key($component_permission);

    $form['permission'][$component_permission_name] = array(
      '#type' => 'item',
      '#markup' => $component_permission[$component_permission_name]['title'],
    );
    $options[$component_permission_name] = '';
    foreach ($role_names as $rid => $name) {
      if (isset($role_permissions[$rid][$component_permission_name])) {
        $status[$rid][] = $component_permission_name;
      }
    }

    // Build the checkboxes for each role.
    foreach ($role_names as $rid => $name) {
      $form['checkboxes'][$rid] = array(
        '#type' => 'checkboxes',
        '#options' => $options,
        '#default_value' => isset($status[$rid]) ? $status[$rid] : array(),
        '#attributes' => array('class' => array('rid-' . $rid)),
      );
      $form['role_names'][$rid] = array('#markup' => check_plain($name), '#tree' => TRUE);
    }

    // Attach the default permissions page JavaScript.
    $form['#attached']['js'][] = drupal_get_path('module', 'user') . '/user.permissions.js';

    return $form;
  }

  public function settingsFormExtractValues($form, &$form_state) {
    $form_values = RulesPluginUI::getFormStateValues($form['settings'], $form_state);
    $this->element->label = $form_values['label'];
    // If the name was changed we have to redirect to the URL that contains
    // the new name, instead of rebuilding on the old URL with the old name
    if ($form['settings']['name']['#default_value'] != $form_values['name']) {
      $module = isset($this->element->module) ? $this->element->module : 'rules';
      $this->element->name = $module . '_' . $form_values['name'];
      $form_state['redirect'] = RulesPluginUI::path($this->element->name);
    }
    $this->element->tags = empty($form_values['tags']) ? array() : drupal_explode_tags($form_values['tags']);

    if (isset($form_values['vars']['items'])) {
      $vars = &$this->element->componentVariables();
      $vars = array();
      if ($this->element instanceof RulesActionContainer) {
        $provides = &$this->element->componentProvidesVariables();
        $provides = array();
      }

      usort($form_values['vars']['items'], 'rules_element_sort_helper');
      foreach ($form_values['vars']['items'] as $item) {
        if ($item['type'] && $item['name'] && $item['label']) {
          $vars[$item['name']] = array('label' => $item['label'], 'type' => $item['type']);
          if (!$item['usage'][0]) {
            $vars[$item['name']]['parameter'] = FALSE;
          }
          if ($item['usage'][1] && isset($provides)) {
            $provides[] = $item['name'];
          }
        }
      }
      // Disable FAPI persistence for the variable form so renumbering works.
      $input = &$form_state['input'];
      foreach ($form['settings']['#parents'] as $parent) {
        $input = &$input[$parent];
      }
      unset($input['vars']);
    }
    $this->element->access_exposed = isset($form_values['access']['access_exposed']) ? $form_values['access']['access_exposed'] : FALSE;
  }

  public function settingsFormValidate($form, &$form_state) {
    $form_values = RulesPluginUI::getFormStateValues($form['settings'], $form_state);
    if ($form['settings']['name']['#default_value'] != $form_values['name'] && rules_config_load($this->element->name)) {
      form_error($form['settings']['name'], t('The machine-readable name %name is already taken.', array('%name' => $form_values['name'])));
    }
  }

  public function settingsFormSubmit($form, &$form_state) {
    if (isset($form_state['values']['settings']['access']) && !empty($this->element->access_exposed)) {
      // Save the permission matrix.
      foreach ($form_state['values']['settings']['access']['permissions']['matrix']['checkboxes'] as $rid => $value) {
        user_role_change_permissions($rid, $value);
      }
    }
  }

  /**
   * Returns the form for configuring the info of a single variable.
   */
  public function getVariableForm($name = '', $info = array(), $provided = FALSE) {
    $form['type'] = array(
      '#type' => 'select',
      '#options' => array(0 => '--') + RulesPluginUI::getOptions('data'),
      '#default_value' => isset($info['type']) ? $info['type'] : 0,
    );
    $form['label'] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => isset($info['label']) ? $info['label'] : '',
    );
    $form['name'] = array(
      '#type' => 'textfield',
      '#size' => 40,
      '#default_value' => $name,
      '#element_validate' => array('rules_ui_element_machine_name_validate'),
    );

    $usage[0] = !isset($info['parameter']) || $info['parameter'] ? 1 : 0;
    $usage[1] = $provided ? 1 : 0;

    $form['usage'] = array(
      '#type' => 'select',
      '#default_value' => implode('', $usage),
      '#options' => array(
        '10' => t('Parameter'),
        '11' => t('Parameter + Provided'),
        '01' => t('Provided'),
      ),
    );
    if ($this->element instanceof RulesConditionContainer) {
      $form['usage']['#disabled'] = TRUE;
    }

    // Just set the weight #default_value for the returned form.
    $form['weight'] = array(
      '#type' => 'weight',
    );
    return $form;
  }

  /**
   * Returns the name of class for the given data type.
   *
   * @param $data_type
   *   The name of the data typ
   * @param $parameter_info
   *   (optional) An array of info about the to be configured parameter.
   */
  public function getDataTypeClass($data_type, $parameter_info = array()) {
    if (!empty($parameter_info['ui class'])) {
      return $parameter_info['ui class'];
    }
    $cache = rules_get_cache();
    $data_info = $cache['data_info'];
    return (is_string($data_type) && isset($data_info[$data_type]['ui class'])) ? $data_info[$data_type]['ui class'] : 'RulesDataUI';
  }

  /**
   * Implements RulesPluginUIInterface.
   * Show a preview of the configuration settings.
   */
  public function buildContent() {
    $config_name = $this->element->root()->name;
    $content['label'] = array(
      '#type' => 'link',
      '#title' => $this->element->label(),
      '#href' => $this->element->isRoot() ? RulesPluginUI::path($config_name) : RulesPluginUI::path($config_name, 'edit', $this->element),
      '#prefix' => '<div class="rules-element-label">',
      '#suffix' => '</div>'
    );
    // Put the elements below in a "description" div.
    $content['description'] = array(
      '#prefix' => '<div class="description">',
    );
    $content['description']['parameter'] = array(
      '#caption' => t('Parameter'),
      '#theme' => 'rules_content_group',
    );
    foreach ($this->element->pluginParameterInfo() as $name => $parameter) {
      $element = array();
      if (!empty($this->element->settings[$name . ':select'])) {
        $element['content'] = array(
         '#markup' => '[' . $this->element->settings[$name . ':select'] . ']',
        );
      }
      elseif (isset($this->element->settings[$name]) && (!isset($parameter['default value']) || $parameter['default value'] != $this->element->settings[$name])) {
        $method = empty($parameter['options list']) ? 'render' : 'renderOptionsLabel';
        $class = $this->getDataTypeClass($parameter['type'], $parameter);
        // We cannot use method_exists() here as it would trigger a PHP bug,
        // @see http://drupal.org/node/1258284
        $element = call_user_func(array($class, $method), $this->element->settings[$name], $name, $parameter, $this->element);
      }
      // Only add parameters that are really configured / not default.
      if ($element) {
        $content['description']['parameter'][$name] = array(
          '#theme' => 'rules_parameter_configuration',
          '#info' => $parameter,
        ) + $element;
      }
    }
    foreach ($this->element->providesVariables() as $name => $var_info) {
      $content['description']['provides'][$name] = array(
        '#theme' => 'rules_variable_view',
        '#info' => $var_info,
        '#name' => $name,
      );
    }
    if (!empty($content['description']['provides'])) {
      $content['description']['provides'] += array(
        '#caption' => t('Provides variables'),
        '#theme' => 'rules_content_group',
      );
    }
    // Add integrity exception messages if there are any for this element.
    try {
      $this->element->integrityCheck();
      // A configuration is still marked as dirty, but already works again.
      if (!empty($this->element->dirty)) {
        rules_config_update_dirty_flag($this->element);
        $variables = array('%label' => $this->element->label(), '%name' => $this->element->name, '@plugin' => $this->element->plugin());
        drupal_set_message(t('The @plugin %label (%name) was marked dirty, but passes the integrity check now and is active again.', $variables));
        rules_clear_cache();
      }
    }
    catch (RulesIntegrityException $e) {
      $content['description']['integrity'] = array(
        '#theme' => 'rules_content_group',
        '#caption' => t('Error'),
        '#attributes' => array('class' => array('rules-content-group-integrity-error')),
        'error' => array(
          '#markup' => filter_xss($e->getMessage()),
        ),
      );
      // Also make sure the rule is marked as dirty.
      if (empty($this->element->dirty)) {
        rules_config_update_dirty_flag($this->element);
        rules_clear_cache();
      }
    }

    $content['#suffix'] = '</div>';
    $content['#type'] = 'container';
    $content['#attributes']['class'][] = 'rules-element-content';
    return $content;
  }

  /**
   * Implements RulesPluginUIInterface.
   */
  public function operations() {
    $name = $this->element->root()->name;
    $render = array(
      '#theme' => 'links__rules',
    );
    $render['#attributes']['class'][] = 'rules-operations';
    $render['#attributes']['class'][] = 'action-links';
    $render['#links']['edit'] = array(
      'title' => t('edit'),
      'href' => RulesPluginUI::path($name, 'edit', $this->element),
    );
    $render['#links']['delete'] = array(
      'title' => t('delete'),
      'href' => RulesPluginUI::path($name, 'delete', $this->element),
    );
    return $render;
  }

  /**
   * Implements RulesPluginUIInterface.
   */
  public function help() {}


  /**
   * Deprecated by the controllers overviewTable() method.
   */
  public static function overviewTable($conditions = array(), $options = array()) {
    return rules_ui()->overviewTable($conditions, $options);
  }

  /**
   * Generates a path using the given operation for the element with the given
   * id of the configuration with the given name.
   */
  public static function path($name, $op = NULL, RulesPlugin $element = NULL, $parameter = FALSE) {
    $element_id = isset($element) ? $element->elementId() : FALSE;
    if (isset(self::$basePath)) {
      $base_path = self::$basePath;
    }
    // Default to the paths used by 'rules_admin', so modules can easily re-use
    // its UI.
    else {
      $base_path = isset($element) && $element instanceof RulesTriggerableInterface ? 'admin/config/workflow/rules/reaction' : 'admin/config/workflow/rules/components';
    }
    return implode('/', array_filter(array($base_path . '/manage', $name, $op, $element_id, $parameter)));
  }

  /**
   * Determines the default redirect target for an edited/deleted element. This
   * is a parent element which is either a rule or the configuration root.
   */
  public static function defaultRedirect(RulesPlugin $element) {
    while (!$element->isRoot()) {
      if ($element instanceof Rule) {
        return self::path($element->root()->name, 'edit', $element);
      }
      $element = $element->parentElement();
    }
    return self::path($element->name);
  }

  /**
   * Returns an array of options to use with a select for the items specified
   * in the given hook.
   *
   * @param $item_type
   *   The item type to get options for. One of 'data', 'event', 'condition' and
   *   'action'.
   * @param $items
   *   (optional) An array of items to restrict the options to.
   *
   * @return
   *   An array of options.
   */
  public static function getOptions($item_type, $items = NULL) {
    $sorted_data = array();
    $ungrouped = array();
    $data = $items ? $items : rules_fetch_data($item_type . '_info');
    foreach ($data as $name => $info) {
      // Verfiy the current user has access to use it.
      if (!user_access('bypass rules access') && !empty($info['access callback']) && !call_user_func($info['access callback'], $item_type, $name)) {
        continue;
      }
      if (!empty($info['group'])) {
        $sorted_data[drupal_ucfirst($info['group'])][$name] = drupal_ucfirst($info['label']);
      }
      else {
        $ungrouped[$name] = drupal_ucfirst($info['label']);
      }
    }
    asort($ungrouped);
    foreach ($sorted_data as $key => $choices) {
      asort($choices);
      $sorted_data[$key] = $choices;
    }
    ksort($sorted_data);
    // Always move the 'Components' group down it it exists.
    if (isset($sorted_data[t('Components')])) {
      $copy = $sorted_data[t('Components')];
      unset($sorted_data[t('Components')]);
      $sorted_data[t('Components')] = $copy;
    }
    return $ungrouped + $sorted_data;
  }

  public static function formDefaults(&$form, &$form_state) {
    form_load_include($form_state, 'inc', 'rules', 'ui/ui.forms');
    // Add our own css.
    $form['#attached']['css'][] = drupal_get_path('module', 'rules') . '/ui/rules.ui.css';
    // Workaround for problems with jquery css in seven theme and the core
    // autocomplete.
    if ($GLOBALS['theme'] == 'seven') {
      $form['#attached']['css'][] = drupal_get_path('module', 'rules') . '/ui/rules.ui.seven.css';
    }

    // Specify the wrapper div used by #ajax.
    $form['#prefix'] = '<div id="rules-form-wrapper">';
    $form['#suffix'] = '</div>';

    // Preserve the base path in the form state. The after build handler will
    // set self::$basePath again for cached forms.
    if (isset(self::$basePath)) {
      $form_state['_rules_base_path'] = RulesPluginUI::$basePath;
      $form['#after_build'][] = 'rules_form_after_build_restore_base_path';
    }
  }

  public static function getTags() {
    $result = db_select('rules_tags')
      ->distinct()
      ->fields('rules_tags', array('tag'))
      ->groupBy('tag')
      ->execute()
      ->fetchCol('tag');
    return drupal_map_assoc($result);
  }
}

/**
 * UI for abstract plugins (conditions & actions).
 */
class RulesAbstractPluginUI extends RulesPluginUI {

  /**
   * Overridden to invoke the abstract plugins form alter callback and to add
   * the negation checkbox for conditions.
   */
  public function form(&$form, &$form_state, $options = array()) {
    parent::form($form, $form_state, $options);

    if ($this->element instanceof RulesCondition) {
      $form['negate'] = array(
        '#title' => t('Negate'),
        '#type' => 'checkbox',
        '#description' => t('If checked, the condition result is negated such that it returns TRUE if it evaluates to FALSE.'),
        '#default_value' => $this->element->isNegated(),
        '#weight' => 5,
      );
    }
    $this->element->call('form_alter', array(&$form, &$form_state, $options));
  }

  public function form_extract_values($form, &$form_state) {
    parent::form_extract_values($form, $form_state);
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);
    if ($this->element instanceof RulesCondition && isset($form_values['negate'])) {
      $this->element->negate($form_values['negate']);
    }
  }

  public function form_validate($form, &$form_state) {
    parent::form_validate($form, $form_state);
    // Validate the edited element and throw validation errors if it fails.
    try {
      $this->element->integrityCheck();
    }
    catch (RulesIntegrityException $e) {
      form_set_error(implode('][', $e->keys), $e->getMessage());
    }
  }
}

/**
 * UI for Rules Container.
 */
class RulesContainerPluginUI extends RulesPluginUI {

  /**
   * Generates a table for editing the contained elements.
   */
  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    parent::form($form, $form_state, $options);
    $form['elements'] = array(
      // Hide during creation or for embedded elements.
      '#access' => empty($options['init']) && $this->element->isRoot(),
      '#tree' => TRUE,
      '#theme' => 'rules_elements',
      '#empty' => t('None'),
      '#caption' => t('Elements')
    );
    $form['elements']['#attributes']['class'][] = 'rules-container-plugin';

    // Recurse over all element childrens or use the provided iterator.
    $iterator = isset($iterator) ? $iterator : $this->element->elements();
    $root_depth = $this->element->depth();
    foreach ($iterator as $key => $child) {
      $id = $child->elementId();

      // Do not render rules as container element when displayed in a rule set.
      $is_container = $child instanceof RulesContainerPlugin && !($child instanceof Rule);
      $form['elements'][$id] = array(
        '#depth' => $child->depth() - $root_depth - 1,
        '#container' => $is_container,
      );
      $form['elements'][$id]['label'] = $child->buildContent();
      $form['elements'][$id]['weight'] = array(
        '#type' => 'weight',
        '#default_value' => $child->weight,
        '#delta' => 20,
      );
      $form['elements'][$id]['parent_id'] = array(
        '#type' => 'hidden',
        // If another iterator is passed in, the childs parent may not equal
        // the current element. Thus ask the child for its parent.
        '#default_value' => $child->parentElement()->elementId(),
      );
      $form['elements'][$id]['element_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $id,
      );
      $form['elements'][$id]['operations'] = $child->operations();
    }

    // Alter the submit button label.
    if (!empty($options['button']) && !empty($options['init'])) {
      $form['submit']['#value'] = t('Continue');
    }
    elseif (!empty($options['button']) && $this->element->isRoot()) {
      $form['submit']['#value'] = t('Save changes');
    }
  }

  /**
   * Applies the values of the form to the given rule configuration.
   */
  public function form_extract_values($form, &$form_state) {
    parent::form_extract_values($form, $form_state);
    $values = RulesPluginUI::getFormStateValues($form, $form_state);
    // Now apply the new hierarchy.
    if (isset($values['elements'])) {
      foreach ($values['elements'] as $id => $data) {
        $child = $this->element->elementMap()->lookup($id);
        $child->weight = $data['weight'];
        $parent = $this->element->elementMap()->lookup($data['parent_id']);
        $child->setParent($parent ? $parent : $this->element);
      }
      $this->element->sortChildren(TRUE);
    }
  }

  public function operations() {
    $ops = parent::operations();
    $add_ops = self::addOperations();
    $ops['#links'] += $add_ops['#links'];
    return $ops;
  }

  /**
   * Gets the Add-* operations for the given element.
   */
  public function addOperations() {
    $name = $this->element->root()->name;
    $render = array(
      '#theme' => 'links__rules',
    );
    $render['#attributes']['class'][] = 'rules-operations-add';
    $render['#attributes']['class'][] = 'action-links';
    foreach (rules_fetch_data('plugin_info') as $plugin => $info) {
      if (!empty($info['embeddable']) && $this->element instanceof $info['embeddable']) {
        $render['#links']['add_' . $plugin] = array(
          'title' => t('Add !name', array('!name' => $plugin)),
          'href' => RulesPluginUI::path($name, 'add', $this->element, $plugin),
        );
      }
    }
    return $render;
  }


  public function buildContent() {
    $content = parent::buildContent();
    // Don't link the title for embedded container plugins, except for rules.
    if (!$this->element->isRoot() && !($this->element instanceof Rule)) {
      $content['label']['#type'] = 'markup';
      $content['label']['#markup'] = check_plain($content['label']['#title']);
      unset($content['label']['#title']);
    }
    elseif ($this->element->isRoot()) {
      $content['description']['settings'] = array(
        '#theme' => 'rules_content_group',
        '#weight' => -4,
        'machine_name' => array(
          '#markup' => t('Machine name') . ': ' . $this->element->name,
        ),
        'weight' => array(
          '#access' => $this->element instanceof RulesTriggerableInterface,
          '#markup' => t('Weight') . ': ' . $this->element->weight,
        ),
      );
      if (!empty($this->element->tags)) {
        $content['description']['tags'] = array(
          '#theme' => 'rules_content_group',
          '#caption' => t('Tags'),
          'tags' => array(
            '#markup' => implode(', ', $this->element->tags),
          ),
        );
      }
      if ($vars = $this->element->componentVariables()) {
        $content['description']['variables'] = array(
          '#caption' => t('Parameter'),
          '#theme' => 'rules_content_group',
        );
        foreach ($vars as $name => $info) {
          if (!isset($info['parameter']) || $info['parameter']) {
            $content['description']['variables'][$name] = array(
              '#theme' => 'rules_variable_view',
              '#info' => $info,
              '#name' => $name,
            );
          }
        }
      }
    }
    return $content;
  }
}

/**
 * UI for Rules condition container.
 */
class RulesConditionContainerUI extends RulesContainerPluginUI {

  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    parent::form($form, $form_state, $options, $iterator);
    // Add the add-* operation links.
    $form['elements']['#add'] = self::addOperations();
    $form['elements']['#attributes']['class'][] = 'rules-condition-container';
    $form['elements']['#caption'] = t('Conditions');

    // By default skip
    if (!empty($options['init']) && !$this->element->isRoot()) {
      $config = $this->element->root();
      $form['init_help'] = array(
        '#type' => 'container',
        '#id' => 'rules-plugin-add-help',
        'content' => array(
          '#markup' => t('You are about to add a new @plugin to the @config-plugin %label. Use indentation to make conditions a part of this logic group. See <a href="@url">the online documentation</a> for more information on condition sets.',
            array('@plugin' => $this->element->plugin(),
                  '@config-plugin' => $config->plugin(),
                  '%label' => $config->label(),
                  '@url' => rules_external_help('condition-components'))),
        ),
      );
    }
    $form['negate'] = array(
      '#title' => t('Negate'),
      '#type' => 'checkbox',
      '#description' => t('If checked, the condition result is negated such that it returns TRUE if it evaluates to FALSE.'),
      '#default_value' => $this->element->isNegated(),
      '#weight' => 5,
    );
  }

  public function form_extract_values($form, &$form_state) {
    parent::form_extract_values($form, $form_state);
    $form_values = RulesPluginUI::getFormStateValues($form, $form_state);
    if (isset($form_values['negate'])) {
      $this->element->negate($form_values['negate']);
    }
  }
}

/**
 * UI for Rules action container.
 */
class RulesActionContainerUI extends RulesContainerPluginUI {

  public function form(&$form, &$form_state, $options = array(), $iterator = NULL) {
    parent::form($form,  $form_state, $options, $iterator);
    // Add the add-* operation links.
    $form['elements']['#add'] = self::addOperations();
    $form['elements']['#attributes']['class'][] = 'rules-action-container';
    $form['elements']['#caption'] = t('Actions');
  }
}
