Command to apply patch:
patch -i file.patch
or
patch -p1 < file.patch

1.
The 0-media-edit_own_permission.patch file is to add more permission.
After applying the path, setting permission, admin has to rebuild the permissions by:
- Go to admin/content/node-settings/rebuild
- Clear cache.

2.
a) The views_galleriffic-patch is to add "Edit Photo" link to Galleriffic.
This patch requires the view to add 2 more fields.
b) Change in Views Galleriffic source code:
- Receive more 2 fields: Author ID & Edit Link.
- Show Edit Link subject to whether the current logged in user is the author.
Explanation:
- The Author ID field is required to check if the current user is the author of the photos.
- The Edit Link field is required to allow owner to edit the photo.
c) With this change, when building the block/page with Views module, admin must provide 2 corresponding field.

3. The nices_menu.patch is to make Nice Menus work with Menu Token.
