<?php
error_reporting(E_ALL);
header('Content-Type: text/html; charset=utf-8');
$dbhost = "xxxxx"; 
$dbuser = "xxxxx";
$dbpass = "xxxxx"; 
$dbname = "xxxxx"; 

$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die ("Error connecting to mysql!");
mysql_select_db($dbname) or die ("Error selecting databese!");

$arduinoId= '001';

if(isset($_POST['table'])){
	$sql = "SELECT date, FROM_UNIXTIME(date) as time, temp_1, temp_2 FROM arduino_".$arduinoId."_sensors";
	$actorsSql = "SELECT FROM_UNIXTIME(date) as time, actor, state FROM arduino_".$arduinoId."_actors";
	if(strtotime($_POST['from']) && strtotime($_POST['to'])){
		$sql .= " WHERE date > ".strtotime($_POST['from'])." AND date < ".strtotime($_POST['to']);
		$actorsSql .= " WHERE date > ".strtotime($_POST['from'])." AND date < ".strtotime($_POST['to']);
	}
	$sql .= " ORDER BY date DESC";
	$actorsSql .= " ORDER BY date DESC";
	if($_POST['sensorslength'] > 0) {
		$sql .=" LIMIT ".mysql_real_escape_string($_POST['sensorslength']);
	}

}

//var_dump($sql);
$result = mysql_query($sql);
$actorsResult = mysql_query($actorsSql);

var_dump($result);

?>
<html>
<head>
	<link href="css/jquery-ui-1.9.2.custom.min.css" rel="stylesheet">
	<link href="css/style.css" rel="stylesheet">
</head>
<body class="claro">
<form id="sensorsform" method="POST" action="">
	<input type="hidden" name="table" value="sensors"><br>
	<label for="from">dátum: </label><input id="fromtimeinput" type="datetime" name="from" value="<?= $_POST['from'] ?>"> - <input id="totimeinput" type="datetime" name="to" value="<?= $_POST['to'] ?>"><br>
	<label for="sensorslength">hány darabot: </label><input type="number" name="sensorslength" value="<?= $_POST['sensorslength'] ?>"><br>

	<input type="submit" value="Mutasd">
</form>

<div id="sensorstable">
<div id="sensorchart">
</div>
<br>
<table border="1" cellpadding="3" cellspacing="0">
	<tr><th>id</th><th>dátum</th><th>temp_1</th><th>temp_2</th></tr>
	<?php
	//[{x: 1, y: 5}, {x: 1.5, y: 1.7},
	$temp_1String = '[';
	$temp_2String = '[';
	//$labelString = '[';
	$max = 0;
	$min = 0;
	$i = 0;
	while($row = mysql_fetch_assoc($result)) {

		if ($i == 0){
			$max = $row['temp_1'];
			$min = $row['temp_1'];
		}
		if($max<$row['temp_1']) {
			$max = $row['temp_1'];
		}else if($min > $row['temp_1']){
			$min = $row['temp_1'];
		}

		if($max<$row['temp_2']) {
			$max = $row['temp_2'];
		}else if($min > $row['temp_2']){
			$min = $row['temp_2'];
		}
		
		$temp_1String .= '{x:'.$row['date'].', y:'.$row['temp_1'].'},';
		$temp_2String .= '{x:'.$row['date'].', y:'.$row['temp_2'].'},';
		//{value: 1, text: "Jan"}
		//$labelString .= '{value:'.$row['temp_1'].', text:"'.$row['time'].'"}, ';
		echo '<tr>';
		echo '<td>'.$row['date'].'</td>';
		echo '<td>'.$row['time'].'</td>';
		echo '<td>'.$row['temp_1'].'</td>';
		echo '<td>'.$row['temp_2'].'</td>';
		echo '</tr>';
		$i++;
	}
	$temp_1String .= ']';
	$temp_2String .= ']';
	//$labelString .= ']';
	?>
</table>
</div>
<div id="actorstable">
<table border="1" cellpadding="3" cellspacing="0">
	<tr><th>dátum</th><th>actor</th><th>state</th></tr>
	<?php
	while($row = mysql_fetch_assoc($actorsResult)) {
		echo '<tr>';
		echo '<td>'.$row['time'].'</td>';
		echo '<td>'.$row['actor'].'</td>';
		echo '<td>'.$row['state'].'</td>';
		echo '</tr>';
	}
	?>
</table>

</div>

<script src="js/jquery-1.8.3.min.js"></script>
<script src="js/jquery-ui-1.9.2.custom.min.js"></script>
<script src="js/jquery-ui-timepicker-addon.js"></script>
<script src="//ajax.googleapis.com/ajax/libs/dojo/1.8.1/dojo/dojo.js"></script>
<script>
$('#fromtimeinput').datetimepicker();
$('#totimeinput').datetimepicker();
// ******************************************************** SENSORS CHART ************************************************************
require([
// Require the basic chart class
"dojox/charting/Chart",

// Require the theme of our choosing
"dojox/charting/themes/Claro",
// Charting plugins: 

// We want to plot Lines 
"dojox/charting/plot2d/Lines",

// Load the Legend, Tooltip, and Magnify classes
"dojox/charting/widget/Legend",
"dojox/charting/action2d/Tooltip",
"dojox/charting/action2d/Magnify",
// We want to use Markers
"dojox/charting/plot2d/Markers",

// We'll use default x/y axes
"dojox/charting/axis2d/Default",

// Wait until the DOM is ready
"dojo/domReady!"
], function(Chart, theme, LinesPlot, Legend, Tooltip, Magnify) {
	// Define the data
	var chartData = <?php echo $temp_1String?>;
	var chartData2 = <?php echo $temp_2String?>;
	// var chartData3 = [3000,12000,17733,9876,12783,12899,13888,13277,14299,12345,12345,15763].reverse();
	// Create the chart within it's "holding" node
	var chart = new Chart("sensorchart");
	
	// Set the theme
	chart.setTheme(theme);

	// Add the only/default plot 
	chart.addPlot("default", {
		type: LinesPlot,
		markers: true
	});   
	// Add axes labelFunc:function(value){return value+" cm";}
	chart.addAxis("x", {labelFunc:function(value){
		var date = new Date(value*1000);

		//var year = date.getYear();
		var month = date.getMonth();
		var day = date.getDay();

		// hours part from the timestamp
		var hours = date.getHours();
		// minutes part from the timestamp
		var minutes = date.getMinutes();
		// seconds part from the timestamp
		var seconds = date.getSeconds();
		// will display time in 10:30:23 format
		return formattedTime = month + '-' + day + '-' + hours + ':' + minutes;

	}});
	chart.addAxis("y", { min:<?php echo $min-0.5;?>, max: <?php echo $max+0.5;?>, vertical: true, fixLower: "major", fixUpper: "major" });

	// Add the series of data
	chart.addSeries("temp_1",chartData);
	chart.addSeries("temp_2",chartData2);
	//chart.addSeries("Monthly Sales - 2008",chartData3);
	// Create the tooltip
	var tip = new Tooltip(chart,"default");
	// Create the magnifier
	var mag = new Magnify(chart,"default");
	// Render the chart!
	chart.render();
	// Create the legend
	var legend = new Legend({ chart: chart }, "legend");
});
// ****************************************************** END OF SENSORS CHART **********************************************************
</script>
</body>
</html>

