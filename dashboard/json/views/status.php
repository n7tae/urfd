<?php

$ReflectorStatus = (file_exists($Service['PIDFile'])) ? 'up' : 'down';

$payload = array(
    'lastupdate'             => gmdate('U', time()),
    'lasturfdupdate'         => gmdate('U', filemtime($Service['XMLFile'])),
    'reflectorstatus'        => $ReflectorStatus,
    'reflectoruptimeseconds' => $Reflector->GetServiceUptime()
);


// json encode payload array
$records = json_encode($payload);

echo $records;

?>
