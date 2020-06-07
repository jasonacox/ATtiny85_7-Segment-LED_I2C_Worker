/*
  ATtiny85 7-Segment LED Display I2C Worker

  Author: Jason A. Cox - @jasonacox

  Date: 6 June 2020

  Components:
      ATiny85 Microcontroller
      74HC595 8-bit Shift Register (Qty 4)
      7-Segement LED Display (Qty 4)
      0.1uF Ceramic Capacitor (Qty 2)
      100uF Electrolytic Capacitor
      5V Power Supply

  Requirement: This sketch requires a version of the Wire library that works with the ATtiny85, e.g.
      ATTinyCore by Spence Konde board manager URL http://drazzy.com/package_drazzy.com_index.json
      ATtiny85 chip at 1Mhz (internal)

  Programming Notes:
      I2C communcation uses PB0/SDA and PB2/SCL. If you use the Tiny AVR Programmer from Sparkfun
      it drives an LED on PB0 and will interfear with I2C communcation. You will need to remove
      the chip from the programmer after uploading to get it to work in the circuit.

*/

/* Includes */
#include <Wire.h>

/* ATtiny85 Pins */
#define SDApin 0      // Pin connected to SDA - Input
#define SCLpin 2      // Pin connected to SCL - Input
#define dataPin 1     // Pin connected to DS    of 74HC595
#define latchPin 3    // Pin connected to ST_CP of 74HC595
#define clockPin 4    // Pin connected to SH_CP of 74HC595

/* Global variables */
byte state = 0;       // State flag to trigger units change

/* Set up 7-segment LED Binary Data

    |--A--|
    F     B
    |--G--|
    E     C
    |--D--|
           H - Decimal

    0b00000000
      ABCDEFGH
*/
static byte numArray[] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11110110, // 9
  0b11111101, // 0. (Decimal point)
  0b01100001, // 1.
  0b11011011, // 2.
  0b11110011, // 3.
  0b01100111, // 4.
  0b10110111, // 5.
  0b10111111, // 6.
  0b11100001, // 7.
  0b11111111, // 8.
  0b11110111, // 9.
  0b00000001, // . (Decimal)
  0b11000110, // . (Degree Mark)
  0b00000000, // blank           (index 22)
  0b00001010, // r               (index 23)
  0b00101110, // h               (index 24)
  0b00010000, // falling v       (index 25)
  0b10000000, // rising  ^       (index 26)
  0b00000010, // negative   -    (index 27)
  0b01101110  // H               (index 28)
}; // All Off

/*
   SETUP
*/
void setup() {

  // Set up pins for driving the 74HC595 shift registers / LEDs
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  // Set up I2C
  Wire.begin(4);                // Join i2c bus with address #4
  Wire.onReceive(receiveEvent); // Register function to process incoming events

  // Set display with startup image
  digitalWrite(latchPin, 0);
  sendOut(0b10010000);
  sendOut(0b10010000);
  sendOut(0b10010000);
  sendOut(0b10010000);
  digitalWrite(latchPin, 1);

  delay(200);
}

/*
   I2C Receive
*/
void receiveEvent(int howMany)
{
  // I2C Master has sent us a payload of bytes to display
  digitalWrite(latchPin, 0);
  while (1 < Wire.available()) // loop through all but the last
  {
    byte c = (byte)Wire.read(); // receive byte as a character
    sendOut(c);
  }
  byte x = (byte)Wire.read();    // receive byte as an integer
  sendOut(x);
  digitalWrite(latchPin, 1);
}

/*
    MAIN LOOP
*/
void loop() {
  delay(1000);
}

/*
   Display - Send myDataOut to 8 bit register to drive LEDs
*/
void sendOut(byte myDataOut) {
  int i = 0;
  int pinState;

  // This sifts 8 bits out MSB first on the rising edge of the clock
  // Clear data and clock output
  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);

  // Send each bit in the byte myDataOut
  for (i = 7; i >= 0; i--)  {
    digitalWrite(clockPin, 0);
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }
    // Send pinState
    digitalWrite(dataPin, pinState);
    // Shift register with Clock
    digitalWrite(clockPin, 1);
    // Zero out data pin
    digitalWrite(dataPin, 0);
  }
  digitalWrite(clockPin, 0);
}
