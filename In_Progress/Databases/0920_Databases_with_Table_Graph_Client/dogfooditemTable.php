<!DOCTYPE html>
<html>
<head>
	<meta charset = "UTF-8">
	<meta http-equiv = "refresh" content = "30">
	<style type = "text/css">
		.spec{
			text-align:center;
		}
		.con{
			text-align:left;
		}
		</style>
</head>

<body>
	<h1 align = "center">Dog Food Vending Machine Order Database</h1>
	<div class = "spec">
		# <b>Dog food sales</b>
		<br></br>
	</div>

	<table border = '1' style = "width = 30%" align = "center">
	<tr align = "center">
		<th>ID</th>
		<th>NAME</th>
		<th>DATE</th>
		<th>TIME</th>
		<th>BREED</th>
		<th>ITEM1</th>
		<th>ITEM2</th>
		<th>ITEM3</th>
		<th>ITEM4</th>
		<th>ITEM5</th>
		<th>ITEM6</th>
	</tr>

	<?php
		$conn = mysqli_connect("localhost", "iot", "pwiot");
		mysqli_select_db($conn, "iotdb");
		$result = mysqli_query($conn, "select * from dog_food_sales");
		while($row = mysqli_fetch_array($result))
		{
			echo "<tr align = center>";
			echo '<th>'.$row['id'].'</td>';
			echo '<th>'.$row['name'].'</td>';
			echo '<th>'.$row['date'].'</td>';
			echo '<th>'.$row['time'].'</td>';
			echo '<th>'.$row['breed'].'</td>';
			echo '<th>'.$row['item1'].'</td>';
			echo '<th>'.$row['item2'].'</td>';
			echo '<th>'.$row['item3'].'</td>';
			echo '<th>'.$row['item4'].'</td>';
			echo '<th>'.$row['item5'].'</td>';
			echo '<th>'.$row['item6'].'</td>';
			echo "</tr>";

		}
		mysqli_close($conn);
	?>
	</table>
</body>
</html>
