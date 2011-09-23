<?php
/**
 * @file
 * A handler to provide filtering of views by badge type
 *
 * @author Richard Skinner (Likeless), http://drupal.org/user/310635
 *
 * @ingroup views_field_handlers
 */
class views_handler_filter_open_badges_user_type extends views_handler_filter_in_operator {
  
  function get_value_options() {
    dpm("views_handler_filter_open_badges_user_type: get_value_options(): entered");
    $this->value_options = array(
      'user' => t('Open Badge'),
      'role' => t('Granted by Role')
    );
  }
}
