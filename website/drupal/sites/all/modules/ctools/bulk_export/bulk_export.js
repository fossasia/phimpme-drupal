
/**
 * @file
 * CTools Bulk Export javascript functions.
 */

(function ($) {

Drupal.behaviors.CToolsBulkExport = {
  attach: function (context) {

    $('#bulk-export-export-form .vertical-tabs-pane', context).drupalSetSummary(function (context) {

      // Check if any individual checkbox is checked.
      if ($('.bulk-selection input:checked', context).length > 0) {
        return Drupal.t('Exportables selected');
      }

      return '';
    });

    // Special bind click on the select-all checkbox.
    $('.select-all').bind('click', function(context) {
      $(this, '.vertical-tabs-pane').drupalSetSummary(context);
    });
  }
};

})(jQuery);
