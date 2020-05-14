<?php




$sql_bool = false;
require "SQLinitial.php";
$sql_bool = $bool_ok;
if($sql_bool)
{


		$query_ask="SELECT * FROM `BLElog` ORDER BY `ID` DESC";

		$retval = mysql_query($query_ask, $conn );
		if (!$retval)
		{
			echo '<br>Error: Could not run searching query: <br>' . mysql_error();

		}

		print "
			<h4>Currently in range devices</h4>
			<table style=\"width:99%; background-color:#354B5E; border:2px solid grey;  border-collapse:collapse; color:#DCDDD8; font-size:8pt; margin-left:10px; margin-left:2px; margin-right:2px; margin-top:2px; margin-bottom:2px; \">
			<td  width=\"5%\"><b>#<b></td><td  width=\"10%\"><b>MAC<b></td><td  width=\"8%\"><b>RSSI<b></td><td  width=\"10%\"><b>Time<b></td><td  width=\"10%\"><b>Duration(s)<b></td>
			";
		while ($row = mysql_fetch_row($retval))
		{
			
			if((time()-$row[4])<=200)
			{
				$dt = new DateTime("@$row[1]");
				//echo $dt->format('Y-m-d H:i:s');
				print "
				
				<tr>
				  <td  width=\"5%\">{$row[5]}</td><td  width=\"10%\">{$row[0]}</td><td  width=\"8%\">{$row[3]}</td><td  width=\"10%\">".$dt->format('Y-m-d H:i:s')."</td><td  width=\"10%\">{$row[2]}</td>
				 </tr>
				";
			}
		}
		
		$retval = mysql_query($query_ask, $conn );
		print "</table>";		
		print "
			<br><h4>Old Logs</h4>
			<table style=\"width:99%; background-color:#F6F1ED; border:2px solid grey;  border-collapse:collapse; color:#5A3F2E; font-size:8pt; margin-left:10px; margin-left:2px; margin-right:2px; margin-top:2px; margin-bottom:2px; \">
			<td  width=\"5%\"><b>#<b></td><td  width=\"10%\"><b>MAC<b></td><td  width=\"8%\"><b>RSSI<b></td><td  width=\"10%\"><b>Time<b></td><td  width=\"10%\"><b>Duration(s)<b></td>
			";
		while ($row = mysql_fetch_row($retval))
		{
			if((time()-$row[4])>200)
			{
				$dt = new DateTime("@$row[1]");
				//echo $dt->format('Y-m-d H:i:s');
				print "
				<tr>
				  <td  width=\"5%\">{$row[5]}</td><td  width=\"10%\">{$row[0]}</td><td  width=\"8%\">{$row[3]}</td><td  width=\"10%\">".$dt->format('Y-m-d H:i:s')."</td><td  width=\"10%\">{$row[2]}</td>
				 </tr>
				";
			}
		}
		print "</table>";

	
	
	
mysql_close();

}
else
{
	echo "<br>Error: Database Server Error<br>";
}


exit();
 
?>