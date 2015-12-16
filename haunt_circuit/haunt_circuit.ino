#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins A0 & A1
RF24 radio(A0,A1);

byte addresses[][6] = {"3devc", "3serv"};      // addresses for this device and the server
byte pipeNo, gotByte;                          // Declare variables for the pipe and the byte received

int relayPin1 = 2;        // IN1 connected to digital pin 2 (Curtains1)
int relayPin2 = 3;        // IN2 connected to digital pin 3 (Curtains2)
int relayPin3 = 4;        // IN3 connected to digital pin 4   (air)
int relayPin4 = 5;        // IN3 connected to digital pin 4   (lights)

void setup() {
  Serial.begin(57600);
  
  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin2, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin3, OUTPUT);      // sets the digital pin as output
  pinMode(relayPin4, OUTPUT);      // sets the digital pin as output
  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin2, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin3, HIGH);        // Prevents relays from starting up engaged
  digitalWrite(relayPin4, LOW);        // Prevents relays from starting up engaged

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
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
Serial.println("hi");
}

void loop() {
  while( radio.available(&pipeNo)){              // Read all available payloads
      Serial.println("receiving wireless data");
    // get the new byte:
    radio.read( &gotByte, 1);  
    if(gotByte == '1') {
      Serial.println("got 1");
       digitalWrite(relayPin1, LOW);
       delay(100);
       digitalWrite(relayPin2, LOW); 
       delay(5000);
       digitalWrite(relayPin1, HIGH);
       digitalWrite(relayPin2, HIGH); 
    } else if (gotByte == '2') {
      Serial.println("got 2");
       digitalWrite(relayPin3, LOW);
       delay(2000);
       digitalWrite(relayPin3, HIGH);
    } else if (gotByte == '3') {
      Serial.println("got 3");
       digitalWrite(relayPin4, HIGH);
       delay(50);
       digitalWrite(relayPin4, LOW);
       delay(100);
       digitalWrite(relayPin4, HIGH); 
       delay(50);
       digitalWrite(relayPin4, LOW);
       delay(200);
       digitalWrite(relayPin4, HIGH); 
       delay(50);
       digitalWrite(relayPin4, LOW);
       delay(50);
       digitalWrite(relayPin4, HIGH); 
       delay(50);
       digitalWrite(relayPin4, LOW);
       delay(100);
       digitalWrite(relayPin4, HIGH);
       delay(500);
       digitalWrite(relayPin4, LOW);
    } else if (gotByte == '0') {
      digitalWrite(relayPin4, HIGH);
    } else if (gotByte == '4') {
      digitalWrite(relayPin4, LOW);
    } else {
       return; 
    }
    radio.writeAckPayload(pipeNo,"got message", 11 ); 
  }

}
