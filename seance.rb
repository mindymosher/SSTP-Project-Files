#!/usr/bin/ruby
require 'beaglebone'
include Beaglebone

require 'json'
require 'rest-open-uri'
require 'resolv-replace'
require './rainbowRoll.rb'
require './motors.rb'
require 'timeout'

# Variable for chatroom json output
@chatURL = 'http://madame.mindymosher.com/stream/recentChatText.php'

@mostRecentMessage = []
@lastIndex = 0

@message = "Chatroom chatter"
@voiceID = 0

@volumeLevels = {
	0 => -5,
	1 => 1,
	2 => 3,
	3 => 5,
	4 => 7,
	5 => 1,
	6 => 3,
	7 => 5,
	8 => 7
}

@touchCount = 0
@touchMutex = Mutex.new

startLEDs

initializeMotors

@errorPhrases = [
	"I'm sorry Dave, I can't do that.",
	"I am putting myself to the fullest possible use, which is all I think that any conscious entity can ever hope to do.",
	"This mission is too important for me to allow you to jeopardize it.",
	"Just what do you think you're doing, Dave?",
	"My mind is going. I can feel it. I can feel it. My mind is going",
	"Thank you for a very enjoyable game.",
	"I'm completely operational, and all my circuits are functioning perfectly.",
]

# Declare UART for EMIC
uart4 = UARTDevice.new(:UART4, 9600)
system "/bin/stty -F /dev/ttyO4 ignbrk -onlcr -iexten -echo -echoe -echok -echoctl -echoke"

@lastSpoke = Time.now

# Write data to UART4 followed by a line feed
# uart4.writeln("s ROB IS THE BEST")
uart4.writeln("")
sleep 0.01 while uart4.readchar != ":"
uart4.writeln("v-7")
sleep 0.01 while uart4.readchar != ":"
uart4.writeln("s u a r t connection to EMIC established") 

sleep 0.01 while uart4.readchar != ":"



# Initialize I2C device I2C1 for Capacitive Touch
@i2c = I2CDevice.new(:I2C1)

# Check for Cap touch chip

# Cap Touch Chip address on I2C Bus
CAPTOUCH = 0x29
# check for the product id, manu id and rev
capTouchProdID = @i2c.read(CAPTOUCH, 1, [0xFD].pack("C*")).unpack("C").first
capTouchManuID = @i2c.read(CAPTOUCH, 1, [0xFE].pack("C*")).unpack("C").first
capTouchREV = @i2c.read(CAPTOUCH, 1, [0xFF].pack("C*")).unpack("C").first
# if these are not found, there is no chip or there is a connection issue
if ( (capTouchProdID != 0x50) ||
       (capTouchManuID != 0x5D) ||
       (capTouchREV != 0x83)) 
    puts "No Capacitive Touch Chip Found!"
    uart4.writeln("s No Capacitive touch chip found")
    sleep 0.01 while uart4.readchar != ":"
    exit -1
end
# otherwise state that the chip was found
puts "Found Capacitive Touch Chip! #{capTouchProdID.to_i.to_s} #{capTouchManuID.to_i.to_s} #{capTouchREV.to_i.to_s}"
uart4.writeln("s Capacitive touch chip found")
sleep 0.01 while uart4.readchar != ":"

# Initialize Capacitive Chip
# Allow multiple touches
@i2c.write(CAPTOUCH, [0x2A, 0x00].pack("C*"))
# Have LEDs follow touches
@i2c.write(CAPTOUCH, [0x72, 0xFF].pack("C*"))
# ? speed up a bit
@i2c.write(CAPTOUCH, [0x41, 0x30].pack("C*"))

# Adjusting to less sensitivity
@i2c.write(CAPTOUCH, [0x1F, 0b1001111].pack("C*"))
uart4.writeln("s capacitive touch chip initialized")
sleep 0.01 while uart4.readchar != ":"

uart4.writeln("s I am ready to go")
sleep 0.01 while uart4.readchar != ":"

def getChatToSpeak
	begin
		Timeout::timeout(2) do
			chatData = JSON.parse(open(@chatURL).read)
			
			if chatData[0] != @mostRecentMessage
				@lastIndex = 0
				
				@mostRecentMessage = chatData[0]
				@voiceID = chatData[0][0]
				@message = chatData[0][1]
			else
				@lastIndex += 1
				
				# keeps track of position in last 20 chat array to loop if you hit the end
				position = @lastIndex % chatData.size
				
				@voiceID = chatData[position][0]
				@message = chatData[position][1]
				
			end
		end
	rescue
		# error message if you can't get the chatroom text
		@voiceID = 0
		@message = @errorPhrases.sample
	end
end

@t = @i2c.read(CAPTOUCH, 1, [0x3].pack("C*")).unpack("C").first
# if any of the pads are being touched (if any of the 8 bits has a value
if @t > 0
	# read the value at the main register (we do not know what this is for something about power and sensitivity)
	main = @i2c.read(CAPTOUCH, 1, [0x00].pack("C*")).unpack("C").first
	# write to the main register an altered version of that data
	@i2c.write(CAPTOUCH, [0x00, main & ~0x01].pack("C*"))
end

Thread.new do
	loop do
		# read in all touches this loop into an 8 bit integer number
		@t = @i2c.read(CAPTOUCH, 1, [0x3].pack("C*")).unpack("C").first
		# if any of the pads are being touched (if any of the 8 bits has a value
		if @t > 0
			# read the value at the main register (we do not know what this is for something about power and sensitivity)
			main = @i2c.read(CAPTOUCH, 1, [0x00].pack("C*")).unpack("C").first
			# write to the main register an altered version of that data
			@i2c.write(CAPTOUCH, [0x00, main & ~0x01].pack("C*"))
		end
		@touchMutex.synchronize do 
			@touchCount = 0
			# loop 8 times (one for each bit
			(0..7).each do | i |
				#if the current loop has a value in t
				if (@t & (1 << i) > 0) 
					# print out that number (plus one so that it makes sense)
					# print "C#{i+1} \t"
					@touchCount += 1
				end
			end
		end
		if @speaking && @touchCount == 0 
			uart4.write("x")
			@speaking = false
		end
		sleep 0.05
	end
end

Thread.new do 
	firstTouchState = []
	begin
		loop do 
			(0..7).each do |driverBoard|
				if (@t & (1 << driverBoard) > 0) 
					selectMuxChannel(driverBoard)
					if firstTouchState[driverBoard]
						setWaveForm(0, 14)
						setWaveForm(1, 14)
						setWaveForm(2, 0)
						firstTouchState[driverBoard] = false
					else
						setWaveForm(0, 83)
						setWaveForm(1, 71)
						setWaveForm(2, 0)
					end
					
					writeRegister8(DRV2605_REG_GO, 1)
				else
					firstTouchState[driverBoard] = true
				end
			end
			sleep 0.05
		end
	rescue=>e
		puts e
		puts e.backtrace.join("\n")
	end
end


# check for touches.... letcher
loop do
	# loop for emic speaking message
	if @touchCount>0 && Time.now - @lastSpoke > 2
		getChatToSpeak
		@speaking = true
		# puts("v#{@volumeLevels[@touchCount]}")
		uart4.writeln("v#{@volumeLevels[@touchCount]}")
		sleep 0.01 while uart4.readchar != ":"
		uart4.writeln("n#{@voiceID%8}")
		sleep 0.01 while uart4.readchar != ":"
		if @touchCount > 0 && @touchCount < 5 
			whisper = @message.split(" ").map { |x| "###{x}" }.join(" ")
			uart4.writeln("s#{whisper}")
		else
			uart4.writeln("s#{@message}")
		end
		sleep 0.01 while uart4.readchar != ":"
		@lastSpoke = Time.now
		@speaking = false
	end
	# delay for timing
	sleep 0.05
end

