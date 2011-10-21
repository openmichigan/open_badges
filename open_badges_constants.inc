<?php

/**
 * Constant values for use since ENUMs are not
 * supported by the schema module...
 */

define('OPEN_BADGES_STATE_UNDEFINED', 0);                   // ?
define('OPEN_BADGES_STATE_EARNED', 1);                      // A
define('OPEN_BADGES_STATE_BAKED', 2);                       // B
define('OPEN_BADGES_STATE_BAKED_AND_SHARED', 3);            // C
define('OPEN_BADGES_STATE_NO_DISPLAY', 4);                  // D
define('OPEN_BADGES_STATE_BAKED_AND_REVOKED', 5);           // E

define('OPEN_BADGES_USER_DECISION_NONE', 0);
define('OPEN_BADGES_USER_DECISION_NO', 1);
define('OPEN_BADGES_USER_DECISION_YES', 2);

?>
