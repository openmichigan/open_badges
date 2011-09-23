<?php
/**
 * @file
 * A handler to provide proper displays for individual user badges.
 *
 * @author Richard Skinner (Likeless), http://drupal.org/user/310635
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_open_badges_badges_badge extends views_handler_field {
  
  //Get the additional fields we need
  function construct() {
    parent::construct();
    $this->additional_fields['name'] = 'name';
    $this->additional_fields['image'] = 'image';
    $this->additional_fields['criteria'] = 'criteria';
    $this->additional_fields['weight'] = 'weight';
  }

  //Add the additional fields to the query
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  //Render the badge HTML
  function render($values) {
    dpm("views_handler_field_open_badges_badges_badge: render(): entered");
    //create the badge object
    $badge->bid = $values->{$this->field_alias};
    $badge->name = $values->{$this->aliases['name']};
    $badge->image = $values->{$this->aliases['image']};
    $badge->criteria = $values->{$this->aliases['criteria']};
    $badge->weight = $values->{$this->aliases['weight']};
    
    //Send it through the standard theme and return it
    return theme('open_badge', $badge);
  }
  
  
}
