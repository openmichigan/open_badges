<?php
/**
 * @file
 * A handler to provide filtering of views by badge state
 *
 * @author
 * Kevin Coffman
 *
 * @ingroup views_field_handlers
 */
class views_handler_filter_open_badges_badge_state extends views_handler_filter_in_operator {
  
  function get_value_options() {
    $this->value_options = array(
      '1' => t('The badge has been earned'),
      '2' => t('The badge has been baked'),
      '3' => t('The badge has been baked and shared with OBI'),
      '4' => t('This state is currently unused'),
      '5' => t('The badge has been revoked'),
    );
  }
}
