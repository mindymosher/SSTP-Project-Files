/*
   Jan 2015 - Altered RF24 file used to remotely control several components
   March 2014 - TMRh20 - Updated along with High Speed RF24 Library fork
   Parts derived from examples by J. Coliz <maniacbug@ymail.com>
*/
/**
 * Example for efficient call-response using ack-payloads 
 * 
 * This example continues to make use of all the normal functionality of the radios including 
 * the auto-ack and auto-retry features, but allows ack-payloads to be written optionlly as well. 
 * This allows very fast call-response communication, with the responding radio never having to 
 * switch out of Primary Receiver mode to send back a payload, but having the option to switch to 
 * primary transmitter if wanting to initiate communication instead of respond to a commmunication. 
 */
 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins A0 & A1
RF24 radio(A0,A1);
                                                                           // Topology
byte listening_addresses[][6] = {"1serv","2serv","3serv","4serv","5serv","6serv"};   // Radio pipe addresses for listening
byte writing_addresses[][6] = {"1devc","2devc","3devc","4devc","5devc","6devc"};   // Radio pipe addresses for writing
char data[8] = "ready\n\0";
char buff[34];
char temp[142];

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char serialIDChar = 0;
String afterIDString = "";

void setup(){

  Serial.begin(57600);
  printf_begin();

  // Setup and configure radio
  

  radio.begin();
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  radio.enableDynamicPayloads();            // send dynamic sized things
  radio.openWritingPipe(writing_addresses[0]);        // Writing to 1devc
  radio.openReadingPipe(1,listening_addresses[0]);      // Open a reading pipe on serv1, pipe 1 - for phone
  radio.openReadingPipe(2,listening_addresses[1]);      // Open a reading pipe on serv2, pipe 2 - for touija
  radio.openReadingPipe(3,listening_addresses[2]);      // Open a reading pipe on serv3, pipe 3 - for relay arduino
  radio.startListening();                 // Start listening
  radio.powerUp();
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  
  // can move this when other code is done or delete it
  radio.stopListening();
  
  printf("Welcome to the Wireless Sender. Sending Message to phone...\n\n");
  // can delete this when other code is done
  stringBreaker(data);
    
  printf("Sending Message to Touija...\n\n");
  radio.openWritingPipe(writing_addresses[1]);
  stringBreaker(data);
    
}

void loop(void) {
// set up regular wired serial for server to speak to arduino

// listen to wired serial for input

// when you get wired serial input, do things based on what you get
  if (stringComplete) {
    // set the id char so we know what to do with the data
    serialIDChar = inputString.charAt(0);
    // print the id char
//    Serial.println(serialIDChar);
    // set a new string to the rest of the data after id string
    afterIDString = inputString.substring(1);
    //stop listening so we can write
    radio.stopListening();
    
    // if you get a message from the website, send it to the phone
    if (serialIDChar == '1') {
      radio.openWritingPipe(writing_addresses[0]);        // Writing to devc1
      afterIDString.toCharArray(temp, 142);
      stringBreaker(temp); 
    }
// if you get a tweet, send it to touija
    if (serialIDChar == '2') {
      radio.openWritingPipe(writing_addresses[1]);        // Writing to devc2
      afterIDString.toCharArray(temp, 142);
      stringBreaker(temp);
    }

// if you get any other signal that is relevant, send it to the arduino with the relays
    if (serialIDChar == '3') {
      radio.openWritingPipe(writing_addresses[2]);        // Writing to devc3
      afterIDString.toCharArray(temp, 2);
      stringBreaker(temp);
    }
    
//  reset input variables so it'll work again
    inputString = "";
    stringComplete = false;
    serialIDChar = 0;
    afterIDString = "";
    // this one clears out the temp char array
    memset(temp, 0, sizeof(temp));
  }
// also listen for acknowledgements, and wait/timeout if you don't get any.  Add acks/errors to a log file

}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
//  Serial.println("Serial Event!");
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
 //  Serial.println(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}


// this method takes a string and breaks it up and transmits it to the phone
// because the wireless chip nrf24l01 can only transmit 32 bytes at a time
void stringBreaker(char *stringIn) {
    int stringLength = strlen(stringIn);
    
    for (int i=0; stringLength > 0; i++) {
        strncpy(buff, stringIn + 32*i, stringLength > 32 ? 32 : stringLength);
        buff[(stringLength > 32 ? 32 : stringLength)] = 0;
        Serial.println(buff);
        if (stringLength < 32) {
           buff[stringLength] = 0; 
        }
        radio.write(&buff, stringLength > 32 ? 32 : stringLength + 1 );
        stringLength -= 32;
    }
}

