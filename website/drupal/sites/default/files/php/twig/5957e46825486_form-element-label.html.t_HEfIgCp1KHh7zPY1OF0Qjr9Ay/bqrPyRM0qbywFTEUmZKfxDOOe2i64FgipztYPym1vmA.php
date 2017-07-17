<?php

/* core/themes/classy/templates/form/form-element-label.html.twig */
class __TwigTemplate_93643d9caff81705a7a3057c7bde92612f300103c648c01dc4a01e556134f42f extends Twig_Template
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
        $tags = array("set" => 16, "if" => 23);
        $filters = array();
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('set', 'if'),
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

        // line 16
        $context["classes"] = array(0 => (((        // line 17
(isset($context["title_display"]) ? $context["title_display"] : null) == "after")) ? ("option") : ("")), 1 => (((        // line 18
(isset($context["title_display"]) ? $context["title_display"] : null) == "invisible")) ? ("visually-hidden") : ("")), 2 => ((        // line 19
(isset($context["required"]) ? $context["required"] : null)) ? ("js-form-required") : ("")), 3 => ((        // line 20
(isset($context["required"]) ? $context["required"] : null)) ? ("form-required") : ("")));
        // line 23
        if (( !twig_test_empty((isset($context["title"]) ? $context["title"] : null)) || (isset($context["required"]) ? $context["required"] : null))) {
            // line 24
            echo "<label";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => (isset($context["classes"]) ? $context["classes"] : null)), "method"), "html", null, true));
            echo ">";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
            echo "</label>";
        }
    }

    public function getTemplateName()
    {
        return "core/themes/classy/templates/form/form-element-label.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  51 => 24,  49 => 23,  47 => 20,  46 => 19,  45 => 18,  44 => 17,  43 => 16,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Theme override for a form element label.
 *
 * Available variables:
 * - title: The label's text.
 * - title_display: Elements title_display setting.
 * - required: An indicator for whether the associated form element is required.
 * - attributes: A list of HTML attributes for the label.
 *
 * @see template_preprocess_form_element_label()
 */
#}
{%
  set classes = [
    title_display == 'after' ? 'option',
    title_display == 'invisible' ? 'visually-hidden',
    required ? 'js-form-required',
    required ? 'form-required',
  ]
%}
{% if title is not empty or required -%}
  <label{{ attributes.addClass(classes) }}>{{ title }}</label>
{%- endif %}
";
    }
}
