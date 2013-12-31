
------ Mission block --------
1. Copy android_button.png, postcards.gif to sites/default/files/
2. Add mission_block.css to a sub-theme.
3. Create a block, name "Mission" with the HTML code in the file Mission_block.txt. Select "Full HTML" as text format.
4. Set this block "Visibility Setting" as "Only the listed pages" with "<front>".

------ Random Photos block ---------
1. Create new Views, named "Random Photos", choose "Create a block".
2. Set this block:
- Format: Unformatted list. Specify "random_photos_row" as Row class.
- Show: Fields
- Fields:
  + Content: Nid (excluded from display).
  + Content: Gallery Media, 
             Formatter: Media, View mode: Preview.
             Multiple Field Settings: Uncheck "Display all..."
             Style settings: Uncheck all.
             Rewrite results: Check "Ouput this field as a link", with Link path: gallery/[nid]
- Filter criteria: "Content: Type"
- Sort Criteria: "Global: Random"
- Pager: Display: 40 items
- Other:
  + CSS class: random_photos_block.
3. Add the random_photos_block.css file to the theme (Add the line
stylesheets[all][] = css/random_photos_block.css
to the .info file of the theme).
4. Go to Administration > Structure > Blocks, set the Feature Photos block to be appear in Content region.
5. Set this block "Visibility Setting" as "Only the listed pages" with "<front>".
