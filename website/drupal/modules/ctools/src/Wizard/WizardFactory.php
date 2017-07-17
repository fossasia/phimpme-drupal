<?php

namespace Drupal\ctools\Wizard;

use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormState;
use Symfony\Component\EventDispatcher\EventDispatcherInterface;

class WizardFactory implements WizardFactoryInterface {

  /**
   * The Form Builder.
   *
   * @var \Drupal\Core\Form\FormBuilderInterface
   */
  protected $builder;

  /**
   * The event dispatcher.
   *
   * @var \Symfony\Component\EventDispatcher\EventDispatcherInterface
   */
  protected $dispatcher;

  /**
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder.
   * @param \Symfony\Component\EventDispatcher\EventDispatcherInterface $event_dispatcher
   *   The event dispatcher.
   */
  public function __construct(FormBuilderInterface $form_builder, EventDispatcherInterface $event_dispatcher) {
    $this->builder = $form_builder;
    $this->dispatcher = $event_dispatcher;
  }

  /**
   * {@inheritdoc}
   */
  public function getWizardForm(FormWizardInterface $wizard, array $parameters = [], $ajax = FALSE) {
    $form_state = $this->getFormState($wizard, $parameters, $ajax);
    $form = $this->builder->buildForm($wizard, $form_state);

    if ($ajax) {
      $form['#attached']['library'][] = 'core/drupal.dialog.ajax';
      $status_messages = array('#type' => 'status_messages');
      // @todo properly inject the renderer. Core should really be doing this work.
      if ($messages = \Drupal::service('renderer')->renderRoot($status_messages)) {
        if (!empty($form['#prefix'])) {
          // Form prefix is expected to be a string. Prepend the messages to
          // that string.
          $form['#prefix'] = '<div class="wizard-messages">' . $messages . '</div>' . $form['#prefix'];
        }
      }
    }
    return $form;
  }

  /**
   * @param string $class
   *   A class name implementing FormWizardInterface.
   * @param array $parameters
   *   The array of parameters specific to this wizard.
   *
   * @return \Drupal\ctools\Wizard\FormWizardInterface
   */
  public function createWizard($class, array $parameters) {
    $arguments = [];
    $reflection = new \ReflectionClass($class);
    $constructor = $reflection->getMethod('__construct');
    foreach ($constructor->getParameters() as $parameter) {
      if (array_key_exists($parameter->name, $parameters)) {
        $arguments[] = $parameters[$parameter->name];
      }
      elseif ($parameter->isDefaultValueAvailable()) {
        $arguments[] = $parameter->getDefaultValue();
      }
    }
    /** @var $wizard \Drupal\ctools\Wizard\FormWizardInterface */
    $wizard = $reflection->newInstanceArgs($arguments);
    return $wizard;
  }

  /**
   * Get the wizard form state.
   *
   * @param \Drupal\ctools\Wizard\FormWizardInterface $wizard
   *   The form wizard.
   * @param array $parameters
   *   The array of parameters specific to this wizard.
   * @param bool $ajax
   *
   * @return \Drupal\Core\Form\FormState
   */
  public function getFormState(FormWizardInterface $wizard, array $parameters, $ajax = FALSE) {
    $form_state = new FormState();
    // If a wizard has no values, initialize them.
    if (!$wizard->getMachineName() || !$wizard->getTempstore()->get($wizard->getMachineName())) {
      $cached_values = $wizard->initValues();
      // Save the cached values that were initialized.
      if ($wizard->getMachineName()) {
        $wizard->getTempstore()->set($wizard->getMachineName(), $cached_values);
      }
    }
    else {
      $cached_values = $wizard->getTempstore()->get($wizard->getMachineName());
    }
    $form_state->setTemporaryValue('wizard', $cached_values);
    $form_state->set('ajax', $ajax);

    $parameters['form'] = [];
    $parameters['form_state'] = $form_state;
    $method = new \ReflectionMethod($wizard, 'buildForm');
    $arguments = [];
    foreach ($method->getParameters() as $parameter) {
      if (array_key_exists($parameter->name, $parameters)) {
        $arguments[] = $parameters[$parameter->name];
      }
      elseif ($parameter->isDefaultValueAvailable()) {
        $arguments[] = $parameter->getDefaultValue();
      }
    }
    unset($parameters['form'], $parameters['form_state']);
    // Remove $form and $form_state from the arguments, and re-index them.
    unset($arguments[0], $arguments[1]);
    $form_state->addBuildInfo('args', array_values($arguments));
    return $form_state;
  }

}
