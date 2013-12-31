
(function ($) {

Drupal.behaviors.fileDisplayStatus = {
  attach: function (context, settings) {
    $('#file-displays-status-wrapper input.form-checkbox', context).once('display-status', function () {
      var $checkbox = $(this);
      // Retrieve the tabledrag row belonging to this display.
      var $row = $('#' + $checkbox.attr('id').replace(/-status$/, '-weight'), context).closest('tr');
      // Retrieve the vertical tab belonging to this display.
      var tab = $('#' + $checkbox.attr('id').replace(/-status$/, '-settings'), context).data('verticalTab');

      // Bind click handler to this checkbox to conditionally show and hide the
      // display's tableDrag row and vertical tab pane.
      $checkbox.bind('click.displayStatusUpdate', function () {
        if ($checkbox.is(':checked')) {
          $row.show();
          if (tab) {
            tab.tabShow().updateSummary();
          }
        }
        else {
          $row.hide();
          if (tab) {
            tab.tabHide().updateSummary();
          }
        }
        // Restripe table after toggling visibility of table row.
        Drupal.tableDrag['file-displays-order'].restripeTable();
      });

      // Attach summary for configurable displays (only for screen-readers).
      if (tab) {
        tab.fieldset.drupalSetSummary(function (tabContext) {
          return $checkbox.is(':checked') ? Drupal.t('Enabled') : Drupal.t('Disabled');
        });
      }

      // Trigger our bound click handler to update elements to initial state.
      $checkbox.triggerHandler('click.displayStatusUpdate');
    });
  }
};

})(jQuery);
