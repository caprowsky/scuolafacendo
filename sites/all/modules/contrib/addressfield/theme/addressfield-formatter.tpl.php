<?php

/**
 * @file
 * TODO: Describe what this template is here for.
 */
?>
<div class="xnal address-default">
  <div class="street-address">
  <?php
    print implode('<br />', array_filter(array(
      !empty($address['name_line']) ? $address['name_line'] : $address['first_name'] . ' ' . $address['last_name'],
      $address['organisation_name'],
      $address['thoroughfare'],
      $address['premise'],
      $address['sub_premise'],
      $address['dependent_locality'],
    )));
    ?>
  </div>
  <div>
    <span class="postal-code"><?php print $address['postal_code'] ?></span> <span class="locality"><?php print $address['locality'] ?></span>
  </div>
  <div>
    <span class="administrative_area"><?php print $address['administrative_area'] ?></span>
  </div>
  <div>
    <span class="country"><?php print $address['country_name'] ?></span>
  </div>
</div>
