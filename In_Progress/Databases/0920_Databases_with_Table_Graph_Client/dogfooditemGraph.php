<?php
	$conn = mysqli_connect("localhost", "iot", "pwiot");
#	mysqli_set_charset($conn, "UTF-8");
	mysqli_select_db($conn, "iotdb");
	$query = "select name, date, time, breed, item1, item2, item3, item4, item5, item6 from dog_food_sales ";
	$result = mysqli_query($conn, $query);

	$data = array(array('Date/Time', 'Breed', 'Item1', 'Item2', 'Item3', 'Item4', 'Item5', 'Item6', breed, item1, item2, item3, item4, item5, item6));

	if($result)
	{
		while($row = mysqli_fetch_array($result))
		{
			array_push($data, array($row['date']."\n".$row['time'], $row['breed'], intval($row['item1']), intval($row['item2']), intval($row['item3']), intval($row['item4']), intval($row['item5']), intval($row['item6'])));
		}
	}

	$options = array(
			'title' => 'Dog Food Vending Machine Order Database',
			'width' => 1000, 'height' => 400,
			'bars' => 'vertical',
			'vAxis' => array('title' => 'Items Sold'),
			'hAxis' => array('title' => 'Date/Time')
			);

?>

<script src="//www.google.com/jsapi"></script>
<script>
var data = <?=json_encode($data) ?>;
var options = <?= json_encode($options) ?>;

google.load('visualization', '1.0', {'packages':['corechart', 'bar']});

google.setOnLoadCallback(function() {
	var chart = new google.visualization.BarChart(document.querySelector('#chart_div'));
	chart.draw(google.visualization.arrayToDataTable(data), options);
	});
	</script>
<div id="chart_div"></div>
