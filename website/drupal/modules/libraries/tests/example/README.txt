
Example library

Version 1

This file is an example file to test version detection.

The various other files in this directory are to test the loading of JavaScript,
CSS and PHP files.
- JavaScript: The filenames of the JavaScript files are asserted to be in the
  raw HTML via SimpleTest. Since the filename could appear, for instance, in an
  error message, this is not very robust. Explicit testing of JavaScript,
  though, is not yet possible with SimpleTest. To allow for easier debugging, we
  place the following text on the page:
  "If this text shows up, no JavaScript test file was loaded."
  This text is replaced via JavaScript by a text of the form:
  "If this text shows up, [[file] was loaded successfully."
  [file] is either 'example_1.js', 'example_2.js', 'example_3.js',
  'example_4.js' or 'libraries_test.js'. If you have SimpleTest's verbose mode
  enabled and see the above text in one of the debug pages, the noted JavaScript
  file was loaded successfully.
- CSS: The filenames of the CSS files are asserted to be in the raw HTML via
  SimpleTest. Since the filename could appear, for instance, in an error
  message, this is not very robust. Explicit testing of CSS, though, is not yet
  possible with SimpleTest. Hence, the CSS files, if loaded, make the following
  text a certain color:
  "If one of the CSS test files has been loaded, this text will be colored:
  - example_1: red
  - example_2: green
  - example_3: orange
  - example_4: blue
  - libraries_test: purple"
  If you have SimpleTest's verbose mode enabled, and see the above text in a
  certain color (i.e. not in black), a CSS file was loaded successfully. Which
  file depends on the color as referenced in the text above.
- PHP: The loading of PHP files is tested by defining a dummy function in the
  PHP files and then checking whether this function was defined using
  function_exists(). This can be checked programatically with SimpleTest.
The loading of integration files is tested with the same method. The integration
files are libraries_test.js, libraries_test.css, libraries_test.inc and are
located in the tests directory alongside libraries_test.module (i.e. they are
not in the same directory as this file).
