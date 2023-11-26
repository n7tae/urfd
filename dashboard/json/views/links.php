<?php

/*** Add links to payload ***/
for ($i=0;$i<$Reflector->NodeCount();$i++) {

    // craft payload array
    $payload[$i] = array(
        'callsign'      => $Reflector->Nodes[$i]->GetCallSign() . ' ' . $Reflector->Nodes[$i]->GetSuffix(),
        'ip'            => $Reflector->Nodes[$i]->GetIP(),
        'linkedmodule'  => $Reflector->Nodes[$i]->GetLinkedModule(),
        'protocol'      => $Reflector->Nodes[$i]->GetProtocol(),
        'connecttime'   => date('Y-m-d\TH:i:sp', $Reflector->Nodes[$i]->GetConnectTime()),
        'lastheardtime' => date('Y-m-d\TH:i:sp', $Reflector->Nodes[$i]->GetLastHeardTime())
    );

}


// json encode payload array
$records = json_encode($payload);

echo $records;

?>