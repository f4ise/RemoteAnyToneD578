#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "d578.h"
#include "dtmf.h"

SoftwareSerial swSerial(swRX, swTX);

uint8_t bufferTX[8] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
char cmdDTMF[2] = {0x00, 0x00};

void setup() {
  // Serial Debug
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // Serial D578
  swSerial.begin(swSPEED);

  // INIT DTMF
  initDTMF();
  Serial.println(F("-- Init.....DONE --"));
}

void loop() {
  
  cmdDTMF[1] = 0;

  // DTMF
  boolean keepAliveSend = 0;

  cmdDTMF[1] = readDTMF();
  keepAliveSend = keepAlive();

  if((cmdDTMF[0] == 0) & (cmdDTMF[1] != 0))
  {    
    
    // Debug
    Serial.println(cmdDTMF[1]);

    switch (cmdDTMF[1])
    {
      case '0':
        bufferTX[4] = bp0;
        break;
      case '1':
        bufferTX[4] = bp1;
        break;    
      case '2':
        bufferTX[4] = bp2;
        break;      
      case '3':
        bufferTX[4] = bp3;
        break;
      case '4':
        bufferTX[4] = bp4;
        break;    
      case '5':
        bufferTX[4] = bp5;
        break;      
      case '6':
        bufferTX[4] = bp6;
        break;
      case '7':
        bufferTX[4] = bp7;
        break;    
      case '8':
        bufferTX[4] = bp8;
        break;      
      case '9':
        bufferTX[4] = bp9;
        break;
      case 'A':
        bufferTX[4] = bpA;
        break;      
      case 'B':
        bufferTX[4] = bpB;
        break;
      case 'C':
        bufferTX[4] = bpC;
        break;    
      case 'D':
        bufferTX[4] = bpD;
        break;      
      case '*':
        //bufferTX[4] = bpST;
        cmdDTMF[0] = '*';
        cmdDTMF[1] = 0;
        break;
      case '#':
        //bufferTX[4] = bpDH;
        cmdDTMF[0] = '#';
        cmdDTMF[1] = 0;
        break;

      default:
        break;
    }
    if(cmdDTMF[1] != 0)
    {
      bufferTX[2] = 0x01;
      swSerial.write(bufferTX, 8);
      delay(200);
      bufferTX[2] = 0x00;
      swSerial.write(bufferTX, 8);
    }
  }

  if((cmdDTMF[0] != 0) & (cmdDTMF[1] != 0))
  {
    // Debug
    Serial.print(cmdDTMF[0]);
    Serial.println(cmdDTMF[1]);
    
    // Long press
    if(cmdDTMF[0] == '*')
    {
      switch (cmdDTMF[1])
      {
        case 'A':
          bufferTX[4] = bpA;
          break;      
        case 'B':
          bufferTX[4] = bpB;
          break;
        case 'C':
          bufferTX[4] = bpC;
          break;    
        case 'D':
          bufferTX[4] = bpD;
          break;      
      }
      
      bufferTX[3] = 0x01;
      bufferTX[2] = 0x01;
      swSerial.write(bufferTX, 8);
      delay(200);
      bufferTX[3] = 0x00;
      bufferTX[2] = 0x00;
      swSerial.write(bufferTX, 8);
    }
   
    cmdDTMF[0] = 0;
  }
  
  cmdDTMF[1] = 0;
  delay(30);

  // D578 Keep Alive Packet 
  if(keepAliveSend == 1)
  {
    swSerial.write(0x06);
  }
  //
}
