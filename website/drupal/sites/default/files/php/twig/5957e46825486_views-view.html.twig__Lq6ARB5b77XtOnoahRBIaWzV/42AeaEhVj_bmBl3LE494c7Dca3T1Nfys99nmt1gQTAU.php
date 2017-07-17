<?php

/* core/themes/classy/templates/views/views-view.html.twig */
class __TwigTemplate_b7b7b0dc10f0d3eb34b7b37e7a2e0bf61b6f7ffaf44b94503c6e69a3c9bc6dc7 extends Twig_Template
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
        $tags = array("set" => 34, "if" => 44);
        $filters = array("clean_class" => 36);
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('set', 'if'),
                array('clean_class'),
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

        // line 34
        $context["classes"] = array(0 => "view", 1 => ("view-" . \Drupal\Component\Utility\Html::getClass(        // line 36
(isset($context["id"]) ? $context["id"] : null))), 2 => ("view-id-" .         // line 37
(isset($context["id"]) ? $context["id"] : null)), 3 => ("view-display-id-" .         // line 38
(isset($context["display_id"]) ? $context["display_id"] : null)), 4 => ((        // line 39
(isset($context["dom_id"]) ? $context["dom_id"] : null)) ? (("js-view-dom-id-" . (isset($context["dom_id"]) ? $context["dom_id"] : null))) : ("")));
        // line 42
        echo "<div";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => (isset($context["classes"]) ? $context["classes"] : null)), "method"), "html", null, true));
        echo ">
  ";
        // line 43
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title_prefix"]) ? $context["title_prefix"] : null), "html", null, true));
        echo "
  ";
        // line 44
        if ((isset($context["title"]) ? $context["title"] : null)) {
            // line 45
            echo "    ";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
            echo "
  ";
        }
        // line 47
        echo "  ";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title_suffix"]) ? $context["title_suffix"] : null), "html", null, true));
        echo "
  ";
        // line 48
        if ((isset($context["header"]) ? $context["header"] : null)) {
            // line 49
            echo "    <div class=\"view-header\">
      ";
            // line 50
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["header"]) ? $context["header"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 53
        echo "  ";
        if ((isset($context["exposed"]) ? $context["exposed"] : null)) {
            // line 54
            echo "    <div class=\"view-filters\">
      ";
            // line 55
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["exposed"]) ? $context["exposed"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 58
        echo "  ";
        if ((isset($context["attachment_before"]) ? $context["attachment_before"] : null)) {
            // line 59
            echo "    <div class=\"attachment attachment-before\">
      ";
            // line 60
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["attachment_before"]) ? $context["attachment_before"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 63
        echo "
  ";
        // line 64
        if ((isset($context["rows"]) ? $context["rows"] : null)) {
            // line 65
            echo "    <div class=\"view-content\">
      ";
            // line 66
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["rows"]) ? $context["rows"] : null), "html", null, true));
            echo "
    </div>
  ";
        } elseif (        // line 68
(isset($context["empty"]) ? $context["empty"] : null)) {
            // line 69
            echo "    <div class=\"view-empty\">
      ";
            // line 70
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["empty"]) ? $context["empty"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 73
        echo "
  ";
        // line 74
        if ((isset($context["pager"]) ? $context["pager"] : null)) {
            // line 75
            echo "    ";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["pager"]) ? $context["pager"] : null), "html", null, true));
            echo "
  ";
        }
        // line 77
        echo "  ";
        if ((isset($context["attachment_after"]) ? $context["attachment_after"] : null)) {
            // line 78
            echo "    <div class=\"attachment attachment-after\">
      ";
            // line 79
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["attachment_after"]) ? $context["attachment_after"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 82
        echo "  ";
        if ((isset($context["more"]) ? $context["more"] : null)) {
            // line 83
            echo "    ";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["more"]) ? $context["more"] : null), "html", null, true));
            echo "
  ";
        }
        // line 85
        echo "  ";
        if ((isset($context["footer"]) ? $context["footer"] : null)) {
            // line 86
            echo "    <div class=\"view-footer\">
      ";
            // line 87
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["footer"]) ? $context["footer"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 90
        echo "  ";
        if ((isset($context["feed_icons"]) ? $context["feed_icons"] : null)) {
            // line 91
            echo "    <div class=\"feed-icons\">
      ";
            // line 92
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["feed_icons"]) ? $context["feed_icons"] : null), "html", null, true));
            echo "
    </div>
  ";
        }
        // line 95
        echo "</div>
";
    }

    public function getTemplateName()
    {
        return "core/themes/classy/templates/views/views-view.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  186 => 95,  180 => 92,  177 => 91,  174 => 90,  168 => 87,  165 => 86,  162 => 85,  156 => 83,  153 => 82,  147 => 79,  144 => 78,  141 => 77,  135 => 75,  133 => 74,  130 => 73,  124 => 70,  121 => 69,  119 => 68,  114 => 66,  111 => 65,  109 => 64,  106 => 63,  100 => 60,  97 => 59,  94 => 58,  88 => 55,  85 => 54,  82 => 53,  76 => 50,  73 => 49,  71 => 48,  66 => 47,  60 => 45,  58 => 44,  54 => 43,  49 => 42,  47 => 39,  46 => 38,  45 => 37,  44 => 36,  43 => 34,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Theme override for a main view template.
 *
 * Available variables:
 * - attributes: Remaining HTML attributes for the element.
 * - css_name: A css-safe version of the view name.
 * - css_class: The user-specified classes names, if any.
 * - header: The optional header.
 * - footer: The optional footer.
 * - rows: The results of the view query, if any.
 * - empty: The content to display if there are no rows.
 * - pager: The optional pager next/prev links to display.
 * - exposed: Exposed widget form/info to display.
 * - feed_icons: Optional feed icons to display.
 * - more: An optional link to the next page of results.
 * - title: Title of the view, only used when displaying in the admin preview.
 * - title_prefix: Additional output populated by modules, intended to be
 *   displayed in front of the view title.
 * - title_suffix: Additional output populated by modules, intended to be
 *   displayed after the view title.
 * - attachment_before: An optional attachment view to be displayed before the
 *   view content.
 * - attachment_after: An optional attachment view to be displayed after the
 *   view content.
 * - dom_id: Unique id for every view being printed to give unique class for
 *   Javascript.
 *
 * @see template_preprocess_views_view()
 */
#}
{%
  set classes = [
    'view',
    'view-' ~ id|clean_class,
    'view-id-' ~ id,
    'view-display-id-' ~ display_id,
    dom_id ? 'js-view-dom-id-' ~ dom_id,
  ]
%}
<div{{ attributes.addClass(classes) }}>
  {{ title_prefix }}
  {% if title %}
    {{ title }}
  {% endif %}
  {{ title_suffix }}
  {% if header %}
    <div class=\"view-header\">
      {{ header }}
    </div>
  {% endif %}
  {% if exposed %}
    <div class=\"view-filters\">
      {{ exposed }}
    </div>
  {% endif %}
  {% if attachment_before %}
    <div class=\"attachment attachment-before\">
      {{ attachment_before }}
    </div>
  {% endif %}

  {% if rows %}
    <div class=\"view-content\">
      {{ rows }}
    </div>
  {% elseif empty %}
    <div class=\"view-empty\">
      {{ empty }}
    </div>
  {% endif %}

  {% if pager %}
    {{ pager }}
  {% endif %}
  {% if attachment_after %}
    <div class=\"attachment attachment-after\">
      {{ attachment_after }}
    </div>
  {% endif %}
  {% if more %}
    {{ more }}
  {% endif %}
  {% if footer %}
    <div class=\"view-footer\">
      {{ footer }}
    </div>
  {% endif %}
  {% if feed_icons %}
    <div class=\"feed-icons\">
      {{ feed_icons }}
    </div>
  {% endif %}
</div>
";
    }
}
