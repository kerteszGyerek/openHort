<?php 
//			sensor data upload:		http://openhort.xn--rt-lia8e.hu/arduino_data/uploadData.php?id=001&action=uploadSensors&data=17.56:17.56
//			actor state upload:		http://openhort.xn--rt-lia8e.hu/arduino_data/uploadData.php?id=001&action=uploadActor&data=Lamp:0



function uploadSensors($id, $data) {
	
$temps = explode(":", $data);
$date = time();
$sql = "INSERT INTO arduino_".mysql_real_escape_string($id)."_sensors 
								VALUES (".$date.","
								.mysql_real_escape_string($temps[0]).","
							.mysql_real_escape_string($temps[1]).");";				
						
					
$result = mysql_query($sql);
if (!$result) {
    die('Invalid query: ' . mysql_error());
}
echo 'OK';
}

function uploadActor($id, $data) {
	
$temp = explode(":", $data);
$date = time();
$sql = "INSERT INTO arduino_".mysql_real_escape_string($id)."_actors 
							VALUES (".$date.",'"
							.mysql_real_escape_string($temp[0])."',"
							.mysql_real_escape_string($temp[1]).");";
							
$result = mysql_query($sql);
if (!$result) {
    die('Invalid query: ' . mysql_error());
}
echo 'OK';
}


$dbhost = "xxxx"; 
$dbuser = "xxxx"; 
$dbpass = "xxxx"; 
$dbname = "xxxx"; 

$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die ("Error connecting to mysql!");
mysql_select_db($dbname) or die ("Error selecting databese!");



switch($_GET['action']) {
	case "uploadSensors": uploadSensors($_GET['id'],$_GET['data']); break;
	case "uploadActor": uploadActor($_GET['id'],$_GET['data']); break;
}



?>
