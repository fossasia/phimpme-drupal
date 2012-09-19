<?php

/**
 * Just some help page. Gives you an overview over the available tags
 * @return string html
 */
function exif_admin_settings() {
  $filepath = drupal_get_path('module', 'exif') .'/sample.jpg';  
  $url = url($filepath);
	
  $output = '';
  $output .= '<h3>' . t('About') . '</h3>';
  $output .= '<p>' . t('The Exif module allows you :');
  $output .= '<ul><li>' . t('extract metadata from an image'). '</li>';
  $output .= '<li>' . t('to classify your images by settings terms in taxonamy vocabulary'). '</li></ul>';
  $output .= t('To classify images, you define <em>vocabularies</em> that contain related <em>terms</em>, and then assign the vocabularies to content types. For more information, see the online handbook entry for the <a href="@taxonomy">Taxonomy module</a>.', array('@taxonomy' => 'http://drupal.org/handbook/modules/taxonomy/'));
  $output .= '</p>';
  $output .= '<h3>' . t('Uses') . '</h3>';
  $output .= '<dl>';
  $output .= '<dt>' . t('Creating vocabularies') . '</dt>';
  $output .= '<dd>' . t('Users with sufficient <a href="@perm">permissions</a> can create <em>vocabularies</em> through the <a href="@taxo">Taxonomy page</a>. The page listing the terms provides a drag-and-drop interface for controlling the order of the terms and sub-terms within a vocabulary, in a hierarchical fashion. A <em>controlled vocabulary</em> classifying music by genre with terms and sub-terms could look as follows:', array('@taxo' => url('admin/structure/taxonomy'), '@perm' => url('admin/config/people/permissions', array('fragment'=>'module-taxonomy'))));
  $output .= t('This module will automatically create in the chosen vocabulary (by default "Photographies\' metadata"), the following structure:');
  $output .= '<ul><li>' . t ('<em>vocabulary</em>: Photographies\'metadata').'</li>';
  $output .= '<ul><li>' . t('<em>term</em>: iptc') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: keywords') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: Paris') . '</li>';
  $output .= '<li>' . t('<em>sub-term</em>: Friends') . '</li>';
  $output .= '</ul></ul>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: caption') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: Le louvre') . '</li>';
  $output .= '</ul></ul></ul>';
  $output .= '<ul><li>' . t('<em>term</em>: exif') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: model') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: KINON DE800') . '</li>';
  $output .= '</ul></ul>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: isospeedratings') . '</li>';
  $output .= '<ul><li>' . t('<em>sub-term</em>: 200') . '</li>';
  $output .= '</ul></ul></ul></ul>';
  $output .= '<dd>' . t('To get metadata information of an image, you have to choose on which node type the extraction should be made.');
  $output .=  t('You also have to create fields with specific names using the new Field UI.');
  $output .=  t('The type of the field can be :');
  $output .= '<ul><li>' . t ('<em>text field</em>: extract information and put it in the text field.'). '</li>';
  $output .= '<li>' . t ('<em>term reference field</em>: extract information, create terms and sub-terms if needed and put it in the field.').'</li>';
  $output .= '</ul>';
  $output .= t('Please, if you want to use term reference field, ensure :');
  $output .= '<ul><li>' . t('you choose the autocompletion widget and').'</li>';;
  $output .= '<li>' . t ('the "Images" Vocabulary exist.').'</li>';
  $output .= '</ul>';
  $output .=  t('TIPS : Note for iptc and exif fields that have several values (like field iptc "keywords" as an example), ');
  $output .=  t('if you want to get all the values, do not forget to configure the field to use unlimited number of values (by default, set to 1).');
  $output .= '</dd>';
  $output .= '</dl>';
  $output .= '<div class="sample-image">';
  $output .= '<h3 class="sample-image">';
  $output .= t('Example of field name and the metadata extracted');
  $output .= '</h3>';
  $output .= '<img class="sample-image" src="'.$url.'"/>';
  $output .= '</div>';
  $rows = array();
  $help = '';
  drupal_add_css(drupal_get_path('module', 'exif') .'/exif.admin.css');
  $exif = _exif_get_class();
  $fullmetadata = $exif->readMetadataTags($filepath);  
  if(is_array($fullmetadata)){
    foreach ($fullmetadata as $section => $section_data) {
      $rows[] = array('data'=>array($section,$help), 'class' => array('tag_type'));
      foreach ($section_data as $key => $value) {
        if ($value != NULL && $value != '' && !$exif->startswith($key,'undefinedtag')) {
          $resultTag = "";
          if (is_array($value)) {
            foreach($value as $innerkey => $innervalue) {
              if( ($innerkey+1) != count($value) ) {
                $resultTag.=$innervalue."; ";
              } else {
                $resultTag.=$innervalue;
              }
            }
          } else {
            $resultTag=check_plain($value);
          }
          $rows[] = array('data'=>array("field_".$section."_".$key,$resultTag), 'class' => array('tag'));
        }
      }
    }
  }
  $header = array(t('Key'),t('Value'));
  $output .= '<p>';
  $variables = array ("header" => $header, "rows" => $rows, "attributes" => array(), "caption" => "","sticky" => array(), "colgroups" => array(), "empty" => array() );
  $output .= theme_table($variables);
  $output .= '</p>';
  return $output;
}



/**
 * The form definition for the admin settings
 * @return array
 *  form definition
 */
function exif_admin_settings_form() {
  $forms = array();
  /*$forms['exif_granularity'] = array(
   '#type'	=>	'select',
   '#title'=>	t('Granularity'),
   '#options'=> array(0=>t('Default'),1=>('Day')),
   '#default_value'=>variable_get('exif_granularity',0),
   '#description'=>t('If a timestamp is select (for example the date the picture was taken), you can specify here how granular the timestamp should be. If you select default it will just take whatever is available in the picture. If you select Day, the Date saved will look something like 13-12-2008. This can be useful if you want to use some kind of grouping on the data.'),
   );*/

  $all_nodetypes = node_type_get_types();
  $all_nt = array();
  foreach ($all_nodetypes as $item) {
    $all_nt[$item->type] = $item->name;
  }
  $forms['exif_nodetypes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Nodetypes'),
    '#options' => $all_nt,
    '#default_value' => variable_get('exif_nodetypes', array()),
    '#description' => t('Select nodetypes which should be checked for itpc & exif data. In case the nodetypes contains more than one image field, the module will use the first one.'),
  );

  if (module_exists("media")) {
    $all_mt = array();
    // Setup media types
    $types = media_type_get_types();
    foreach ($types as $key) {
      $all_mt[$key->name] = $key->name;
    }
    $forms['exif_mediatypes'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Mediatypes'),
      '#options' => $all_mt,
      '#default_value' => variable_get('exif_mediatypes', array()),
      '#description' => t('Select mediatypes which should be checked for itpc & exif data.'),
    );
  }


  $forms['exif_update'] = array(
    '#type' => 'checkbox',
    '#title' => t('Refresh on node update'),
    '#default_value' => variable_get('exif_update', TRUE),
    '#description' => t('If you enable this option, Exif data is being updated when the node is being updated.'),
  );

  $forms['exif_empty_values'] = array(
    '#type' => 'checkbox',
    '#title' => t('Write empty image data?'),
    '#default_value' => variable_get('exif_empty_values', TRUE),
    '#description' => t("If checked all values will be written. So for example if you want to read the creation date from EXIF, but it's not available, it will just write an empty string. If unchecked, empty strings will not be written. This might be the desired behavior, if you have a default value for the CCK field."),
  );

  $all_vocabularies = taxonomy_get_vocabularies();
  $all_vocs = array();
  $all_vocs[0] = 'None';
  foreach($all_vocabularies as $item){
    $all_vocs[$item->vid] = $item->name;
  }
  $forms['exif_vocabulary'] = array(
		'#type'	=>	'select',
		'#title'=>	t('Vocabulary'),
		'#options'=> $all_vocs,
		'#default_value'=>variable_get('exif_vocabulary',array()),
		'#description'=> t('Select vocabulary which should be used for itpc & exif data.'),
  );
  
  //TODO: add a button to create a vocabulary "photographies'metadata" (exif,iptc and xmp data contains in jpef file)
  //TODO : add a button to create an Photography node with default exif field (title,model,keywords) and default behavior but 'promoted to front'
  //TODO : Check if the media module is install to add automatically the image type active and add active default exif field (title,model,keywords).
  return system_settings_form($forms);
}
?>