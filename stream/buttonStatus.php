<?php
	include_once("buttons.php");
	
	readButtons();
	
	$data['time'] = time();
	
	$data['buttons'] = $buttonInfo;
	
	echo json_encode($data);
?>