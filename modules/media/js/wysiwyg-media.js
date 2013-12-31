
/**
 *  @file
 *  Attach Media WYSIWYG behaviors.
 */

(function ($) {

Drupal.media = Drupal.media || {};

// Define the behavior.
Drupal.wysiwyg.plugins.media = {

  /**
   * Initializes the tag map.
   */
  initializeTagMap: function () {
    if (typeof Drupal.settings.tagmap == 'undefined') {
      Drupal.settings.tagmap = { };
    }
  },
  /**
   * Execute the button.
   * @TODO: Debug calls from this are never called. What's its function?
   */
  invoke: function (data, settings, instanceId) {
    if (data.format == 'html') {
      Drupal.media.popups.mediaBrowser(function (mediaFiles) {
        Drupal.wysiwyg.plugins.media.mediaBrowserOnSelect(mediaFiles, instanceId);
      }, settings['global']);
    }
  },

  /**
   * Respond to the mediaBrowser's onSelect event.
   * @TODO: Debug calls from this are never called. What's its function?
   */
  mediaBrowserOnSelect: function (mediaFiles, instanceId) {
    var mediaFile = mediaFiles[0];
    var options = {};
    Drupal.media.popups.mediaStyleSelector(mediaFile, function (formattedMedia) {
      Drupal.wysiwyg.plugins.media.insertMediaFile(mediaFile, formattedMedia.type, formattedMedia.html, formattedMedia.options, Drupal.wysiwyg.instances[instanceId]);
    }, options);

    return;
  },

  insertMediaFile: function (mediaFile, viewMode, formattedMedia, options, wysiwygInstance) {

    this.initializeTagMap();
    // @TODO: the folks @ ckeditor have told us that there is no way
    // to reliably add wrapper divs via normal HTML.
    // There is some method of adding a "fake element"
    // But until then, we're just going to embed to img.
    // This is pretty hacked for now.
    //
    var imgElement = $(this.stripDivs(formattedMedia));
    this.addImageAttributes(imgElement, mediaFile.fid, viewMode, options);

    var toInsert = this.outerHTML(imgElement);
    // Create an inline tag
    var inlineTag = Drupal.wysiwyg.plugins.media.createTag(imgElement);
    // Add it to the tag map in case the user switches input formats
    Drupal.settings.tagmap[inlineTag] = toInsert;
    wysiwygInstance.insert(toInsert);
  },

  /**
   * Gets the HTML content of an element
   *
   * @param jQuery element
   */
  outerHTML: function (element) {
    return $('<div>').append( element.eq(0).clone() ).html();
  },

  addImageAttributes: function (imgElement, fid, view_mode, additional) {
    //    imgElement.attr('fid', fid);
    //    imgElement.attr('view_mode', view_mode);
    // Class so we can find this image later.
    imgElement.addClass('media-image');
    this.forceAttributesIntoClass(imgElement, fid, view_mode, additional);
    if (additional) {
      for (k in additional) {
        if (additional.hasOwnProperty(k)) {
          if (k === 'attr') {
            imgElement.attr(k, additional[k]);
          }
        }
      }
    }
  },

  /**
   * Due to problems handling wrapping divs in ckeditor, this is needed.
   *
   * Going forward, if we don't care about supporting other editors
   * we can use the fakeobjects plugin to ckeditor to provide cleaner
   * transparency between what Drupal will output <div class="field..."><img></div>
   * instead of just <img>, for now though, we're going to remove all the stuff surrounding the images.
   *
   * @param String formattedMedia
   *  Element containing the image
   *
   * @return HTML of <img> tag inside formattedMedia
   */
  stripDivs: function (formattedMedia) {
    // Check to see if the image tag has divs to strip
    var stripped = null;
    if ($(formattedMedia).is('img')) {
      stripped = this.outerHTML($(formattedMedia));
    } else {
      stripped = this.outerHTML($('img', $(formattedMedia)));
    }
    // This will fail if we pass the img tag without anything wrapping it, like we do when re-enabling WYSIWYG
    return stripped;
  },

  /**
   * Attach function, called when a rich text editor loads.
   * This finds all [[tags]] and replaces them with the html
   * that needs to show in the editor.
   *
   */
  attach: function (content, settings, instanceId) {
    var matches = content.match(/\[\[.*?\]\]/g);
    this.initializeTagMap();
    var tagmap = Drupal.settings.tagmap;
    if (matches) {
      var inlineTag = "";
      for (i = 0; i < matches.length; i++) {
        inlineTag = matches[i];
        if (tagmap[inlineTag]) {
          // This probably needs some work...
          // We need to somehow get the fid propogated here.
          // We really want to
          var tagContent = tagmap[inlineTag];
          var mediaMarkup = this.stripDivs(tagContent); // THis is <div>..<img>

          var _tag = inlineTag;
          _tag = _tag.replace('[[','');
          _tag = _tag.replace(']]','');
          try {
            mediaObj = JSON.parse(_tag);
          }
          catch(err) {
            mediaObj = null;
          }
          if(mediaObj) {
            var imgElement = $(mediaMarkup);
            this.addImageAttributes(imgElement, mediaObj.fid, mediaObj.view_mode);
            var toInsert = this.outerHTML(imgElement);
            content = content.replace(inlineTag, toInsert);
          }
        }
        else {
          debug.debug("Could not find content for " + inlineTag);
        }
      }
    }
    return content;
  },

  /**
   * Detach function, called when a rich text editor detaches
   */
  detach: function (content, settings, instanceId) {
    // Replace all Media placeholder images with the appropriate inline json
    // string. Using a regular expression instead of jQuery manipulation to
    // prevent <script> tags from being displaced.
    // @see http://drupal.org/node/1280758.
    if (matches = content.match(/<img[^>]+class=([\'"])media-image[^>]*>/gi)) {
      for (var i = 0; i < matches.length; i++) {
        var imageTag = matches[i];
        var inlineTag = Drupal.wysiwyg.plugins.media.createTag($(imageTag));
        Drupal.settings.tagmap[inlineTag] = imageTag;
        content = content.replace(imageTag, inlineTag);
      }
    }
    return content;
  },

  /**
   * @param jQuery imgNode
   *  Image node to create tag from
   */
  createTag: function (imgNode) {
    // Currently this is the <img> itself
    // Collect all attribs to be stashed into tagContent
    var mediaAttributes = {};
    var imgElement = imgNode[0];
    var sorter = [];

    // @todo: this does not work in IE, width and height are always 0.
    for (i=0; i< imgElement.attributes.length; i++) {
      var attr = imgElement.attributes[i];
      if (attr.specified == true) {
        if (attr.name !== 'class') {
          sorter.push(attr);
        }
        else {
          // Exctract expando properties from the class field.
          var attributes = this.getAttributesFromClass(attr.value);
          for (var name in attributes) {
            if (attributes.hasOwnProperty(name)) {
              var value = attributes[name];
              if (value.type && value.type === 'attr') {
                sorter.push(value);
              }
            }
          }
        }
      }
    }

    sorter.sort(this.sortAttributes);

    for (var prop in sorter) {
      mediaAttributes[sorter[prop].name] = sorter[prop].value;
    }

    // The following 5 ifs are dedicated to IE7
    // If the style is null, it is because IE7 can't read values from itself
    if (jQuery.browser.msie && jQuery.browser.version == '7.0') {
      if (mediaAttributes.style === "null") {
        var imgHeight = imgNode.css('height');
        var imgWidth = imgNode.css('width');
        mediaAttributes.style = {
          height: imgHeight,
          width: imgWidth
        }
        if (!mediaAttributes['width']) {
          mediaAttributes['width'] = imgWidth;
        }
        if (!mediaAttributes['height']) {
          mediaAttributes['height'] = imgHeight;
        }
      }
      // If the attribute width is zero, get the CSS width
      if (Number(mediaAttributes['width']) === 0) {
        mediaAttributes['width'] = imgNode.css('width');
      }
      // IE7 does support 'auto' as a value of the width attribute. It will not
      // display the image if this value is allowed to pass through
      if (mediaAttributes['width'] === 'auto') {
        delete mediaAttributes['width'];
      }
      // If the attribute height is zero, get the CSS height
      if (Number(mediaAttributes['height']) === 0) {
        mediaAttributes['height'] = imgNode.css('height');
      }
      // IE7 does support 'auto' as a value of the height attribute. It will not
      // display the image if this value is allowed to pass through
      if (mediaAttributes['height'] === 'auto') {
        delete mediaAttributes['height'];
      }
    }

    // Remove elements from attribs using the blacklist
    for (var blackList in Drupal.settings.media.blacklist) {
      delete mediaAttributes[Drupal.settings.media.blacklist[blackList]];
    }
    tagContent = {
      "type": 'media',
      // @todo: This will be selected from the format form
      "view_mode": attributes['view_mode'].value,
      "fid" : attributes['fid'].value,
      "attributes": mediaAttributes
    };
    return '[[' + JSON.stringify(tagContent) + ']]';
  },

  /**
   * Forces custom attributes into the class field of the specified image.
   *
   * Due to a bug in some versions of Firefox
   * (http://forums.mozillazine.org/viewtopic.php?f=9&t=1991855), the
   * custom attributes used to share information about the image are
   * being stripped as the image markup is set into the rich text
   * editor.  Here we encode these attributes into the class field so
   * the data survives.
   *
   * @param imgElement
   *   The image
   * @fid
   *   The file id.
   * @param view_mode
   *   The view mode.
   * @param additional
   *   Additional attributes to add to the image.
   */
  forceAttributesIntoClass: function (imgElement, fid, view_mode, additional) {
    var wysiwyg = imgElement.attr('wysiwyg');
    if (wysiwyg) {
      imgElement.addClass('attr__wysiwyg__' + wysiwyg);
    }
    var format = imgElement.attr('format');
    if (format) {
      imgElement.addClass('attr__format__' + format);
    }
    var typeOf = imgElement.attr('typeof');
    if (typeOf) {
      imgElement.addClass('attr__typeof__' + typeOf);
    }
    if (fid) {
      imgElement.addClass('img__fid__' + fid);
    }
    if (view_mode) {
      imgElement.addClass('img__view_mode__' + view_mode);
    }
    if (additional) {
      for (var name in additional) {
        if (additional.hasOwnProperty(name)) {
          if (name !== 'alt') {
            imgElement.addClass('attr__' + name + '__' + additional[name]);
          }
        }
      }
    }
  },

  /**
   * Retrieves encoded attributes from the specified class string.
   *
   * @param classString
   *   A string containing the value of the class attribute.
   * @return
   *   An array containing the attribute names as keys, and an object
   *   with the name, value, and attribute type (either 'attr' or
   *   'img', depending on whether it is an image attribute or should
   *   be it the attributes section)
   */
  getAttributesFromClass: function (classString) {
    var actualClasses = [];
    var otherAttributes = [];
    var classes = classString.split(' ');
    var regexp = new RegExp('^(attr|img)__([^\S]*)__([^\S]*)$');
    for (var index = 0; index < classes.length; index++) {
      var matches = classes[index].match(regexp);
      if (matches && matches.length === 4) {
        otherAttributes[matches[2]] = {name: matches[2], value: matches[3], type: matches[1]};
      }
      else {
        actualClasses.push(classes[index]);
      }
    }
    if (actualClasses.length > 0) {
      otherAttributes['class'] = {name: 'class', value: actualClasses.join(' '), type: 'attr'};
    }
    return otherAttributes;
  },

  /*
   *
   */
  sortAttributes: function (a, b) {
    var nameA = a.name.toLowerCase();
    var nameB = b.name.toLowerCase();
    if (nameA < nameB) {
      return -1;
    }
    if (nameA > nameB) {
      return 1;
    }
    return 0;
  }
};

})(jQuery);
