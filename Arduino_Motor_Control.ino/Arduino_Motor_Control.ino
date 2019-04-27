
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 24 Apr. 2019
 By Shounak L.

 */

#include <Stepper.h>

const int stepsPerRevolution = 2038;  // change this to fit the number of steps per revolution
const int quarterTurn = stepsPerRevolution/4;
//const int halfTurn = stepsPerRevolution/2;
const int SPEED = 12;

Stepper right(stepsPerRevolution, 8, 10, 9, 11);
Stepper left (stepsPerRevolution, 2,4,3,5);
Stepper up (stepsPerRevolution, 22,24,23,25);
Stepper down (stepsPerRevolution, 26,28,27,29);
Stepper front (stepsPerRevolution, 30,32,31,33);
Stepper back (stepsPerRevolution, 34,36,35,37);



void setup() {
  right.setSpeed(SPEED);
  left.setSpeed(SPEED);
  up.setSpeed(SPEED);
  down.setSpeed(SPEED);
  front.setSpeed(SPEED);
  back.setSpeed(SPEED);
  // initialize the serial port:
  Serial.begin(9600);
  //Serial.print("READY");
}

void loop() {
  switch (Serial.read()) {
    case 'F':
      front.step(quarterTurn);
      break;
    case 'f':
      front.step(-quarterTurn);
      break;
    case 'B':
      back.step(quarterTurn);
      break;
    case 'b':
      back.step(-quarterTurn);
      break;
    case 'L':
      left.step(quarterTurn);
      break;
    case 'l':
      left.step(-quarterTurn);
      break;
    case 'U':
      up.step(quarterTurn);
      break;
    case 'u':
      up.step(-quarterTurn);
      break;   
    case 'R':
      right.step(quarterTurn);
      break;
    case 'r':
      right.step(-quarterTurn);
      break;
    case 'D':
      down.step(quarterTurn);
      break;
    case 'd':
      down.step(-quarterTurn);
      break; 
    case 'Z':
      Serial.print("DONE");
      break;  
    default:
      break;
  }
}
