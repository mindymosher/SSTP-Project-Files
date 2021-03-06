<?php

/* do not run after 9pm or before 10am */
if ( date("G") >= 23 || date("G") < 9 ){
 echo "Please return between 9:30 AM and 8:00 PM EST."; 
 return;

}

include_once("buttons.php");

if ($_POST['button'] == "phone"){
	$string = $_POST["phoneMessage"];
	$string = preg_replace('/[^a-z0-9,\.! ]/i', '', $string);
	
	if (strlen($string) > 140) {
		$string = trim(substr($string, 0, 140));
	}

	if ( strlen($string) < 1 )
	{
		echo "Please enter a message";
		return;
	}

	if (!buttonAvail("phone")) {
		echo "Phone in use, please try again.";
		return;
	}

	buttonClicked("phone");
	system("/bin/stty -F /dev/ttyACM0 57600 -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -hupcl");
	// open the file for the usb serial interface
	$f = fopen("/dev/ttyACM0", "w");
	// set the string variable, depending on what vars were posted

	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	$num = fwrite($f, "1" . $string . "\n");
	// close the file
	fclose($f);
	// open the parlorchat file
	$c = fopen("Chat2/room/parlorchat.txt", "a");
	fwrite($c, "<span>Phone says: " . $string . "</span>\n");
	fclose($c);


	// print feedback to webpage
	echo "Thank you! Message sent!";
	
} elseif ($_POST['button'] == "fan1") {
	if(!buttonAvail("fan1")) {
		echo "Please wait awhile before blowing the curtains again.";
		return;
	}
	buttonClicked("fan1");
	
	// open the file for the usb serial interface
	system("/bin/stty -F /dev/ttyACM0 57600 -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -hupcl");
	$f = fopen("/dev/ttyACM0", "w");
	
	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	$num = fwrite($f, "31\n");
	// close the file
	fclose($f);

	// print feedback to webpage
	echo "You have blown the curtains!";

} elseif ($_POST['button'] == "fan2") {
	if(!buttonAvail("fan2")) {
		echo "Please wait awhile before gusting again.";
		return;
	}
	buttonClicked("fan2");
	
	// open the file for the usb serial interface
	system("/bin/stty -F /dev/ttyACM0 57600 -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -hupcl");
	$f = fopen("/dev/ttyACM0", "w");
	
	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	$num = fwrite($f, "32\n");
	// close the file
	fclose($f);

	// print feedback to webpage
	echo "You have gusted wind!";

} elseif ($_POST['button'] == "lights") {
	if(!buttonAvail("lights")) {
		echo "Please wait awhile before flickering the lights again.";
		return;
	}
	buttonClicked("lights");
	
	// open the file for the usb serial interface
	system("/bin/stty -F /dev/ttyACM0 57600 -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -hupcl");
	$f = fopen("/dev/ttyACM0", "w");
	
	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	$num = fwrite($f, "33\n");
	// close the file
	fclose($f);

	// print feedback to webpage
	echo "You have successfully flickered the lights!";

} elseif ($_POST['button'] == "knock") {
	if(!buttonAvail("knock")) {
		echo "Too many knocks! Please wait awhile before knocking again.";
		return;
	}
	buttonClicked("knock");
	
	// open the file for the usb serial interface
	//$f = fopen("/dev/ttyACM0", "w");
	
	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	//$num = fwrite($f, "5\n");
	// close the file
	//fclose($f);


	system("/var/www/html/stream/runasMindy '/usr/bin/aplay /var/www/html/stream/doorKnock.wav'");

	// print feedback to webpage
	echo "You have successfully knocked!";
	

} elseif ($_POST['button'] == "sounds") {
	if(!buttonAvail("sounds")) {
		echo "Please wait awhile before triggering sounds again.";
		return;
	}
	buttonClicked("sounds");
	
	// open the file for the usb serial interface
	//$f = fopen("/dev/ttyACM0", "w");
	
	// write to the file
	// fwrite returns number of bytes it was able to write (-1 if fail)
	// assign to variable for feedback 
	//$num = fwrite($f, "6\n");
	// close the file
	//fclose($f);
	$songclips[] = "musicclip1.wav";
	$songclips[] = "musicclip2.wav";
	$songclips[] = "musicclip3.wav";
	$song = array_rand($songclips);
	

	system("/var/www/html/stream/runasMindy '/usr/bin/aplay /var/www/html/stream/$songclips[$song]'");

	// print feedback to webpage
	echo "You have successfully triggered a sound!";

}




?>
