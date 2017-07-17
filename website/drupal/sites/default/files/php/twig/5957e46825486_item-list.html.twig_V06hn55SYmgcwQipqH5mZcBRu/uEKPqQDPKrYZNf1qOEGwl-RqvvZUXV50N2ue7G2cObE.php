<?php

/* core/themes/classy/templates/dataset/item-list.html.twig */
class __TwigTemplate_0b16b27c57c795c8e89d3cf89038bd97149a2915eff9fd5fb51b2e680af81d92 extends Twig_Template
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
        $tags = array("if" => 22, "set" => 23, "for" => 33);
        $filters = array();
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('if', 'set', 'for'),
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

        // line 22
        if ($this->getAttribute((isset($context["context"]) ? $context["context"] : null), "list_style", array())) {
            // line 23
            $context["wrapper_attributes"] = $this->getAttribute((isset($context["wrapper_attributes"]) ? $context["wrapper_attributes"] : null), "addClass", array(0 => ("item-list--" . $this->getAttribute((isset($context["context"]) ? $context["context"] : null), "list_style", array()))), "method");
            // line 24
            $context["attributes"] = $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => ("item-list__" . $this->getAttribute((isset($context["context"]) ? $context["context"] : null), "list_style", array()))), "method");
        }
        // line 26
        if (((isset($context["items"]) ? $context["items"] : null) || (isset($context["empty"]) ? $context["empty"] : null))) {
            // line 27
            echo "<div";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["wrapper_attributes"]) ? $context["wrapper_attributes"] : null), "addClass", array(0 => "item-list"), "method"), "html", null, true));
            echo ">";
            // line 28
            if ( !twig_test_empty((isset($context["title"]) ? $context["title"] : null))) {
                // line 29
                echo "<h3>";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
                echo "</h3>";
            }
            // line 31
            if ((isset($context["items"]) ? $context["items"] : null)) {
                // line 32
                echo "<";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["list_type"]) ? $context["list_type"] : null), "html", null, true));
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["attributes"]) ? $context["attributes"] : null), "html", null, true));
                echo ">";
                // line 33
                $context['_parent'] = $context;
                $context['_seq'] = twig_ensure_traversable((isset($context["items"]) ? $context["items"] : null));
                foreach ($context['_seq'] as $context["_key"] => $context["item"]) {
                    // line 34
                    echo "<li";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["item"], "attributes", array()), "html", null, true));
                    echo ">";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["item"], "value", array()), "html", null, true));
                    echo "</li>";
                }
                $_parent = $context['_parent'];
                unset($context['_seq'], $context['_iterated'], $context['_key'], $context['item'], $context['_parent'], $context['loop']);
                $context = array_intersect_key($context, $_parent) + $_parent;
                // line 36
                echo "</";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["list_type"]) ? $context["list_type"] : null), "html", null, true));
                echo ">";
            } else {
                // line 38
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["empty"]) ? $context["empty"] : null), "html", null, true));
            }
            // line 40
            echo "</div>";
        }
    }

    public function getTemplateName()
    {
        return "core/themes/classy/templates/dataset/item-list.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  92 => 40,  89 => 38,  84 => 36,  74 => 34,  70 => 33,  65 => 32,  63 => 31,  58 => 29,  56 => 28,  52 => 27,  50 => 26,  47 => 24,  45 => 23,  43 => 22,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Theme override for an item list.
 *
 * Available variables:
 * - items: A list of items. Each item contains:
 *   - attributes: HTML attributes to be applied to each list item.
 *   - value: The content of the list element.
 * - title: The title of the list.
 * - list_type: The tag for list element (\"ul\" or \"ol\").
 * - wrapper_attributes: HTML attributes to be applied to the list wrapper.
 * - attributes: HTML attributes to be applied to the list.
 * - empty: A message to display when there are no items. Allowed value is a
 *   string or render array.
 * - context: A list of contextual data associated with the list. May contain:
 *   - list_style: The custom list style.
 *
 * @see template_preprocess_item_list()
 */
#}
{% if context.list_style %}
  {%- set wrapper_attributes = wrapper_attributes.addClass('item-list--' ~ context.list_style) %}
  {%- set attributes = attributes.addClass('item-list__' ~ context.list_style) %}
{% endif %}
{% if items or empty -%}
  <div{{ wrapper_attributes.addClass('item-list') }}>
    {%- if title is not empty -%}
      <h3>{{ title }}</h3>
    {%- endif -%}
    {%- if items -%}
      <{{ list_type }}{{ attributes }}>
        {%- for item in items -%}
          <li{{ item.attributes }}>{{ item.value }}</li>
        {%- endfor -%}
      </{{ list_type }}>
    {%- else -%}
      {{- empty -}}
    {%- endif -%}
  </div>
{%- endif %}
";
    }
}
