
Entity API module
-----------------
by Wolfgang Ziegler, nuppla@zites.net

This module extends the entity API of Drupal core in order to provide a unified
way to deal with entities and their properties. Additionally, it provides an
entity CRUD controller, which helps simplifying the creation of new entity types.


This is an API module. You only need to enable it if a module depends on it or
you are interested in using it for development.

This README is for interested developers. If you are not interested in developing,
you may stop reading now.

--------------------------------------------------------------------------------
                                Entity API
--------------------------------------------------------------------------------

  * The module provides API functions allowing modules to create, save, delete
    or to determine access for entities based on any entity type, for which the
    necessary metadata is available. The module comes with integration for all
    core entity types, as well as for entities provided via the Entity CRUD API
    (see below). However for any other entity type implemented by a contrib
    module, the module integration has to be provided the contrib module itself. 

  * Thus the module provides API functions like entity_save(), entity_create(),
    entity_delete(), entity_view() and entity_access() among others. 
    entity_load(), entity_label() and entity_uri() are already provided by
    Drupal core.

 *  For more information about how to provide this metadata, have a look at the
    API documentation, i.e. entity_metadata_hook_entity_info().
    
--------------------------------------------------------------------------------
               Entity CRUD API - Providing new entity types
--------------------------------------------------------------------------------

 * This API helps you defining a new entity type. It provides an entity
   controller, which implements full CRUD functionality for your entities.
   
 * To make use of the CRUD functionality you may just use the API functions
   entity_create(), entity_delete() and entity_save(). 

   Alternatively you may specify a class to use for your entities, for which the
   "Entity" class is provided. In particular, it is useful to extend this class
   in order to easily customize the entity type, e.g. saving.
   
 * The controller supports fieldable entities, however it does not yet support
   revisions. There is also a controller which supports implementing exportable
   entities.

 * The Entity CRUD API helps with providing additional module integration too,
   e.g. exportable entities are automatically integrate with the Features
   module. These module integrations are implemented in separate controller
   classes, which may be overridden and deactivated on their own.
   
 * There is also an optional ui controller class, which assits with providing an
   administrative UI for managing entities of a certain type.
   
 * For more details check out the documentation in the drupal.org handbook
   http://drupal.org/node/878804 as well as the API documentation, i.e.
   entity_crud_hook_entity_info().
 

 Basic steps to add a new entity type:
---------------------------------------
 
  * You might want to study the code of the "entity_test.module".
  
  * Describe your entities db table as usual in hook_schema().
  
  * Just use the "Entity" directly or extend it with your own class.
    To see how to provide a separate class have a look at the "EntityClass" from
    the "entity_test.module". 
  
  * Implement hook_entity_info() for your entity. At least specifiy the
    controller class (EntityAPIController, EntityAPIControllerExportable or your
    own), your db table and your entity's keys.
    Again just look at "entity_test.module"'s hook_entity_info() for guidance.

  * If you want your entity to be fieldable just set 'fieldable' in
    hook_entity_info() to TRUE. The field API attachers are called automatically
    in the entity CRUD functions then.
    
  * The entity API is able to deal with bundle objects too (e.g. the node type
    object). For that just specify another entity type for the bundle objects
    and set the 'bundle of' property for it.
    Again just look at "entity_test.module"'s hook_entity_info() for guidance.

  * Schema fields marked as 'serialized' are automatically unserialized upon
    loading as well as serialized on saving. If the 'merge' attribute is also
    set to TRUE the unserialized data is automatically "merged" into the entity.

  * Further details can be found at http://drupal.org/node/878804.    
    


--------------------------------------------------------------------------------
                Entity Properties & Entity metadata wrappers
--------------------------------------------------------------------------------

  * This module introduces a unique place for metadata about entity properties:
    hook_entity_property_info(), whereas hook_entity_property_info() may be
    placed in your module's {YOUR_MODULE}.info.inc include file. For details
    have a look at the API documentation, i.e. hook_entity_property_info() and
    at http://drupal.org/node/878876.

  * The information about entity properties contains the data type and callbacks
    for how to get and set the data of property. That way the data of an entity
    can be easily re-used, e.g. to export it into other data formats like XML.
 
  * For making use of this information (metadata) the module provides some
    wrapper classes which ease getting and setting values. The wrapper supports
    chained usage for retrieving wrappers of entity properties, e.g. to get a
    node author's mail address one could use:
    
       $wrapper = entity_metadata_wrapper('node', $node);
       $wrapper->author->mail->value();
       
    To update the user's mail address one could use
    
       $wrapper->author->mail->set('sepp@example.com');
       
       or
       
       $wrapper->author->mail = 'sepp@example.com'; 
       
    The wrappers always return the data as described in the property
    information, which may be retrieved directly via entity_get_property_info()
    or from the wrapper:
    
       $mail_info = $wrapper->author->mail->info();
       
    In order to force getting a textual value sanitized for output one can use, 
    e.g.

       $wrapper->title->value(array('sanitize' => TRUE));
       
    to get the sanitized node title. When a property is already returned
    sanitized by default, like the node body, one possibly wants to get the
    not-sanitized data as it would appear in a browser for other use-cases.
    To do so one can enable the 'decode' option, which ensures for any sanitized
    data the tags are stripped and HTML entities are decoded before the property
    is returned:

       $wrapper->body->value->value(array('decode' => TRUE));

    That way one always gets the data as shown to the user. However if you
    really want to get the raw, unprocessed value, even for sanitized textual
    data, you can do so via:

      $wrapper->body->value->raw();

 