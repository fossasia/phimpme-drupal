<?php
//$Id:

/**
 *
 * @author Jean-Philippe Hautin
 * @author Raphael Schär
 * This is a helper class to handle the whole data processing of exif
 *
 */
Class Exif {
  static private $instance = NULL;

  /**
   * We are implementing a singleton pattern
   */
  private function __construct() {
  }

  public static function getInstance() {
    if(is_null(self::$instance)) {
      self::$instance = new self;
    }
    return self::$instance;
  }

  public static function getMetadataSections() {
    $sections = array('exif', 'file', 'computed', 'ifd0', 'gps', 'winxp', 'iptc', 'xmp');
    return $sections;
  }

  /**
   * Going through all the fields that have been created for a given node type
   * and try to figure out which match the naming convention -> so that we know
   * which exif information we have to read
   *
   * Naming convention are: field_exif_xxx (xxx would be the name of the exif
   * tag to read
   *
   * @param $arCckFields array of CCK fields
   * @return array a list of exif tags to read for this image
   */
  public function getMetadataFields($arCckFields=array()) {
    $arSections = Exif::getMetadataSections();
    foreach ($arCckFields as $drupal_field => $metadata_settings) {
      $metadata_field = $metadata_settings['metadata_field'];
      $ar = explode("_", $metadata_field);
      if (isset($ar[0]) && in_array($ar[0], $arSections)) {
        $section = $ar[0];
        unset($ar[0]);
        $arCckFields[$drupal_field]['metadata_field'] =  array('section'=>$section, 'tag'=>implode("_", $ar));
      }
    }
    return $arCckFields;
  }


  /**
   * Helper function to reformat fields where required.
   *
   * Some values (lat/lon) break down into structures, not strings.
   * Dates should be parsed nicely.
   */
  function _reformat($data) {
    // Make the key lowercase as field names must be.
    $data = array_change_key_case($data, CASE_LOWER);
    foreach ($data as $key => &$value) {
      if (is_array($value))  {
        $value = array_change_key_case($value, CASE_LOWER);
        switch ($key) {
          // GPS values
          case 'gps_latitude':
          case 'gps_longitude':
          case 'gpslatitude':
          case 'gpslongitude':
            $value = $this->_exif_reformat_DMS2D($value, $data[$key . 'ref']);
            break;
        }
      } else {
	if (!drupal_validate_utf8($value)) {
	    $value=utf8_encode($value);
	}
        switch ($key) {
          // String values.
          case 'usercomment':
			if ($this->startswith($value,'UNICODE')) {
				$value=substr($value,8);
        	}
        	break;
          // Date values.
          case 'filedatetime':
          	$value=date('c',$value);
          	break;
          case 'datetimeoriginal':
          case 'datetime':
          case 'datetimedigitized':
            // In case we get a datefield, we need to reformat it to the ISO 8601 standard:
            // which will look something like 2004-02-12T15:19:21
            $date_time = explode(" ", $value);
            $date_time[0] = str_replace(":", "-", $date_time[0]);
            if (variable_get('exif_granularity', 0) == 1) {
              $date_time[1] = "00:00:00";
            }
            $value = implode("T", $date_time);
            break;
          // GPS values.
          case 'gpsaltitude':
          case 'gpsimgdirection':
            $value = $this->_exif_reformat_DMS2D($value, $data[$key . 'ref']);
            break;
          // Flash values.
          case 'flash':
            $flash_descriptions = $this->getFlashDescriptions();
            if (isset($flash_descriptions[$value])) {
              $value = $flash_descriptions[$value];
            }
	    break;
           // Exposure values.
           case 'exposuretime':
             if (strpos($value, '/') !== FALSE) {
               $value = $this->_normalise_fraction($value) . 's';
             }
             break;
           // Focal Length values.
           case 'focallength':
             if (strpos($value, '/') !== FALSE) {
               $value = $this->_normalise_fraction($value) . 'mm';
             }
             break;
        }
      }
    }
    return $data;
  }

  public function startswith($hay, $needle) {
    return substr($hay, 0, strlen($needle)) === $needle;
  }


  function _exif_reencode_to_utf8($value)
  {
    $unicode_list=unpack("v*", $value)  ;
    $result = "";
    foreach($unicode_list as $key => $value) {
      if ($value!=0) {
        $one_character = pack("C", $value);
        $temp = mb_convert_encoding('&#' . $value . ';', 'UTF-8', 'HTML-ENTITIES');
        $result .= $temp;
      }
    }
    return $result;
  }

   /**
    * Normalise fractions.
    */
   function _normalise_fraction($fraction) {
     $parts = explode('/', $fraction);
     $top = $parts[0];
     $bottom = $parts[1];
 
     if ($top > $bottom) {
       // Value > 1
       if (($top % $bottom) == 0) {
         $value = ($top / $bottom);
       } else {
         $value = round(($top / $bottom), 2);
       }
     } else if ($top == $bottom) {
       // Value = 1
       $value = '1';
     } else {
       // Value < 1
       if ($top == 1) {
         $value = '1/' . $bottom;
       }
       else {
         $value = '1/' . round(($bottom / $top) ,0);
       }
     }
     return $value;
   }  

  /**
   * Helper function to change GPS co-ords into decimals.
   */
  function _exif_reformat_DMS2D($value, $ref) {
    if (!is_array($value)) {
      $value = array($value);
    }
    $dec = 0;
    $granularity = 0;
    foreach ($value as $element) {
      $parts = explode('/', $element);
      $dec += (float) (((float) $parts[0] /  (float) $parts[1]) / pow(60, $granularity));
      $granularity++;
    }
    if ($ref == 'S' || $ref == 'W') {
      $dec *= -1;
    }
    return $dec;
  }

  /**
   * $arOptions liste of options for the method :
   * # enable_sections : (default : TRUE) retreive also sections.
   * @param unknown_type $file
   * @param unknown_type $arTagNames
   * @param unknown_type $arOptions
   */
  public function readMetadataTags($file, $enable_sections = TRUE) {
    if (!file_exists($file)) {
      return array();
    }
    $data1 = $this->readExifTags($file,$enable_sections);
    $data2 = $this->readIPTCTags($file,$enable_sections);
    if (class_exists('SXMPFiles')) {
      $data3 = $this->readXMPTags($file,$enable_sections);
      $data = array_merge($data1, $data2, $data3);
    } else {
      $data = array_merge($data1, $data2);
    }

    if(is_array($data)){
      foreach ($data as $section => $section_data) {
        $section_data=$this->_reformat($section_data);
        $data[$section]=$section_data;
      }
    }
    return $data;
  }

  function filterMetadataTags($arSmallMetadata, $arTagNames) {
    $info = array();
    foreach ($arTagNames as $drupal_field => $metadata_settings) {
      $tagName = $metadata_settings['metadata_field'];
      if (!empty($arSmallMetadata[$tagName['section']][$tagName['tag']])) {
        $info[$tagName['section']][$tagName['tag']] = $arSmallMetadata[$tagName['section']][$tagName['tag']];
      }
    }
    return $info;
  }

  /**
   * Read the Information from a picture according to the fields specified in CCK
   * @param $file
   * @param $arTagNames
   * @return array
   */
  public function readExifTags($file, $enable_sections = TRUE) {
    $ar_supported_types = array('jpg', 'jpeg');
    if (!in_array(strtolower($this->getFileType($file)), $ar_supported_types)) {
      return array();
    }
    $exif = exif_read_data($file, 0,$enable_sections);
    $arSmallExif = array();
    foreach ((array)$exif as $key1 => $value1) {
       
      if (is_array($value1)) {
        $value2 = array ();
        foreach ((array)$value1 as $key3 => $value3) {
          $value[strtolower($key3)]= $value3 ;
        }
      } else {
        $value = $value1;
      }
      $arSmallExif[strtolower($key1)] = $value;

    }
    return $arSmallExif;
  }

  private function getFileType($file) {
    $ar = explode('.', $file);
    $ending = $ar[count($ar)-1];
    return $ending;
  }

  /**
   * Read IPTC tags.
   *
   * @param String $file
   * 	Path to image to read IPTC from
   *
   */
  public function readIPTCTags($file, $enable_sections) {
    $humanReadableKey = $this->getHumanReadableIPTCkey();
    $size = GetImageSize ($file, $infoImage);
    $iptc = empty($infoImage["APP13"]) ? array() : iptcparse($infoImage["APP13"]);
    $arSmallIPTC = array();
    if (is_array($iptc)) {
      foreach ($iptc as $key => $value) {
        if (count($value)==1) {
          $resultTag = $value[0];
        } else {
          $resultTag = $value;
        }
        if (array_key_exists($key, $humanReadableKey)) {
        	$humanKey = $humanReadableKey[$key];
        	$arSmallIPTC[$humanKey] = $resultTag;
        } else {
        	$arSmallIPTC[$key] = $resultTag;
        }
      }
    }
    if ($enable_sections) {
      return array ('iptc' => $arSmallIPTC);
    } else {
      return $arSmallIPTC;
    }
  }

  /**
   * Read XMP data from an image file.
   *
   * @param $file
   *   File path.
   *
   * @param $arTagNames
   *   Available metadata fields.
   *
   * @return
   *   XMP image metadata.
   *
   * @todo
   *   Support for different array keys.
   */
  public function readXMPTags($file, $enable_sections = TRUE) {
    // Get a CCK-XMP mapping.
    $map  = $this->getXMPFields();
    $xmp  = $this->openXMP($file);
    $info = array();

    if ($xmp != FALSE) {
      // Iterate over XMP fields defined by CCK.
      foreach ($arTagNames as $tagName) {
        if ($tagName['section'] == "xmp") {
          // Get XMP field.
          $config                                          = $map[$tagName['tag']];
          $field                                           = $this->readXMPItem($xmp, $config);
          $info[$tagName['section'] .'_'. $tagName['tag']] = $field;
        }
      }
      $this->closeXMP($xmp);
    }
    if ($enable_sections) {
      return array ('xmp' => $info);
    } else {
      return $info;
    }
  }

  /**
   * Open an image file for XMP data extraction.
   *
   * @param $file
   *   File path.
   *
   * @return
   *   Array with XMP file and metadata.
   */
  function openXMP($file) {
    // Setup.
    $xmpfiles = new SXMPFiles();
    $xmpmeta  = new SXMPMeta();

    // Open.
    $xmpfiles->OpenFile($file);
    // Get XMP metadata into the object.
    if ($xmpfiles->GetXMP($xmpmeta)) {
      // Sort metadata.
      $xmpmeta->Sort();
      return array('files' => $xmpfiles, 'meta' => $xmpmeta);
    }
    // No XMP data available.
    return FALSE;
  }

  /**
   * Close a file opened for XMP data extraction.
   *
   * @param $xmp
   *   XMP array as returned from openXMP().
   */
  function closeXMP($xmp) {
    $xmp['files']->CloseFile();
  }

  /**
   * Read a single item from an image file.
   *
   * @param $xmp
   *   XMP array as returned from openXMP().
   *
   * @param $config
   *   XMP field configuration.
   *
   * @param $key
   *   In case of array field type, the numeric field key.
   *
   * @return
   *   Field value.
   */
  public function readXMPItem($xmp, $config, $key = 0) {
    // Setup.
    $xmpfiles = $xmp['files'];
    $xmpmeta  = $xmp['meta'];

    // Try to read XMP data if the namespace is available.
    if(@$xmpmeta->GetNamespacePrefix($config['ns'])) {
      if ($config['type'] == 'property') {
        $value = @$xmpmeta->GetProperty($config['ns'], $config['name']);
      }
      elseif ($config['type'] == 'array') {
        $value = @$xmpmeta->GetArrayItem($key, $config['ns'], $config['name']);
      }
      elseif ($config['type'] == 'struct') {
        $value = @$xmpmeta->GetStructField($config['ns'], $config['struct'], $config['ns'], $config['name']);
      }
    }

    return $value;
  }

  public function getHumanReadableExifKeys() {
    return array(
"file_filename",
"file_filedatetime",
"file_filesize",
"file_filetype",
"file_mimetype",
"file_sectionsfound",
"computed_filename",
"computed_filedatetime",
"computed_filesize",
"computed_filetype",
"computed_mimetype",
"computed_sectionsfound",
"computed_html",
"computed_height",
"computed_width",
"computed_iscolor",
"computed_byteordermotorola",
"computed_ccdwidth",
"computed_aperturefnumber",
"computed_usercomment",
"computed_usercommentencoding",
"computed_thumbnail.filetype",
"computed_thumbnail.mimetype",
"ifd0_filename",
"ifd0_filedatetime",
"ifd0_filesize",
"ifd0_filetype",
"ifd0_mimetype",
"ifd0_sectionsfound",
"ifd0_html",
"ifd0_height",
"ifd0_width",
"ifd0_iscolor",
"ifd0_byteordermotorola",
"ifd0_ccdwidth",
"ifd0_aperturefnumber",
"ifd0_usercomment",
"ifd0_usercommentencoding",
"ifd0_thumbnail.filetype",
"ifd0_thumbnail.mimetype",
"ifd0_imagedescription",
"ifd0_make",
"ifd0_model",
"ifd0_orientation",
"ifd0_xresolution",
"ifd0_yresolution",
"ifd0_resolutionunit",
"ifd0_software",
"ifd0_datetime",
"ifd0_artist",
"ifd0_ycbcrpositioning",
"ifd0_title",
"ifd0_comments",
"ifd0_author",
"ifd0_subject",
"ifd0_exif_ifd_pointer",
"ifd0_gps_ifd_pointer",
"thumbnail_filename",
"thumbnail_filedatetime",
"thumbnail_filesize",
"thumbnail_filetype",
"thumbnail_mimetype",
"thumbnail_sectionsfound",
"thumbnail_html",
"thumbnail_height",
"thumbnail_width",
"thumbnail_iscolor",
"thumbnail_byteordermotorola",
"thumbnail_ccdwidth",
"thumbnail_aperturefnumber",
"thumbnail_usercomment",
"thumbnail_usercommentencoding",
"thumbnail_thumbnail.filetype",
"thumbnail_thumbnail.mimetype",
"thumbnail_imagedescription",
"thumbnail_make",
"thumbnail_model",
"thumbnail_orientation",
"thumbnail_xresolution",
"thumbnail_yresolution",
"thumbnail_resolutionunit",
"thumbnail_software",
"thumbnail_datetime",
"thumbnail_artist",
"thumbnail_ycbcrpositioning",
"thumbnail_title",
"thumbnail_comments",
"thumbnail_author",
"thumbnail_subject",
"thumbnail_exif_ifd_pointer",
"thumbnail_gps_ifd_pointer",
"thumbnail_compression",
"thumbnail_jpeginterchangeformat",
"thumbnail_jpeginterchangeformatlength",
"exif_filename",
"exif_filedatetime",
"exif_filesize",
"exif_filetype",
"exif_mimetype",
"exif_sectionsfound",
"exif_html",
"exif_height",
"exif_width",
"exif_iscolor",
"exif_byteordermotorola",
"exif_ccdwidth",
"exif_aperturefnumber",
"exif_usercomment",
"exif_usercommentencoding",
"exif_thumbnail.filetype",
"exif_thumbnail.mimetype",
"exif_imagedescription",
"exif_make",
"exif_model",
"exif_orientation",
"exif_xresolution",
"exif_yresolution",
"exif_resolutionunit",
"exif_software",
"exif_datetime",
"exif_artist",
"exif_ycbcrpositioning",
"exif_title",
"exif_comments",
"exif_author",
"exif_subject",
"exif_exif_ifd_pointer",
"exif_gps_ifd_pointer",
"exif_compression",
"exif_jpeginterchangeformat",
"exif_jpeginterchangeformatlength",
"exif_exposuretime",
"exif_fnumber",
"exif_exposureprogram",
"exif_isospeedratings",
"exif_exifversion",
"exif_datetimeoriginal",
"exif_datetimedigitized",
"exif_componentsconfiguration",
"exif_shutterspeedvalue",
"exif_aperturevalue",
"exif_exposurebiasvalue",
"exif_meteringmode",
"exif_flash",
"exif_focallength",
"exif_flashpixversion",
"exif_colorspace",
"exif_exifimagewidth",
"exif_exifimagelength",
"exif_interoperabilityoffset",
"exif_focalplanexresolution",
"exif_focalplaneyresolution",
"exif_focalplaneresolutionunit",
"exif_imageuniqueid",
"gps_filename",
"gps_filedatetime",
"gps_filesize",
"gps_filetype",
"gps_mimetype",
"gps_sectionsfound",
"gps_html",
"gps_height",
"gps_width",
"gps_iscolor",
"gps_byteordermotorola",
"gps_ccdwidth",
"gps_aperturefnumber",
"gps_usercomment",
"gps_usercommentencoding",
"gps_thumbnail.filetype",
"gps_thumbnail.mimetype",
"gps_imagedescription",
"gps_make",
"gps_model",
"gps_orientation",
"gps_xresolution",
"gps_yresolution",
"gps_resolutionunit",
"gps_software",
"gps_datetime",
"gps_artist",
"gps_ycbcrpositioning",
"gps_title",
"gps_comments",
"gps_author",
"gps_subject",
"gps_exif_ifd_pointer",
"gps_gps_ifd_pointer",
"gps_compression",
"gps_jpeginterchangeformat",
"gps_jpeginterchangeformatlength",
"gps_exposuretime",
"gps_fnumber",
"gps_exposureprogram",
"gps_isospeedratings",
"gps_exifversion",
"gps_datetimeoriginal",
"gps_datetimedigitized",
"gps_componentsconfiguration",
"gps_shutterspeedvalue",
"gps_aperturevalue",
"gps_exposurebiasvalue",
"gps_meteringmode",
"gps_flash",
"gps_focallength",
"gps_flashpixversion",
"gps_colorspace",
"gps_exifimagewidth",
"gps_exifimagelength",
"gps_interoperabilityoffset",
"gps_gpsimgdirectionref",
"gps_gpsimgdirection",
"gps_focalplanexresolution",
"gps_focalplaneyresolution",
"gps_focalplaneresolutionunit",
"gps_imageuniqueid",
"gps_gpsversion",
"gps_gpslatituderef",
"gps_gpslatitude",
"gps_gpslongituderef",
"gps_gpslongitude",
"gps_gpsaltituderef",
"gps_gpsaltitude",
"interop_filename",
"interop_filedatetime",
"interop_filesize",
"interop_filetype",
"interop_mimetype",
"interop_sectionsfound",
"interop_html",
"interop_height",
"interop_width",
"interop_iscolor",
"interop_byteordermotorola",
"interop_ccdwidth",
"interop_aperturefnumber",
"interop_usercomment",
"interop_usercommentencoding",
"interop_thumbnail.filetype",
"interop_thumbnail.mimetype",
"interop_imagedescription",
"interop_make",
"interop_model",
"interop_orientation",
"interop_xresolution",
"interop_yresolution",
"interop_resolutionunit",
"interop_software",
"interop_datetime",
"interop_artist",
"interop_ycbcrpositioning",
"interop_title",
"interop_comments",
"interop_author",
"interop_subject",
"interop_exif_ifd_pointer",
"interop_gps_ifd_pointer",
"interop_compression",
"interop_jpeginterchangeformat",
"interop_jpeginterchangeformatlength",
"interop_exposuretime",
"interop_fnumber",
"interop_exposureprogram",
"interop_isospeedratings",
"interop_exifversion",
"interop_datetimeoriginal",
"interop_datetimedigitized",
"interop_componentsconfiguration",
"interop_shutterspeedvalue",
"interop_aperturevalue",
"interop_exposurebiasvalue",
"interop_meteringmode",
"interop_flash",
"interop_focallength",
"interop_flashpixversion",
"interop_colorspace",
"interop_exifimagewidth",
"interop_exifimagelength",
"interop_interoperabilityoffset",
"interop_focalplanexresolution",
"interop_focalplaneyresolution",
"interop_focalplaneresolutionunit",
"interop_imageuniqueid",
"interop_gpsversion",
"interop_gpslatituderef",
"interop_gpslatitude",
"interop_gpslongituderef",
"interop_gpslongitude",
"interop_gpsaltituderef",
"interop_gpsaltitude",
"interop_interoperabilityindex",
"interop_interoperabilityversion",
"winxp_filename",
"winxp_filedatetime",
"winxp_filesize",
"winxp_filetype",
"winxp_mimetype",
"winxp_sectionsfound",
"winxp_html",
"winxp_height",
"winxp_width",
"winxp_iscolor",
"winxp_byteordermotorola",
"winxp_ccdwidth",
"winxp_aperturefnumber",
"winxp_usercomment",
"winxp_usercommentencoding",
"winxp_thumbnail.filetype",
"winxp_thumbnail.mimetype",
"winxp_imagedescription",
"winxp_make",
"winxp_model",
"winxp_orientation",
"winxp_xresolution",
"winxp_yresolution",
"winxp_resolutionunit",
"winxp_software",
"winxp_datetime",
"winxp_artist",
"winxp_ycbcrpositioning",
"winxp_title",
"winxp_comments",
"winxp_author",
"winxp_subject",
"winxp_exif_ifd_pointer",
"winxp_gps_ifd_pointer",
"winxp_compression",
"winxp_jpeginterchangeformat",
"winxp_jpeginterchangeformatlength",
"winxp_exposuretime",
"winxp_fnumber",
"winxp_exposureprogram",
"winxp_isospeedratings",
"winxp_exifversion",
"winxp_datetimeoriginal",
"winxp_datetimedigitized",
"winxp_componentsconfiguration",
"winxp_shutterspeedvalue",
"winxp_aperturevalue",
"winxp_exposurebiasvalue",
"winxp_meteringmode",
"winxp_flash",
"winxp_focallength",
"winxp_flashpixversion",
"winxp_colorspace",
"winxp_exifimagewidth",
"winxp_exifimagelength",
"winxp_interoperabilityoffset",
"winxp_focalplanexresolution",
"winxp_focalplaneyresolution",
"winxp_focalplaneresolutionunit",
"winxp_imageuniqueid",
"winxp_gpsversion",
"winxp_gpslatituderef",
"winxp_gpslatitude",
"winxp_gpslongituderef",
"winxp_gpslongitude",
"winxp_gpsaltituderef",
"winxp_gpsaltitude",
"winxp_interoperabilityindex",
"winxp_interoperabilityversion",
    );
  }

  /**
   * Just some little helper function to get the iptc fields
   * @return array
   *
   */
  public function getHumanReadableIPTCkey() {
    return array(
      "2#202" => "object_data_preview_data",
      "2#201" => "object_data_preview_file_format_version",
      "2#200" => "object_data_preview_file_format",
      "2#154" => "audio_outcue",
      "2#153" => "audio_duration",
      "2#152" => "audio_sampling_resolution",
      "2#151" => "audio_sampling_rate",
      "2#150" => "audio_type",
      "2#135" => "language_identifier",
      "2#131" => "image_orientation",
      "2#130" => "image_type",
      "2#125" => "rasterized_caption",    
      "2#122" => "writer",
      "2#120" => "caption",
      "2#118" => "contact",
      "2#116" => "copyright_notice",
      "2#115" => "source",
      "2#110" => "credit",
      "2#105" => "headline",
      "2#103" => "original_transmission_reference",
      "2#101" => "country_name",
      "2#100" => "country_code",
      "2#095" => "state",
      "2#092" => "sublocation",
      "2#090" => "city",
      "2#085" => "by_line_title",
      "2#080" => "by_line",
      "2#075" => "object_cycle",
      "2#070" => "program_version",
      "2#065" => "originating_program",
      "2#063" => "digital_creation_time",
      "2#062" => "digital_creation_date",   
      "2#060" => "creation_time",
      "2#055" => "creation_date",
      "2#050" => "reference_number",
      "2#047" => "reference_date",
      "2#045" => "reference_service",
      "2#042" => "action_advised",
      "2#040" => "special_instruction",
      "2#038" => "expiration_time",
      "2#037" => "expiration_date",
      "2#035" => "release_time",
      "2#030" => "release_date",
      "2#027" => "content_location_name",
      "2#026" => "content_location_code",
      "2#025" => "keywords",
      "2#022" => "fixture_identifier",
      "2#020" => "supplemental_category", 
      "2#015" => "category",
      "2#010" => "subject_reference", 
      "2#010" => "urgency",
      "2#008" => "editorial_update",
      "2#007" => "edit_status",
      "2#005" => "object_name",
      "2#004" => "object_attribute_reference",
      "2#003" => "object_type_reference",
      "2#000" => "record_version",
      "1#090" => "envelope_character_set"    
      );
  }

  /**
   * XMP fields mapper. As we're dealing with a mapper between RDF
   * elements and CCK fields, we have to define custom keys that
   * both on the field name and the namespace used.
   *
   * And, as the XMP specs also defines some datatypes like properties,
   * arrays and structures, we have to deal with those as well.
   *
   * @return array
   *   Mapping between CCK and XMP fields.
   */
  public function getXMPFields() {
    return array(
      'headline'            => array(
        'name'              => 'Headline',
        'ns'                => 'http://ns.adobe.com/photoshop/1.0/',
        'type'              => 'property',
    ),
      'authorsposition'     => array(
        'name'              => 'AuthorsPosition',
        'ns'                => 'http://ns.adobe.com/photoshop/1.0/',
        'type'              => 'property',
    ),
      'source'              => array(
        'name'              => 'Source',
        'ns'                => 'http://ns.adobe.com/photoshop/1.0/',
        'type'              => 'property',
    ),
      'instructions'        => array(
        'name'              => 'Instructions',
        'ns'                => 'http://ns.adobe.com/photoshop/1.0/',
        'type'              => 'property',
    ),
      'subject'             => array(
        'name'              => 'subject',
        'ns'                => 'http://purl.org/dc/elements/1.1/',
        'type'              => 'array',
    ),
      'description'         => array(
        'name'              => 'description',
        'ns'                => 'http://purl.org/dc/elements/1.1/',
        'type'              => 'array',
    ),
      'creator'             => array(
        'name'              => 'creator',
        'ns'                => 'http://purl.org/dc/elements/1.1/',
        'type'              => 'array',
    ),
      'rights'              => array(
        'name'              => 'rights',
        'ns'                => 'http://purl.org/dc/elements/1.1/',
        'type'              => 'array',
    ),
      'title'              => array(
        'name'              => 'title',
        'ns'                => 'http://purl.org/dc/elements/1.1/',
        'type'              => 'array',
    ),
      'ciadrextadr'         => array(
        'name'              => 'CiAdrExtadr',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'struct',
        'struct'            => 'CreatorContactInfo',
    ),
      'ciemailwork'         => array(
        'name'              => 'CiEmailWork',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'struct',
        'struct'            => 'CreatorContactInfo',
    ),
      'ciurlwork'           => array(
        'name'              => 'CiUrlWork',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'struct',
        'struct'            => 'CreatorContactInfo',
    ),
      'scene'               => array(
        'name'              => 'Scene',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'array',
    ),
      'subjectcode'         => array(
        'name'              => 'SubjectCode',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'array',
    ),
      'hierarchicalsubject' => array(
        'name'              => 'hierarchicalSubject',
        'ns'                => 'http://ns.adobe.com/lightroom/1.0/',
        'type'              => 'array',
    ),
      'location'            => array(
        'name'              => 'Location',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'property',
    ),
      'credit'              => array(
        'name'              => 'Credit',
        'ns'                => 'http://ns.adobe.com/photoshop/1.0/',
        'type'              => 'property',
    ),
      'countrycode'         => array(
        'name'              => 'CountryCode',
        'ns'                => 'http://iptc.org/std/Iptc4xmpCore/1.0/xmlns/',
        'type'              => 'property',
    ),
    );
  }

  public function getFieldKeys() {
    $exif_keys_temp = $this->getHumanReadableExifKeys();
    $exif_keys = array();
    foreach( $exif_keys_temp as $value ) {
      $exif_keys[$value] = $value;
    }
    $iptc_keys_temp = array_values($this->getHumanReadableIPTCkey());
    $iptc_keys = array();
    foreach( $iptc_keys_temp as $value ) {
      $current_value = "iptc_".$value;
      $iptc_keys[$current_value] = $current_value;
    }
    $xmp_keys = array();
    $xmp_keys_temp = array_keys($this->getXMPFields());
    foreach( $xmp_keys_temp as $value ) {
      $current_value = "xmp_".$value;
      $xmp_keys[$current_value] = $current_value;
    }
    $fields = array_merge($exif_keys,$iptc_keys,$xmp_keys);
    return $fields;
  }

  /**
   * Convert 'Flash' values to their human-readable descriptions.
   */
  public function getFlashDescriptions() {
    return array(
      '0' => t('Flash did not fire.'),
      '1' => t('Flash fired.'),
      '5' => t('Strobe return light not detected.'),
      '7' => t('Strobe return light detected.'),
      '9' => t('Flash fired, compulsory flash mode'),
      '13' => t('Flash fired, compulsory flash mode, return light not detected'),
      '15' => t('Flash fired, compulsory flash mode, return light detected'),
      '16' => t('Flash did not fire, compulsory flash mode'),
      '24' => t('Flash did not fire, auto mode'),
      '25' => t('Flash fired, auto mode'),
      '29' => t('Flash fired, auto mode, return light not detected'),
      '31' => t('Flash fired, auto mode, return light detected'),
      '32' => t('No flash function'),
      '65' => t('Flash fired, red-eye reduction mode'),
      '69' => t('Flash fired, red-eye reduction mode, return light not detected'),
      '71' => t('Flash fired, red-eye reduction mode, return light detected'),
      '73' => t('Flash fired, compulsory flash mode, red-eye reduction mode'),
      '77' => t('Flash fired, compulsory flash mode, red-eye reduction mode, return light not detected'),
      '79' => t('Flash fired, compulsory flash mode, red-eye reduction mode, return light detected'),
      '89' => t('Flash fired, auto mode, red-eye reduction mode'),
      '93' => t('Flash fired, auto mode, return light not detected, red-eye reduction mode'),
      '95' => t('Flash fired, auto mode, return light detected, red-eye reduction mode'),
    );
  }


}
