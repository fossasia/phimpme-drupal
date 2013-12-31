In order to install this test system, please follow the following steps:
 
1. Restore database from sql file in drupal/website/sql folder
  a. Login to mysql use command line:
     mysql -uroot -p
  b. create phimpme_drupal database : 
     CREATE DATABASE phimpme_drupal;
     exit;

  c. Restore database :
     mysql -u root -p phimpme_drupal < link to phimpme_drupal.sql file

     e.g :
     mysql -u root -p phimpme_drupal < ~/phimpme.cms/drupal/website/Sql/phimpme_drupal.sql

2. Move drupal folder to /var/www/ or your specific lolalhost directory and change permission for this folder.
    - Command: cd /var/www/drupal
    - Command: sudo chmod -R 777 drupal/

3. Go to /var/www/drupal/sites/default and Open settings.php change website information in line 204 (change hostname, database name, database password).

$databases['default']['default'] = array(
      'driver' => 'mysql',
      'database' => 'phimpme_drupal',
      'username' => 'your mysql user_name',
      'password' => 'your mysql password',
      'host' => 'localhost',
      'prefix' => '',
    );

Save the file.
  
4. Test result: localhost/drupal 
    -Turn off clean urls to use in local, open browser type: localhost/drupal?q=admin/config/search/clean-urls
    - if have request login with admin permission, try login with : 
      + UserName : test
      + Password : test

5. Test login to drupal website with username/password :
    - Username : test
    - Password : test

6. To test Drupal website with Phimp.Me app
    - Connect your phone and your computer same network.
    - Type ifconfig to detect your ip address.
       Command: ifconfig
    Read IP address and typ the following into the phimpme drupal form on the app:
    Username: test
    Password: test
    Services link: e.g. on your localhost with the following IP http://192.168.1.19/drupal/?q=api/


