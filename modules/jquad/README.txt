jQuery UI Account Dialogs 1.0 for Drupal 6.x
Author: Mark Carver <mark.carver@me.com>
-------------------------------------------------------------------------------

This module provides seamless jQuery UI dialog integrations for login, register,
password reset and logout links through out your Drupal installation.


Features:
  * Fallback Support - If user has JavaScript disabled, links are unaltered and
    login/login links will continue to their original destination.
  * Realtime authentication - Speed up user login attempts without having to
    load the same page and bootstrapping Drupal for each attempt.
  * MD5 password encryption - Encrypts passwords using the same Drupal method of
    storage. No clear passwords sent, regardless of SSL.
  * SSL Support via Secure Pages - Secure your login path and provide secure
    logins while user is still on non-secure pages.
  * Native Form API validation - This module integrates naturally with other
    modules that already handle user accounts. Works seamlessly for modules such
    as LDAP Integration.

Installation
-------------------------------------------------------------------------------
Go to Administer -> Site building -> Modules and enable the jQuery UI Account
Dialogs module in the User Interface section.

jQuery UI Account Dialogs should work right out of the box with most themes. The
setup is fairly simple, however you might want to reconfigure your theme if it uses
custom URL paths (see below for which links are processed).

Configuration
-------------------------------------------------------------------------------
Go to Administer -> Site configuration -> jQuery UI Account Dialogs and configure
your desired setup:

* MD5 Encryption
    Enforce MD5 encryption when sending user passwords through the dialog box.
    WARNING: MD5 Encryption can sometimes conflict with other modules. This is
    often the result of a module needing clear-text passwords to be received
    so it can process them as needed. Such is the case with the LDAP Integration
    module.
* Redirect to current page after login
    If enabled and after successful login, the user will be redirected to the page
    they are currently viewing instead of their account.
    NOTE: This setting only applies to the dialog forms. Standard form pages are
    not altered.
* Show dialog for all "Login" links
    Links matching /?=user/login or /user/login will prompt the user to provide
    their name and password to complete login.
* Show dialog for all "Create new account" or "Register" links
    Links matching /?=user/register or /user/register will prompt the user to
    complete the registration form.
* Show dialog for all "Retrieve Password" links
    Links matching /?=user/password or /user/password will prompt the user to
    provide their username or email address to receive an email.
* Show dialog for all "Logout" links
    Links matching /?=logout or /logout will prompt the user to confirm their
    decision to continue logging out.
