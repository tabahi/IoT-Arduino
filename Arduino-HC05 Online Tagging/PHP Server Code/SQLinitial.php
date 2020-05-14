<?php



$bool_ok = false;
$user="db_username";
$password="db_password";
$database="db_name";
$conn = mysql_connect('db_hostname',$user,$password);
if(! $conn )
{
		  die('Error: Could not connect: <br>' . mysql_error());
		  $bool_ok = false;
}
else
{
		//echo "Dataserver Connected<br>";
  
		@mysql_select_db($database) or die( "<br>Error9829: Unable to find database<br>");
		/*
		$query_create_table="CREATE TABLE IF NOT EXISTS BLElog (MAC varchar(64) NOT NULL, stime int(50) NOT NULL, duration int(50) NOT NULL,RSSI int(50) NOT NULL, utime int(50) NOT NULL, ID int(10) NOT NULL auto_increment, UNIQUE id (ID))";
		$retval = mysql_query($query_create_table, $conn );
		if(! $retval )
		{
			  die('Error: Could not create database at server.: <br>' . mysql_error());
			  $bool_ok = false;
			  mysql_close();
		}
		else
		{
				//echo "Dataserver Connection: ok <br>";
				$bool_ok = true;
		}
		*/
		$bool_ok = true;
}
		

		
?>