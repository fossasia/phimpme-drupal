Introduction
===========

It is a very simple module which provides an integration between widely
used modules "LoginToboggan" and "Rules".

Installation
============
1. Enable the module in the module listing page.
2. In LoginToboggan settings check "Set password" option.
3. Thats it!!

For best results, you may want to uncheck the "Immediate login" option on
LoginToboggan settings. This is just for cosmetic reasons, as in most use
cases you might want users to click on the verification link first before
actually letting them in the site.

Usage
=====
Create a rule. You will now see "When the user account is validated"
event under LoginToboggan category.
In this event, the validated user's account information is available.
Use this event to implement various automation tasks.

Currently maintained by Sudhir (porwal.lucky@gmail.com)
