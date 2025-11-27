/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required
to interface



based on MPR121test
modified & extended by Stefan Krüger


**********************************************************/
#include <Adafruit_MPR121.h>
#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();

const uint8_t channelFirst = 0;
const uint8_t channelLast = 11; // max 13
uint16_t currTouched;

/*void plotValues()
{
  currTouched = cap.touched();

  for (int chan = channelFirst; chan <= channelLast; chan++)
  {
    if (currTouched & _BV(chan))
    {
      Serial.print("Channel ");
      Serial.print(chan);
      Serial.println(" active");
    }
  }
}*/
void setup_touch()
{
  delay(1000);
  //   Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("MPR121 Autoconfiguration Test. (MPR121-AutoConfig.ino)");
  Wire.begin();
  if (!cap.begin(0x5A, &Wire))
  {
    Serial.println("MPR121 not found, check wiring?");
    while (1)
      ;
  }
  Serial.println("MPR121 found!");

  cap.setAutoconfig(true);
}

bool isAnyChannelTouched()
{
  currTouched = cap.touched();

  for (int chan = channelFirst; chan <= channelLast; chan++)
  {
    if (currTouched & _BV(chan))
    {
      return true; // at least one channel touched
    }
  }
  return false;
}
/*Serial.print("Channel ");
Serial.print(chan);
Serial.println(" active");*/

/*void screenIn()
{
  // read the state of the the input pin:
  lastState = digitalRead(currTouched);

  if (lastState == LOW && currTouched == HIGH)
    Serial.println("The sensor is touched");

  else if (lastState == HIGH && currTouched == LOW)
    Serial.println("The sensor is is released");

  // save the the last state
  lastState = currTouched;
}*/
/*void loop()
{
  plotValues();
  delay(10);
}*/