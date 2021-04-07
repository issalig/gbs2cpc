// description: GBS8200 configuration to work with CPC RGBS signal.
// author: issalig
//
// credits:
//   original code from https://github.com/pulkomandy/gembascan for LM4F120 board
//   StartWire and StopWire from https://github.com/ramapcsx2/gbs-control
//
// status:
//   It works with ESP8266 Wemos Mini and arduino
//
// play:
// Connect GND, SCL -> D1 (wemos), SDA -> D2 (wemos)

#include <Wire.h>
#include "StartArray.h"
#include "ProgramArray288p.h"

#if defined(__AVR_ATtiny85__)    
#define SCL 7
#define SDA 5
#endif

void scan_i2c() {
  Serial.println("Start I2C scanner ...");
  Serial.print("\r\n");
  byte count = 0;
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found I2C Device: ");
      Serial.print(" (0x");
      Serial.print(i, HEX);
      Serial.println(")");
      count++;
      delay(1);
    }
  }
  Serial.print("\r\n");
  Serial.println("Finish I2C scanner");
  Serial.print("Found ");
  Serial.print(count, HEX);
  Serial.println(" Device(s).");
}

void startWire() {
  Wire.begin();
  // The i2c wire library sets pullup resistors on by default.
  // Disable these to detect/work with GBS onboard pullups

#if defined(__AVR__)    
  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);
#elif defined(ESP8266)
  pinMode(SCL, OUTPUT_OPEN_DRAIN);
  pinMode(SDA, OUTPUT_OPEN_DRAIN);
#endif  
  // no issues even at 700k, requires ESP8266 160Mhz CPU clock, else (80Mhz) uses 400k in library
  // no problem with Si5351 at 700k either
  Wire.setClock(400000);
  //Wire.setClock(700000);
}

void stopWire() {
  pinMode(SCL, INPUT);
  pinMode(SDA, INPUT);
  delayMicroseconds(80);
}


void setup() {
  Serial.begin(115200);
  startWire();
  //scan_i2c();

  Serial.print("Starting GBS8200...\n");
  for (int i = 0; i < sizeof(startArray); i = i + 2)
  {
    Wire.beginTransmission(0x17);
    Wire.write(startArray[i]);
    Wire.write(startArray[i + 1]);
    Wire.endTransmission();
  }

  pal288p();
}

void loop() {
  //nothing for now, all is done in setup
}

int pal288p() {
  Serial.print("Configuring to PAL progressive");

  for (int bank = 0; bank < 6; bank++) {
    // Switch bank command 
    Wire.beginTransmission(0x17);
    Wire.write(0xF0);
    // Param: bank number
    Wire.write(bank);
    Wire.endTransmission();

    for (int i = 0; i < programArray288[bank].size; i++)
    {
      int io = (i + programArray288[bank].offset) & 0xFF;
      // Send them 16 bytes at a time, first giving the register number.
      if ((i % 16) == 0) {
        // Start of a new block
        Wire.beginTransmission(0x17);
        Wire.write(io);     
        Wire.write(programArray288[bank].data[i]);     
      } else if (i % 16 == 15) {
        // End of 16byte block, or end of bank        
        Wire.write(programArray288[bank].data[i]);
        Wire.endTransmission();
      } else {
        Wire.write(programArray288[bank].data[i]);
      }
    }
  }

  return 0;
}

//TODO: test this functions, just a quick and dirty translation

int peek(unsigned long reg)
{

  Wire.beginTransmission(0x17);
  // Switch bank command
  Wire.write(0XF0);
  // Param: bank number  
  Wire.write(reg / 256);
  Wire.endTransmission();
  delay(5);

  // Send register address byte 
  Wire.beginTransmission(0x17);
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(5);

  // Get register value  
  int value;

  Wire.requestFrom(0x17, (uint8_t)1);
  while (Wire.available())
  {
    value = Wire.read();
    Serial.println(); Serial.println(value);
  }


#if defined(__AVR__)  
  Serial.print("reg "); Serial.print(reg);
  Serial.print("value "); Serial.println(value);
#elif defined(ESP8266)
  Serial.printf("Reg %x value %x\n", reg, value);
#endif  

  return 0;
}


int poke( unsigned long reg, unsigned long val )
{
 
  // Switch bank command
  Wire.beginTransmission(0x17);
  Wire.write(0xF0);
 
  // Param: bank number
  Wire.write(reg / 256);
  Wire.endTransmission();
  delay(5);

  // Send register address byte  
  Wire.beginTransmission(0x17);
  Wire.write(0xFF);
  
  // Set register value  
  Wire.write(val&0xFF);
  Wire.endTransmission();

   
#if defined(__AVR__)  
  Serial.print("reg "); Serial.print(reg);
  Serial.print("set to "); Serial.println(val);
#elif defined(ESP8266)
  Serial.printf("Reg %x set to %x\n", reg, val);
#endif  

  
return 0;
}
