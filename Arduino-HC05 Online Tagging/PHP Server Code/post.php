<?php
 
if($_POST["data"])
{
	$indata = $_POST["data"];
	$sets = explode(';', $indata);
	echo "\r\n";
	foreach ($sets as $sete) 
	{
		$pieces = explode('$', $sete);
		$mac = $pieces[0];
		$rssi = intval($pieces[1]);
		$sdelay = intval($pieces[2]);
		$duration = round(intval($pieces[3])/1000);
		
		$seconds_start = round((round(microtime(true) * 1000) - $sdelay)/1000);
		
		if (strlen($mac)>6)
		{
			echo $mac.",".$rssi.",".$seconds_start.",".$duration."\n";
			file_put_contents("log.html",  $macid.",".$rssi.",".$seconds_start.",".$duration."\n", FILE_APPEND);
		
			sql_function_rssi($mac, $seconds_start, $duration, $rssi);
		}
	}
	
	echo "@RECORDED";
	echo "\r\n";
}




 function sql_function_rssi($mac, $stime, $duration, $rssi) 
 {
    try 
	{


		 $sql_bool = false;
		 require "SQLinitial.php";
		 $sql_bool = $bool_ok;
		 
		 $utimez = time();
		 
		 $utime_up = $utimez + 600 + 20;
		 $utime_down = $utimez - 600 - 20;
		 
		 $seconds_start_up = $stime + 3;
		 $seconds_start_down = $stime - 3;
		 
		 
		 if($sql_bool)
		 {
			
				try 
				{
					mysql_query("DELETE FROM  BLElog WHERE MAC='$mac' AND utime<'$utime_up' AND utime>'$utime_down' AND stime>'$seconds_start_down' AND stime<'$seconds_start_up'", $conn );
				}
				catch (Exception $e) 
				{
					$errors_on_server .= $e;	
				}
			
			
			
			$query = "INSERT INTO BLElog (MAC, stime, duration, RSSI, utime) VALUES('$mac', '$stime', '$duration', '$rssi', '$utimez')";
			$retval = mysql_query($query, $conn );

			if(! $retval )
			{
			  die('<br>Error9829: Could not connect to server <br>' . mysql_error());
			}
			else
			{
			
			}
			
		 }			
		 else
		 {
			echo "<br>Error9829: Database Server Error<br>";
		 }
		 
		 mysql_close();




	}
	catch (Exception $e) 
	{ 
		$errors_on_server .= $e;	
	}
}
 
 
?>