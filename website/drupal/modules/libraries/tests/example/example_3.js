
/**
 * @file
 * Test JavaScript file for Libraries loading.
 *
 * Replace the text in the 'libraries-test-javascript' div. See README.txt for
 * more information.
 */

(function ($) {

Drupal.behaviors.librariesTest = {
  attach: function(context, settings) {
    $('.libraries-test-javascript').text('If this text shows up, example_3.js was loaded successfully.')
  }
};

})(jQuery);
