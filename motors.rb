# Initialize I2C device I2C2
@i2c2 = I2CDevice.new(:I2C2)

@s0 = GPIOPin.new(:P9_21, :OUT)
@s1 = GPIOPin.new(:P9_23, :OUT)
@s2 = GPIOPin.new(:P9_27, :OUT)
@s3 = GPIOPin.new(:P9_26, :OUT)

@controlPin = [@s0, @s1, @s2, @s3]

@muxChannel=[
    [0,0,0,0], #channel 0
    [1,0,0,0], #channel 1
    [0,1,0,0], #channel 2
    [1,1,0,0], #channel 3
    [0,0,1,0], #channel 4
    [1,0,1,0], #channel 5
    [0,1,1,0], #channel 6
    [1,1,1,0], #channel 7
    [0,0,0,1], #channel 8
    [1,0,0,1], #channel 9
    [0,1,0,1], #channel 10
    [1,1,0,1], #channel 11
    [0,0,1,1], #channel 12
    [1,0,1,1], #channel 13
    [0,1,1,1], #channel 14
    [1,1,1,1]  #channel 15
  ]

DRV2605_ADDR = 0x5A

DRV2605_REG_STATUS = 0x00
DRV2605_REG_MODE = 0x01
DRV2605_MODE_INTTRIG  = 0x00
DRV2605_MODE_EXTTRIGEDGE  = 0x01
DRV2605_MODE_EXTTRIGLVL  = 0x02
DRV2605_MODE_PWMANALOG  = 0x03
DRV2605_MODE_AUDIOVIBE  = 0x04
DRV2605_MODE_REALTIME  = 0x05
DRV2605_MODE_DIAGNOS  = 0x06
DRV2605_MODE_AUTOCAL  = 0x07

DRV2605_REG_RTPIN = 0x02
DRV2605_REG_LIBRARY = 0x03
DRV2605_REG_WAVESEQ1 = 0x04
DRV2605_REG_WAVESEQ2 = 0x05
DRV2605_REG_WAVESEQ3 = 0x06
DRV2605_REG_WAVESEQ4 = 0x07
DRV2605_REG_WAVESEQ5 = 0x08
DRV2605_REG_WAVESEQ6 = 0x09
DRV2605_REG_WAVESEQ7 = 0x0A
DRV2605_REG_WAVESEQ8 = 0x0B

DRV2605_REG_GO = 0x0C
DRV2605_REG_OVERDRIVE = 0x0D
DRV2605_REG_SUSTAINPOS = 0x0E
DRV2605_REG_SUSTAINNEG = 0x0F
DRV2605_REG_BREAK = 0x10
DRV2605_REG_AUDIOCTRL = 0x11
DRV2605_REG_AUDIOLVL = 0x12
DRV2605_REG_AUDIOMAX = 0x13
DRV2605_REG_RATEDV = 0x16
DRV2605_REG_CLAMPV = 0x17
DRV2605_REG_AUTOCALCOMP = 0x18
DRV2605_REG_AUTOCALEMP = 0x19
DRV2605_REG_FEEDBACK = 0x1A
DRV2605_REG_CONTROL1 = 0x1B
DRV2605_REG_CONTROL2 = 0x1C
DRV2605_REG_CONTROL3 = 0x1D
DRV2605_REG_CONTROL4 = 0x1E
DRV2605_REG_VBAT = 0x21
DRV2605_REG_LRARESON = 0x22


# test to see if i2c device is recognized and working
# puts i2c.read(DRV2605_ADDR, 1, [DRV2605_REG_STATUS].pack("C*")).unpack("C")

def initializeDriverBoard 
	writeRegister8(DRV2605_REG_MODE, 0x00) # out of standby

	writeRegister8(DRV2605_REG_RTPIN, 0x00) # no real-time-playback

	writeRegister8(DRV2605_REG_WAVESEQ1, 1) # strong click
	writeRegister8(DRV2605_REG_WAVESEQ2, 0)

	writeRegister8(DRV2605_REG_OVERDRIVE, 0) # no overdrive

	writeRegister8(DRV2605_REG_SUSTAINPOS, 0)
	writeRegister8(DRV2605_REG_SUSTAINNEG, 0)
	writeRegister8(DRV2605_REG_BREAK, 0)
	writeRegister8(DRV2605_REG_AUDIOMAX, 0x64)

	# ERM open loop

	# turn off N_ERM_LRA
	writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F)
	# turn on ERM_OPEN_LOOP
	writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20)
	
	writeRegister8(DRV2605_REG_MODE, DRV2605_MODE_INTTRIG);
	
end

def writeRegister8(register, value)
	@i2c2.write(DRV2605_ADDR, [register, value].pack("C*"))
end

def readRegister8(register)
	@i2c2.read(DRV2605_ADDR, 1, [register].pack("C*")).unpack("C").first
end

def setWaveForm(slot, wave)
	writeRegister8(DRV2605_REG_WAVESEQ1+slot, wave);
end
	
def selectMuxChannel(channel)
	(0..3).each do |i|
		# set control pins, if it's 1 set high, if not set low
		@controlPin[i].digital_write(@muxChannel[channel][i] == 1 ? :HIGH : :LOW)
	end
end
	
def initializeMotors
	@s0.digital_write(:LOW)
	@s1.digital_write(:LOW)
	@s2.digital_write(:LOW)
	@s3.digital_write(:LOW)
	
	#initialize each driver board
	(0..7).each do |driverBoard|
		selectMuxChannel(driverBoard)
		initializeDriverBoard
	end
end