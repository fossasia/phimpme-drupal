#!/bin/bash

# Run this from the terminal inside a drupal root folder
# i.e. DRUPAL_ROOT_DIR/sites/all/modules/contrib/ctools/tests/ctools.drush.sh

function stamp {
  echo ==============
  echo timestamp : `date`
  echo ==============
}

DRUPAL_ROOT=`drush dd`
MODULE_DIR="$DRUPAL_ROOT/sites/all/modules"
MODULE_NAME="ctools_drush_test"

stamp

echo 'Enabling views module.'
drush en views ctools --yes

stamp
echo 'Reading all export info'
drush ctools-export-info

stamp
echo 'Reading all export info with format'
drush ctools-export-info --format=json

stamp
echo 'Reading tables only from export info'
drush ctools-export-info --tables-only

stamp
echo 'Reading tables only from export info with format'
drush ctools-export-info --tables-only --format=json

stamp
echo 'Reading all disabled exportables'
drush ctools-export-info --disabled

stamp
echo 'Enabling all default views'
drush ctools-export-enable views_view --yes

stamp
echo 'View all default views export data'
drush ctools-export-view views_view --yes

stamp
echo 'View default "archive" view export data'
drush ctools-export-view views_view archive

stamp
echo 'Disable default "archive" view'
drush ctools-export-disable views_view archive

stamp
echo 'Enable default "archive" view'
drush ctools-export-enable views_view archive

stamp
echo 'Reading all enabled exportables (archive disabled)'
drush ctools-export-info

stamp
echo 'Disabling all default views'
drush ctools-export-disable views_view --yes

stamp
echo 'Revert all default views'
drush ctools-export-revert views_view --yes

stamp
echo 'Bulk export all objects'
drush ctools-export $MODULE_NAME --subdir='tests' --choice=1

stamp
echo 'Show all files in created folder'
ls -lAR "$MODULE_DIR/tests/$MODULE_NAME"

stamp
echo 'Removing exported object files'
rm -Rf $MODULE_DIR/tests
