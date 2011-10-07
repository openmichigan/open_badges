<?php

/**
 * @file
 * This defines views hooks for the Open_badges module. It will be loaded automatically as needed by the Views module.
 *
 * @author Heine Deelstra (Heine), http://drupal.org/user/17943
 * @author Richard Skinner (Likeless), http://drupal.org/user/310635
 *
 */

/**
 * Implements hook_views_handlers().
 */
function open_badges_views_handlers() {
  //dpm("open_badges_views_handlers: entered");
  return array(
    'handlers' => array(
      'views_handler_field_open_badges_badges_badge' => array(
        'parent' => 'views_handler_field',
      ),
      'views_handler_field_open_badges_user_uid' => array(
        'parent' => 'views_handler_field',
      ),
      'views_handler_filter_open_badges_user_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/**
 * Implements hook_views_data().
 */
function open_badges_views_data() {
  //dpm("open_badges_views_data: entered");
  // Describe the open_badges_user table.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['open_badges_user']['table']['group']  = t('Open Badges');

  // How to join this table to the users table
  $data['open_badges_user']['table']['join']['users'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );

  // How to join this table to the node table
  $data['open_badges_user']['table']['join']['node'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );

  // How to join to node_revisions; through the node table for consistent authorship.
  $data['open_badges_user']['table']['join']['node_revisions'] = array(
    'left_table' => 'node',
    'left_field' => 'uid',
    'field' => 'uid',
  );

  // Describe the open_badges_badges table.
  $data['open_badges_badges']['table']['group']  = t('Open Badges');

  //We always join the open_badges_badges table on to the open_badges_user table
  $open_badges_badges_join_info = array(
    'left_table' => 'open_badges_user',
    'left_field' => 'bid',
    'field' => 'bid',
  );

  $data['open_badges_badges']['table']['join']['users'] = $open_badges_badges_join_info;
  $data['open_badges_badges']['table']['join']['node'] = $open_badges_badges_join_info;
  $data['open_badges_badges']['table']['join']['node_revisions'] = $open_badges_badges_join_info;


  // Information for processing badge IDs
  $data['open_badges_user']['bid'] = array(
    'title' => t('Bid'),
    'help' => t('The badge ID of the open badge itself.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a bid as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'Badge ID',
      'numeric' => TRUE,
    ),
    //Info for filtering by bid
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a bid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Information for processing badge types
  $data['open_badges_user']['type'] = array(
    'title' => t('Type'),
    'help' => t('Whether the badge was set as part of the role, or individually assigned.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    //Info for filtering by badge type
    'filter' => array(
      'handler' => 'views_handler_filter_open_badges_user_type',
    ),
    // Information for sorting on a bid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Information for processing badges by name
  $data['open_badges_badges']['name'] = array(
    'title' => t('Badge Name'),
    'help' => t('The name of the open badge.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
      'name field' => 'Badge Name',
    ),
    //Info for filtering by badge name
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    // Information for sorting on a bid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Information for processing badges weights
  $data['open_badges_badges']['weight'] = array(
    'title' => t('Badge Weight'),
    'help' => t('The weight of the badge, as defined on the open badges settings page.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['open_badges_badges']['badge'] = array(
    'title' => t('Open Badge'),
    'real field' => 'bid',
    'help' => t('An individual open badge.'),
    'field' => array(
      'handler' => 'views_handler_field_open_badges_badges_badge',
    ),
  );

  // Add the grouped open badges to the user table
  $data['users']['open_badges_html'] = array(
    'real field' => 'uid',
    'title' => t('Grouped Open Badges'),
    'help' => t("All the user's open badges."),
    'group' => t('Open Badges'),
    'field' => array(
      'handler' => 'views_handler_field_open_badges_user_uid',
    ),
  );

  //dpm("open_badges_views_data: returning data...");
  //dpr($data, FALSE, "Data from open_badges_views_data");
  return $data;
}
