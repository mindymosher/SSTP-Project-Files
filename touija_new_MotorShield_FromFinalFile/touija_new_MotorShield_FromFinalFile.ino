// Touija Board
// by Mindy Mosher
//
// Control 2 Stepper Motors to spell out Tweets received over serial communication
//
// Requires the AFMotor library (https://github.com/adafruit/Adafruit-Motor-Shield-library)
// And AccelStepper with AFMotor support (https://github.com/adafruit/AccelStepper)
// And RF24 library (https://github.com/TMRh20/RF24)
// Public domain!


// do not move beyond 2680 on x axis (at either end, 2650 in the middle), this is the limit! 1340 on y axis
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// create the motorshield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// request the stepper motor from the motor shield so that we can use it
Adafruit_StepperMotor *motor1 = AFMS.getStepper(400, 1);
Adafruit_StepperMotor *motor2 = AFMS.getStepper(400, 2);

// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins A0 & A1
RF24 radio(A0,A1);

byte addresses[][6] = {"1Node","2Node"};

// Set up roles to simplify testing 
boolean role;                                    // The main role variable, holds the current role identifier
boolean role_ping_out = 1, role_pong_back = 0;   // The two different roles.

boolean zeroX = false;
boolean zeroY = false;
boolean mustZero = true;
boolean stringComplete = false;
boolean doneSpelling = false;

boolean test = true;

String message = "";
char first = 0;

int xButton = 0;
int yButton = 0;

int xCoord = 0;
int yCoord = 0;

int a[] = {-230, 610};
int b[] = {-425, 640};
int c[] = {-620, 670};
int d[] = {-845, 690};
int e[] = {-1060, 700};
int f[] = {-1260, 720};
int g[] = {-1460, 730};
int h[] = {-1665, 720};
int i[] = {-1860, 710};
int j[] = {-2030, 700};
int k[] = {-2220, 680};
int l[] = {-2410, 660};
int m[] = {-2635, 640};
int n[] = {-200, 260};
int o[] = {-400, 300};
int p[] = {-620, 320};
int q[] = {-800, 330};
int r[] = {-1000, 340};
int s[] = {-1190, 360};
int t[] = {-1400, 380};
int u[] = {-1610, 380};
int v[] = {-1815, 370};
int w[] = {-2050, 360};
int x[] = {-2310, 320};
int y[] = {-2515, 300};
int z[] = {-2680, 270};
int zero[] = {-100, 1250};
int one[] = {-280, 1300};
int two[] = {-490, 1340};
int three[] = {-740, 1340};
int four[] = {-990, 1340};
int five[] = {-1220, 1340};
int six[] = {-1460, 1340};
int seven[] = {-1700, 1340};
int eight[] = {-1940, 1340};
int nine[] = {-2200, 1340};
int question[] = {-2400, 1340};
int exclam[] = {-2600, 1300};
int hash[] = {-210, 950};
int period[] = {-380, 920};
int comma[] = {-520, 940};
int dash[] = {-650, 1030};
int underscore[] = {-850, 970};
int colon[] = {-1030, 1030};
int semicolon[] = {-1150, 1030};
int frontslash[] = {-1320, 1050};
int backslash[] = {-1460, 1050};
int dollarsign[] = {-1660, 1050};
int percent[] = {-1870, 1050};
int at[] = {-2170, 1040};
int oparen[] = {-2370, 1020};
int cparen[] = {-2550, 1000};
int asterisk[] = {-2675, 960};
int homeLoc[] = {-1470, 50};

// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!
void forwardstep1() {  
  motor1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {  
  motor1->onestep(BACKWARD, DOUBLE);
}
// wrappers for the second motor!
void forwardstep2() {  
  motor2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  motor2->onestep(BACKWARD, DOUBLE);
}

// Motor shield has two motor ports, now we'll wrap them in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup() {
  Serial.begin(57600); 
  
  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400 KHz
  
  pinMode(3, INPUT);
  pinMode(2, INPUT);
  digitalWrite(3, HIGH);
  digitalWrite(2, HIGH);
  
  AFMS.begin(); // start the motor shield

  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(1000.0);
  
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(1000.0);
  
//  printf_begin();
//  printf("\n\rRF24/examples/GettingStarted/\n\r");
//  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  // Setup and configure rf radio
//  radio.begin();                          // Start up the radio
//  radio.setAutoAck(1);                    // Ensure autoACK is enabled
//  radio.setRetries(15,15);                // Max delay between retries & number of retries
//  radio.openWritingPipe(addresses[1]);
//  radio.openReadingPipe(1,addresses[0]);
//  
//  radio.startListening();                 // Start listening
//  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop() {
  xButton = digitalRead(2);
  yButton = digitalRead(3);
  Serial.println(xButton);
//     Serial.println("Beginning of loop");
  // zero the planchette
  if(mustZero) zeroPlanchette();
  
  if(doneSpelling)  {
    sendHome();
    doneSpelling = false;
  }
  while(Serial.available() > 0) {
   
    if (first != '\n') {
     first = (char)Serial.read();
     message += first;
    }
    if (first == '\n') {
      stringComplete = true;
   }
  }
  if (stringComplete) {
    spellTweet(message);
    doneSpelling = true;
    stringComplete = false;
    message = "";
    first = 0;
  }

//  /****************** Pong Back Role ***************************/
//if ( role == role_pong_back )
//  {
//    if( radio.available()){
//      unsigned long got_time;                                       // Variable for the received timestamp
//      while (radio.available()) {                                   // While there is data ready
//        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
//      }    
//     
//      radio.stopListening();                                        // First, stop listening so we can talk   
//      radio.write( &got_time, sizeof(unsigned long) );              // Send the final one back.      
//      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
//      printf("Sent response %lu \n\r", got_time);  
//   }
// }
//  

}

void zeroPlanchette() {
  Serial.println("zero planchette");
     stepper1.moveTo(5000);
     stepper2.moveTo(5000);
     mustZero = true;
  while ((!zeroX || !zeroY) && mustZero){
//      Serial.println("Reading button states");
       xButton = digitalRead(2);
       yButton = digitalRead(3);
       
     if (!zeroY) {
//       Serial.println("running stepper1");
       stepper1.run(); 
     }
     if (!zeroX) {
//       Serial.println("running stepper2");
       stepper2.run();
     }
     if(xButton == LOW && !zeroX) {
//       Serial.print("X Zeroed!");
       zeroX = true; 
       motor2->release();
       stepper2.setCurrentPosition(0);
       stepper2.moveTo(0);
     }
     if(yButton == LOW && !zeroY) {
//       Serial.print("Y Zeroed!");
       zeroY = true;
       motor1->release(); 
       stepper1.setCurrentPosition(0);
       stepper1.moveTo(0);
     }

     if(zeroX && zeroY) {
       mustZero = false;
     }
  }
  sendHome();
}

void sendHome() {
  Serial.println("send home");
    moveToPosition(homeLoc[0], homeLoc[1]);
    motor1->release();
    motor2->release();
}

void moveToPosition(long xPos, long yPos) {
  Serial.print("move to position");
  Serial.print(xPos);
  Serial.println(yPos);
  
  stepper1.moveTo(yPos);
  stepper2.moveTo(xPos);
  while(stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
    stepper1.run();
    stepper2.run();
  }  
}

void spellTweet(String tweet) {
  char lastLetter = 0;
  char currentLetter = 0;
  tweet.toLowerCase();
   for(int loopI=0; loopI < tweet.length(); loopI++) {
     lastLetter = currentLetter;
     currentLetter = tweet.charAt(loopI);
     Serial.println(currentLetter);
      switch (currentLetter) {
         case 'a':
           xCoord = a[0];
           yCoord = a[1]; 
           break; 
         case 'b':
           xCoord = b[0];
           yCoord = b[1];
           break; 
         case 'c':
           xCoord = c[0];
           yCoord = c[1];
           break; 
         case 'd':
           xCoord = d[0];
           yCoord = d[1];
           break; 
         case 'e':
           xCoord = e[0];
           yCoord = e[1];
           break; 
         case 'f':
           xCoord = f[0];
           yCoord = f[1];
           break; 
         case 'g':
           xCoord = g[0];
           yCoord = g[1];
           break; 
         case 'h':
           xCoord = h[0];
           yCoord = h[1];
           break; 
         case 'i':
           xCoord = i[0];
           yCoord = i[1];
           break; 
         case 'j':
           xCoord = j[0];
           yCoord = j[1];
           break; 
         case 'k':
           xCoord = k[0];
           yCoord = k[1];
           break; 
         case 'l':
           xCoord = l[0];
           yCoord = l[1];
           break; 
         case 'm':
           xCoord = m[0];
           yCoord = m[1];
           break; 
         case 'n':
           xCoord = n[0];
           yCoord = n[1];
           break; 
         case 'o':
           xCoord = o[0];
           yCoord = o[1];
           break; 
         case 'p':
           xCoord = p[0];
           yCoord = p[1];
           break; 
         case 'q':
           xCoord = q[0];
           yCoord = q[1];
           break; 
         case 'r':
           xCoord = r[0];
           yCoord = r[1];
           break; 
         case 's':
           xCoord = s[0];
           yCoord = s[1];
           break; 
         case 't':
           xCoord = t[0];
           yCoord = t[1];
           break; 
         case 'u':
           xCoord = u[0];
           yCoord = u[1];
           break; 
         case 'v':
           xCoord = v[0];
           yCoord = v[1];
           break; 
         case 'w':
           xCoord = w[0];
           yCoord = w[1];
           break; 
         case 'x':
           xCoord = x[0];
           yCoord = x[1];
           break; 
         case 'y':
           xCoord = y[0];
           yCoord = y[1];
           break; 
         case 'z':
           xCoord = z[0];
           yCoord = z[1];
           break;  
         case '#':
           xCoord = hash[0];
           yCoord = hash[1];
           break;  
         case '.':
           xCoord = period[0];
           yCoord = period[1];
           break;  
         case ',':
           xCoord = comma[0];
           yCoord = comma[1];
           break;  
         case '-':
           xCoord = dash[0];
           yCoord = dash[1];
           break;  
         case '_':
           xCoord = underscore[0];
           yCoord = underscore[1];
           break;  
         case ':':
           xCoord = colon[0];
           yCoord = colon[1];
           break;  
         case ';':
           xCoord = semicolon[0];
           yCoord = semicolon[1];
           break;  
         case '/':
           xCoord = frontslash[0];
           yCoord = frontslash[1];
           break;  
         case '\\':
           xCoord = backslash[0];
           yCoord = backslash[1];
           break;  
         case '$':
           xCoord = dollarsign[0];
           yCoord = dollarsign[1];
           break;  
         case '%':
           xCoord = percent[0];
           yCoord = percent[1];
           break;  
         case '@':
           xCoord = at[0];
           yCoord = at[1];
           break;  
         case '(':
           xCoord = oparen[0];
           yCoord = oparen[1];
           break;  
         case ')':
           xCoord = cparen[0];
           yCoord = cparen[1];
           break;  
         case '*':
           xCoord = asterisk[0];
           yCoord = asterisk[1];
           break;  
         case ' ':
           xCoord = homeLoc[0];
           yCoord = homeLoc[1];
           break;
         case '0':
           xCoord = zero[0];
           yCoord = zero[1];
           break; 
         case '1':
           xCoord = one[0];
           yCoord = one[1];
           break;  
         case '2':
           xCoord = two[0];
           yCoord = two[1];
           break;  
         case '3':
           xCoord = three[0];
           yCoord = three[1];
           break;  
         case '4':
           xCoord = four[0];
           yCoord = four[1];
           break;  
         case '5':
           xCoord = five[0];
           yCoord = five[1];
           break;  
         case '6':
           xCoord = six[0];
           yCoord = six[1];
           break;  
         case '7':
           xCoord = seven[0];
           yCoord = seven[1];
           break;  
         case '8':
           xCoord = eight[0];
           yCoord = eight[1];
           break;  
         case '9':
           xCoord = nine[0];
           yCoord = nine[1];
           break;  
         case '?':
           xCoord = question[0];
           yCoord = question[1];
           break;  
         case '!':
           xCoord = exclam[0];
           yCoord = exclam[1];
           break; 
         case '\n':
           break;
         default:
           xCoord = homeLoc[0];
           yCoord = homeLoc[1];
           break; 
      }
      if (currentLetter == lastLetter) {
        moveToPosition(xCoord -150, yCoord + 150); 
      }
      moveToPosition(xCoord, yCoord);
      delay(500);
   }
  motor1->release();
  motor2->release();
}



