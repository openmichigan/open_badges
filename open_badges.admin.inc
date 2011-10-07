<?php

/**
 * @file
 * @brief Open Badges admin functions
 *
 * This file contains all the admin functions used by the module.
 *
 * @author Jeff Robbins (jjeff), http://drupal.org/user/17190
 * @author Chad Phillips (hunmonk), http://drupal.org/user/22079
 * @author Heine Deelstra (Heine), http://drupal.org/user/17943
 * @author Nuno Veloso (nunoveloso18), http://drupal.org/user/80656
 * @author Richard Skinner (Likeless), http://drupal.org/user/310635
 * @author Nancy Wichmann (NancyDru), http://drupal.org/user/101412
 *
 */

/**
 * Form builder; list of badges
 */
function open_badges_badgelist_form() {
  //dpm("open_badges_badgelist_form: entered");
  // Load the badges that we want to display.
  $form['header'] = array('#type' => 'value', '#value' => array(
    array('data' => t('Name'), 'field' => 'name'),
    array('data' => t('Badge')),
    array('data' => t('Description')),
    //array('data' => t('Criteria URL')),
    array('data' => t('Recipients')),
    //array('data' => t('Weight'), 'field' => 'weight', 'sort' => 'asc'),
    array('data' => t('Edit')),
    array('data' => t('Delete'))
  ));

  $result = pager_query('SELECT bid, name, image, description, criteria, weight FROM {open_badges_badges} ubb '. tablesort_sql($form['header']['#value']), 50 );

  // Build a table listing the appropriate badges.
  while ($badge = db_fetch_object($result)) {
    //dpm($badge, "open_badges_badgelist_form: badge");
    $badge->class = 'badge ' . _open_badges_class($badge);
    $form['name'][$badge->bid] = array('#value' => check_plain($badge->name));
    $form['badge'][$badge->bid] = array('#value' => theme('open_badge', $badge));
    $form['description'][$badge->bid] = array('#value' => check_plain($badge->description));
    //$form['criteria'][$badge->bid] = array('#value' => l($badge->criteria, $badge->criteria));
    //$form['weight'][$badge->bid] = array('#type' => 'textfield', '#size' => 4, '#maxlength' => 255, '#default_value' => $badge->weight );
    $form['edit'][$badge->bid] = array('#value' => l(t('edit'), 'admin/user/open_badges/edit/'. $badge->bid));
    $form['delete'][$badge->bid] = array('#value' => l(t('delete'), 'admin/user/open_badges/delete/'. $badge->bid));

    // Get a list of all the recipients of this badge
    $recipients = array();
    $all_r = open_badges_get_badge_recipients($badge->bid);
    //dpm($all_r, "All recipients returned from open_badges_get_badge_recipients");
    for($i = 0; $i < count($all_r); $i++) {
      $recipients[] = $all_r[$i]->name . ' (' . $all_r[$i]->type . ')';
    }
    $num_recipients = count($recipients);
    array_unshift($recipients, t('There are @count recipients', array('@count' => $num_recipients)));
    if ($num_recipients > 5) {
      array_splice(&$recipients, 6, $num_recipients, l(t('add/remove'), 'admin/user/open_badges/recipients/'. $badge->bid));
    } else {
      $recipients[] = l(t('add/remove'), 'admin/user/open_badges/recipients/'. $badge->bid);
    }
    $form['recipients'][$badge->bid] = array('#value' => implode('<br />', $recipients));

  }

  // Don't need submit button since we removed weight!
  //$form['submit'] = array('#type' => 'submit', '#value' => t('Update'));

  $form['pager'] = array('#value' => theme('pager', NULL, 50, 0));

  //dpm($form, "open_badges_badgelist_form: The form that has been built");
  return $form;
}

/**
 * Validate open_badges_badgelist_form form submissions.
 *
 * All weights should be numeric.
 */
function open_badges_badgelist_form_validate($form, &$form_state) {
  if (isset($form['weight']) && is_array($form['weight'])) {
    foreach (element_children($form['weight']) as $bid) {
      if (!is_numeric($form_state['values'][$bid])) {
        form_set_error($bid, t('All weight values must be numeric.'));
      }
    }
  }
}

/**
 * Process open_badges_badgelist_form form submissions.
 *
 * Update the badge weights.
 */
function open_badges_badgelist_form_submit($form, &$form_state) {
  if (isset($form['weight']) && is_array($form['weight'])) {
    foreach (element_children($form['weight']) as $bid) {
      db_query("UPDATE {open_badges_badges} SET weight = %d WHERE bid = %d",
      $form_state['values'][$bid],
      $bid
      );
    }
    drupal_set_message(t('The badge weights have been updated.'));
  }
}

/**
 * Theme the badge list form.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @ingroup themeable
 */
function theme_open_badges_badgelist_form($form) {
  //dpm("theme_open_badges_badgelist_form: entered");
  $output = '';

  // Loop through the array items in the name array to get all the bids for our listed badges.
  if (isset($form['name']) && is_array($form['name'])) {
    foreach (element_children($form['name']) as $key) {
      //We only want bids as values of $key
      if (!is_numeric($key)) {
        continue;
      }

      // Create the rows array for the table theme.
      $row = array();
      $row[] = drupal_render($form['name'][$key]);
      $row[] = drupal_render($form['badge'][$key]);
      $row[] = drupal_render($form['description'][$key]);
      //$row[] = drupal_render($form['criteria'][$key]);
      $row[] = drupal_render($form['recipients'][$key]);
      //$row[] = drupal_render($form['weight'][$key]);
      $row[] = drupal_render($form['edit'][$key]);
      $row[] = drupal_render($form['delete'][$key]);
      $rows[] = $row;
    }

    // Add the submit button.
    $rows[] = array(
      array('data' => drupal_render($form['submit']), 'align' => 'center', 'colspan' => '20'),
      );
  }
  else {
    $rows[] = array(array('data' => t('No badges available.'), 'colspan' => '5'));
  }

  // Theme all that we have processed so far into a table.
  $output .= theme('table', $form['header']['#value'], $rows);

  // Create the table's pager.
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  // Render any remaining form elements.
  $output .= drupal_render($form);

  return $output;
}

/**
 * Theme the open badges roles form.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @ingroup themeable
 */
function theme_open_badges_roles_form($form) {
  //dpm($form, "theme_open_badges_roles_form: entered");
  $output = '';

  $rows = array();
  $row = array();
  $row[] = drupal_render($form['blocked'][0]['badge']);
  $row[] = drupal_render($form['blocked'][0]['issuedate']);
  $row[] = drupal_render($form['blocked'][0]['expirationdate']);
  $row[] = drupal_render($form['blocked'][0]['evidenceurl']);
  $rows[] = $row;
  $form['blocked']['#children'] = theme('table', $form['header']['#value'], $rows);

  // Loop through the array items in the name array to get all the bids for our listed badges.
  if (isset($form['roles']) && is_array($form['roles'])) {
    $rows = array();
    foreach (element_children($form['roles']) as $key) {
      //We only want rids as values of $key
      if (!is_numeric($key)) {
        continue;
      }

      // Create the rows array for the table theme.
      $row = array();
      $row[] = drupal_render($form['roles'][$key]['badge']);
      $row[] = drupal_render($form['roles'][$key]['issuedate']);
      $row[] = drupal_render($form['roles'][$key]['expirationdate']);
      $row[] = drupal_render($form['roles'][$key]['evidenceurl']);
      $rows[] = $row;
    }
  }
  else {
    $rows[] = array(array('data' => t('No roles defined.'), 'colspan' => '5'));
  }

  // Theme all that we have processed so far into a table.
  $form['roles']['#children'] = theme('table', $form['header']['#value'], $rows);

  // Create the table's pager.
  if ($form['pager']['#value']) {
    $output .= drupal_render($form['pager']);
  }

  // Render any remaining form elements.
  $output .= drupal_render($form);

  return $output;
}

/**
 * Define the edit form for userbadges.
 */
function open_badges_edit_form($form_state, $bid = NULL) {
  // If we have been given an existing badge (bid) then get all the badge info into $edit.
  if (is_numeric($bid)) {
    $edit = db_fetch_object(db_query('SELECT * FROM {open_badges_badges} WHERE bid = %d', $bid));
    if (is_numeric($edit->bid)) {
      $form['bid'] = array(
        '#type' => 'value',
        '#value' => $edit->bid,
      );
    }
  }

  // Are we using a library image or an image URL?
  if ( isset($edit) && valid_url($edit->image, TRUE) ) {
    $imageurl = $edit->image;
    $libraryimage = '';
  }
  else {
    $libraryimage = $edit->image;
    $imageurl = '';
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($edit) ? $edit->name : '',
    '#size' => 40,
    '#maxlength' => 100,
    '#description' => t('Name for the badge. Will be displayed as a tooltip when rolling over the badge.'),
    '#required' => TRUE,
  );
  $form['criteria'] = array(
    '#type' => 'textfield',
    '#title' => t('Criteria URL'),
    '#default_value' => isset($edit) ? $edit->criteria : '',
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('URL where a description of the badge and the criteria for earning the badge can be found.'),
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description (128 characters)'),
    '#default_value' => isset($edit) ? $edit->description : '',
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('Short description of the badge.'),
    '#required' => TRUE,
  );
  $form['imageurl'] = array(
    '#type' => 'textfield',
    '#title' => t('Image URL'),
    '#default_value' => $imageurl,
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('The image URL for this badge. If you want to use an image from the open badges image library, select from the list below.'),
  );
  $form['issuername'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge issuer name'),
    '#default_value' => isset($edit) ? $edit->issuername : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('Human-readable name of the issuing agent. Defaults to \'' . variable_get('open_badges_defaultissuername', '') . '\' if not specified.'),
  );
  $form['issuerorg'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge issuer organization'),
    '#default_value' => isset($edit) ? $edit->issuerorg : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('The name of the organization for which the badge is being issued.'),
  );
  $form['issuercontact'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge issuer contact information'),
    '#default_value' => isset($edit) ? $edit->issuercontact : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('A human-monitored email address associated with the issuer.'),
  );
  $form['issuerurl'] = array(
    '#type' => 'textfield',
    '#title' => t('Badge issuer URL'),
    '#default_value' => isset($edit) ? $edit->issuerurl : '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('URL for the badge issuer.'),
  );
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#size' => 4,
    '#maxlength' => 10,
    '#default_value' => isset($edit) ? $edit->weight : 0,
    '#description' => t('Lighter weighted items float to the top of lists. Heavier items go to the bottom. You must enter a number (negative values are allowed).'),
    '#required' => TRUE,
  );

  // Tokens help.
  if (module_exists('token')) {
    $form['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['token_help']['help'] = array(
      '#value' => theme('token_help', array('userbadge', 'user')),
    );
  }
  else {
    $form['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['token_help']['help'] = array(
      // Ignore Coder flag on the link.
      '#value' => t('Install the !link module if you want this URL to include dynamic elements such as badge ID numbers.', array('!link' => l('Token', 'http://drupal.org/project/token', array('absolute' => TRUE)))),
      );
  }

  if (module_exists('taxonomy')) {
    if (variable_get('open_badges_vid', '')) {
      $form['tid'] = taxonomy_form(variable_get('open_badges_vid', ''), $edit->tid);
      // Taxonomy sets the weight to -15, so we should get rid of it to preserve the ordering.
      unset($form['tid']['#weight']);
    }
    else {
      $form['tid']['help'] = array(
        '#value' => t('Activate the taxonomy module if you want to associate badges with taxonomy terms.'),
      );
    }
  }

  $form['unhideable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cannot be Hidden'),
    '#default_value' => $edit->unhideable,
    '#description' => t('If this is set, the badge cannot be hidden by being moved down in weight. It will always show up.'),
  );
  $form['fixedweight'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fixed Weight'),
    '#default_value' => $edit->fixedweight,
    '#description' => t('If this is set, the badge cannot have a user assigned weight, regardless of settings.'),
  );
  $form['doesnotcounttolimit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Does Not Count to Limit'),
    '#default_value' => $edit->doesnotcounttolimit,
    '#description' => t('If this is set, the badge does not count towards the limit for number of badges to display per user.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $selects = array('' => t('Use the image URL above instead of a library image.')) + open_badges_image_selects();
  if (count($selects)) {
    $form['image'] = array(
      '#type' => 'radios',
      '#title' => t('Image Library'),
      '#default_value' => $libraryimage,
      '#options' => $selects,
      '#description' => t('If you have not entered an image URL above, you can select an image from this open badges image library to associate with this badge. You can upload additional images to the library under the <a href="@url">images</a> tab.', array('@url' => url("admin/user/open_badges/images"))),
    );
  }
  else {
    $form['noimages'] = array(
      '#type' => 'item',
      '#title' => t('Image'),
      '#value' => t('No badge images uploaded.'),
      '#description' => t('You can enter an image URL directly above, but if you want to instead upload your image to the open badges image library, use <a href="@upload_link">this link</a>. Note that private download is incompatible with this library and should use direct URL entry instead.', array('@upload_link' => url("admin/user/open_badges/images"))),
    );
  }
  $form['submit2'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Validate open_badges_edit_form form submissions.
 */
function open_badges_edit_form_validate($form, &$form_state) {
  // Either a URL or an item from the image library should be selected, but not both.
  if ($form_state['values']['imageurl'] && $form_state['values']['image']) {
    form_set_error('image', t('You cannot both enter an image URL and select an image from the library too. A badge can only have one image.'));
  }
  elseif (!$form_state['values']['imageurl'] && !$form_state['values']['image']) {
    form_set_error('image', t('You need to either enter an image URL or select an image from the library. Your badge needs an image.'));
  }

  // The criterial URL must be a valid url.
  if ($form_state['values']['criteria'] && !valid_url($form_state['values']['criteria'], TRUE)) {
    form_set_error('criteria', t('This is not a valid criteria URL. You need to enter a complete URL, including the "http://" at the start.'));
  }

  // The image URL must be a valid url.
  if ($form_state['values']['imageurl'] && !valid_url($form_state['values']['imageurl'], TRUE)) {
    form_set_error('imageurl', t('This is not a valid image URL. You need to enter a complete image URL, including the "http://" at the start.'));
  }

  // The issuer URL must be a valid url.
  if ($form_state['values']['issuerurl'] && !valid_url($form_state['values']['issuerurl'], TRUE)) {
    form_set_error('issuerurl', t('This is not a valid issuer URL. You need to enter a complete URL, including the "http://" at the start.'));
  }

  // Weights must be numeric.
  if (!is_numeric($form_state['values']['weight'])) {
    form_set_error('weight', t('Your value for the weight must be a number. Negative numbers are allowed, but not decimals.'));
  }

}

/**
 * Process open_badges_edit_form form submissions.
 *
 * Inserts the badge into the DB and sets a success message
 */
function open_badges_edit_form_submit($form, &$form_state) {
  $edit = $form_state['values'];
  $edit = (object)$edit;

  $image = $edit->imageurl ? trim($edit->imageurl) : $edit->image;

  // If the badge already exists, delete it and re-insert it.
  if (isset($edit->bid) && preg_match("/^[0-9]+$/D", $edit->bid)) {
    db_query('DELETE FROM {open_badges_badges} WHERE bid = %d', $edit->bid);
    $result = db_query("
      INSERT INTO {open_badges_badges} (bid, name, image, criteria, description, issuername, issuerorg, issuercontact, issuerurl, weight, tid, unhideable, fixedweight, doesnotcounttolimit)
      VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d)",
      $edit->bid, trim($edit->name), $image, trim($edit->criteria), trim($edit->description), trim($edit->issuername), trim($edit->issuerorg), trim($edit->issuercontact), trim($edit->issuerurl), $edit->weight, $edit->unhideable, $edit->fixedweight, $edit->doesnotcounttolimit, $edit->tid
    );
  }
  // If the badge does not already exist, create it anew.
  else {
    $result = db_query("
      INSERT INTO {open_badges_badges} (name, image, criteria, description, issuername, issuerorg, issuercontact, issuerurl, weight, tid, unhideable, fixedweight, doesnotcounttolimit)
      VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d)",
      trim($edit->name), $image, trim($edit->criteria), trim($edit->description), trim($edit->issuername), trim($edit->issuerorg), trim($edit->issuercontact), trim($edit->issuerurl), $edit->weight, $edit->unhideable, $edit->fixedweight, $edit->doesnotcounttolimit, $edit->tid
    );
  }
  if ($result) {
    drupal_set_message(t('Badge %badgename saved.', array('%badgename' => $edit->name)));
  }
  else {
    drupal_set_message(t('There was a problem saving the badge information into the database.'));
  }
  $form_state['redirect'] = 'admin/user/open_badges';
}


/**
 * Define the bulk edit form for open badges.
 */
function open_badges_recipients_form($form_state, $bid = NULL) {
  //dpm("open_badges_recipients_form: entered, bid is {$bid}");
  $form = array();
  $title = '';

  if ($bid == NULL) {
    $form['badgeid'] = array(
      '#type' => 'textfield',
      '#title' => t('Open Badge to process'),
      '#maxlength' => 255,
      '#autocomplete_path' => 'open_badges/autocomplete',
      '#default_value' => '',
      '#field_suffix' => '',
      );
  } else {
    $badgeinfo = open_badges_get_badge($bid);
    //dpm($badgeinfo, "The badge information for badge id ". $bid);

    $form['submittedbadgeid'] = array(
      '#type' => 'textfield',
      '#title' => t('Open Badge to process'),
      '#maxlength' => 255,
      '#autocomplete_path' => 'open_badges/autocomplete',
      '#default_value' => $badgeinfo->name . ' (Badge ID ' . $badgeinfo->bid . ')',
      '#field_suffix' => '',
      );

    $form['issuedate'] = array(
      '#type' => 'textfield',
      '#default_value' => 'yyyy-mm-dd',
      '#title' => t('Issue Date'),
      '#description' => t('Defaults to \'now\''),
      '#size' => 20,
      '#maxlength' => 30
    );
    $form['expirationdate'] = array(
      '#type' => 'textfield',
      '#default_value' => 'yyyy-mm-dd',
      '#title' => t('Expiration Date'),
      '#description' => t('Defaults to \'never\''),
      '#size' => 20,
      '#maxlength' => 30
    );
    $form['evidenceurl'] = array(
      '#type' => 'textfield',
      '#title' => t('Evidence URL'),
      '#size' => 30,
      '#maxlength' => 255
    );

    // Get a list of all current users and their badges
    $all_u = open_badges_get_users();
    //dpm($all_u, 'The list of all the users... ???');

    $remove_array = array();
    $add_array = array();

    foreach ($all_u as $u) {
      //dpm($u, 'Looking at user');
      if (isset($u['none'])) {
        // Those with no badges automatically go on the add list
        $add_array[$u['none']->uid] = $u['none']->name . '(uid ' . $u['none']->uid . ')';
      } else if (isset($u['user']) && isset($u['user'][$bid])) {
        // Don't include those that received this badge because of 'role' in the remove list
        $remove_array[$u['user'][$bid]->uid] = $u['user'][$bid]->name . '(uid ' . $u['user'][$bid]->uid . ')';
      } else {
        if (isset($u['user'])) {
          foreach($u['user'] as $uu) {
            $add_array[$uu->uid] = $uu->name . '(uid ' . $uu->uid . ')';
            break;
          }
        }
        if (isset($u['role'])) {
          foreach($u['role'] as $ur) {
            $add_array[$ur->uid] = $ur->name . '(uid ' . $ur->uid . ')';
            break;
          }
        }
      }
    }
    //dpm($add_array, 'The add array');
    //dpm($remove_array, 'The remove array');

    // Get a list of all the current recipients of this badge
    $recipients = array();
    $all_r = open_badges_get_badge_recipients($bid);
    //dpm($all_r, "All recipients returned from open_badges_get_badge_recipients");
    for($i = 0; $i < count($all_r); $i++) {
      $recipients[] = $all_r[$i]->name . ' (' . $all_r[$i]->type . ')';
    }
    $num_recipients = count($recipients);
    array_unshift($recipients, t('There are currently @count recipients of @badgename (Badge ID @badgeid): <br \>',
    array('@count' => $num_recipients, '@badgename' => $badgeinfo->name, '@badgeid' => $bid)));
    $form['currentrecipients'][$badge->bid] = array('#value' => implode('<br />', $recipients));

    $form['addrecipients'] = array(
      '#type' => 'select',
      '#title' => t('User(s) to be added'),
      '#options' => $add_array,
      '#multiple' => true,
      '#size' => 10,
    );
    $form['removerecipients'] = array(
      '#type' => 'select',
      '#title' => t('User(s) to be removed'),
      '#options' => $remove_array,
      '#multiple' => true,
      '#size' => 10,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function open_badges_recipients_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Validate badge value if present
  if (!empty($values['badgeid'])) {
    // The field isn't empty, so we should validate it.
    $validation = open_badges_badge_autocomplete_validation($values['badgeid']);
    //Is it correctly formatted?
    if ($validation[1] == 'string') {
      form_set_error('badgeid', t('"@value" is not a valid badge name. Try using the autocomplete function (requires javascript).', array('@value' => $values['badgeid'])));
    }
    // The format was correct, but we need to check if the bid exists.
    elseif ($validation[1] == 'nobid') {
      form_set_error('badgeid', t('@value is not a valid badge ID. Try using the autocomplete function (requires javascript).', array('@value' => $validation[0])));
    }
  }
  // Verify dates if something other than the default
  if (!empty($values['issuedate']) && $values['issuedate'] != 'yyyy-mm-dd') {
    if (!preg_match('/\d{4}\-\d{2}-\d{2}/', $values['issuedate'])) {
      form_set_error('issuedate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['issuedate'])));
    }
  }
  if (!empty($values['expirationdate']) && $values['expirationdate'] != 'yyyy-mm-dd') {
    if (!preg_match('/\d{4}\-\d{2}-\d{2}/', $values['expirationdate'])) {
      form_set_error('expirationdate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['expirationdate'])));
    }
  }
}

function open_badges_recipients_form_submit($form, &$form_state) {
  //dpm("open_badges_recipients_form_submit: entered");
  //dpm($form, "The form array");
  //dpm($form_state, "The form_state array");

  $values = $form_state['values'];

  if (preg_match('/\('. t('Badge ID') .' (\d+)\)/', $values['badgeid'], $matches)) {
    $bid = $matches[1];
    $form_state['redirect'] = 'admin/user/open_badges/recipients/' . $bid;
    return;
  } else if (preg_match('/\('. t('Badge ID') .' (\d+)\)/', $values['submittedbadgeid'], $matches)) {
    //dpm("We have a submission to change:" . $matches[0] . ":" . $matches[1] . ":");
    $bid = $matches[1];

    $extras = array('type' => 'user');

    // If these values were supplied in the form, use them when adding badges
    $issuedate = $values['issuedate'];
    if (!empty($issuedate) && $issuedate != 'yyyy-mm-dd') {
      $extras['issuedate'] = strtotime($issuedate);
    }

    $expirationdate = $values['expirationdate'];
    if (!empty($expirationdate) && $expirationdate != 'yyyy-mm-dd') {
      $extras['expirationdate'] = strtotime($expirationdate);
    }

    if (!empty($values['evidenceurl'])) {
      $extras['evidenceurl'] = $values['evidenceurl'];
    }

    dpm("Should be adding " . count($values['addrecipients']) . ' new users');
    // Add the badge to the selected user(s)
    foreach ($values['addrecipients'] as $uid) {
      $success = (boolean) open_badges_user_add_badge($uid, $bid, $extras);
      if ($success) {
        watchdog('action', 'Added open badge %badge to user %uid.', array('%badge' => $bid, '%uid' => $uid));
        drupal_set_message(t('Successfully added open badge %badge to user %uid', array('%badge' => $bid, '%uid' => $uid)));
      }
      else {
        watchdog('action', 'Unable to add open badge %badge to user %uid.', array('%badge' => $bid, '%uid' => $uid), WATCHDOG_WARNING);
        drupal_set_message(t('Encountered an error attempting to add open badge %badge to user %uid', array('%badge' => $bid, '%uid' => $uid)));
      }
    }

    dpm("Should be removing " . count($values['removerecipients']) . ' existing users');
    // Remove the badge from the selected user(s)
    foreach ($values['removerecipients'] as $uid) {
      $success = (boolean) open_badges_user_remove_badge($uid, $bid, 'user');
      if ($success) {
        watchdog('action', 'Removed open badge %badge from user %uid.', array('%badge' => $bid, '%uid' => $uid));
        drupal_set_message(t('Successfully removed open badge %badge from user %uid', array('%badge' => $bid, '%uid' => $uid)));
      }
      else {
        watchdog('action', 'Unable to remove open badge %badge from user %uid.', array('%badge' => $bid, '%uid' => $uid), WATCHDOG_WARNING);
        drupal_set_message(t('Encountered an error attempting to remove open badge from user %uid', array('%uid' => $uid)));
      }
    }

    $form_state['redirect'] = 'admin/user/open_badges/recipients/' . $bid;
  } else {
    // Should never get here, but handle it anyway
    drupal_set_message(t('Could not determine the Badge ID to be processed.'));
    $form_state['redirect'] = 'admin/user/open_badges/recipients';
    return;
  }
}


/**
 * form to upload the badge images or to delete existing ones
 */
function open_badges_images_form($form_state) {
  $form = array('#skip_duplicate_check' => TRUE);
  if (module_exists('upload')) {
    $form['new']['upload'] = array('#type' => 'file', '#title' => t('Upload image'), '#description' => t('(Must be PNG; Max size of 256KB)'), '#size' => 40);
    $form['new']['attach'] = array('#type' => 'submit', '#value' => t('Upload'));
  }
  else {
    drupal_set_message(t('Upload of images requires the upload module to be enabled.'), 'error');
  }

  $form['#attributes']['enctype'] = 'multipart/form-data';

  $selects = open_badges_image_selects();
  if (count($selects)) {
    $form['images'] = array('#tree' => TRUE);
    foreach ($selects as $imagepath => $imageimg) {
      $form['images'][$imagepath] = array(
        '#type' => 'checkbox',
        '#title' => $imageimg,
        '#return_value' => 1,
        '#default_value' => FALSE,
        '#description' => check_plain($imagepath),
      );
    }
    $form['delete_image'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  return $form;
}

/**
 * Validate the uploaded image
 *
 * Check whether:
 * Delete has been chosen AND a checkbox has been selected
 * OR
 * Upload has been chosen AND the file upload form is not empty.
 */
function open_badges_images_form_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  if ($op == t('Upload')) {
    $dir = file_create_path('open_badges');
    $is_writable = file_check_directory($dir, 1);
    if ($is_writable) {
      $validators = array(
        'file_validate_extensions' => array('png'),
        'file_validate_size' => array(262144),
      );
      if ($file = file_save_upload('upload', $validators, $dir)) {
        if (!image_get_info($file->filepath)) {
          file_delete($file->filepath);
          form_set_error('upload', t('Uploaded image file does not appear to be a valid PNG image file, or is larger than 256kb.  Please try again'));
        }
        else {
          open_badges_hold_temporary_file($file);
          $form_state['values']['file_image'] = $file;
        }
      }
      else {
        form_set_error('upload', t('Cannot save image.  Enter the path to an image and try again.'));
      }
    }
    else {
      form_set_error('upload', t('Cannot save image - directory not writable'));
    }
  }
  elseif ($op == t('Delete')) {
    if (count(array_filter($form_state['values']['images'])) == 0) {
      form_set_error('images', t('Please select images to delete.'));
    }
  }
}

function open_badges_hold_temporary_file($file = NULL) {
  static $static_file;
  if (isset($file)) {
    $static_file = $file;
  }
  return $file;
}

/**
 * Submission action for open_badges_images_form
 *
 * Save the uploaded file or delete the selected one
 */
function open_badges_images_form_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  // Save uploaded files.
  if ($op == t('Upload')) {
    $file = $form_state['values']['file_image'];
    //dpm($file, "open_badges_images_form_submit: The uploaded file");
    file_set_status($file, FILE_STATUS_PERMANENT);
  }
  elseif ($op == t('Delete')) {
    foreach ($form_state['values']['images'] as $path => $is_removed) {
      if ($is_removed) {
        $to_delete[] = $path;
      }
    }
    if (is_array($to_delete)) {
      open_badges_image_delete($to_delete);
    }
  }
}

/**
 * Delete the specified image
 */
function open_badges_image_delete($to_delete) {
  foreach ($to_delete as $path) {
    if (file_check_location($path, file_create_path('open_badges'))) {
      file_delete($path);
    }
  }
}

/**
 * form to associated badges with roles
 */
function open_badges_roles_form() {
  // No badges for the anonymous role.
  $roles = user_roles(TRUE);
  $badges = open_badges_get_roles(NULL, array('returnbadges' => TRUE));

  $form['blocked'] = array(
    '#type' => 'fieldset',
    '#title' => t('Blocked Badge'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $form['blocked'][0]['badge'] = array(
    '#type' => 'textfield',
    '#title' => t('blocked user'),
    '#size' => 40,
    '#maxlength' => 255,
    '#autocomplete_path' => 'open_badges/autocomplete',
    '#default_value' => isset($badges[0]) ? $badges[0]->name . ' ' . t('(Badge ID') . ' ' . $badges[0]->bid .')' : '',
    '#field_suffix' => isset($badges[0]) ? ' ' . t('Current:') . ' ' . $badges[0]->image : '',
  );
  $form['blocked'][0]['issuedate'] = array(
    '#type' => 'textfield',
    '#default_value' => 'yyyy-mm-dd',
    '#title' => t('Issue Date'),
    '#description' => t('Defaults to \'now\''),
    '#size' => 20,
    '#maxlength' => 30
  );
  $form['blocked'][0]['expirationdate'] = array(
    '#type' => 'textfield',
    '#default_value' => 'yyyy-mm-dd',
    '#title' => t('Expiration Date'),
    '#description' => t('Defaults to \'never\''),
    '#size' => 20,
    '#maxlength' => 30
  );
  $form['blocked'][0]['evidenceurl'] = array(
    '#type' => 'textfield',
    '#title' => t('Evidence URL'),
    '#size' => 30,
    '#maxlength' => 255
  );

  $form['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Role Badges'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $rows = array();
  foreach ($roles as $rid => $role) {
    $form['roles'][$rid] = array();

    $form['roles'][$rid]['badge'] = array(
      '#type' => 'textfield',
      '#title' => $role,
      '#maxlength' => 255,
      '#autocomplete_path' => 'open_badges/autocomplete',
      '#default_value' => isset($badges[$rid]) ? $badges[$rid]->name . ' '
        . t('(Badge ID') . ' ' . $badges[$rid]->bid .')' : '',
      '#field_suffix' => isset($badges[$rid]) ?
        ' ' . t('Current:') . ' ' . $badges[$rid]->image : '',
      );
    $form['roles'][$rid]['issuedate'] = array(
      '#type' => 'textfield',
      '#default_value' => 'yyyy-mm-dd',
      '#title' => t('Issue Date'),
      '#description' => t('Defaults to \'now\''),
      '#size' => 20,
      '#maxlength' => 30
    );
    $form['roles'][$rid]['expirationdate'] = array(
      '#type' => 'textfield',
      '#default_value' => 'yyyy-mm-dd',
      '#title' => t('Expiration Date'),
      '#description' => t('Defaults to \'never\''),
      '#size' => 20,
      '#maxlength' => 30
    );
    $form['roles'][$rid]['evidenceurl'] = array(
      '#type' => 'textfield',
      '#title' => t('Evidence URL'),
      '#size' => 30,
      '#maxlength' => 255
    );
  }

  $form[] = array(
    '#type' => 'submit',
    '#value' => t('Save Role Badges'),
  );
  return $form;
}

/**
 * validation function for open_badges_roles_form
 */
function open_badges_roles_form_validate($form, &$form_state) {
  //dpm($form_state, "roles_form_validate: form_state");
  $array = $form_state['values']['roles'] + $form_state['values']['blocked'];
  // Go through all the entries and make sure they all have a valid badge ID.
  foreach ($array as $field => $values) {
    //dpm($field, "roles_form_validate: The field");
    //dpm($values, "roles_form_validate: The values");
    if (!empty($values['badge'])) {
      // The field isn't empty, so we should validate it.
      $validation = open_badges_badge_autocomplete_validation($values['badge']);
      //Is it correctly formatted?
      if ($validation[1] == 'string') {
        if ($field == 0) {
          form_set_error('blocked]['. $field. '][badge', t('"@value" is not a valid badge name. Try using the autocomplete function (requires javascript).', array('@value' => $values['badge'])));
        }
        else {
          form_set_error('roles]['. $field. '][badge', t('"@value" is not a valid badge name. Try using the autocomplete function (requires javascript).', array('@value' => $values['badge'])));
        }
      }
      // The format was correct, but we need to check if the bid exists.
      elseif ($validation[1] == 'nobid') {
        if ($field == 0) {
          form_set_error('blocked]['. $field. '][badge', t('@value is not a valid badge ID. Try using the autocomplete function (requires javascript).', array('@value' => $validation[0])));
        }
        else {
          form_set_error('roles]['. $field. '][badge', t('@value is not a valid badge ID. Try using the autocomplete function (requires javascript).', array('@value' => $validation[0])));
        }
      }
      // Verify dates if something other than the default
      if (!empty($values['issuedate']) && $values['issuedate'] != 'yyyy-mm-dd') {
        if (!preg_match('/\d{4}\-\d{2}-\d{2}/', $values['issuedate'])) {
          if ($field == 0) {
            form_set_error('blocked]['. $field . '][issuedate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['issuedate'])));
          } else {
            form_set_error('roles][' . $field . '][issuedate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['issuedate'])));
          }
        }
      }
      if (!empty($values['expirationdate']) && $values['expirationdate'] != 'yyyy-mm-dd') {
        if (!preg_match('/\d{4}\-\d{2}-\d{2}/', $values['expirationdate'])) {
          if ($field == 0) {
            form_set_error('blocked]['. $field . '][expirationdate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['expirationdate'])));
          } else {
            form_set_error('roles][' . $field . '][expirationdate', t('@value is not in the required \'yyyy-mm-dd\' format.', array('@value' => $values['expirationdate'])));
          }
                  }
      }
    }
  }
}

/**
 * submission function for open_badges_roles_form
 */
function open_badges_roles_form_submit($form, &$form_state) {
  $array = $form_state['values']['roles'] + $form_state['values']['blocked'];
  //dpm($array, "The array formed after combining 'roles' and 'blocked'");
  foreach ($array as $field => &$values) {
    // We now have a string as our badge, so just extract the bid.
    preg_match('/\('. t('Badge ID') .' (\d+)\)/', $values['badge'], $matches);
    // Transform our value into just the bid.
    $values['badge'] = $matches[1];
  }
  open_badges_save_roles($array);
}

/**
 * Form for general module settings.
 */
function open_badges_settings_form() {
  $noyes = array(t('No'), t('Yes'));


  $form['defaultissuerorigin'] = array(
    '#type' => 'textfield',
    '#title' => t('The issuer origin URL'),
    '#size' => 75,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('open_badges_defaultissuerorigin', ''),
    '#description' => t('Origin of the issuer. This is the &lt;protocol&gt;://&lt;host&gt;:&lt;port&gt;. Must match the origin of the hosted assertion (and in the future, the origin of the public key).'),
    );

  $form['defaultissuername'] = array(
    '#type' => 'textfield',
    '#title' => t('Default issuing organization name'),
    '#size' => 75,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('open_badges_defaultissuername', ''),
    '#description' => t('The organization name to use a default if no organization is specified when creating a new badge.'),
    );

  $form['badgebakingurl'] = array(
    '#type' => 'textfield',
    '#title' => t('URL for "baking" a badge'),
    '#size' => 75,
    '#maxlength' => 255,
    '#default_value' => variable_get('open_badges_badgebakingurl', ''),
    '#description' => t('The URL for the badge baking process.  See XXX for more information on "baking" a badge.'),
    );

  $form['badgebackpackurl'] = array(
    '#type' => 'textfield',
    '#title' => t('URL for sending a badge to the OBI backpack'),
    '#size' => 75,
    '#maxlength' => 255,
    '#default_value' => variable_get('open_badges_badgebakingurl', ''),
    '#description' => t('The URL for sharing the badge with the user\'s OBI backpack.  See XXX for more information on OBI backpacks.'),
    );

    $form['showone'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of badges to display'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => variable_get('open_badges_showone', 0),
    '#description' => t('Only this many badges with the lightest weights will be shown.') .'<br/>'.
      t('Set to zero to apply no limit.') .'<br/>'.
      t('Note that if multiple badges have the same lightest weight, only one of them will appear (first by alphabetical order).'),
    );

  $form['showblocked'] = array(
    '#type' => 'radios',
    '#options' => $noyes,
    '#title' => t('Only show blocked open badge'),
    '#default_value' => variable_get('open_badges_showblocked', 0),
    '#description' => t('If checked, only the badge associated to blocked users will be shown, overriding other badges the user eventually has as well as any other settings.') .' <br />'.
      t('Note that if there is no badge associated to blocked users, no badges will appear.') .' <br />'.
      t('This option only acts on blocked users and has no repercussions on active open badges.'),
    '#attributes' => array('class' => 'container-inline'),
    );

  $form['userweight'] = array(
    '#type' => 'radios',
    '#options' => $noyes,
    '#title' => t('Allow users to reorder badges'),
    '#default_value' => variable_get('open_badges_userweight', 0),
    '#description' => t('If checked, users will have the ability to reweight their badges in their profile, enabling them to set what order their badges display, and also which badges will display if a limit is set above.') .' <br />'.
      t('Note that you can make individual badges exempt from this function, allowing you to force them to the top or bottom of the list by giving them a very high or low weight value.') .' <br />',
    '#attributes' => array('class' => 'container-inline'),
    );

  if (module_exists('imagecache')) {
    $form['imagecache'] = array(
      '#type' => 'radios',
      '#options' => $noyes,
      '#title' => t('Allow ImageCache to size badges'),
      '#default_value' => variable_get('open_badges_imagecache', 0),
      '#description' => t('The ImageCache module is available, if you choose this option, it will be used to format the badge image. The preset name is "open-badges".'),
      '#attributes' => array('class' => 'container-inline'),
      );
  }
  else {
    $form['imagecache'] = array('#type' => 'value', '#value' => 0);
  }

  $form['nobadges'] = array(
    '#type' => 'textfield',
    '#title' => t('"No badges" message'),
    '#size' => 60,
    '#maxlength' => 500,
    '#default_value' => variable_get('open_badges_nobadges', ''),
    '#description' => t('This provides this message if the user has no badges, rather than nothing. Leave it blank for no message.'),
    );

  $form['notifications'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification Emails'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $form['notifications']['from'] = array(
    '#type' => 'textfield',
    '#title' => t('From address'),
    '#size' => 40,
    '#maxlength' => 255,
    '#default_value' => variable_get('open_badges_fromaddress', ''),
  );
  $form['notifications']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#size' => 40,
    '#maxlength' => 255,
    '#default_value' => variable_get('open_badges_subject', ''),
  );
  $form['notifications']['bodyshared'] = array(
    '#type' => 'textarea',
    '#title' => t('Body for message when already shared with OBI'),
    '#default_value' => variable_get('open_badges_bodyshared', ''),
  );
  $form['notifications']['bodynotshared'] = array(
    '#type' => 'textarea',
    '#title' => t('Body for message when badge has not yet been shared with OBI'),
    '#default_value' => variable_get('open_badges_bodynotshared', ''),
  );

  // Tokens help.
  if (module_exists('token')) {
    $form['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      );
    $form['token_help']['help'] = array(
      '#value' => theme('token_help', array('userbadge', 'user')),
      );
  }
  else {
    $form['token_help']['help'] = array(
      // Ignore Coder flag on link.
      '#value' => t('Install the !link module if you want this URL to include dynamic elements such as badge ID numbers.', array('!link' => l('Token', 'http://drupal.org/project/token', array('absolute' => TRUE)))),
      );
  }

  // Let the user select a vocabulary to associate with badges.
  if (module_exists('taxonomy')) {
    //Build the options for the element
    $vocabs = taxonomy_get_vocabularies();
    $selects = array('' => t('&lt;none&gt;'));
    foreach ($vocabs as $vid => $vocab) {
      $selects[$vid] = $vocab->name;
    }

    $form['vid'] = array(
      '#type' => 'radios',
      '#title' => t('Vocabulary'),
      '#default_value' => variable_get('open_badges_vid', ''),
      '#options' => $selects,
      '#description' => t('Optional. Select a vocabulary to associate badges with. You can optionally associate each badge with one term in this vocabulary.'),
      '#attributes' => array('class' => 'container-inline'),
      );
  }

  $form[] = array('#type' => 'submit', '#value' => t('Save configuration'));
  return $form;
}

/**
 * validate open_badges_settings_form input
 */
function open_badges_settings_form_validate($form, $form_state) {
  // 'showone' must be numeric.
  if (!is_numeric($form_state['values']['showone']) || $form_state['values']['showone'] < 0 ) {
    form_set_error('showone', t('Must be a positive number.'));
  }
}

/**
 * submission function for open_badges_settings_form
 *
 * Set the variables and display a message about the set values.
 */
function open_badges_settings_form_submit($form, $form_state) {
  variable_set('open_badges_showone', $form_state['values']['showone']);
  variable_set('open_badges_showblocked', $form_state['values']['showblocked']);
  variable_set('open_badges_userweight', $form_state['values']['userweight']);
  variable_set('open_badges_imagecache', $form_state['values']['imagecache']);
  variable_set('open_badges_nobadges', $form_state['values']['nobadges']);

  $defaultissuerorigin = trim($form_state['values']['defaultissuerorigin']);
  if (empty($defaultissuerorigin)) {
    variable_del('open_badges_defaultissuerorigin');
  }
  else {
    variable_set('open_badges_defaultissuerorigin', $defaultissuerorigin);
  }

  // Only set a default issuername if we have some non-whitespace in there.
  $defaultissuername = trim($form_state['values']['defaultissuername']);
  if (empty($defaultissuername)) {
    variable_del('open_badges_defaultissuername');
  }
  else {
    variable_set('open_badges_defaultissuername', $defaultissuername);
  }

  // Only set a vid if we have a value, otherwise wipe it for none/no taxonomy.
  if ($form_state['values']['vid']) {
    variable_set('open_badges_vid', $form_state['values']['vid']);
  }
  else {
    variable_del('open_badges_vid');
  }

  $badgebakingurl = trim($form_state['values']['badgebakingurl']);
  if (empty($badgebakingurl)) {
    variable_del('open_badges_badgebakingurl');
  } else {
    variable_set('open_badges_badgebakingurl', $badgebakingurl);
  }

  $badgebackpackurl = trim($form_state['values']['badgebackpackurl']);
  if (empty($badgebackpackurl)) {
    variable_del('open_badges_badgebackpackurl');
  } else {
    variable_set('open_badges_badgebackpackurl', $badgebackpackurl);
  }

  $fromaddress = trim($form_state['values']['notifications']['from']);
  if (empty($fromaddress)) {
    variable_del('open_badges_fromaddress');
  } else {
    variable_set('open_badges_fromaddress', $fromaddress);
  }

  $subject = trim($form_state['values']['notifications']['subject']);
  if (empty($subject)) {
    variable_del('open_badges_subject');
  } else {
    variable_set('open_badges_subject', $subject);
  }

  $bodyshared = trim($form_state['values']['notifications']['bodyshared']);
  if (empty($bodyshared)) {
    variable_del('open_badges_bodyshared');
  } else {
    variable_set('open_badges_bodyshared', $bodyshared);
  }

  $bodynotshared = trim($form_state['values']['notifications']['bodynotshared']);
  if (empty($bodynotshared)) {
    variable_del('open_badges_bodynotshared');
  } else {
    variable_set('open_badges_bodynotshared', $bodynotshared);
  }

  drupal_set_message(t('Configuration saved.'));
}

/**
 * Select options for printing a list of images in the image library
 *
 * @return
 *   An array of images from the image library, in the form $filename => $image_html
 */
function open_badges_image_selects() {
  $selects = array();
  $dir = file_create_path('open_badges');
  // Limit this to only PNG, which is the only thing accepted by Mozilla OBI
  $files = file_scan_directory($dir, '.*\.((p|P)(n|N)(g|G))', array('.', '..', 'CVS'), 0, FALSE);
  foreach ($files as $file) {
    $selects[$file->filename] = theme('image', $file->filename, $file->filename, $file->filename);
  }

  return $selects;
}

