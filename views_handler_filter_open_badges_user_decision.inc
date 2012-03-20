<?php
/**
 * @file
 * A handler to provide filtering of views by user decision
 *
 * @author
 * Kevin Coffman
 *
 * @ingroup views_field_handlers
 */
class views_handler_filter_open_badges_user_decision extends views_handler_filter_in_operator {
  
  function get_value_options() {
    $this->value_options = array(
      '0' => t('No choice has been made'),
      '1' => t('NO'),
      '2' => t('YES'),
    );
  }
}
