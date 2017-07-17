<?php

namespace Drupal\ctools\Controller;

use Drupal\Core\Controller\ControllerResolverInterface;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\ctools\Wizard\WizardFactoryInterface;

/**
 * Wrapping controller for wizard forms that serve as the main page body.
 */
class WizardEntityFormController extends WizardFormController {

  /**
   * The entity manager service.
   *
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $entityManager;

  /**
   * @param \Drupal\Core\Controller\ControllerResolverInterface $controller_resolver
   *   The controller resolver.
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder.
   * @param \Drupal\ctools\Wizard\WizardFactoryInterface $wizard_factory
   *   The wizard factory.
   * @param \Drupal\Core\Entity\EntityManagerInterface $manager
   *   The entity manager.
   */
  public function __construct(ControllerResolverInterface $controller_resolver, FormBuilderInterface $form_builder, WizardFactoryInterface $wizard_factory, EntityManagerInterface $manager) {
    parent::__construct($controller_resolver, $form_builder, $wizard_factory);
    $this->entityManager = $manager;
  }

  /**
   * {@inheritdoc}
   */
  protected function getFormArgument(RouteMatchInterface $route_match) {
    $form_arg = $route_match->getRouteObject()->getDefault('_entity_wizard');
    list($entity_type_id, $operation) = explode('.', $form_arg);
    $definition = $this->entityManager->getDefinition($entity_type_id);
    $handlers = $definition->getHandlerClasses();
    if (empty($handlers['wizard'][$operation])) {
      throw new \Exception(sprintf('Unsupported wizard operation %s', $operation));
    }
    return $handlers['wizard'][$operation];
  }

}
