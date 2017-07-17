<?php

/* core/themes/seven/templates/admin-block-content.html.twig */
class __TwigTemplate_e16240c1fe5a9e60de57b8bfa2a152400a7a8deaaaed4998d3e6297eaffc1f5d extends Twig_Template
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
        $tags = array("set" => 21, "if" => 26, "for" => 28);
        $filters = array();
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('set', 'if', 'for'),
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

        // line 21
        $context["classes"] = array(0 => "admin-list", 1 => ((        // line 23
(isset($context["compact"]) ? $context["compact"] : null)) ? ("compact") : ("")));
        // line 26
        if ((isset($context["content"]) ? $context["content"] : null)) {
            // line 27
            echo "  <ul";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => (isset($context["classes"]) ? $context["classes"] : null)), "method"), "html", null, true));
            echo ">
    ";
            // line 28
            $context['_parent'] = $context;
            $context['_seq'] = twig_ensure_traversable((isset($context["content"]) ? $context["content"] : null));
            foreach ($context['_seq'] as $context["_key"] => $context["item"]) {
                // line 29
                echo "      <li><a href=\"";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["item"], "url", array()), "html", null, true));
                echo "\"><span class=\"label\">";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["item"], "title", array()), "html", null, true));
                echo "</span><div class=\"description\">";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["item"], "description", array()), "html", null, true));
                echo "</div></a></li>
    ";
            }
            $_parent = $context['_parent'];
            unset($context['_seq'], $context['_iterated'], $context['_key'], $context['item'], $context['_parent'], $context['loop']);
            $context = array_intersect_key($context, $_parent) + $_parent;
            // line 31
            echo "  </ul>
";
        }
    }

    public function getTemplateName()
    {
        return "core/themes/seven/templates/admin-block-content.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  70 => 31,  57 => 29,  53 => 28,  48 => 27,  46 => 26,  44 => 23,  43 => 21,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Seven's theme implementation for the content of an administrative block.
 *
 * Uses unordered list markup in both compact and extended modes.
 *
 * Available variables:
 * - content: List of administrative menu items. Each menu item contains:
 *   - url: Path to the admin section.
 *   - title: Short name of the section.
 *   - description: Description of the administrative menu item.
 * - attributes: HTML attributes to be added to the element.
 * - compact: Boolean indicating whether compact mode is turned on or not.
 *
 * @see template_preprocess_admin_block_content()
 * @see seven_preprocess_admin_block_content()
 */
#}
{%
  set classes = [
    'admin-list',
    compact ? 'compact',
  ]
%}
{% if content %}
  <ul{{ attributes.addClass(classes) }}>
    {% for item in content %}
      <li><a href=\"{{ item.url }}\"><span class=\"label\">{{ item.title }}</span><div class=\"description\">{{ item.description }}</div></a></li>
    {% endfor %}
  </ul>
{% endif %}
";
    }
}
