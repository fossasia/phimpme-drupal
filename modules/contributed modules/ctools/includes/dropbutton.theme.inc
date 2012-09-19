<?php
// $Id$

/**
 * @file
 * Provide a javascript based dropbutton menu.
 *
 * The dropbutton menu will show up as a button with a clickable twisty pointer
 * to the right. When clicked the button will expand, showing the list of links.
 *
 * The dropbutton will stay open until either the user has moved the mouse
 * away from the box for > .5 seconds, or can be immediately closed by
 * clicking the twisty again. The code is smart enough that if the mouse
 * moves away and then back within the .5 second window, it will not
 * re-close.
 *
 * Multiple dropbuttons can be placed per page.
 *
 * If only one link is passed to the theme function, the function will render
 * a ctools-button with no twisty.  The twisty is only rendered when 2 or more
 * links are provided. The wrapping element will be classed with both
 * ctools-button and ctools-dropbutton when a dropbutton is rendered.
 *
 * If the user does not have javascript enabled, the link will not appear,
 * and instead by default the list of links will appear as a normal inline
 * list.
 *
 * The menu is minimally styled by default, and to make it look different
 * will require your own CSS. You can apply your own class to the
 * dropbutton to help style it.
 *
 * The twisty is rendered as a border on a widthless and heightless element.
 * There is no image for the twisty.
 * The color of the twisty is the color of the link by default. To adjust the
 * size of the twisty, adjust the border widths on .ctools-twisty. The adjust
 * the color of the twisty, assign a new color to the .ctools-button class or
 * assign a color to .ctools-twisty. You shouldn't need to adjust border-color.
 *
 * Use the theme() function to render dropbutton e.g.
 * theme('links__ctools_dropbutton', array()) where array contains a renderable
 * array of links.
 */

/**
 * Delegated implementation of hook_theme()
 */
function ctools_dropbutton_theme(&$items) {
  $items['links__ctools_dropbutton'] = array(
    'variables' => array('title' => NULL, 'links' => NULL, 'image' => FALSE, 'class' => NULL),
    'file' => 'includes/dropbutton.theme.inc',
  );
}

/**
 * Create a dropbutton menu.
 *
 * @param $title
 *   The text to place in the clickable area to activate the dropbutton. This
 *   text is indented to -9999px by default.
 * @param $links
 *   A list of links to provide within the dropbutton, suitable for use
 *   in via Drupal's theme('links').
 * @param $image
 *   If true, the dropbutton link is an image and will not get extra decorations
 *   that a text dropbutton link will.
 * @param $class
 *   An optional class to add to the dropbutton's container div to allow you
 *   to style a single dropbutton however you like without interfering with
 *   other dropbuttons.
 */
function theme_links__ctools_dropbutton($vars) {
  // Check to see if the number of links is greater than 1;
  // otherwise just treat this like a button.
  if (!empty($vars['links'])) {
    $is_drop_button = (count($vars['links']) > 1);

    // Add needed files
    if ($is_drop_button) {
      ctools_add_js('dropbutton');
      ctools_add_css('dropbutton');
    }
    ctools_add_css('button');

    // Provide a unique identifier for every button on the page.
    static $id = 0;
    $id++;

    // Wrapping div
    $class = 'ctools-no-js';
    $class .= ($is_drop_button) ? ' ctools-dropbutton' : '';
    $class .= ' ctools-button';
    if (!empty($vars['class'])) {
      $class .= ($vars['class']) ? (' ' . implode(' ', $vars['class'])) : '';
    }

    $output = '';

    $output .= '<div class="' . $class . '" id="ctools-button-' . $id . '">';

    // Add a twisty if this is a dropbutton
    if ($is_drop_button) {
      $vars['title'] = ($vars['title'] ? check_plain($vars['title']) : t('open'));

      $output .= '<div class="ctools-link">';
      if ($vars['image']) {
        $output .= '<a href="#" class="ctools-twisty ctools-image">' . $vars['title'] . '</a>';
      }
      else {
        $output .= '<a href="#" class="ctools-twisty ctools-text">' . $vars['title'] . '</a>';
      }
      $output .= '</div>'; // ctools-link
    }

    // The button content
    $output .= '<div class="ctools-content">';

    // Check for attributes. theme_links expects an array().
    $vars['attributes'] = (!empty($vars['attributes'])) ? $vars['attributes'] : array();

    // Remove the inline and links classes from links if they exist.
    // These classes are added and styled by Drupal core and mess up the default
    // styling of any link list.
    if (!empty($vars['attributes']['class'])) {
      $classes = $vars['attributes']['class'];
      foreach ($classes as $key => $class) {
        if ($class === 'inline' || $class === 'links') {
          unset($vars['attributes']['class'][$key]);
        }
      }
    }

    // Call theme_links to render the list of links.
    $output .= theme_links(array('links' => $vars['links'], 'attributes' => $vars['attributes'], 'heading' => ''));
    $output .= '</div>'; // ctools-content
    $output .= '</div>'; // ctools-dropbutton
    return $output;
  }
  else {
    return '';
  }
}

