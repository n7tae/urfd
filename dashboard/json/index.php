<?php

header("Access-Control-Allow-Origin: *");
header('Content-Type: application/json');

date_default_timezone_set("UTC");

if (file_exists($_SERVER['DOCUMENT_ROOT'] . "/pgs/functions.php")) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/functions.php");
if (file_exists($_SERVER['DOCUMENT_ROOT'] . "/pgs/config.inc.php")) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/config.inc.php");

if (!class_exists('ParseXML')) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/class.parsexml.php");
if (!class_exists('Node')) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/class.node.php");
if (!class_exists('xReflector')) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/class.reflector.php");
if (!class_exists('Station')) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/class.station.php");
if (!class_exists('Peer')) require_once($_SERVER['DOCUMENT_ROOT'] . "/pgs/class.peer.php");

$Reflector = new xReflector();
$Reflector->SetXMLFile($Service['XMLFile']);
$Reflector->SetPIDFile($Service['PIDFile']);
$Reflector->LoadXML();
$Reflector->SetFlagFile($_SERVER['DOCUMENT_ROOT'] . "/pgs/country.csv");
$Reflector->LoadFlags();

$Request = $_SERVER['REQUEST_URI'];
$ViewDir = '/views/';

switch ($Request) {
    case '/json/links':
        require __DIR__ . $ViewDir . 'links.php';
        break;

    case '/json/metadata':
        require __DIR__ . $ViewDir . 'metadata.php';
        break;

    case '/json/modulesinuse':
        require __DIR__ . $ViewDir . 'modulesinuse.php';
        break;
    
    case '/json/peers':
        require __DIR__ . $ViewDir . 'peers.php';
        break;

    case '/json/reflector':
        require __DIR__ . $ViewDir . 'reflector.php';
        break;

    case '/json/stations':
        require __DIR__ . $ViewDir . 'stations.php';
        break;

    case '/json/status':
        require __DIR__ . $ViewDir . 'status.php';
        break;

    default:
        header('Content-Type: text/plain');
        http_response_code(404);
        echo('404 page not found');

}

?>