<?php

	$userStyle[] = "allura";
	$userStyle[] = "oswald";
	$userStyle[] = "yesteryear";
	$userStyle[] = "slabo";
	$userStyle[] = "seaweed";
	$userStyle[] = "lora";
	$userStyle[] = "dynalight";
	$userStyle[] = "playfair";

  $function = htmlentities(strip_tags($_POST['function']), ENT_QUOTES);
	$file = htmlentities(strip_tags($_POST['file']), ENT_QUOTES);
    
  $log = array();
    
    switch ($function) {
    
    	 case ('getState'):
    	 
        	 if (file_exists($file)) {
               $lines = file($file);
        	 }
             $log['state'] = count($lines);
              
        	 break;	
        	 
    	 case ('send'):
    	 
		     $nickname = htmlentities(strip_tags($_POST['nickname']), ENT_QUOTES);
		     $patterns = array("/:\)/", "/:D/", "/:p/", "/:P/", "/:\(/");
			 $replacements = array("<img src='smiles/smile.gif'/>", "<img src='smiles/bigsmile.png'/>", "<img src='smiles/tongue.png'/>", "<img src='smiles/tongue.png'/>", "<img src='smiles/sad.png'/>");
			 $reg_exUrl = "/(http|https|ftp|ftps)\:\/\/[a-zA-Z0-9\-\.]+\.[a-zA-Z]{2,3}(\/\S*)?/";
			 $blankexp = "/^[ ]*?\n/";
			 $message = htmlentities(strip_tags($_POST['message']), ENT_QUOTES);
			 
    		 if (strlen($message)!=0 && !preg_match($blankexp, $message)) {
            	
    			 if (preg_match($reg_exUrl, $message, $url)) {
           			$message = preg_replace($reg_exUrl, '<a href="'.$url[0].'" target="_blank">'.$url[0].'</a>', $message);
    			 } 
    			 $message = preg_replace($patterns, $replacements, $message);
				 $font = $userStyle[((int)$nickname)%count($userStyle)];
            	 fwrite(fopen($file, 'a'), "<span data-uid='" . (int)$nickname . "' class='" . $font . "'>". $message = str_replace("\n", " ", subStr($message, 0, 255)) . "</span>\n"); 
    		 }
    		 
        	 break;
    	
    }
    
    echo json_encode($log);

?>