<?php

$Url = @parse_url($CallingHome['MyDashBoardURL']);
$Net4 = @dns_get_record($Url['host'], DNS_A);
$Net6 = @dns_get_record($Url['host'], DNS_AAAA);

/*** add metadata to payload ***/
$payload = array(
    'dashboard_version'  => $PageOptions['DashboardVersion'],
    'ipV4'               => @$Net4[0]['ip'],
    'ipV6'               => @$Net6[0]['ipv6'],
    'reflector_callsign' => str_replace("XLX", "URF", $Reflector->GetReflectorName()),
    'reflector_version'  => $Reflector->GetVersion(),
    'sysop_email'        => $PageOptions['ContactEmail']
);


// json encode payload array
$records = json_encode($payload);

echo $records;

?>