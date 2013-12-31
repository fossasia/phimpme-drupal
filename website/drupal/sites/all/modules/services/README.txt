
Goals
==============
- Create a unified Drupal API for web services to be exposed in a variety of 
  different server formats.  
- Provide a service browser to be able to test methods.
- Allow distribution of API keys for developer access.

Documentation
==============
http://drupal.org/node/109782

Installation
============
If you are using the rest server you will need to download the latest version of SYPC and Mimeparse:
wget http://spyc.googlecode.com/svn/trunk/spyc.php -O  servers/rest_server/lib/spyc.php

Once downloaded you need to add spyc.php to the rest_server/lib folder which exists under
the location you have installed services in.
