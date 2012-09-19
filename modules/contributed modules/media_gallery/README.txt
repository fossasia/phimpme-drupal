INSTALLATION
------------

1. Download Media Gallery from http://drupal.org/project/media_gallery

   Always use the latest official release. Unpack it in your contributed
   modules directory (usually sites/all/modules).

2. Download other necessary projects.

   In addition to Media Gallery itself, you'll need several other modules and
   libraries on your site in order for your site's gallery functionality to
   work as designed.

   You should generally download the latest official release of each project.
   However, this version of Media Gallery has been specifically tested with the
   versions shown in parentheses below. If you do not use these exact versions,
   it is likely that this release will not work correctly.

   Required:

   a. Media (latest 1.x release, tested with 7.x-1.0-rc2)
      - Download from http://drupal.org/project/media and unpack it in your
        contributed modules directory (usually sites/all/modules).
   b. Multiform (7.x-1.0-beta2)
      - Download from http://drupal.org/project/multiform and unpack it in your
        contributed modules directory (usually sites/all/modules).
   c. Chaos tool suite (latest release, tested with 7.x-1.0-beta1)
      - Download from http://drupal.org/project/ctools and unpack it in your
        contributed modules directory (usually sites/all/modules).
   d. ColorBox jQuery plugin (latest release, tested with 1.3.17)
      - Download from http://colorpowered.com/colorbox and unpack it in
        sites/all/libraries (if the directory doesn't exist, create it first).

   Recommended for the best experience:

   e. Media Browser Plus (latest release, tested with 7.x-1.0-beta3)
      - Download from http://drupal.org/project/media_browser_plus and unpack
        it in your contributed modules directory (usually sites/all/modules).
   f. Media YouTube (latest release, tested with 7.x-1.0-alpha5)
      - Download from http://drupal.org/project/media_youtube and unpack it in
        your contributed modules directory (usually sites/all/modules).
   g. Plupload (latest release, tested with 7.x-1.0-beta3)
      - First, download the module from http://drupal.org/project/plupload and
        unpack it in your contributed modules directory (usually
        sites/all/modules).
      - Next, download the Plupload JavaScript library from
        http://www.plupload.com (latest release, tested with 1.4.3.2) and
        unpack it in sites/all/libraries (if the directory doesn't exist,
        create it first). See the Plupload module's README.txt file for
        up-to-date instructions.

3. Enable the modules.

   a. Visit your site's Administration > Modules page.
   b. Enable Media Gallery itself to get the basic gallery functionality
      working. This will automatically enable all required modules.
   c. Optionally enable Media Browser Plus to get a better experience browsing
      and adding media to your galleries.
   d. Optionally enable Media YouTube to allow you to add YouTube videos to
      your galleries.
   e. Optionally enable Plupload to allow you to quickly add large numbers of
      images to your gallery at once, rather than having to upload them one at
      a time.

GETTING STARTED
---------------

There are a lot of fun things you can do with this module. Here are some
suggestions for getting started:

1. Go to Add content, and create a gallery.
2. The gallery will be empty by default, so use the "Add media" link to quickly
   upload a few images from your computer. Now you have a completely functional
   gallery, just like that!
3. Use the "Add media" link again, and switch to the "Web" tab. Find a YouTube
   video you like, and paste its URL there. Now it will be in the gallery too.
   (Note that the "Library" tab allows you to add existing media items from
   your site - for example, images that were uploaded using other modules - to
   the gallery as well.)
4. Back in the gallery, you can use drag-and-drop to rearrange the images and
   videos to your liking.
5. Use the "Edit media" tab to quickly edit all your items at once (for
   example, to give each item in the gallery a description, or to tag it).
6. Now click on one of the images in your gallery and watch it pop up in a
   lightbox. Inside the lightbox, you can scroll through the gallery items
   individually, or watch them play in a slideshow. Note that you can play your
   video from inside the lightbox also.
7. Back in the gallery, you can use the "Edit gallery" tab to customize several
   ways in which the gallery displays, and also make the gallery available as a
   block (which you can then place on your site by going to Administration >
   Structure > Blocks).
8. Your site's main menu should have a link called Galleries which was
   automatically added when you turned on the module. This page displays a
   collection of all the galleries you created. You can use drag-and-drop to
   rearrange the galleries here also.
9. The "Edit all galleries" tab that's available from here also lets you
   customize some additional aspects of how the Galleries page is displayed.

There's lots more to explore, so have fun!

CAVEATS
-------

This module is still in beta, and many of the modules it depends on are in beta
themselves, or even in alpha.

We therefore don't recommend using it on production sites, unless you're able
to tolerate bugs, maintain frequent database backups of your site, and keep
up-to-date on the module's development so that you can quickly apply any
patches if security issues are discovered. (As per Drupal's official policy at
http://drupal.org/security-advisory-policy, security issues discovered in a
module which has an alpha, beta, or other non-stable release will not result in
an advisory being issued and may be discussed publicly before a fix is
available.)

If you find bugs, please use the "Issues for Media Gallery" section at
http://drupal.org/project/media_gallery to search for existing bug reports
about the same issue, and if there isn't one, file a new bug report. Please
help us make this module better!  In addition to coding and reporting bugs, we
can use help managing the issue queue as well as writing documentation. If you
want to help but don't know how, feel free to file a support request in the
issue queue, and we'll get you started with some suggestions.
