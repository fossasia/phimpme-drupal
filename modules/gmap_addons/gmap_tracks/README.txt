
This will let you use PLT tracks.

--- macro extensions ---
  track - Draws a line based on the points in the .plt file.

[gmap |track=red/5/0.7:track.plt]

@@@ The part before the : is currently not working.


--- gmap_tracks extensions ---

// Other parts of $map left out for clarity.
<?php
$map = array(
  // Array of tracks to load and inject.
  // See below for format.
  'tracks' => array(),
);

$map['tracks'][] = array(

  // Style (currently broken.)
  // @@@ Originally was keyed? Original doc refers to "'type', 'color', etc... same as the shape line."
  'style' => array(),

  // .PLT file to load. Filename is relative to the drupal base url.
  'filename' => 'files/testing.plt',
);
?>
