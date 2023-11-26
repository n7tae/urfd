<?php

$ReflectorStatus = (file_exists($Service['PIDFile'])) ? 'up' : 'down';

$payload = array(
    'lastupdate'             => date('U', time()),
    'lasturfdupdate'         => date('U', filemtime($Service['XMLFile'])),
    'reflectorstatus'        => $ReflectorStatus,
    'reflectoruptimeseconds' => $Reflector->GetServiceUptime()
);


// json encode payload array
$records = json_encode($payload);

echo $records;

?>
