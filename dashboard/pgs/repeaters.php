<table class="table table-striped table-hover">
	<tr class="table-center">
		<th class="col-md-1">#</th>
		<th class="col-md-1">Flag</th>
		<th class="col-md-2">DV Station</th>
		<th class="col-md-2">Last Heard</th>
		<th class="col-md-2">Linked for</th>
		<th class="col-md-1">Protocol</th>
<?php

if ($PageOptions['RepeatersPage']['IPModus'] != 'HideIP') {
	echo '
	<th class="col-md-1">IP</th>';
}

?>
		</tr>
<?php
function startsWith($haystack, $needle) {
    return $needle === "" || strrpos($haystack, $needle, -strlen($haystack)) !== false;
}

function searchForKey($field, $needle, $array) {
	foreach ($array as $key => $val) {
		if ($val[$field] === $needle) {
			return $key;
		}
	}
	return null;
}

function getLinkedGateways($logLines) {

	$gateways = Array();
	for ($i = count($logLines); $i>0; $i--) {
		if(isset($logLines[$i])){
			$logLine = $logLines[$i];

			if (strpos($logLine, "Starting P25Reflector")) {
				return $gateways;
			}
			if (strpos($logLine, "No repeaters/gateways linked")) {
				return $gateways;
			}
			if (strpos($logLine, "Currently linked repeaters")) {
				for ($j = $i+1; $j <= count($logLines); $j++) {
					if(isset($logLines[$j])){
						$logLine = $logLines[$j];
						if (!startsWith(substr($logLine,27), "   ")) {
							return $gateways;
						} else {
							//$Reflector->SetTotalNodes($Reflector->GetTotalNodes() + 1); 
							$timestamp = substr($logLine, 3, 19);
							$callsign = substr($logLine, 31, 11);
							//$callsign = explode(" ", $callsign);
							$ipport = substr($logLine, 43);
							//$ipport = explode(":", $ipport);
							$key1 = searchForKey("ipport",$ipport, $gateways);
							$key2 = searchForKey("callsign",$callsign, $gateways);
							if (($key1 === NULL) && ($key2 == NULL)) {
								array_push($gateways, Array('callsign'=>$callsign,'timestamp'=>$timestamp,'ipport'=>$ipport));
							}
						}
					}
				}
			}
		}
	}
	return $gateways;
}

$Reflector->LoadFlags();
$i = 0;

for ($i=0;$i<$Reflector->NodeCount();$i++) {

	echo '<tr class="table-center">
	<td>'.($i+1).'</td>
	<td>';
	list ($Flag, $Name) = $Reflector->GetFlag($Reflector->Nodes[$i]->GetCallSign());
	if (file_exists("./img/flags/".$Flag.".png")) {
		echo '<a href="#" class="tip"><img src="./img/flags/'.$Flag.'.png" class="table-flag" alt="'.$Name.'"><span>'.$Name.'</span></a>';
	}
	echo '</td>
	<td>'.$Reflector->Nodes[$i]->GetCallSign();
	echo '</td>
	<td>'.date("d.m.Y H:i", $Reflector->Nodes[$i]->GetLastHeardTime()).'</td>
	<td>'.FormatSeconds(time()-$Reflector->Nodes[$i]->GetConnectTime()).' s</td>
	<td>'.$Reflector->Nodes[$i]->GetProtocol().'</td>';
	if ($PageOptions['RepeatersPage']['IPModus'] != 'HideIP') {
		echo '<td>';
		$Bytes = explode(".", $Reflector->Nodes[$i]->GetIP());
		$MC = $PageOptions['RepeatersPage']['MasqueradeCharacter'];
		if ($Bytes !== false && count($Bytes) == 4) {
			switch ($PageOptions['RepeatersPage']['IPModus']) {
				case 'ShowLast1ByteOfIP':
					echo $MC.'.'.$MC.'.'.$MC.'.'.$Bytes[3];
					break;
				case 'ShowLast2ByteOfIP':
					echo $MC.'.'.$MC.'.'.$Bytes[2].'.'.$Bytes[3]; break;
				case 'ShowLast3ByteOfIP':
					echo $MC.'.'.$Bytes[1].'.'.$Bytes[2].'.'.$Bytes[3];
					break;
				default:
					echo $Reflector->Nodes[$i]->GetIP();
			}
		} else {
			$ipstr = $Reflector->Nodes[$i]->GetIP();
			$count = substr_count($ipstr, ":");
			if ($count > 1) {
				if (1 == substr_count($ipstr, "::")) { $ipstr = str_replace("::", str_repeat(":", 9 - $count), $ipstr); }
				if (7 == substr_count($ipstr, ":")) {
					echo $MC.':'.$MC.':'.$MC.':'.$MC.':'.$MC.':'.$MC;
					$Bytes = explode(":", $ipstr);
					for( $k=6; $k<8; $k++) { echo (0==strlen($Bytes[$k])) ? ':0' : ':'.$Bytes[$k]; }
				}
			}
		}
		echo '</td>';
   }
   echo '</tr>';
   
   
   if ($i == $PageOptions['RepeatersPage']['LimitTo']) { $i = $Reflector->NodeCount()+1; }
}

//$Reflector->SetTotalNodes($Reflector->NodeCount());

$logLines = array();
//error_log(print_r("logLines ".count($logLines)."\n", TRUE)); 
if ($log = fopen("/var/log/reflectors/P25-9846-".date("Y-m-d").".log", 'r')) {
	while ($logLine = fgets($log)) {
		array_push($logLines, $logLine);
	}
	fclose($log);
}
//error_log(print_r("logLines ".count($logLines)."\n", TRUE)); 
$gateways = getLinkedGateways($logLines) ;
//error_log(print_r("gateways ".count($gateways)."\n", TRUE)); 
//$Reflector->SetTotalNodes($Reflector->NodeCount() + count($p25gateways));
	
foreach ($gateways as $gateway) {
	$i += 1;
	echo '<tr class="table-center">';
	echo "<td>$i</td><td>";
	list ($Flag, $Name) = $Reflector->GetFlag($gateway['callsign']);
	if (file_exists("./img/flags/".$Flag.".png")) {
		echo '<a href="#" class="tip"><img src="./img/flags/'.$Flag.'.png" class="table-flag" alt="'.$Name.'"><span>'.$Name.'</span></a></td>';
	}
	$cs = explode(" ", $gateway['callsign']);
	$ip = explode(":", $gateway['ipport']);
	
	echo "<td>$cs[0]</td><td></td><td></td><td>P25</td><td>$ip[0]</td></tr>";
}

if ($log = fopen("/var/log/reflectors/NXDNReflector-".date("Y-m-d").".log", 'r')) {
	while ($logLine = fgets($log)) {
		array_push($logLines, $logLine);
	}
	fclose($log);
}
$gateways = getLinkedGateways($logLines);
	
foreach ($gateways as $gateway) {
	$i += 1;
	echo '<tr class="table-center">';
	echo "<td>$i</td><td>";
	list ($Flag, $Name) = $Reflector->GetFlag($gateway['callsign']);
	if (file_exists("./img/flags/".$Flag.".png")) {
		echo '<a href="#" class="tip"><img src="./img/flags/'.$Flag.'.png" class="table-flag" alt="'.$Name.'"><span>'.$Name.'</span></a></td>';
	}
	$cs = explode(" ", $gateway['callsign']);
	$ip = explode(":", $gateway['ipport']);
	
	echo "<td>$cs[0]</td><td></td><td></td><td>NXDN</td><td>$ip[0]</td></tr>";
}

?>

</table>
