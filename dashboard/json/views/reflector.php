<?php

$Status = (file_exists($Service['PIDFile'])) ? 'up' : 'down';

$payload = array(
    'lastupdatechecktime' => date('Y-m-d\TH:i:sp', time()),
    'status'              => $Status,
    'uptime'              => $Reflector->GetServiceUptime()
);


/*** add data to payload ***/
$payload['data'] = array(
    'filetime' => date('Y-m-d\TH:i:sp', filemtime($Service['XMLFile'])),
    'callsign' => str_replace("XLX", "URF", $Reflector->GetReflectorName()),
    'version'  => $Reflector->GetVersion()
);


/*** Add peers to payload ***/
for ($i=0;$i<$Reflector->PeerCount();$i++) {

    $payload['data']['peers'][] = array(
        'callsign'      => $Reflector->Peers[$i]->GetCallSign(),
        'ip'            => $Reflector->Peers[$i]->GetIP(),
        'linkedmodule'  => $Reflector->Peers[$i]->GetLinkedModule(),
        'connecttime'   => date('Y-m-d\TH:i:sp', $Reflector->Peers[$i]->GetConnectTime()),
        'lastheardtime' => date('Y-m-d\TH:i:sp', $Reflector->Peers[$i]->GetLastHeardTime())
    );

}


/*** Add nodes to payload ***/
for ($i=0;$i<$Reflector->NodeCount();$i++) {

    // craft payload array
    $payload['data']['nodes'][] = array(
        'callsign'      => $Reflector->Nodes[$i]->GetCallSign() . ' ' . $Reflector->Nodes[$i]->GetSuffix(),
        'ip'            => $Reflector->Nodes[$i]->GetIP(),
        'linkedmodule'  => $Reflector->Nodes[$i]->GetLinkedModule(),
        'protocol'      => $Reflector->Nodes[$i]->GetProtocol(),
        'connecttime'   => date('Y-m-d\TH:i:sp', $Reflector->Nodes[$i]->GetConnectTime()),
        'lastheardtime' => date('Y-m-d\TH:i:sp', $Reflector->Nodes[$i]->GetLastHeardTime())
    );

}


/*** Add stations to payload ***/
for ($i=0;$i<$Reflector->StationCount();$i++) {
	
    // craft payload array
    $payload['data']['stations'][] = array(
        'callsign'      => $Reflector->Stations[$i]->GetCallSign(),
        'vianode'       => $Reflector->Stations[$i]->GetVia(),
        'onmodule'      => $Reflector->Stations[$i]->GetModule(),
        'viapeer'       => $Reflector->Stations[$i]->GetPeer(),
        'lastheardtime' => date('Y-m-d\TH:i:sp', $Reflector->Stations[$i]->GetLastHeardTime())
    );

}


// json encode payload array
$records = json_encode($payload);

echo $records;

?>