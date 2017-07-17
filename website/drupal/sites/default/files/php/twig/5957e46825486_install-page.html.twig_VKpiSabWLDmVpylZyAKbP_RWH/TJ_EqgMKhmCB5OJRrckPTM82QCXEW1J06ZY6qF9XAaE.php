<?php

/* core/themes/seven/templates/install-page.html.twig */
class __TwigTemplate_a12f702501704c19484e7780346f56ad2ce513b50211fddf0ab4bd98a06b9c82 extends Twig_Template
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
            echo "      <h1 class=\"page-title\">
        ";
            // line 17
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["site_name"]) ? $context["site_name"] : null), "html", null, true));
            echo "
        ";
            // line 18
            if ((isset($context["site_version"]) ? $context["site_version"] : null)) {
                // line 19
                echo "          <span class=\"site-version\">";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["site_version"]) ? $context["site_version"] : null), "html", null, true));
                echo "</span>
        ";
            }
            // line 21
            echo "      </h1>
    ";
        }
        // line 23
        echo "  </header>

  ";
        // line 25
        if ($this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_first", array())) {
            // line 26
            echo "    <aside class=\"layout-sidebar-first\" role=\"complementary\">
      ";
            // line 27
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_first", array()), "html", null, true));
            echo "
    </aside>";
            // line 29
            echo "  ";
        }
        // line 30
        echo "
  <main role=\"main\">
    ";
        // line 32
        if ((isset($context["title"]) ? $context["title"] : null)) {
            // line 33
            echo "      <h2 class=\"heading-a\">";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
            echo "</h2>
    ";
        }
        // line 35
        echo "    ";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "highlighted", array()), "html", null, true));
        echo "
    ";
        // line 36
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "content", array()), "html", null, true));
        echo "
  </main>

  ";
        // line 39
        if ($this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_second", array())) {
            // line 40
            echo "    <aside class=\"layout-sidebar-second\" role=\"complementary\">
      ";
            // line 41
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "sidebar_second", array()), "html", null, true));
            echo "
    </aside>";
            // line 43
            echo "  ";
        }
        // line 44
        echo "
  ";
        // line 45
        if ($this->getAttribute((isset($context["page"]) ? $context["page"] : null), "page_bottom", array())) {
            // line 46
            echo "    <footer role=\"contentinfo\">
      ";
            // line 47
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["page"]) ? $context["page"] : null), "page_bottom", array()), "html", null, true));
            echo "
    </footer>
  ";
        }
        // line 50
        echo "
</div>";
    }

    public function getTemplateName()
    {
        return "core/themes/seven/templates/install-page.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  134 => 50,  128 => 47,  125 => 46,  123 => 45,  120 => 44,  117 => 43,  113 => 41,  110 => 40,  108 => 39,  102 => 36,  97 => 35,  91 => 33,  89 => 32,  85 => 30,  82 => 29,  78 => 27,  75 => 26,  73 => 25,  69 => 23,  65 => 21,  59 => 19,  57 => 18,  53 => 17,  50 => 16,  48 => 15,  43 => 12,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Seven theme implementation to display a Drupal installation page.
 *
 * All available variables are mirrored in page.html.twig.
 * Some may be blank but they are provided for consistency.
 *
 * @see template_preprocess_install_page()
 */
#}
<div class=\"layout-container\">

  <header role=\"banner\">
    {% if site_name %}
      <h1 class=\"page-title\">
        {{ site_name }}
        {% if site_version %}
          <span class=\"site-version\">{{ site_version }}</span>
        {% endif %}
      </h1>
    {% endif %}
  </header>

  {% if page.sidebar_first %}
    <aside class=\"layout-sidebar-first\" role=\"complementary\">
      {{ page.sidebar_first }}
    </aside>{# /.layout-sidebar-first #}
  {% endif %}

  <main role=\"main\">
    {% if title %}
      <h2 class=\"heading-a\">{{ title }}</h2>
    {% endif %}
    {{ page.highlighted }}
    {{ page.content }}
  </main>

  {% if page.sidebar_second %}
    <aside class=\"layout-sidebar-second\" role=\"complementary\">
      {{ page.sidebar_second }}
    </aside>{# /.layout-sidebar-second #}
  {% endif %}

  {% if page.page_bottom %}
    <footer role=\"contentinfo\">
      {{ page.page_bottom }}
    </footer>
  {% endif %}

</div>{# /.layout-container #}
";
    }
}
