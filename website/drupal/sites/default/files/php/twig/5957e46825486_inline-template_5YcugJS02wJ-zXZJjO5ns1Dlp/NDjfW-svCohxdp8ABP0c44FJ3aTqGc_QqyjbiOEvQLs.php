<?php

/* {# inline_template_start #}<div class="permission"><span class="title">{{ title }}</span>{% if description or warning %}<div class="description">{% if warning %}<em class="permission-warning">{{ warning }}</em> {% endif %}{{ description }}</div>{% endif %}</div> */
class __TwigTemplate_c49bd5ef621a21b509bb7e01331729d64d69e20c76562dd003290542cdb73e8b extends Twig_Template
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
        $tags = array("if" => 1);
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

        // line 1
        echo "<div class=\"permission\"><span class=\"title\">";
        echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["title"]) ? $context["title"] : null), "html", null, true));
        echo "</span>";
        if (((isset($context["description"]) ? $context["description"] : null) || (isset($context["warning"]) ? $context["warning"] : null))) {
            echo "<div class=\"description\">";
            if ((isset($context["warning"]) ? $context["warning"] : null)) {
                echo "<em class=\"permission-warning\">";
                echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["warning"]) ? $context["warning"] : null), "html", null, true));
                echo "</em> ";
            }
            echo $this->env->getExtension('sandbox')->ensureToStringAllowed($this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["description"]) ? $context["description"] : null), "html", null, true));
            echo "</div>";
        }
        echo "</div>";
    }

    public function getTemplateName()
    {
        return "{# inline_template_start #}<div class=\"permission\"><span class=\"title\">{{ title }}</span>{% if description or warning %}<div class=\"description\">{% if warning %}<em class=\"permission-warning\">{{ warning }}</em> {% endif %}{{ description }}</div>{% endif %}</div>";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  43 => 1,);
    }

    public function getSource()
    {
        return "{# inline_template_start #}<div class=\"permission\"><span class=\"title\">{{ title }}</span>{% if description or warning %}<div class=\"description\">{% if warning %}<em class=\"permission-warning\">{{ warning }}</em> {% endif %}{{ description }}</div>{% endif %}</div>";
    }
}
