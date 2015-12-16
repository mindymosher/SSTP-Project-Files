<?php
	/* This file gets the latest 20 lines from the parlorchat.txt file, and 
	   cuts out the chatroom styling tags. It outputs this information in 
	   clean json format 
	   */
	$chatFile = "/var/www/html/stream/Chat2/room/parlorchat.txt";
	
	$chatData = file($chatFile);
	
	for ($i = count($chatData)-1; $i >= 0 && $i > count($chatData) - 100; $i--) {
		$uid = trim(preg_replace('~.*?data-uid=\'(\d+)\'.*~', '\1', $chatData[$i]));
		$string = trim(preg_replace('~^<[^>]+?>(.*?)</span>$~', '\1', html_entity_decode($chatData[$i])));
		
		if ( preg_match("/^Phone says:/", $string) )
			continue;

		//$string = preg_replace('/^Phone says:/', '', $string);

		$data[] = array((int)$uid, $string);

		if ( count($data) >= 20 )
			break;

	}
	
	echo json_encode($data);
?>
