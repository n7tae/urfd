<?php

function GetSystemUptime() {
   $out = exec("uptime");
   return substr($out, 0, strpos($out, ","));
}

function Debug($message) {
   echo '<br><hr><pre>';
   print_r($message);
   echo '</pre><hr><br>';
}

function ParseTime($Input) {
    if (strpos($Input, "<") !== false) {
        $Input = substr($Input, 0, strpos($Input, "<"));
    }
    return strtotime($Input);
}

function FormatSeconds($seconds) {
  $seconds = abs($seconds); 
  return sprintf("%d days %02d:%02d:%02d", $seconds/60/60/24,($seconds/60/60)%24,($seconds/60)%60,$seconds%60);
} 

function CreateCode ($laenge) {   
	$zeichen = "1234567890abcdefghijklmnopqrstuvwyxzABCDEFGHIJKLMNAOPQRSTUVWYXZ";   
	mt_srand( (double) microtime() * 1000000); 
	$out = "";
	for ($i=1;$i<=$laenge;$i++){ 
		$out .= $zeichen[mt_rand(0,(strlen($zeichen)-1))];       
	}         
	return $out;  
}

?>
