<?php

namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Core\Asset\AssetCollectionRendererInterface;
use Drupal\Core\Asset\AssetResolverInterface;
use Drupal\Core\Asset\AttachedAssets;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Render\RendererInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\Routing\Route;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_view",
 *   methods = {
 *     "GET"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityView"
 * )
 */
class EntityView extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * @var RendererInterface
   */
  protected $renderer;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static($configuration, $plugin_id, $plugin_definition, $container->get('renderer'), $container->get('asset.resolver'), $container->get('asset.css.collection_renderer'), $container->get('asset.js.collection_renderer'));
  }

  /**
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param RendererInterface $renderer
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, RendererInterface $renderer, AssetResolverInterface $asset_resolver, AssetCollectionRendererInterface $css_collection_renderer, AssetCollectionRendererInterface $js_collection_renderer) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->renderer = $renderer;
    $this->assetResolver = $asset_resolver;
    $this->cssCollectionRenderer = $css_collection_renderer;
    $this->jsCollectionRenderer = $js_collection_renderer;
  }

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {
    $route->setRequirement('_entity_access', $this->getDerivativeId() . '.view');
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $view_mode = 'full';
    if ($request->query->has('view_mode')) {
      $view_mode = $request->query->get('view_mode');
    }
    /* @var $entity \Drupal\Core\Entity\EntityInterface */
    $entity = $this->getContextValue($this->getDerivativeId());
    $view_builder = \Drupal::entityManager()->getViewBuilder($entity->getEntityTypeId());
    $render_array = $view_builder->view($entity, $view_mode);

    $result = [];
    $result['body'] = $this->renderer->renderRoot($render_array);
    $all_assets = $this->gatherAssetMarkup($render_array);
    $result += $this->renderAssets($all_assets);

    return $result;
  }

  /**
   * Renders an array of assets.
   *
   * @param array $all_assets
   *   An array of all unrendered assets keyed by type.
   *
   * @return array
   *   An array of all rendered assets keyed by type.
   */
  protected function renderAssets(array $all_assets) {
    $result = [];
    foreach ($all_assets as $asset_type => $assets) {
      $result[$asset_type] = [];
      foreach ($assets as $asset) {
        $result[$asset_type][] = $this->renderer->renderRoot($asset);
      }
    }

    return $result;
  }
  /**
   * Gathers the markup for each type of asset.
   *
   * @param array $render_array
   *   The render array for the entity.
   *
   * @return array
   *   An array of rendered assets. See self::getRenderedEntity() for the keys.
   *
   * @todo template_preprocess_html() should be split up and made reusable.
   */
  protected function gatherAssetMarkup(array $render_array) {
    $assets = AttachedAssets::createFromRenderArray($render_array);
    // Render the asset collections.
    $css_assets = $this->assetResolver->getCssAssets($assets, FALSE);
    $variables['styles'] = $this->cssCollectionRenderer->render($css_assets);
    list($js_assets_header, $js_assets_footer) = $this->assetResolver->getJsAssets($assets, FALSE);
    $variables['scripts'] = $this->jsCollectionRenderer->render($js_assets_header);
    $variables['scripts_bottom'] = $this->jsCollectionRenderer->render($js_assets_footer);
    // @todo Handle all non-asset attachments.
    // $variables['head'] = drupal_get_html_head(FALSE);
    return $variables;
  }

}
