<?php

function jquad_user_authenticate($form_values) {
  global $user;  
  // Force another authentication attempt if previous modules were not successful. This can happen if the MD5 encryption is enabled by this module.
  // If you notice that another module should be included in the MD5 conflict check, please inform the module maintainers in the project's issue queue.
  // This will only autheticate if the user is currently in the database.
  // Based from http://api.drupal.org/api/drupal/modules--user--user.module/function/user_authenticate/6
  if (!$user->uid && isset($form_values['md5'])) {
    $account = jquad_user_load(array('name' => $form_values['name'], 'pass' => trim($form_values['pass']), 'status' => 1));
    if ($account && drupal_is_denied('mail', $account->mail)) {
      form_set_error('name', t('The name %name is registered using a reserved e-mail address and therefore could not be logged in.', array('%name' => $account->name)));
    }
    if (!form_get_errors() && !empty($form_values['name']) && !empty($form_values['pass']) && $account) {
      $user = $account;
      user_authenticate_finalize($form_values);
    }
    else {
      watchdog('user', 'Login attempt failed for %user.', array('%user' => $form_values['name']));
    }
  }
  // If there is still no user loaded, throw a form error.
  if (!$user->uid) {
    form_set_error('name', t('Sorry, unrecognized username or password. <a href="@password">Have you forgotten your password?</a>', array('@password' => url('user/password'))));
  }
}

// Based from http://api.drupal.org/api/drupal/modules--user--user.module/function/user_load/6
// Altered to use password as is, was already altered using JavaScript to be MD5 encrypted.
function jquad_user_load($user_info = array()) {
  // Dynamically compose a SQL query:
  $query = array();
  $params = array();

  if (is_numeric($user_info)) {
    $user_info = array('uid' => $user_info);
  }
  elseif (!is_array($user_info)) {
    return FALSE;
  }

  foreach ($user_info as $key => $value) {
    if ($key == 'uid' || $key == 'status') {
      $query[] = "$key = %d";
      $params[] = $value;
    }
    else if ($key == 'pass') {
      $query[] = "pass = '%s'";
      $params[] = $value;
    }
    else {
      $query[] = "LOWER($key) = LOWER('%s')";
      $params[] = $value;
    }
  }
  $result = db_query('SELECT * FROM {users} u WHERE ' . implode(' AND ', $query), $params);

  if ($user = db_fetch_object($result)) {
    $user = drupal_unpack($user);

    $user->roles = array();
    if ($user->uid) {
      $user->roles[DRUPAL_AUTHENTICATED_RID] = 'authenticated user';
    }
    else {
      $user->roles[DRUPAL_ANONYMOUS_RID] = 'anonymous user';
    }
    $result = db_query('SELECT r.rid, r.name FROM {role} r INNER JOIN {users_roles} ur ON ur.rid = r.rid WHERE ur.uid = %d', $user->uid);
    while ($role = db_fetch_object($result)) {
      $user->roles[$role->rid] = $role->name;
    }
    user_module_invoke('load', $user_info, $user);
  }
  else {
    $user = FALSE;
  }

  return $user;
}
