#include <SoftwareSerial.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define rxPin 9
#define txPin 10
#define ledPin 13

SoftwareSerial emicSerial = SoftwareSerial(rxPin, txPin);
// receiver pin
int recPin = 2;
int hookVal = 0;

int bellLPower = 3;
int bellLGround = 5;
int bellRPower = 4;
int bellRGround = 6;

long voiceNum = 0;

String phoneMessage = "";

boolean offHook = false;
boolean wasOffHook = false;
boolean stringComplete = false;
boolean haveMessage = false;

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins A0 & A1
RF24 radio(A0,A1);

byte addresses[][6] = {"devc1", "serv1"};      // addresses for this device (devc1) and the server (serv1)
byte pipeNo, gotByte;                          // Declare variables for the pipe and the byte received

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  pinMode(recPin, INPUT);
  digitalWrite(recPin, HIGH);
  attachInterrupt(0, receiverChangeState, CHANGE);
  
  pinMode(bellLPower, OUTPUT);
  pinMode(bellLGround, OUTPUT);
  pinMode(bellRPower, OUTPUT);
  pinMode(bellRGround, OUTPUT);
  
  digitalWrite(bellLPower, HIGH);
  digitalWrite(bellLGround, LOW);
  digitalWrite(bellRPower, HIGH);
  digitalWrite(bellRGround, LOW);
    
  // reserve 200 bytes for the inputString:
  phoneMessage.reserve(200);
  
  emicSerial.begin(9600);
  Serial.begin(9600);
  hookVal = digitalRead(recPin);
  digitalWrite(ledPin, LOW);
  
  /*
    When the Emic 2 powers on, it takes about 3 seconds for it to successfully
    initialize. It then sends a ":" character to indicate it's ready to accept
    commands. If the Emic 2 is already initialized, a CR will also cause it
    to send a ":"
  */
  emicSerial.print('\n');             // Send a CR in case the system is already up
  while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
  delay(10);                          // Short delay
  emicSerial.flush();                 // Flush the receive buffer
  
   // Setup and configure radio

  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.openWritingPipe(addresses[1]);        // Writing to Server
  radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on devc1, pipe 1
  radio.startListening();                 // Start listening
  radio.powerUp();
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop() {
  if (hookVal == LOW ) {
    offHook = true;
    if (!wasOffHook) {
      Serial.println("off Hook");
      Serial.flush();
      wasOffHook = true;
      delay(100);
    }
  } else {
    offHook = false;
    if (wasOffHook) {
      Serial.println("on Hook");
      Serial.flush();
      wasOffHook = false;
      delay(100);
    }
  }
 
  while( radio.available(&pipeNo)){              // Read all available payloads
      Serial.println("receiving wireless data");
    // get the new byte:
    radio.read( &gotByte, 1 );  
    // add it to the inputString:
    phoneMessage += (char)gotByte;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (gotByte == '\n') {
      stringComplete = true;
      haveMessage = true;
    }
  }
  
  if (stringComplete) {
    radio.writeAckPayload(pipeNo,"got message", 11 ); 
    Serial.println("Do the thing!");
    // ring the phone, send the message to emic on pickup, 
    doTheThing();
    stringComplete = false;
  }
}

void doTheThing() {
  int i=7;  
  while(haveMessage && i >=0 ) {
    ringBell(10);
    i--;
  }
  Serial.println("setting have message to false");
  haveMessage = false;
  phoneMessage = "";
}

void ringBell(int numRings) {
  Serial.println("Ringing!");
  for(int i=0; i<numRings && hookVal == HIGH; i++) {
    leftBellOn();
    rightBellOn();
  }
  delay(700);
}


void receiverChangeState() {
  hookVal = digitalRead(recPin);
  if(hookVal == LOW) {
    // if the receiver is picked up say so
    // if there's a message
      if(haveMessage) {
       // speak the message
       sayMessage(phoneMessage);
       // reset variables for message receiving
       haveMessage = false;
       phoneMessage = "";
       stringComplete = false;
    }
  } else {
    
    emicSerial.print("X");
    emicSerial.print('\n');
  }
}

void sayMessage(String message) {
    // pick a random voice
    voiceNum = random(0, 8);
    emicSerial.print('N');
    emicSerial.print(voiceNum);
    emicSerial.print('\n');
    
    // this first section sends a string with the volume at 0
    // so that when you pick up the phone there is a pause before
    // it reads you what was submitted via web
    emicSerial.print('V-48\n');
    emicSerial.print('s this string serves as a pause');
    emicSerial.print('\n');
    emicSerial.print('V0\n');
    emicSerial.print('S');
    emicSerial.print(message);
    emicSerial.print('\n');  
}

void leftBellOn() {
  digitalWrite(bellRPower, HIGH);
  digitalWrite(bellRGround, LOW);
  delay(10);
  digitalWrite(bellLGround, HIGH);
  digitalWrite(bellLPower, LOW);
  delay(50);
}

void rightBellOn() {
  digitalWrite(bellLPower, HIGH);
  digitalWrite(bellLGround, LOW);
  delay(10);
  digitalWrite(bellRGround, HIGH);
  digitalWrite(bellRPower, LOW);
  delay(50);
}
