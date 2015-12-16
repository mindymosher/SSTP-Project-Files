#!/usr/bin/env ruby
require 'beaglebone'
include Beaglebone

# Initialize SPI device SPI0
@spi = SPIDevice.new(:SPI1, SPI::SPI_MODE_0, 2000000)

@nLEDs = 8

@latchBytes = (@nLEDs + 31) / 32

@leds = [0x80, 0x80, 0x80] * @nLEDs

@leds += [0] * @latchBytes

@speedHash = {
	0 => 0.01,
	1 => 0.0075,
	2 => 0.005,
	3 => 0.0025,
	4 => 0.001,
	5 => 0.00075,
	6 => 0.0005,
	7 => 0.00025,
	8 => 0.0001,
}


def stripColor(r, g, b) 
	#(((g | 0x80) << 16) | ((r | 0x80) <<  8) | (b | 0x80)) 
	[g | 0x80, r | 0x80, b | 0x80]
end

def wheel(wheelPos, brightness = 1)
  r, g, b = 0
  case(wheelPos / 128)
 
    when 0
      r = 127 - wheelPos % 128   #Red down
      g = wheelPos % 128      # Green up
      b = 0                  #blue off
       
    when 1
      g = 127 - wheelPos % 128  #green down
      b = wheelPos % 128      #blue up
      r = 0                  #red off
       
    when 2
      b = 127 - wheelPos % 128  #blue down 
      r = wheelPos % 128      #red up
      g = 0                  #green off
       
	end
	#puts [g,r,b].inspect
  return(stripColor((r*brightness).ceil,(g*brightness).ceil,(b*brightness).ceil))
end

def stripShow
	#puts @leds.inspect
	@spi.xfer(@leds.pack("C*"))
end

def stripSetPixelColor(pixel, color)
	return if pixel >= @nLEDs
	g = color[0]
	r = color[1]
	b = color[2]
	#puts [g,r,b].inspect
	@leds[pixel*3] = g
	@leds[pixel*3+1] = r
	@leds[pixel*3+2] = b
end

def rainbowCycle(wait, brightness = 1) 
   (0..384*5-1).each do |j|
		(0..@nLEDs-1).each do |i|
			stripSetPixelColor(i, wheel( ((i * 384 / @nLEDs) + j) % 384, brightness))
		end   
    stripShow  # write all the pixels out
    sleep wait
   end  
end

def blackOut
	(0..@nLEDs -1).each do |pixel|
		stripSetPixelColor(pixel, [0x80,0x80,0x80])
	end
	stripShow
end

def startLEDs
	Thread.new do
		begin
			wait = 0.01
			brightness = 1
			brightnessStep = 0.05
			loop do
				(0..384*5-1).each do |j|
					@touchMutex.synchronize do
						wait = @speedHash[@touchCount]
						if brightness < 1 && @touchCount > 0
							brightness += brightnessStep
						elsif brightness > 0 && @touchCount == 0
							brightness -= brightnessStep
						end
					end
					(0..@nLEDs-1).each do |i|
						stripSetPixelColor(i, wheel( ((i * 384 / @nLEDs) + j) % 384, brightness))
					end  
					if @touchCount == 0 && brightness == 0
						blackOut
					else
						stripShow # write all the pixels out
					end
					sleep wait
			   end 
			end
		rescue Exception => e
			puts "blackout!"
			blackOut
			raise 
		end
	end
end
