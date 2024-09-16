<?php
	$conn = mysqli_connect("localhost", "iot", "pwiot");
#	mysqli_set_charset($conn, "UTF-8");
	mysqli_select_db($conn, "iotdb");
	$query = "select name, date, time, num1, num2, num3, num4, num5, num6 from dog_food_categories ";
	$result = mysqli_query($conn, $query);

	$data = array(array('LSW_SQL','num1','num2','num3','num4','num5','num6'));

	if($result)
	{
		while($row = mysqli_fetch_array($result))
		{
			array_push($data, array($row['date']."\n".$row['time'], intval($row['num1']),intval($row['num2']) ,intval($row['num3']), intval($row['num4'], intval($row['num5'], intval($row['num6']));
		}
	}

	$options = array(
			'title' => 'Dog Food Categories - Num1 ~ Num6',
			'width' => 1000, 'height' => 400,
			'curveType' => 'function'
			);

?>

<script src="//www.google.com/jsapi"></script>
<script>
var data = <?=json_encode($data) ?>;
var options = <?= json_encode($options) ?>;

google.load('visualization', '1.0', {'packages':['corechart']});

google.setOnLoadCallback(function() {
	var chart = new google.visualization.BarChart(document.querySelector('#chart_div'));
	chart.draw(google.visualization.arrayToDataTable(data), options);
	});
	</script>
<div id="chart_div"></div>
