<?php

/**
 * Implementation of hook_theme().
 */
function jquad_theme($existing, $type, $theme, $path) {
  return array(
    'jquad_throbber' => array(),
  );
}

function theme_jquad_throbber() {
  return drupal_get_path('module', 'jquad') . '/throbber.gif';
}
