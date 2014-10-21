<?php

/**
 * Admin page callback file for userpoints extend role module
 */

/**
 * List userpoints extend role rules
 */
function userpoints_extend_role_overview() {
  $header = array(
    t('Category'),
    t('Multiple Threshold'),
    t('Roles'),
    t('Extend Amount (months)'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();

  // Get all extend role rules from the database
  $result = db_select('userpoints_extend_role_rules', 'r')
    ->fields('r', array('rid', 'categorytid', 'multiple_threshold', 'roles', 'extend_amount'))
    ->orderBy('weight')
    ->execute()
    ->fetchAll();

  // Loop through list and add them to the table
  foreach ($result as $data) {
    $roles = array_map('check_plain', unserialize($data->roles));
    if (empty($roles))
      $roles = array();
    $rows[] = array(
      theme('userpoints_extend_role_categorytid', array('categorytid' => $data->categorytid)),
      theme('userpoints_extend_role_multiple_threshold', array('multiple_threshold' => $data->multiple_threshold)),
      theme('userpoints_extend_role_roles', array('items' => $roles)),
      theme('userpoints_extend_role_extend_amount', array('extend_amount' => $data->extend_amount)),
      l(t('Edit'), 'admin/config/people/userpoints-extend-role/edit/' . $data->rid),
      l(t('Delete'), 'admin/config/people/userpoints-extend-role/delete/' . $data->rid),
    );
  }

  if (!rows) {
    $rows[] = array(array(
      'data' => t('No rules available.'),
      'colspan' => 6,
    ));
  }

  $build['userpoints-extend-role_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  return $build;
}

function theme_userpoints_extend_role_categorytid($variables) {
  $term = taxonomy_term_load($variables['categorytid']);
  $output = nl2br($term ? $term->name : '');

  if (empty($output)) {
    $output = '<i>' . t('Empty') . '</i>';
  }

  return $output;
}

function theme_userpoints_extend_role_multiple_threshold($variables) {
  $output = nl2br($variables['multiple_threshold']);
  return $output;
}

function theme_userpoints_extend_role_roles($variables) {
  $items = array_values(array_intersect_key(user_roles(), $variables['items']));
  if (empty($items)) {
    return '<i>' . t('No roles') . '</i>';
  }
  return theme('item_list', array('items' => $items));
}

function theme_userpoints_extend_role_extend_amount($variables) {
  $output = nl2br($variables['extend_amount']);
  return $output;
}

function userpoints_extend_role_edit_form($form, &$form_state, array $rule = array())
{
  // default values
  $rule += array(
    'categorytid' => 0,
    'multiple_threshold' => 10,
    'roles' => array(),
    'extend_amount' => 1,
    'rid' => NULL,
    'weight' => 0,
  );

  $access = user_access('administer users');

  $form['categorytid'] = array(
    '#type'           => 'select',
    '#title'          => t('Category'),
    '#default_value'  => $rule['categorytid'],
    '#options'        => userpoints_get_categories(),
    '#description'    => t('Userpoints of this category will be used to determine if the role expiration should be extended. You can modify what categories are available by modifying the <a href="!url">Userpoints taxonomy</a>',
      array('!url' => url('admin/content/taxonomy/' . variable_get(USERPOINTS_CATEGORY_DEFAULT_VID, '')))),
  );

  $form['multiple_threshold'] = array(
    '#type'           => 'textfield',
    '#title'          => t('!Points multiple for extending role expiry', userpoints_translation()),
    '#default_value'  => $rule['multiple_threshold'],
    '#size'           => 5,
    '#maxlength'      => 5,
    '#description'    => t('The multiple of points for which the role expiry will be extended. For example, if set to 30, then after each multiple of 30 ponts of the specified category, the role will be extended by the amount of time configured below. Leaving at default of 0 will not perform any extensions.'),
  );

  $form['roles'] = array(
    '#type'           => 'checkboxes',
    '#title'          => t('Roles'),
    '#default_value'  => $rule['roles'],
    '#options'        => user_roles(TRUE),
    '#description'     => t('Role(s) to extend when points cross threshold multiple boundary.  If the user doesn\'t have an expiry for that role, it wil fail silently and go on to the next role.'),
  );

  $form['extend_amount'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Time to extend role expiration by, in months'),
    '#default_value'  => $rule['extend_amount'],
    '#size'           => 5,
    '#maxlength'      => 5,
    '#description'    => t('Number of months to extend the role(s) expirations by when each threshold level is passed.'),
  );
  $form['weight'] = array(

    '#title' => t('Weight'),
    '#default_value' => $rule['weight'],
    '#description' => t('When evaluating userpoints extend role rules, those with lighter (smaller) weights get evaluated before rules with heavier (larger) weights.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if ($rule['rid']) {
    $form['rid'] = array(
      '#type' => 'hidden',
      '#value' => $rule['rid'],
    );
  }

  return $form;
}
 
/**
 * Validatd the userpoints extend role edit page form submission.
 */
 
function userpoints_extend_role_edit_form_validate($form, &$form_state) {
}

/**
 * Process the userpoints extend role edit page form submission.
 */

function userpoints_extend_role_edit_form_submit($form, &$form_state) {
  $form_state['values']['roles'] = serialize(array_filter($form_state['values']['roles']));

  if (empty($form_state['values']['rid'])) {
    drupal_write_record('userpoints_extend_role_rules', $form_state['values']);
  }
  else {
    drupal_write_record('userpoints_extend_role_rules', $form_state['values'], array('rid'));
  }
  $categories = userpoints_get_categories();
  $tid = $form_state['values']['categorytid'];
  drupal_set_message(t('Userpoints extend role rule for category %category has been saved.', array('%category' => $categories[$tid])));

  $form_state['redirect'] = 'admin/config/people/userpoints-extend-role';
}

/**
 * Form builder for deleting a userpoints extend role rule.
 */
function userpoints_extend_role_delete_form($form, &$form_state, array $rule) {
  $form['userpoints_extend_role'] = array(
    '#type' => 'value',
    '#value' => $rule,
  );

  $categories = userpoints_get_categories();
  $tid = $rule['categorytid'];

  return confirm_form(
    $form,
    t('Are you sure you want to delete the userpoints extend role rule for category %category?', array('%category' => $categories[$tid])),
    'admin/config/people/userpoints-extend-role',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for the confirm delete userpoints extend role form.
 */
function userpoints_extend_role_rule_delete_form_submit($form, &$form_state) {
  $rule = $form['userpoints_extend_role']['#value'];
  $categories = userpoints_get_categories();
  $tid = $rule['categorytid'];

  db_delete('userpoints_extend_role_rule')
    ->condition('rid', $rule['rid'])
    ->execute();

  drupal_set_message(t('The userpoints extend role rule for category %cat has been deleted.', array('%cat' => $categories[$tid])));
  $form_state['redirect'] = 'admin/config/people/userpoints-extend-role'; 
}
