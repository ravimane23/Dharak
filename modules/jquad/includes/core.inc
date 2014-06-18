<?php

function jquad_allowed_forms() {
  global $user;
  $forms = array();
  // form_id => menu_path
  if (user_is_anonymous() && (bool) variable_get('jquad_login', TRUE)) {
    $forms['user_login'] = 'user/login';
  }
  if (user_register_access() && (bool) variable_get('jquad_register', TRUE)) {
    $forms['user_register'] = 'user/register';
  }
  if (user_is_anonymous() && (bool) variable_get('jquad_password', TRUE)) {
    $forms['user_pass'] = 'user/password';
  }
  if (user_is_logged_in() && (bool) variable_get('jquad_logout', TRUE)) {
    $forms['logout'] = 'logout';
  }
  // @TODO: Process multi-step forms for user account edits
  // if ($user->uid) {
  //   $forms['user_edit'] = 'user/' . $user->uid . '/edit';
  // }
  return $forms;
}

function jquad_md5_conflicts() {
  static $conflicting_module = array();
  $modules = array(
    'ldapauth', // LDAP Integration module needs clear-text passwords to be sent to authenticate with servers
  );
  foreach ($modules as $module) {
    if (!isset($conflicting_module[$module])) {
      if (module_exists($module)) {
        $info = drupal_parse_info_file(drupal_get_path('module', $module) . '/' . $module . '.info');
        $conflicting_module[$module] = '<strong>' . t($info['name']) . '</strong> (' . $module . ')';
        break;
      }
    }
  }
  return $conflicting_module;
}

function jquad_to_js($var) {
  switch (gettype($var)) {
    case 'boolean':
      return $var ? 'true' : 'false'; // Lowercase necessary!
    case 'integer':
    case 'double':
      return $var;
    case 'resource':
    case 'string':
      return '"'. str_replace(array("\n", "\r", "<",      ">",      "&",      "'",      '"'),
                              array('\\\n', '\\\r', '\\\u003c', '\\\u003e', '\\\u0026', '\\\u0027', '\\\u0022'),
                              $var) .'"';
    case 'array':
      // Arrays in JSON can't be associative. If the array is empty or if it
      // has sequential whole number keys starting with 0, it's not associative
      // so we can go ahead and convert it as an array.
      if (empty ($var) || array_keys($var) === range(0, sizeof($var) - 1)) {
        $output = array();
        foreach ($var as $v) {
          $output[] = jquad_to_js($v);
        }
        return '[ '. implode(', ', $output) .' ]';
      }
      // Otherwise, fall through to convert the array as an object.
    case 'object':
      $output = array();
      foreach ($var as $k => $v) {
        $output[] = jquad_to_js(strval($k)) .': '. jquad_to_js($v);
      }
      return '{ '. implode(', ', $output) .' }';
    default:
      return 'null';
  }
}
