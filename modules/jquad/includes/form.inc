<?php

function jquad_get_form($form_id) {
  global $user;
  $json = array(
    'status' => FALSE,
  );
  if (array_key_exists($form_id, jquad_allowed_forms())) {
    switch ($form_id) {
      case 'user_login' || 'user_register' || 'user_pass' || 'logout':
        module_load_include('inc', 'user', 'user.pages');
        break;
    }
    $json['status'] = TRUE;
    if ($form_id == 'logout') {
      $json['form'] = '<form>' . t('Do you want to logout of your current session?') . '<input type="hidden" name="form_id" value="logout" /><input type="submit" value="Logout" /></form>';
    }
    else {
      $json['form'] = drupal_get_form($form_id);
    }
  }
  else {
    $json['status'] = FALSE;
    $json['html'] = t('An error occurred while retrieving the form.');
    switch ($form_id) {
      case 'user_login':
        if ($user->uid) {
          $json['html'] = t('You are already logged in.');
        }
        break;
    }
  }
  drupal_json($json);
  exit();
}


function jquad_process_form($form_id){
  if (array_key_exists($form_id, jquad_allowed_forms())) {
    $form_values = $_POST;    
    if ($form_id == 'logout') {
      watchdog('user', 'Session closed for %name.', array('%name' => $user->name));
      // Destroy the current session:
      session_destroy();
      // Only variables can be passed by reference workaround.
      $null = NULL;
      user_module_invoke('logout', $null, $user);
      // Load the anonymous user
      $user = drupal_anonymous_user();
      jquad_return_form(array(
        'redirect' => url('<front>', array('absolute' => TRUE)),
      ), $form_values);
    }
    else {
      // When forms validate, they can throw form_set_error which in turn calls drupal_set_messages.
      // Ensure we save current site's message state so we don't generate duplicates messages from form errors to the main site
      $_SESSION['site_messages'] = drupal_get_messages();
      // Process form for validation and submission
      $form_state = array('values' => $form_values, 'submitted' => TRUE);
      drupal_execute($form_id, $form_state);
    }
  }
}

/**
 * Implementation of hook_form_alter().
 */
function jquad_form_alter(&$form, $form_state, $form_id) {
  // Clear cache after saving settings, menus need to be rebuilt.
  if ($form_id == 'jquad_settings') {
    $form['#submit'][] = 'jquad_settings_submit';
  }
  // Only alter forms that are allowed for validation
  $forms = jquad_allowed_forms();
  if (array_key_exists ($form_id, $forms)) {
    // Ensure the that this is being processed from a valid URL
    if ((arg(0) == 'ajax' && arg(1) == 'jquad' && arg(2) == 'form') || (array_pop(arg()) == 'process')) {
      // Make this form cacheable as it is being processed via AJAX.
      $form['#cache'] = TRUE;
      // Set form path
      $path = $forms[$form_id];
      // Ensure user login is secure if supported
      if (module_exists('securepages')) {
        $secure = (bool)securepages_match($path);
        $path = securepages_url($path, array('absolute' => TRUE, 'secure' => $secure));
      }
      else {
        $path = url($path, array('absolute' => TRUE));
      }
      // Ensure form is posted to the correct url
      $form['#action'] = $path;
      // Use validation only as a way to return JSON data if form doesn't validate, should be ran after everything else.
      $form['#validate'][] = 'jquad_validate_form';
      // Use submit only as a way to return JSON data after the form has validated, should be ran after everything else.
      $form['#submit'][] = 'jquad_submit_form';
    }
  }
}

function jquad_validate_form($form, &$form_state) {
  $form_values = $form_state['clicked_button']['#post'];
  // Check for forms that need additional validation in special cases where this module could alter the form.
  switch ($form_values['form_id']) {
    case 'user_login':
      if (isset($_POST['md5'])) {
        $form_values['md5'] = $_POST['md5'];
      }
      // Load user include
      module_load_include('inc', 'jquad', 'includes/user');
      jquad_user_authenticate($form_values);
      break;
    default:
      break;
  }
  // Revert messages to previous state so any messages created by the submit handlers can be shown on main site after redirect.
  $_SESSION['messages'] = $_SESSION['site_messages'];
  
  // If form has errors, return errors immeadiately so user can correct it.
  $errors = form_get_errors();
  if (!empty($errors)) {
    jquad_return_form(array(
      'status' => FALSE,
      'messages' => $errors,
    ), $form_values);
  }
}

function jquad_submit_form($form, &$form_state) {
  $json = array(
    'status' => TRUE,
    'messages' => array(),
  );
  if (!empty($form_state['redirect']) && !(bool) variable_get('jquad_redirect', TRUE)) {
    $json['redirect'] = url($form_state['redirect'], array('absolute' => TRUE));
  }
  jquad_return_form($json, $form_state['clicked_button']['#post']);
}

function jquad_return_form($json = array(), $form_values) {
  // Load default JSON return data
  $json = array_merge(array(
    'status' => TRUE,
    'messages' => array(),
  ), $json);
  // Determine which type of response to send back
  if (isset($form_values['windowname'])) {
    print '<script type="text/javascript">window.name=\'' . jquad_to_js($json) . '\';</script>';
  }
  else {
    print drupal_json($json);
  }
  exit();
}
