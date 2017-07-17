<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "vocabulary_tree_get",
 *   methods = {
 *     "GET"
 *   },
 *   translatable = true,
 *    title = @Translation("Taxonomy Vocabulary Get Tree"),
 *   description = @Translation("Returns term hierarchy."),
 *   category = @Translation("Taxonomy"),
 *   path = "taxonomy/{vocabulary}/get-tree"
 * )
 */
class VocabularyTreeGet extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * {@inheritdoc}
   */
  /*public function processRoute(Route $route) {
  // TODO - Check perms of taxonomy vocabulary access.
  //$route->setRequirement('_entity_access', $this->getDerivativeId() .'.view');
  }*/

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    // The query string parameter 'path' must exist in order to load the
    // node that correlates to path value provided.
    /*if (!$request->query->has('vocabulary')) {
    throw new HttpException(404);
    }*/

    $vocabulary_id = $request->get('vocabulary');
    /* @var $termStorage \Drupal\taxonomy\TermStorageInterface */
    $termStorage = \Drupal::entityTypeManager()->getStorage('taxonomy_term');
    // Load taxonomy terms for tax menu vocab.
    /* @var $terms \Drupal\taxonomy\TermInterface[] */
    $terms = $termStorage->loadTree($vocabulary_id);
    $terms_array = [];
    /* @var $term \Drupal\taxonomy\TermInterface */
    foreach ($terms as $term) {
      $terms_array[] = (array) $term;
    }

    return $terms_array;
  }

}
