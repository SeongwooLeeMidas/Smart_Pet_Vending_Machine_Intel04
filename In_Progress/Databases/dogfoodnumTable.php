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
	<h1 align = "center">Dog Food Vending Machine Database</h1>
	<div class = "spec">
		# <b>Dog food categories</b>
		<br></br>
	</div>

	<table border = '1' style = "width = 30%" align = "center">
	<tr align = "center">
		<th>ID</th>
		<th>NAME</th>
		<th>DATE</th>
		<th>TIME</th>
		<th>NUM1</th>
		<th>NUM2</th>
		<th>NUM3</th>
		<th>NUM4</th>
		<th>NUM5</th>
		<th>NUM6</th>
	</tr>

	<?php
		$conn = mysqli_connect("localhost", "iot", "pwiot");
		mysqli_select_db($conn, "dog_food_categories");
		$result = mysqli_query($conn, "select * from dog_food_categories");
		while($row = mysqli_fetch_array($result))
		{
			echo "<tr align = center>";
			echo '<th>'.$row['id'].'</td>';
			echo '<th>'.$row['name'].'</td>';
			echo '<th>'.$row['date'].'</td>';
			echo '<th>'.$row['time'].'</td>';
			echo '<th>'.$row['num1'].'</td>';
			echo '<th>'.$row['num2'].'</td>';
			echo '<th>'.$row['num3'].'</td>';
			echo '<th>'.$row['num4'].'</td>';
			echo '<th>'.$row['num5'].'</td>';
			echo '<th>'.$row['num6'].'</td>';
			echo "</tr>";

		}
		mysqli_close($conn);
	?>
	</table>
</body>
</html>
