<?php
/* 
Author: Kenrick Beckett
Author URL: http://kenrickbeckett.com
Name: Chat Engine 2.0

*/
require_once("../dbcon.php");

session_start();
$_SESSION['last'] = time();
session_write_close();

//Start Array
$data = array();
// Get data to work with
		$current = cleanInput($_REQUEST['current']);
		$room = cleanInput($_REQUEST['room']);
		$username = cleanInput($_REQUEST['username']);
		$now = time();
// INSERT your data (if is not already there)
       	$findUser = "SELECT * FROM `chat_users_rooms` WHERE `username` = '$username' AND `room` ='$room' ";
		
		if(!hasData($findUser))
				{
					$insertUser = "INSERT INTO `chat_users_rooms` (`id`, `username`, `room`, `mod_time`) VALUES ( NULL , '$username', '$room', '$now')";
					mysql_query($insertUser) or die(mysql_error());
				}		
		 	$findUser2 = "SELECT * FROM `chat_users` WHERE `username` = '$username'";
			if(!hasData($findUser2))
				{
					$insertUser2 = "INSERT INTO `chat_users` (`id` ,`username` , `status` ,`time_mod`)
					VALUES (NULL , '$username', '1', '$now')";
					mysql_query($insertUser2);
					$data['check'] = 'true';
				}			
		$finish = time() + 30;
		$olduser = $now - 5;
		$getRoomUsers = mysql_query("SELECT * FROM `chat_users_rooms` WHERE `room` = '$room' AND `mod_time` >= '$olduser'");
		$check = mysql_num_rows($getRoomUsers);
        	
	    while(true)
		{
			usleep(500000);
			$now = time();
			mysql_query("UPDATE `chat_users` SET `time_mod` = '$now' WHERE `username` = '$username'");
			mysql_query("UPDATE `chat_users_rooms` SET `mod_time` = '$now' WHERE `username` = '$username' AND `room` ='$room'  LIMIT 1") ;
			error_reporting(0);
			session_start();
			error_reporting(ini_get('error_reporting'));
			$_SESSION['last'] = time();
			session_write_close();
			$olduser = $now - 5;
			$eraseuser = $now - 30;
			mysql_query("DELETE FROM `chat_users_rooms` WHERE `mod_time` <  '$eraseuser'");
			mysql_query("DELETE FROM `chat_users` WHERE `time_mod` <  '$eraseuser'");
			$check = mysql_num_rows(mysql_query("SELECT * FROM `chat_users_rooms` WHERE `room` = '$room' AND `mod_time` >= '$olduser'"));
			if($now <= $finish)
			{
				if($check != $current){
				 break;
				}
				echo " ";
				flush();
                                ob_flush();
			}
			else
			{
				 break;	
		    }
        }		 		
// Get People in chat
		if(mysql_num_rows($getRoomUsers) != $current)
		{
			$data['numOfUsers'] = mysql_num_rows($getRoomUsers);
			// Get the user list (Finally!!!)
			$data['userlist'] = array();
			while($user = mysql_fetch_array($getRoomUsers))
			{
				$data['userlist'][] = $user['username'];
			}
			$data['userlist'] = array_reverse($data['userlist']);
		}
		else
		{
			$data['numOfUsers'] = $current;	
			while($user = mysql_fetch_array($getRoomUsers))
			{
				$data['userlist'][] = $user['username'];
			}
			$data['userlist'] = array_reverse($data['userlist']);
		}
		echo json_encode($data);

?>
