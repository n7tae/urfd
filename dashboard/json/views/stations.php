<?php

/*** Add stations to payload ***/
for ($i=0;$i<$Reflector->StationCount();$i++) {

    $tmp = preg_split('/\s+/', $Reflector->Stations[$i]->GetCallSign(), -1, PREG_SPLIT_NO_EMPTY);
    $Callsign = $tmp[0];

    $tmp = preg_split('/\s+/', $Reflector->Stations[$i]->GetVia(), -1, PREG_SPLIT_NO_EMPTY);
    $CallsignSuffix = $tmp[1];
	
    // craft payload array
    $payload['stations'][$i] = array(
        'callsign'       => $Callsign,
        'callsignsuffix' => $CallsignSuffix,
        'vianode'        => $Reflector->Stations[$i]->GetVia(),
        'onmodule'       => $Reflector->Stations[$i]->GetModule(),
        'lastheard'      => date('Y-m-d\TH:i:sp', $Reflector->Stations[$i]->GetLastHeardTime())
    );

    list ($CountryCode, $Country) = $Reflector->GetFlag($Reflector->Stations[$i]->GetCallSign());

    $payload['stations'][$i]['country'] = array (
        'country'     => $Country,
        'countrycode' => $CountryCode
    );

}


// json encode payload array
$records = json_encode($payload);

echo $records;

?>