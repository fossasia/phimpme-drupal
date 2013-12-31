<?php

/**
 * Generate new context classes by argument settings on the view.
 */
function ctools_views_get_argument_context($argument) {
  if ($argument['type'] == 'context') {
    if (strpos($argument['context'], '.')) {
      list($context, $converter) = explode('.', $argument['context'], 2);
    }
    else {
      // Backwards-compat for before we had a system for delimiting the data
      // we retrieve out of context objects.
      $context = $argument['context'];
    }
    if ($context == 'term' || $context == 'vocabulary') {
      $context = 'entity:taxonomy_' . $context;
    }
    elseif ($entity = entity_get_info($context)) {
      $context = 'entity:' . $context;
    }
    $class = 'ctools_context_' . (empty($argument['context_optional']) ? 'required' : 'optional');
    $new_context = new $class($argument['label'], $context);
    return $new_context;
  }
}
