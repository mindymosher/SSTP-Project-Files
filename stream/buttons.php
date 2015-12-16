<?php

$buttonInfo['phone'] = array("lastClicked"=>0, "delay"=>60, "nextAvail"=>0);
$buttonInfo['fan1'] = array("lastClicked"=>0, "delay"=>15, "nextAvail"=>0);
$buttonInfo['fan2'] = array("lastClicked"=>0, "delay"=>20, "nextAvail"=>0);
$buttonInfo['lights'] = array("lastClicked"=>0, "delay"=>30, "nextAvail"=>0);
$buttonInfo['knock'] = array("lastClicked"=>0, "delay"=>30, "nextAvail"=>0);
$buttonInfo['sounds'] = array("lastClicked"=>0, "delay"=>30, "nextAvail"=>0);

$buttons = array_keys($buttonInfo);

$buttonAttr = array_keys($buttonInfo['phone']);

function writeButtons() {
	global $buttons, $buttonInfo, $buttonAttr;
	global $buttonsRead;
	if (!$buttonsRead) {
		readButtons();
	}
	$fp = fopen("buttonData.txt", "r+");

	if (flock($fp, LOCK_EX)) {  // acquire an exclusive lock
		ftruncate($fp, 0);      // truncate file
		fwrite($fp, json_encode($buttonInfo));
		fflush($fp);            // flush output before releasing the lock
		flock($fp, LOCK_UN);    // release the lock
	} else {
		// echo "Couldn't get the lock!";
		return false;
	}

	fclose($fp);
	return true;
	
}

function readButtons() {
	global $buttonsRead;
	global $buttons, $buttonInfo, $buttonAttr;
	$fp = fopen("buttonData.txt", "r");

	if (flock($fp, LOCK_SH)) {  // acquire an shared lock
		rewind($fp);      // truncate file
		$buttonData = json_decode(fread($fp,filesize("buttonData.txt")), true);
		flock($fp, LOCK_UN);    // release the lock
	} else {
		// echo "Couldn't get the lock!";
	}
	
	foreach ($buttons as $button) {
		foreach ($buttonAttr as $attribute) {
			if ($buttonData[$button][$attribute]) {
				$buttonInfo[$button][$attribute] = $buttonData[$button][$attribute];
			}
		}
	}
	
	fclose($fp);
	$buttonsRead = true;
	return true;
}

function buttonClicked($name) {
	global $buttons, $buttonInfo, $buttonAttr;
	if (!in_array($name, $buttons)) {
		return;
	} 
	readButtons();
	$buttonInfo[$name]["lastClicked"] = time();
	$buttonInfo[$name]["nextAvail"] = $buttonInfo[$name]["lastClicked"] + $buttonInfo[$name]["delay"];
	
	writeButtons();
}

function buttonAvail($name) {
	global $buttons, $buttonInfo, $buttonAttr;
	if (!in_array($name, $buttons)) {
		return false;
	} 
	readButtons();
	if ($buttonInfo[$name]['nextAvail'] <= time()) {
		return true;
	}
	return false;
}

?>