<?php
    $color = $_GET["color"];
	$con = mysqli_connect("localhost", "root", "root","SmartFactory"); 
	if (mysqli_connect_errno())
	{
		echo "Failed to connect to MySQL: " . mysqli_connect_error();
	}
    $sql = "insert into sf(`color`) values('$color')";
    mysqli_query($con, $sql);    
    print($sql);

    mysqli_close($con);
?>