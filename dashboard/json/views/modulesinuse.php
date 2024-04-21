<?php

$Modules = $Reflector->GetModules();
sort($Modules, SORT_STRING);


/*** Add modules to payload ***/
for ($i=0;$i<count($Modules);$i++) {

    $payload[$i] = array(
        'name' => $Modules[$i]
    );

    $Users = $Reflector->GetNodesInModulesByID($Modules[$i]);

    for ($j=0;$j<count($Users);$j++) {

        $payload[$i]['callsigns'][$j] = $Reflector->GetCallsignAndSuffixByID($Users[$j]);

    }

}


// json encode payload array
$records = json_encode($payload);

echo $records;

?>