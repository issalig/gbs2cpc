// description: GBS8200 configuration to work with CPC RGBS signal.
// author: issalig
//
// credits:
//   original code from https://github.com/pulkomandy/gembascan for LM4F120 board
//   StartWire and StopWire from https://github.com/ramapcsx2/gbs-control
//
// status:
//   It works with ESP8266 Wemos Mini
//
// play:
// Connect GND, SCL -> D1 (wemos), SDA -> D2 (wemos)

#include <Wire.h>
#include "StartArray.h"
#include "ProgramArray288p.h"

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
  pinMode(SCL, OUTPUT_OPEN_DRAIN);
  pinMode(SDA, OUTPUT_OPEN_DRAIN);
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


  Serial.begin(115200); // Arduino IDE Serial Monitor requires the same 115200 bauds!

  startWire();
  scan_i2c();


  //preprare I2C
  //I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), false);

  Serial.print("Starting GBS8200...\n");

  //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, 0x17, false);
  for (int i = 0; i < sizeof(startArray); i = i + 2)
  {
    //I2CMasterDataPut(I2C0_MASTER_BASE, startArray[i]);
    //I2CMasterControl(I2C0_MASTER_BASE,
    //                 (i & 1) ? I2C_MASTER_CMD_BURST_SEND_FINISH : I2C_MASTER_CMD_BURST_SEND_START);
    //while (I2CMasterBusy(I2C0_MASTER_BASE));
    Wire.beginTransmission(0x17);
    Wire.write(startArray[i]);
    Wire.write(startArray[i + 1]);
    Wire.endTransmission();
  }

  Serial.print("Set 288p");
  pal288p();

}
void loop() {
}

int pal288p() {
  Serial.print("Configuring to PAL progressive");

  //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, 0x17, false);
  for (int bank = 0; bank < 6; bank++) {
    // Switch bank command
    //I2CMasterDataPut(I2C0_MASTER_BASE, 0xF0);
    //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    Wire.beginTransmission(0x17);

    Wire.write(0xF0);
    //while(I2CMasterBusy(I2C0_MASTER_BASE));
    // Param: bank number
    //I2CMasterDataPut(I2C0_MASTER_BASE, bank);
    //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    //while(I2CMasterBusy(I2C0_MASTER_BASE));
    Wire.write(bank);
    Wire.endTransmission();

    for (int i = 0; i < programArray288[bank].size; i++)
    {
      int io = (i + programArray288[bank].offset) & 0xFF;
      // Send them 16 bytes at a time, first giving the register number.
      if ((i % 16) == 0) {
        // Start of a new block
        //I2CMasterDataPut(I2C0_MASTER_BASE, io);
        //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        //while(I2CMasterBusy(I2C0_MASTER_BASE));
        Wire.beginTransmission(0x17);
        Wire.write(io);
        //I2CMasterDataPut(I2C0_MASTER_BASE, programArray288[bank].data[i]);
        //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        //while(I2CMasterBusy(I2C0_MASTER_BASE));
        Wire.write(programArray288[bank].data[i]);
        //Wire.endTransmission();


      } else if (i % 16 == 15) {
        // End of 16byte block, or end of bank
        //I2CMasterDataPut(I2C0_MASTER_BASE, programArray288[bank].data[i]);
        //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        //while(I2CMasterBusy(I2C0_MASTER_BASE));
        //Wire.beginTransmission(0x17);
        Wire.write(programArray288[bank].data[i]);
        Wire.endTransmission();
      } else {
        //I2CMasterDataPut(I2C0_MASTER_BASE, programArray288[bank].data[i]);
        //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        //while(I2CMasterBusy(I2C0_MASTER_BASE));
        //Wire.beginTransmission(0x17);
        Wire.write(programArray288[bank].data[i]);
        //Wire.endTransmission();

      }
    }
  }

  return 0;
}

//TODO: test this functions, just a quick and dirty translation

int peek(unsigned long reg)
{

  //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, 0x17, false);
  Wire.beginTransmission(0x17);

  // Switch bank command
  //I2CMasterDataPut(I2C0_MASTER_BASE, 0xF0);
  //    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  //    while(I2CMasterBusy(I2C0_MASTER_BASE));
  Wire.write(0XF0);

  // Param: bank number
  //I2CMasterDataPut(I2C0_MASTER_BASE, reg / 256);
  //    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  //    while(I2CMasterBusy(I2C0_MASTER_BASE));
  //      SysCtlDelay(SysCtlClockGet() / (1000 / 3));

  Wire.write(reg / 256);
  Wire.endTransmission();
  delay(5);

  // Send register address byte
  //I2CMasterDataPut(I2C0_MASTER_BASE, reg & 0xFF);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));
  //SysCtlDelay(SysCtlClockGet() / (1000 / 3));

  Wire.beginTransmission(0x17);
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(5);

  // Get register value
  //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, 0x17, true);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));

  //int value = I2CMasterDataGet(I2C0_MASTER_BASE);
  int value;


  Wire.requestFrom(0x17, (uint8_t)1);
  while (Wire.available())
  {
    value = Wire.read();
    Serial.println(); Serial.println(value);
  }
  Serial.printf("Reg %x value %x\n", reg, value);


  return 0;
}


int poke( unsigned long reg, unsigned long val )
{
 
  //I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, 0x17, false);
  // Switch bank command
  //I2CMasterDataPut(I2C0_MASTER_BASE, 0xF0);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));
  Wire.beginTransmission(0x17);
  Wire.write(0xF0);
 
  // Param: bank number
  //I2CMasterDataPut(I2C0_MASTER_BASE, reg / 256);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));
  //SysCtlDelay(SysCtlClockGet() / (1000 / 3));
 
  Wire.write(reg / 256);
  Wire.endTransmission();
  delay(5);

  // Send register address byte
  //I2CMasterDataPut(I2C0_MASTER_BASE, reg & 0xFF);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));

  Wire.beginTransmission(0x17);
  Wire.write(0xFF);
  
  // Set register value
  //I2CMasterDataPut(I2C0_MASTER_BASE, val & 0xFF);
  //I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
  //while (I2CMasterBusy(I2C0_MASTER_BASE));
  
  Wire.write(0xFF);
  Wire.endTransmission();

  Serial.printf("Reg %x set to %x\n", reg, val);


return 0;
}
