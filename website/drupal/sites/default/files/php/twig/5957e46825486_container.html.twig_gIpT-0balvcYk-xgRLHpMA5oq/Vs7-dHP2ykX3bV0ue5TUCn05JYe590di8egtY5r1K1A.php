<?php

/* core/themes/classy/templates/form/container.html.twig */
class __TwigTemplate_6e78b323890c8b4a6b9439cc8d76b5c7f1f751ce26848cd5c1e06cca730fd768 extends Twig_Template
{
    public function __construct(Twig_Environment $env)
    {
        parent::__construct($env);

        $this->parent = false;

        $this->blocks = array(
        );
    }

    protected function doDisplay(array $context, array $blocks = array())
    {
        $tags = array("set" => 23);
        $filters = array();
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('set'),
                array(),
                array()
            );
        } catch (Twig_Sandbox_SecurityError $e) {
            $e->setTemplateFile($this->getTemplateName());

            if ($e instanceof Twig_Sandbox_SecurityNotAllowedTagError && isset($tags[$e->getTagName()])) {
                $e->setTemplateLine($tags[$e->getTagName()]);
            } elseif ($e instanceof Twig_Sandbox_SecurityNotAllowedFilterError && isset($filters[$e->getFilterName()])) {
                $e->setTemplateLine($filters[$e->getFilterName()]);
            } elseif ($e instanceof Twig_Sandbox_SecurityNotAllowedFunctionError && isset($functions[$e->getFunctionName()])) {
                $e->setTemplateLine($functions[$e->getFunctionName()]);
            }

            throw $e;
        }

        // line 23
        $context["classes"] = array(0 => ((        // line 24
(isset($context["has_parent"]) ? $context["has_parent"] : null)) ? ("js-form-wrapper") : ("")), 1 => ((        // line 25
(isset($context["has_parent"]) ? $context["has_parent"] : null)) ? ("form-wrapper") : ("")));
        // line 28
        echo "<div";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => (isset($context["classes"]) ? $context["classes"] : null)), "method"), "html", null, true));
        echo ">";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["children"]) ? $context["children"] : null), "html", null, true));
        echo "</div>
";
    }

    public function getTemplateName()
    {
        return "core/themes/classy/templates/form/container.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  47 => 28,  45 => 25,  44 => 24,  43 => 23,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Theme override of a container used to wrap child elements.
 *
 * Used for grouped form items. Can also be used as a theme wrapper for any
 * renderable element, to surround it with a <div> and HTML attributes.
 * See \\Drupal\\Core\\Render\\Element\\RenderElement for more
 * information on the #theme_wrappers render array property, and
 * \\Drupal\\Core\\Render\\Element\\container for usage of the container render
 * element.
 *
 * Available variables:
 * - attributes: HTML attributes for the containing element.
 * - children: The rendered child elements of the container.
 * - has_parent: A flag to indicate that the container has one or more parent
     containers.
 *
 * @see template_preprocess_container()
 */
#}
{%
  set classes = [
    has_parent ? 'js-form-wrapper',
    has_parent ? 'form-wrapper',
  ]
%}
<div{{ attributes.addClass(classes) }}>{{ children }}</div>
";
    }
}
