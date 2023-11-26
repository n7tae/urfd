<?php

/*** Add links to payload ***/
for ($i=0;$i<$Reflector->PeerCount();$i++) {

    $payload[$i] = array(
        'callsign'      => $Reflector->Peers[$i]->GetCallSign(),
        'ip'            => $Reflector->Peers[$i]->GetIP(),
        'linkedmodule'  => $Reflector->Peers[$i]->GetLinkedModule(),
        'connecttime'   => date('Y-m-d\TH:i:sp', $Reflector->Peers[$i]->GetConnectTime()),
        'lastheardtime' => date('Y-m-d\TH:i:sp', $Reflector->Peers[$i]->GetLastHeardTime())
    );

}


// json encode payload array
$records = json_encode($payload);

echo $records;

?>