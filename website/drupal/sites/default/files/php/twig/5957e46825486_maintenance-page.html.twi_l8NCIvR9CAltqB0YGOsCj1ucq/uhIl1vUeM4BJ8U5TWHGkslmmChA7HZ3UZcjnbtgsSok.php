<?php

/* core/themes/seven/templates/maintenance-page.html.twig */
class __TwigTemplate_9e44a4474390da89f36a9b8026c148c0435baf61122d045dfd18b8df929a5d00 extends Twig_Template
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
        $tags = array("if" => 15);
        $filters = array();
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('if'),
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

        // line 12
        echo "<div class=\"layout-container\">

  <header role=\"banner\">
    ";
        // line 15
        if ((isset($context["site_name"]) ? $context["site_name"] : null)) {
            // line 16
            echo "      <h1 class=\"page-title\">";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["site_name"]) ? $context["site_name"] : null), "html", null, true));
            echo "</h1>
    ";
        }
        // line 18
        echo "  </header>

  ";
        // line 20
        if ($this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_first", array())) {
            // line 21
            echo "    <aside class=\"layout-sidebar-first\" role=\"complementary\">
      ";
            // line 22
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_first", array()), "html", null, true));
            echo "
    </aside>";
            // line 24
            echo "  ";
        }
        // line 25
        echo "
  <main role=\"main\">
    ";
        // line 27
        if ((isset($context["title"]) ? $context["title"] : null)) {
            // line 28
            echo "      <h1>";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
            echo "</h1>
    ";
        }
        // line 30
        echo "    ";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "highlighted", array()), "html", null, true));
        echo "
    ";
        // line 31
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "content", array()), "html", null, true));
        echo "
  </main>

  ";
        // line 34
        if ($this->getAttribute((isset($context["page"]) ? $context["page"] : null), "page_bottom", array())) {
            // line 35
            echo "    <footer role=\"contentinfo\">
      ";
            // line 36
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "page_bottom", array()), "html", null, true));
            echo "
    </footer>
  ";
        }
        // line 39
        echo "
</div>";
    }

    public function getTemplateName()
    {
        return "core/themes/seven/templates/maintenance-page.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  106 => 39,  100 => 36,  97 => 35,  95 => 34,  89 => 31,  84 => 30,  78 => 28,  76 => 27,  72 => 25,  69 => 24,  65 => 22,  62 => 21,  60 => 20,  56 => 18,  50 => 16,  48 => 15,  43 => 12,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Seven's theme implementation to display a single Drupal page while offline.
 *
 * All available variables are mirrored in page.html.twig.
 * Some may be blank but they are provided for consistency.
 *
 * @see template_preprocess_maintenance_page()
 */
#}
<div class=\"layout-container\">

  <header role=\"banner\">
    {% if site_name %}
      <h1 class=\"page-title\">{{ site_name }}</h1>
    {% endif %}
  </header>

  {% if page.sidebar_first %}
    <aside class=\"layout-sidebar-first\" role=\"complementary\">
      {{ page.sidebar_first }}
    </aside>{# /.layout-sidebar-first #}
  {% endif %}

  <main role=\"main\">
    {% if title %}
      <h1>{{ title }}</h1>
    {% endif %}
    {{ page.highlighted }}
    {{ page.content }}
  </main>

  {% if page.page_bottom %}
    <footer role=\"contentinfo\">
      {{ page.page_bottom }}
    </footer>
  {% endif %}

</div>{# /.layout-container #}
";
    }
}
