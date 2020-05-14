    <?php
 
if($_POST["t1"])
{
	$data = $_POST["t1"].",".$_POST["t2"].",".$_POST["count"].",".$_POST["delay"]."\r\n";
	file_put_contents("log.html", $data."<br>", FILE_APPEND);
	echo $data;
	echo "@RECIEVED";
}
 
?>