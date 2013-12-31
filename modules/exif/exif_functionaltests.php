<?php
abstract class ExifFunctionalTestCase extends DrupalWebTestCase {
  protected $privileged_user;
  /**
   * Create a new file field.
   *
   * @param $name
   *   The name of the new field (all lowercase), exclude the "field_" prefix.
   * @param $type_name
   *   The node type that this field will be added to.
   * @param $field_settings
   *   A list of field settings that will be added to the defaults.
   * @param $instance_settings
   *   A list of instance settings that will be added to the instance defaults.
   * @param $widget_settings
   *   A list of widget settings that will be added to the widget defaults.
   */
  function createFileField($name, $type_name, $field_settings = array(), $instance_settings = array(), $widget_settings = array()) {
    $field = array(
      'field_name' => $name,
      'type' => 'file',
      'settings' => array(),
      'cardinality' => !empty($field_settings['cardinality']) ? $field_settings['cardinality'] : 1,
    );
    $field['settings'] = array_merge($field['settings'], $field_settings);
    field_create_field($field);

    $this->attachFileField($name, 'node', $type_name, $instance_settings, $widget_settings);
  }

  /**
   * Attach a file field to an entity.
   *
   * @param $name
   *   The name of the new field (all lowercase), exclude the "field_" prefix.
   * @param $entity_type
   *   The entity type this field will be added to.
   * @param $bundle
   *   The bundle this field will be added to.
   * @param $field_settings
   *   A list of field settings that will be added to the defaults.
   * @param $instance_settings
   *   A list of instance settings that will be added to the instance defaults.
   * @param $widget_settings
   *   A list of widget settings that will be added to the widget defaults.
   */
  function attachFileField($name, $entity_type, $bundle, $instance_settings = array(), $widget_settings = array()) {
    $instance = array(
      'field_name' => $name,
      'label' => $name,
      'entity_type' => $entity_type,
      'bundle' => $bundle,
      'required' => !empty($instance_settings['required']),
      'settings' => array(),
      'widget' => array(
        'type' => 'file_generic',
        'settings' => array(),
    ),
    );
    $instance['settings'] = array_merge($instance['settings'], $instance_settings);
    $instance['widget']['settings'] = array_merge($instance['widget']['settings'], $widget_settings);
    field_create_instance($instance);
  }

  /**
   * Get the fid of the last inserted file.
   */
  function getLastFileId() {
    return (int) db_query('SELECT MAX(fid) FROM {file_managed}')->fetchField();
  }
  public function addNewFieldToContentType($contenttype,$fieldLabel,$fieldName,$fieldType,$fieldWidget,$fieldSettings = array(),$settings = array()) {
    $edit = array();
    $edit["fields[_add_new_field][label]"]=$fieldLabel;
    $edit["fields[_add_new_field][field_name]"]=$fieldName;
    $edit["fields[_add_new_field][type]"]=$fieldType;
    $edit["fields[_add_new_field][widget_type]"]=$fieldWidget;
    $this->drupalPost("admin/structure/types/manage/{$contenttype->type}/fields",$edit,t("Save"));
    $this->drupalPost(NULL,$fieldSettings,"Save field settings");
    $this->drupalPost(NULL,$settings,"Save settings");
  }



  public function addExistingFieldToContentType($contenttype,$fieldLabel,$fieldName,$fieldWidget,$fieldSettings = array()) {
    $edit = array();
    $edit["fields[_add_existing_field][label]"]=$fieldLabel;
    $edit["fields[_add_existing_field][field_name]"]='field_'.$fieldName;
    $edit["fields[_add_existing_field][widget_type]"]=$fieldWidget;
    $this->drupalPost("admin/structure/types/manage/{$contenttype->type}/fields",$edit,t("Save"));
    $this->drupalPost(NULL,$fieldSettings,"Save settings");
  }

  public function manageDisplay($contenttype,$fieldName,$label = 'hidden',$format='hidden',$displays = array('full','teaser')) {
    $edit = array();
    //accepted values are : above, inline, hidden
    $edit["fields[field_{$fieldName}][label]"]=$label;
    //accepted values for term are : taxonomy_term_reference_link, taxonomy_term_reference_plain, hidden
    //accepted value for text are : text_plain, hidden
    $edit["fields[field_{$fieldName}][type]"]=$format;
    foreach ( $displays as  $key => $display ) {
      $edit["view_modes_custom[{$display}]"]=$display;
    }
    $this->drupalPost("admin/structure/types/manage/{$contenttype->type}/display",$edit,t("Save"));
  }

  public function activateMetadataOnObjectTypes($objectTypes = array(),$vid = 1) {
    if (!empty($objectTypes)) {
      $edit = array();
      foreach ( $objectTypes as  $key => $objectType ) {
        $edit["exif_nodetypes[{$objectType}]"]=$objectType;
      }
      $edit["exif_vocabulary"]=$vid;//already existing vocabulaty tags - vid = 1
      $this->drupalPost("admin/config/media/exif/settings",$edit,t("Save configuration"));
    }
  }

  public function setUp() {
    parent::setUp($this->initModules());  // Enable any modules required for the test
    // Create and log in our privileged user.
    $settings = array (
      'type' => 'photo',
      'name' => 'Photography',
      'base' => 'node_content',
      'description' => 'show a photo and the metadata',
      'help' => '',
      'title_label' => 'Title',
      'body_label' => 'Body',
      'has_title' => 1,
      'has_body' => 0,          	
    );
    $type = $this->drupalCreateContentType($settings);
    //Types
    //image/Image
    //taxonomy_term_reference/Term reference
    //text/Text
    //Widgets
    //exif_readonly/metadata from image
    //image_image/Image

    $this->privileged_user = $this->drupalCreateUser(array(
      'administer image metadata',
      'administer content types',
      'administer nodes',
      'administer taxonomy',
	  'create photo content',	
	  'edit any photo content'
	  ));
	  $this->assertNotEqual(FALSE,$this->privileged_user,"user created.",'Exif');
	  $this->drupalLogin($this->privileged_user);

	  $this->addExistingFieldToContentType($type,'photo','image','image_image');
	  $this->addNewFieldToContentType($type,'model','exif_model','taxonomy_term_reference','exif_readonly');
	  $this->addNewFieldToContentType($type,'keywords','iptc_keywords','taxonomy_term_reference','exif_readonly',array(),array("field[cardinality]" => -1));
	  $this->addNewFieldToContentType($type,'usercomment','exif_usercomment','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'latitude','gps_gpslatitude','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'longitude','gps_gpslongitude','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'latituderef','gps_gpslatituderef','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'longituderef','gps_gpslongituderef','text','exif_readonly');	 
	  $this->addNewFieldToContentType($type,'artist','ifd0_artist','taxonomy_term_reference','exif_readonly'); 
	  $this->addNewFieldToContentType($type,'title','ifd0_title','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'flash','exif_flash','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'date','exif_datetime','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'DateObject','ifd0_datetime','datetime','exif_readonly');
	  $this->addNewFieldToContentType($type,'fileDate','exif_filedatetime','text','exif_readonly');
	  $this->addNewFieldToContentType($type,'fileDateObject','ifd0_filedatetime','datetime','exif_readonly');
	  
	  $this->activateMetadataOnObjectTypes(array($type->type));

	  $this->drupalGet("admin/structure/types/manage/photo/fields");
	  $this->drupalGet("node/add");
	  $this->assertResponse(200, t('User is allowed to add content types.'),'Exif');
  }

  /**
   * must return an array of module to be activated.
   */
  public abstract function initModules();


  public function testCreatePhotoNodeWithoutImage() {
    $settings = array(
          'type' => 'photo',
          'title' => $this->randomName(32),
    );
    $node = $this->drupalCreateNode($settings);
    $this->assertNotNull($node,"node created",'Exif');
    $this->verbose('Node created: ' . var_export($node, TRUE));
    $this->drupalGet("node/{$node->nid}/edit");
    $this->assertResponse(200, t('User is allowed to edit the content.'),'Exif');
    $this->assertText(t("@title", array('@title' => $settings['title'])), "Found title in edit form",'Exif');
    $this->drupalPost(NULL,array(),t('Save'));
    $this->assertResponse(200, t('trying to submit node.'),'Exif');
    $this->assertNoText("The content on this page has either been modified by another user, or you have already submitted modifications using this form. As a result, your changes cannot be saved.",t('form has been correctly submitted'),'Exif');
    $noedit = array();
    $this->drupalGet("node/{$node->nid}");
    $this->assertResponse(200, t('photography node edition is complete.'),'Exif');
  }

  public function testCreatePhotoNodeWithImage() {
    $settings = array(
          'type' => 'photo',
          'title' => $this->randomName(32),
    );
    $node = $this->drupalCreateNode($settings);
    $this->assertNotNull($node,"node created",'Exif');
    $this->verbose('Node created: ' . var_export($node, TRUE));
    $this->drupalGet("node/{$node->nid}/edit");
    //check field settings are hidden.
    $this->assertRaw("exif.css");
    //assert hidden field are present
    $this->assertField("field_iptc_keywords[und][0][tid]","hidden field keywords is present","Exif");
    $this->assertField("field_exif_model[und][0][tid]","hidden field model is present","Exif");
    $this->assertResponse(200, t('User is allowed to edit the content.'),'Exif');
    $this->assertText(t("@title", array('@title' => $settings['title'])), "Found title in edit form",'Exif');
    $img_path = drupal_get_path('module', 'exif')."/sample.jpg";
    $this->assertTrue(file_exists($img_path),"file {$img_path} exists.","Exif");
    $file_upload = array (
			'files[field_image_und_0]' => $img_path
    );
    $this->drupalPostAJAX(NULL, $file_upload,'field_image_und_0_upload_button');
    $this->assertResponse(200, t('photo is uploaded.'),'Exif');
    $this->drupalGet("node/{$node->nid}/edit");
    $fid = $this->getLastFileId();
    $file_uploaded_attach = array (
			'field_image[und][0][fid]' => $fid
    );
    $this->drupalPost(NULL,$file_uploaded_attach,t('Save'));
    $this->assertResponse(200, t('trying to submit node.'),'Exif');
    $this->assertNoText("The content on this page has either been modified by another user, or you have already submitted modifications using this form. As a result, your changes cannot be saved.",t('form has been correctly submitted'),'Exif');
    $this->drupalGet("node/{$node->nid}");
    $this->assertResponse(200, t('photography node edition is complete.'),'Exif');
    //check for label
    $this->assertText("model");
    $this->assertText("keywords");
    $this->assertText("latitude");
    $this->assertText("longitude");
    $this->assertText("comment");
    $this->assertText("artist");
    $this->assertText("title");
    $this->assertText("flash");
    $this->assertText("date");
    $this->assertText("DateObject");
    $this->assertText("fileDate");
    $this->assertText("fileDateObject");
    //check for values
    $this->assertText("Canon EOS 350D DIGITAL","model value is correct","Exif");
    $this->assertText("annika","keyword n°1 is correct","Exif");
    $this->assertText("geburtstag","keyword n°2 is correct","Exif");
    $this->assertText("O&#039;Brien","keyword n°3 is correct (apostrophe in text)","Exif");
    $this->assertText("51.2977","latitude is correct","Exif");
    $this->assertText("12.2206","longitude is correct","Exif");
    $this->assertText("ich bin ein kleiner kommentar","comment is correct","Exif");
    $this->assertNoText("UNICODEich bin ein kleiner kommentar","comment is correct","Exif");
    $this->assertText("Raphael Schär","artist is correct","Exif");
    $this->assertText("Der Titel","title is correct","Exif");
    $this->assertText("Flash fired, compulsory flash mode","flash is correct","Exif");
    
    $this->assertText("2009-01-23T08:52:43","date (wihtout Date module) is correct","Exif");
    $this->assertText("2009/01/23 08:52:43","date (with Date module) is correct","Exif");
    
    $this->assertText("2011-11-05T21:39:10+01:00","file date (wihtout Date module) is correct","Exif");
    $this->assertText("2011/11/05 21:39:10","file date (with Date module) is correct","Exif");    
  }

}
?>
