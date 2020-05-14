<?php


	if($_POST["data"])
	{

			try 
			{
			
			$datafile = 'data.html';
			file_put_contents($datafile, $_POST['data'], FILE_APPEND);
			}
			catch (Exception $e)
			{ echo "Error".$e; }
		
	}
	else
	{
		echo 'Error: No data posted';
	}


















?>