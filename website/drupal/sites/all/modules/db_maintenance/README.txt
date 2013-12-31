Database Maintenance Module

Author:
David Kent Norman
http://deekayen.net/

DESCRIPTION
-----------
Runs an optimization query on selected tables for your database.
This should probably NOT be used to optimize every table in your
Drupal installation.

Per PostgreSQL documentation's recommendation, this module does
not use the VACUUM FULL operation that locks the tables; this
module can operate in parallel with normal reading and writing
of PostgreSQL tables.

MySQL's OPTIMIZE query uses table locks.
http://drupal.org/node/91621 is an example
where the table locking during the OPTIMIZE procedure could
interfere with basic functionality of your site when using MySQL.

Keep it down to tables where there is lot of data movement like
accesslog, cache, sessions, and watchdog. It's probably better to
make a separate, more infrequent cron as part of your regular
server management if you want to optimize your node tables.

INSTALLATION
------------
See INSTALL.txt in this directory.