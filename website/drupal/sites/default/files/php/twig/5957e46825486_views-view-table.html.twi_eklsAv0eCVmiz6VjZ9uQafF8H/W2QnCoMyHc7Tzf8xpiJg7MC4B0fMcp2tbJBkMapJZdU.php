<?php

/* core/themes/classy/templates/views/views-view-table.html.twig */
class __TwigTemplate_7a393c22cd6f7673811813dd7df87e98ab3c70e603de167953e2c4a84ae0fd26 extends Twig_Template
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
        $tags = array("set" => 34, "if" => 43, "for" => 65);
        $filters = array("length" => 37, "merge" => 106);
        $functions = array();

        try {
            $this->env->getExtension('sandbox')->checkSecurity(
                array('set', 'if', 'for'),
                array('length', 'merge'),
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
        $context["classes"] = array(0 => "views-table", 1 => "views-view-table", 2 => ("cols-" . twig_length_filter($this->env,         // line 37
(isset($context["header"]) ? $context["header"] : null))), 3 => ((        // line 38
(isset($context["responsive"]) ? $context["responsive"] : null)) ? ("responsive-enabled") : ("")), 4 => ((        // line 39
(isset($context["sticky"]) ? $context["sticky"] : null)) ? ("sticky-enabled") : ("")));
        // line 42
        echo "<table";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["attributes"]) ? $context["attributes"] : null), "addClass", array(0 => (isset($context["classes"]) ? $context["classes"] : null)), "method"), "html", null, true));
        echo ">
  ";
        // line 43
        if ((isset($context["caption_needed"]) ? $context["caption_needed"] : null)) {
            // line 44
            echo "    <caption>
    ";
            // line 45
            if ((isset($context["caption"]) ? $context["caption"] : null)) {
                // line 46
                echo "      ";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["caption"]) ? $context["caption"] : null), "html", null, true));
                echo "
    ";
            } else {
                // line 48
                echo "      ";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
                echo "
    ";
            }
            // line 50
            echo "    ";
            if (( !twig_test_empty((isset($context["summary"]) ? $context["summary"] : null)) ||  !twig_test_empty((isset($context["description"]) ? $context["description"] : null)))) {
                // line 51
                echo "      <details>
        ";
                // line 52
                if ( !twig_test_empty((isset($context["summary"]) ? $context["summary"] : null))) {
                    // line 53
                    echo "          <summary>";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["summary"]) ? $context["summary"] : null), "html", null, true));
                    echo "</summary>
        ";
                }
                // line 55
                echo "        ";
                if ( !twig_test_empty((isset($context["description"]) ? $context["description"] : null))) {
                    // line 56
                    echo "          ";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["description"]) ? $context["description"] : null), "html", null, true));
                    echo "
        ";
                }
                // line 58
                echo "      </details>
    ";
            }
            // line 60
            echo "    </caption>
  ";
        }
        // line 62
        echo "  ";
        if ((isset($context["header"]) ? $context["header"] : null)) {
            // line 63
            echo "    <thead>
      <tr>
        ";
            // line 65
            $context['_parent'] = $context;
            $context['_seq'] = twig_ensure_traversable((isset($context["header"]) ? $context["header"] : null));
            foreach ($context['_seq'] as $context["key"] => $context["column"]) {
                // line 66
                echo "          ";
                if ($this->getAttribute($context["column"], "default_classes", array())) {
                    // line 67
                    echo "            ";
                    // line 68
                    $context["column_classes"] = array(0 => "views-field", 1 => ("views-field-" . $this->getAttribute(                    // line 70
(isset($context["fields"]) ? $context["fields"] : null), $context["key"], array(), "array")));
                    // line 73
                    echo "          ";
                }
                // line 74
                echo "          <th";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($this->getAttribute($this->getAttribute($context["column"], "attributes", array()), "addClass", array(0 => (isset($context["column_classes"]) ? $context["column_classes"] : null)), "method"), "setAttribute", array(0 => "scope", 1 => "col"), "method"), "html", null, true));
                echo ">";
                // line 75
                if ($this->getAttribute($context["column"], "wrapper_element", array())) {
                    // line 76
                    echo "<";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "wrapper_element", array()), "html", null, true));
                    echo ">";
                    // line 77
                    if ($this->getAttribute($context["column"], "url", array())) {
                        // line 78
                        echo "<a href=\"";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "url", array()), "html", null, true));
                        echo "\" title=\"";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "title", array()), "html", null, true));
                        echo "\">";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "content", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "sort_indicator", array()), "html", null, true));
                        echo "</a>";
                    } else {
                        // line 80
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "content", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "sort_indicator", array()), "html", null, true));
                    }
                    // line 82
                    echo "</";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "wrapper_element", array()), "html", null, true));
                    echo ">";
                } else {
                    // line 84
                    if ($this->getAttribute($context["column"], "url", array())) {
                        // line 85
                        echo "<a href=\"";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "url", array()), "html", null, true));
                        echo "\" title=\"";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "title", array()), "html", null, true));
                        echo "\">";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "content", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "sort_indicator", array()), "html", null, true));
                        echo "</a>";
                    } else {
                        // line 87
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "content", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "sort_indicator", array()), "html", null, true));
                    }
                }
                // line 90
                echo "</th>
        ";
            }
            $_parent = $context['_parent'];
            unset($context['_seq'], $context['_iterated'], $context['key'], $context['column'], $context['_parent'], $context['loop']);
            $context = array_intersect_key($context, $_parent) + $_parent;
            // line 92
            echo "      </tr>
    </thead>
  ";
        }
        // line 95
        echo "  <tbody>
    ";
        // line 96
        $context['_parent'] = $context;
        $context['_seq'] = twig_ensure_traversable((isset($context["rows"]) ? $context["rows"] : null));
        foreach ($context['_seq'] as $context["_key"] => $context["row"]) {
            // line 97
            echo "      <tr";
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["row"], "attributes", array()), "html", null, true));
            echo ">
        ";
            // line 98
            $context['_parent'] = $context;
            $context['_seq'] = twig_ensure_traversable($this->getAttribute($context["row"], "columns", array()));
            foreach ($context['_seq'] as $context["key"] => $context["column"]) {
                // line 99
                echo "          ";
                if ($this->getAttribute($context["column"], "default_classes", array())) {
                    // line 100
                    echo "            ";
                    // line 101
                    $context["column_classes"] = array(0 => "views-field");
                    // line 105
                    echo "            ";
                    $context['_parent'] = $context;
                    $context['_seq'] = twig_ensure_traversable($this->getAttribute($context["column"], "fields", array()));
                    foreach ($context['_seq'] as $context["_key"] => $context["field"]) {
                        // line 106
                        echo "              ";
                        $context["column_classes"] = twig_array_merge((isset($context["column_classes"]) ? $context["column_classes"] : null), array(0 => ("views-field-" . $context["field"])));
                        // line 107
                        echo "            ";
                    }
                    $_parent = $context['_parent'];
                    unset($context['_seq'], $context['_iterated'], $context['_key'], $context['field'], $context['_parent'], $context['loop']);
                    $context = array_intersect_key($context, $_parent) + $_parent;
                    // line 108
                    echo "          ";
                }
                // line 109
                echo "          <td";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($this->getAttribute($context["column"], "attributes", array()), "addClass", array(0 => (isset($context["column_classes"]) ? $context["column_classes"] : null)), "method"), "html", null, true));
                echo ">";
                // line 110
                if ($this->getAttribute($context["column"], "wrapper_element", array())) {
                    // line 111
                    echo "<";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "wrapper_element", array()), "html", null, true));
                    echo ">
              ";
                    // line 112
                    $context['_parent'] = $context;
                    $context['_seq'] = twig_ensure_traversable($this->getAttribute($context["column"], "content", array()));
                    foreach ($context['_seq'] as $context["_key"] => $context["content"]) {
                        // line 113
                        echo "                ";
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["content"], "separator", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["content"], "field_output", array()), "html", null, true));
                        echo "
              ";
                    }
                    $_parent = $context['_parent'];
                    unset($context['_seq'], $context['_iterated'], $context['_key'], $context['content'], $context['_parent'], $context['loop']);
                    $context = array_intersect_key($context, $_parent) + $_parent;
                    // line 115
                    echo "              </";
                    echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["column"], "wrapper_element", array()), "html", null, true));
                    echo ">";
                } else {
                    // line 117
                    $context['_parent'] = $context;
                    $context['_seq'] = twig_ensure_traversable($this->getAttribute($context["column"], "content", array()));
                    foreach ($context['_seq'] as $context["_key"] => $context["content"]) {
                        // line 118
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["content"], "separator", array()), "html", null, true));
                        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute($context["content"], "field_output", array()), "html", null, true));
                    }
                    $_parent = $context['_parent'];
                    unset($context['_seq'], $context['_iterated'], $context['_key'], $context['content'], $context['_parent'], $context['loop']);
                    $context = array_intersect_key($context, $_parent) + $_parent;
                }
                // line 121
                echo "          </td>
        ";
            }
            $_parent = $context['_parent'];
            unset($context['_seq'], $context['_iterated'], $context['key'], $context['column'], $context['_parent'], $context['loop']);
            $context = array_intersect_key($context, $_parent) + $_parent;
            // line 123
            echo "      </tr>
    ";
        }
        $_parent = $context['_parent'];
        unset($context['_seq'], $context['_iterated'], $context['_key'], $context['row'], $context['_parent'], $context['loop']);
        $context = array_intersect_key($context, $_parent) + $_parent;
        // line 125
        echo "  </tbody>
</table>
";
    }

    public function getTemplateName()
    {
        return "core/themes/classy/templates/views/views-view-table.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  281 => 125,  274 => 123,  267 => 121,  259 => 118,  255 => 117,  250 => 115,  240 => 113,  236 => 112,  231 => 111,  229 => 110,  225 => 109,  222 => 108,  216 => 107,  213 => 106,  208 => 105,  206 => 101,  204 => 100,  201 => 99,  197 => 98,  192 => 97,  188 => 96,  185 => 95,  180 => 92,  173 => 90,  168 => 87,  158 => 85,  156 => 84,  151 => 82,  147 => 80,  137 => 78,  135 => 77,  131 => 76,  129 => 75,  125 => 74,  122 => 73,  120 => 70,  119 => 68,  117 => 67,  114 => 66,  110 => 65,  106 => 63,  103 => 62,  99 => 60,  95 => 58,  89 => 56,  86 => 55,  80 => 53,  78 => 52,  75 => 51,  72 => 50,  66 => 48,  60 => 46,  58 => 45,  55 => 44,  53 => 43,  48 => 42,  46 => 39,  45 => 38,  44 => 37,  43 => 34,);
    }

    public function getSource()
    {
        return "{#
/**
 * @file
 * Theme override for displaying a view as a table.
 *
 * Available variables:
 * - attributes: Remaining HTML attributes for the element.
 *   - class: HTML classes that can be used to style contextually through CSS.
 * - title : The title of this group of rows.
 * - header: The table header columns.
 *   - attributes: Remaining HTML attributes for the element.
 *   - content: HTML classes to apply to each header cell, indexed by
 *   the header's key.
 *   - default_classes: A flag indicating whether default classes should be
 *     used.
 * - caption_needed: Is the caption tag needed.
 * - caption: The caption for this table.
 * - accessibility_description: Extended description for the table details.
 * - accessibility_summary: Summary for the table details.
 * - rows: Table row items. Rows are keyed by row number.
 *   - attributes: HTML classes to apply to each row.
 *   - columns: Row column items. Columns are keyed by column number.
 *     - attributes: HTML classes to apply to each column.
 *     - content: The column content.
 *   - default_classes: A flag indicating whether default classes should be
 *     used.
 * - responsive: A flag indicating whether table is responsive.
 * - sticky: A flag indicating whether table header is sticky.
 *
 * @see template_preprocess_views_view_table()
 */
#}
{%
  set classes = [
    'views-table',
    'views-view-table',
    'cols-' ~ header|length,
    responsive ? 'responsive-enabled',
    sticky ? 'sticky-enabled',
  ]
%}
<table{{ attributes.addClass(classes) }}>
  {% if caption_needed %}
    <caption>
    {% if caption %}
      {{ caption }}
    {% else %}
      {{ title }}
    {% endif %}
    {% if (summary is not empty) or (description is not empty) %}
      <details>
        {% if summary is not empty %}
          <summary>{{ summary }}</summary>
        {% endif %}
        {% if description is not empty %}
          {{ description }}
        {% endif %}
      </details>
    {% endif %}
    </caption>
  {% endif %}
  {% if header %}
    <thead>
      <tr>
        {% for key, column in header %}
          {% if column.default_classes %}
            {%
              set column_classes = [
                'views-field',
                'views-field-' ~ fields[key],
              ]
            %}
          {% endif %}
          <th{{ column.attributes.addClass(column_classes).setAttribute('scope', 'col') }}>
            {%- if column.wrapper_element -%}
              <{{ column.wrapper_element }}>
                {%- if column.url -%}
                  <a href=\"{{ column.url }}\" title=\"{{ column.title }}\">{{ column.content }}{{ column.sort_indicator }}</a>
                {%- else -%}
                  {{ column.content }}{{ column.sort_indicator }}
                {%- endif -%}
              </{{ column.wrapper_element }}>
            {%- else -%}
              {%- if column.url -%}
                <a href=\"{{ column.url }}\" title=\"{{ column.title }}\">{{ column.content }}{{ column.sort_indicator }}</a>
              {%- else -%}
                {{- column.content }}{{ column.sort_indicator }}
              {%- endif -%}
            {%- endif -%}
          </th>
        {% endfor %}
      </tr>
    </thead>
  {% endif %}
  <tbody>
    {% for row in rows %}
      <tr{{ row.attributes }}>
        {% for key, column in row.columns %}
          {% if column.default_classes %}
            {%
              set column_classes = [
                'views-field'
              ]
            %}
            {% for field in column.fields %}
              {% set column_classes = column_classes|merge(['views-field-' ~ field]) %}
            {% endfor %}
          {% endif %}
          <td{{ column.attributes.addClass(column_classes) }}>
            {%- if column.wrapper_element -%}
              <{{ column.wrapper_element }}>
              {% for content in column.content %}
                {{ content.separator }}{{ content.field_output }}
              {% endfor %}
              </{{ column.wrapper_element }}>
            {%- else -%}
              {% for content in column.content %}
                {{- content.separator }}{{ content.field_output -}}
              {% endfor %}
            {%- endif %}
          </td>
        {% endfor %}
      </tr>
    {% endfor %}
  </tbody>
</table>
";
    }
}
