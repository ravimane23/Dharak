<?php

function jquad_settings() {  
  $form = array();
  $form['jquad_md5'] = array(
    '#type' => 'checkbox', 
    '#title' => t('MD5 Encryption'),
    '#default_value' => variable_get('jquad_md5', TRUE), 
    '#description' => t('Enforce MD5 encryption when sending user passwords through the dialog box.'),
  );
  $modules = jquad_md5_conflicts();
  if (!empty($modules)) {
    $form['jquad_md5']['#value'] = FALSE;
    $form['jquad_md5']['#disabled'] = TRUE;
    $form['jquad_md5']['#description'] .= '<div class="error">' . t('MD5 Encryption conflicts with the following modules. This is often the result of a module needing clear-text passwords to be received so it can process them as needed: ') . '<ul><li>' . implode('</li><li>', $modules) . '</li></ul></div>';
  }
  $form['jquad_redirect'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Redirect to current page after login'), 
    '#default_value' => variable_get('jquad_redirect', TRUE), 
    '#description' => t('If enabled and after successful login, the user will be redirected to the page they are currently viewing instead of their account.<br /><strong>NOTE:</strong> This setting only applies to the dialog login form. The core Drupal login page is not altered.'),
  );
  $form['jquad_login'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Show dialog for all "Login" links'), 
    '#default_value' => variable_get('jquad_login', TRUE), 
    '#description' => t('Links matching <strong><code>/?=user/login</code></strong> or <strong><code>/user/login</code></strong> will prompt the user to provide their name and password to complete login.'),
  );
  $form['jquad_register'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Show dialog for all "Create new account" or "Register" links'), 
    '#default_value' => variable_get('jquad_register', TRUE), 
    '#description' => t('Links matching <strong><code>/?=user/register</code></strong> or <strong><code>/user/register</code></strong> will prompt the user to complete the registration form.'),
  );
  $form['jquad_password'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Show dialog for all "Retrieve Password" links'), 
    '#default_value' => variable_get('jquad_password', TRUE), 
    '#description' => t('Links matching <strong><code>/?=user/password</code></strong> or <strong><code>/user/password</code></strong> will prompt the user to provide their username or email address to receive an email.'),
  );
  $form['jquad_logout'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Show dialog for all "Logout" links'), 
    '#default_value' => variable_get('jquad_logout', TRUE), 
    '#description' => t('Links matching <strong><code>/?=logout</code></strong> or <strong><code>/logout</code></strong> will prompt the user to confirm their decision to continue logging out.'),
  );
  return system_settings_form($form);
}

function jquad_settings_submit($form, &$form_state) {
  drupal_flush_all_caches();
  drupal_set_message(t('Caches cleared.'));
}