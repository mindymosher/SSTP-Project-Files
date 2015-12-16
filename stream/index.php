<?php
	session_start();
	if (isset($_SESSION['last']) && $_SESSION['last'] < time() - 30) {
		session_unset();
	}

	require_once("Chat2/dbcon.php");
	$number = 1;
	
	while (!isset($_SESSION['userid'])) {
		
		$username = $number;
		
		$getUser = "SELECT * 
					FROM chat_users 
					WHERE username = '$username'";
					

		if (!hasData($getUser)) {
			$now = time();
			
			$postUsers = "INSERT INTO chat_users (
				id,
				username,
				status,
				time_mod
				)
			VALUES (
				NULL, '$username', '1', '$now'
			)";
				
			$insertUser = "INSERT INTO chat_users_rooms (
				id,
				username,
				room,
				mod_time
				)
			VALUES (
				NULL,
				'$username',
				'Parlor',
				'$now'
			)";
				
			mysql_query($postUsers);
			mysql_query($insertUser);
			
			$_SESSION['userid'] = $username;
			$_SESSION['last'] = time();
		} else {
			$number++;
		}
	}
	
	$roomResults = mysql_query("SELECT * FROM chat_rooms WHERE name = 'Parlor'");
	while ($rooms = mysql_fetch_array($roomResults)) {
		$file = $rooms['file'];
	}
	

?>

<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<Title>SSTP: Social Spiritual Transfer Protocol</title>

		<script language="javascript" type="text/javascript"
		src="/javascript/jquery/jquery.js"></script>

		<script language="javascript" type="text/javascript"
		src="/javascript/jquery-ui/jquery-ui.js"></script>
		<link type="text/css"
   			 href="/javascript/jquery-ui/css/smoothness/jquery-ui.css"
    			 rel="stylesheet" />
		<link type="text/css" rel="stylesheet" href="css/style.css">
		<link rel="stylesheet" type="text/css" href="Chat2/main.css"/>
		
		<link href='http://fonts.googleapis.com/css?family=Allura|Yesteryear|Seaweed+Script|Dynalight|Oswald|Slabo+27px|Lora|Playfair+Display' rel='stylesheet' type='text/css'>
		
		<script type="text/javascript" src="Chat2/room/chat.js"></script>
		<script type="text/javascript">
			var chat = new Chat('<?php echo $file; ?>');
			chat.init();
			/* chat.getUsers(<?php echo "'Parlor','" .$_SESSION['userid'] . "'"; ?>); */
			chat.getNumUsers(<?php echo "'Parlor','" .$_SESSION['userid'] . "'"; ?>);
			var name = '<?php echo $_SESSION['userid'];?>';
		</script>
		<script type="text/javascript" src="Chat2/room/settings.js"></script>
		<script>
			var dialog, form, buttonResult;
			function checkButtons() {
				$.get('buttonStatus.php', {}, updateButtons, "json");
				setTimeout(checkButtons, 1000);
			}
			
			function updateButtons(data) {
				var sTime = data['time'];
				for(var buttonName in data['buttons']) {
					var buttonData = data['buttons'][buttonName];
					if(buttonData['nextAvail'] > sTime) {
						$('#' + buttonName).addClass("disabled");
						$('#' + buttonName).attr("title", buttonData['nextAvail'] - sTime + " seconds until available");
					} else {
						$('#' + buttonName).removeClass("disabled");
						$('#' + buttonName).attr("title", "");
					}
				}
				
			}
			
			function sendPhoneMessage() {
				$.post( "handler.php", {button: "phone", phoneMessage : $("#message").val()}, function(data, status, xhr){ 
					$('#button-result p').replaceWith("<p>"+data+"</p>");
					buttonResult.dialog("open");
				}); 
				dialog.dialog("close");
			}
			
			function buttonClicked() {
				$.post("handler.php", {button: $(this).attr("id")}, function(data, status, xhr) {
					$('#button-result p').replaceWith("<p>"+data+"</p>");
					buttonResult.dialog("open");
				});
			}

		$(function() {
			checkButtons();
			
			dialog = $("#call-phone").dialog({
				autoOpen: false,
				height: 200,
				width: 450,
				modal: true,
				buttons: {
					"Send message": sendPhoneMessage,
					Cancel: function() {
						dialog.dialog( "close" );
					}
				},
				close: function() {
					form[ 0 ].reset();
				}
			});
			form = dialog.find( "form" ).on( "submit", function( event ) {
				event.preventDefault();
				sendPhoneMessage($("#message"));
			});
			 $( "#phone" ).on( "click", function() {
				dialog.dialog( "open" );
			});
			
			$("#knock").on("click", buttonClicked);
			$("#lights").on("click", buttonClicked);
			$("#fan1").on("click", buttonClicked);
			$("#fan2").on("click", buttonClicked);
			$("#sounds").on("click", buttonClicked);
			
			buttonResult = $("#button-result").dialog({
				autoOpen: false,
				height: 200,
				width: 450,
				modal: true,
				buttons: {
					Ok: function() {
						$(this).dialog("close");
					}
				}
			});
			
			
		});
		</script>	
		
	</head>
	<body>
	<div id="call-phone">
		<p>Send a message to the phone!  (140 character limit.)</p>
		<form>
			<fieldset>
				<label for="message">Message</label>
				<input type="text" name="message" id="message" value="" class="text ui-widget-content ui-corner-all">
				
				<!-- Allow form submission with keyboard without duplicating the dialog button -->
				<input type="submit" tabindex="-1" style="position:absolute; top:-1000px">
			</fieldset>
		</form>
	</div>
	<div id="button-result">
		<p></p>
	</div>
		<div id="main">
			<h1 class="title">SSTP:</h1>
			<section id="subtitle">
				<h2 class="title">Social Spiritual</h2>
				<h3 class="title">Transfer Protocol</h3>
			</section>
			<div class="clear"></div>
			<section id="vidBox">
				<div id="cameraButtons">
					<section id="head">
						<img src="img/camera.png" title="camera" alt="camera" />
					</section>
					<section id="camSelect">
						<div class="camButton" id="vid1">1</div>
						<div class="camButton" id="vid2">2</div>
						<div class="camButton" id="vid3">3</div>
						<div class="camButton" id="vid4">4</div>
						<div class="camButton" id="vid5">5</div>
					</section>
				</div>
				<iframe id="videoFrame" frameborder="0" height="480px" width="700px" src="index1.html"></iframe>
				<section id="serialButton">
					
						<img id="sounds" src="img/phono.png" alt="Music" title="Music" />
										
						<img id="phone" src="img/phone.png" alt="Message Phone" title="Message Phone" />
					
						<img id="fan1" src="img/curtains.png" alt="Move Curtains" title="Move Curtains" />
					
						<img id="fan2" src="img/fan.png" alt="Wind" title="Wind" />
					
						<img id="knock" src="img/knocker.png" alt="Knock" title="Knock" />
					
						<img id="lights" src="img/lamp.png" alt="Flicker Lights" title="Flicker Lights" />
				</section>
			</section>
			<div id="chat">
				<section id="chatterbox">
					<div id="chat-wrap">
						<div id="chat-area"></div>
					</div>
					<form id="send-message-area" action="">
						<textarea id="sendie" maxlength='256'></textarea>
					</form>
				</section>
				<section id="chat-right">
					<div id="usercount"></div>
					<!--<div id="userlist"></div>-->
					<section id="links">
						<a href="about.php" alt="About">About</a>
						<br /><br /><p>Closing Reception: Thursday April 30, 2015 5-8pm</p>
					</section>
				</section>
			</div>
		</div>
		<script type="text/javascript">
			$("#serialSend").click(function() {
				$.get("/test.php");
			});
			$("#vid1").click(function() {
				$("#videoFrame").attr('src', 'index1.html');
				
			});
			$("#vid2").click(function() {
				$("#videoFrame").attr('src', 'index2.html');
				
			});
			$("#vid3").click(function() {
				$("#videoFrame").attr('src', 'index3.html');
				
			});
			$("#vid4").click(function() {
				$("#videoFrame").attr('src', 'index4.html');
				
			});
			$("#vid5").click(function() {
				$("#videoFrame").attr('src', 'index5.html');
				
			});
		</script>

	</body>
</html>
