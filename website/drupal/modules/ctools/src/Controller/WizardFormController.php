<?php

namespace Drupal\ctools\Controller;

use Drupal\Core\Controller\ControllerResolverInterface;
use Drupal\Core\Controller\FormController;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\ctools\Wizard\FormWizardInterface;
use Drupal\ctools\Wizard\WizardFactoryInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Wrapping controller for wizard forms that serve as the main page body.
 */
class WizardFormController extends FormController {

  /**
   * The class resolver.
   *
   * @var \Drupal\Core\DependencyInjection\ClassResolverInterface;
   */
  protected $classResolver;

  /**
   * Tempstore Factory for keeping track of values in each step of the wizard.
   *
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * The event dispatcher.
   *
   * @var \Symfony\Component\EventDispatcher\EventDispatcherInterface
   */
  protected $dispatcher;

  /**
   * @param \Drupal\Core\Controller\ControllerResolverInterface $controller_resolver
   *   The controller resolver.
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder.
   * @param \Drupal\ctools\Wizard\WizardFactoryInterface $wizard_factory
   *   The wizard factory.
   */
  public function __construct(ControllerResolverInterface $controller_resolver, FormBuilderInterface $form_builder, WizardFactoryInterface $wizard_factory) {
    parent::__construct($controller_resolver, $form_builder);
    $this->wizardFactory = $wizard_factory;
  }

  /**
   * {@inheritdoc}
   */
  protected function getFormArgument(RouteMatchInterface $route_match) {
    return $route_match->getRouteObject()->getDefault('_wizard');
  }

  /**
   * Wizards are not instantiated as simply as forms, so this method is unused.
   */
  protected function getFormObject(RouteMatchInterface $route_match, $form_arg) {
    if (!is_subclass_of($form_arg, '\Drupal\ctools\Wizard\FormWizardInterface')) {
      throw new \Exception("The _wizard default must reference a class instance of \\Drupal\\ctools\\Wizard\\FormWizardInterface.");
    }
    $parameters = $route_match->getParameters()->all();
    $parameters += $form_arg::getParameters();
    $parameters['route_match'] = $route_match;
    return $this->wizardFactory->createWizard($form_arg, $parameters);
  }

  /**
   * {@inheritdoc}
   */
  public function getContentResult(Request $request, RouteMatchInterface $route_match) {
    $wizard = $this->getFormObject($route_match, $this->getFormArgument($route_match));
    $ajax = $request->attributes->get('js') == 'ajax' ? TRUE : FALSE;

    return $this->wizardFactory->getWizardForm($wizard, $request->attributes->all(), $ajax);
  }

}
