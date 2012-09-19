
Description
-----------

GMap is an API and a related set of modules which allows the integration of Google Maps with a Drupal site.

gmap.module: The main module. Contains the API, the basic map functions, and an input filter to create macros into working maps with minimal effort.

gmap_location.module: Provides map features for Location.module (v2 and v3).

gmap_macro_builder.module: End-user UI for easily creating GMap macros.

gmap_taxonomy.module: API and utility for changing map markers of for points from Location.module based on taxonomy terms.

gmap_views.module: GMap <-> Views.module interface. Provides a "GMap View" view type (like "Teaser List" or "Table View") to display items in a View on a Google Map.

Installation
------------

* To install, follow the general directions available at:
http://drupal.org/getting-started/5/install-contrib/modules

* You will need a Google Maps API key for your website. You can get one at:
http://www.google.com/apis/maps/signup.html
Enter your Google Maps API key in the GMap settings page (admin/settings/gmap).

* You may need to make changes to your theme so that Google Maps can display correctly. See the section on "Google Maps and XHTML" below.

* If you would like to use GMap macros directly in nodes, you will need to add the GMap Macro filter to an input format (or create a new input format that includes it). Read http://drupal.org/node/213156 for more information on input formats.

If you are using the HTML filter, it will need to appear BEFORE the GMap filter; otherwise the HTML filter will remove the GMap code. To modify the order of filters in an input format, go to the "Rearrange" tab on the input format's configuration page (Administer > Site configuration > Input formats, then click "Configure" by your format).

* If you would like to use third party functionality such as mouse wheel support
or Clusterer, read thirdparty/README.txt for download links and instructions.

* If you are translating this module to a different language also see the gmap.strings.php file for further installation instructions.  This is required for translation of marker type strings to work.


Google Maps and XHTML
---------------------

* Google Maps may have rendering issues when not using an XHTML doctype; Google recommends that your theme be standards-compliant XHTML, and suggests the following DOCTYPE:

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

**editor's note: remove notes on VML when the api version is forced to >= 2.91.
* For polylines to work in Internet Explorer, you will need to add
the VML namespace to your <html> tag. Google recommends the following:

<html xmlns="http://www.w3.org/1999/xhtml" xmlns:v="urn:schemas-microsoft-com:vml">

See http://code.google.com/apis/maps/documentation/index.html#XHTML_and_VML
for more information. This won't affect you unless you're displaying lines on your Google Maps.


Macro
-----
**NOTE: This section needs revision still!**
**does it though? --bec

"GMap macros" are text-based representations of Google Maps. A GMap macro can be created with the GMap Macro Builder tool or by hand. Any map parameter not specified by a macro will use the defaults from the GMap settings page (Administer > Site configuration > GMap). There are several places where you may use Gmap macros, mainly:
1) GMap settings, like the GMap Location module's settings page.
2) Any node where the GMap filter is enabled (as part of the input format).

A GMap macro will look something like this (see the advanced help pages (or the files in help/ in the module folder) for syntax details):
[gmap markers=blue::41.902277040963696,-87.6708984375 |zoom=5 |center=42.94033923363183,-88.857421875 |width=300px |height=200px |control=Small |type=Map]

The GMap Macro Builder is a GUI for creating GMap macros; you may use it to create a map with points, lines, polygons, and circles, and then copy and paste the resulting macro text. After you insert the macro into a node, you can edit it using raw values that you get from elsewhere to create a different set of points or lines on the map.

If you've enabled the gmap_macro_builder.module, you can access the GMap macro builder at the 'map/macro' path (there will be "Build a GMap macro" link in your Navigation menu).

Note that there are many more options you can set on your maps if you are willing to edit macros by hand. For example, you may add KML or GeoRSS overlays to GMaps, but this option isn't available through the macro builder. Again, see the advanced help pages for syntax details.


User and node maps
------------------

User and node location maps are made available by gmap_location.module, and work in conjunction with location.module. Any user that has the 'view user map' or 'view node map' permission can see the user or node maps, respectively. These are maps with a marker for every user or node, and are located at the 'map/user' and 'map/node' paths (links to these maps are placed in the Navigation menu).

Users with the 'view user location details' permission can click markers on the User map to see information on individual users.

GMap Location also provides two map blocks that work with node locations: a "Location map" block that displays the location markers associated with the node being viewed, and an "Author map" block that displays a marker representing the node author's location.

GMap Location provides an interactive Google Map to the Location module for setting the latitude and longitude; users must have Location's "submit latitude/longitude" to use this feature.

Markers
-------

The 'markers' directory contains many useful markers, and you can add custom markers by placing PNG files (markers must be in PNG format) in the markers directory and creating a ".ini" file for them. Use the existing .ini files as a guide--start with "markers/colors.ini".

If you have created custom markers and are willing to release them under the GPL for inclusion in GMap, please file an issue in the issue queue at: http://drupal.org/project/issues/gmap

Demo
----

GMap Macros (GMap module):
http://www.webgeer.com/gmapdemo

GMap Macro Builder module:
http://gmap.chicagotech.org/map/macro

GMap Location module:
http://photo-tips.ca/

GMap Location module user map:
http://www.webgeer.com/map/users

Credit
------

Gmap for Drupal is part of the Mapedelic suite - a collection of Drupal modules providing a variety of mapping and geographic information functionality.  Work on Gmap for Drupal is sponsored by the Chicago Technology Cooperative.
http://chicagotech.org

GMap was refactored and updated for Drupal 5 by Brandon Bergren (Bdragon).
http://drupal.org/user/53081

GMap for Drupal 4.6-4.7 was written by James Blake
http://www.webgeer.com/James

Thanks to the following for their contributions:
* Robert Douglass, who revamped crucial parts and cleaned up many smaller things.
* Paul Rollo, who explained how to include a location map in a block.
* Nick Jehlen, who commissioned much of the initial work of gmap_location.module for the website http://enoughfear.com.
