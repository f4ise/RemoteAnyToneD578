#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "d578.h"
#include "dtmf.h"

SoftwareSerial swSerial(swRX, swTX);

uint8_t bufferTX[8] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};

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
  // DTMF
  char valDTMF = 0;
  boolean keepAliveSend = 0;

  valDTMF = readDTMF();
  keepAliveSend = keepAlive();

  if(valDTMF != 0)
  {    

    switch (valDTMF)
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
        bufferTX[4] = bpST;
        break;
      case '#':
        bufferTX[4] = bpDH;
        break;

      default:
        break;
    }
    
    bufferTX[2] = 0x01;
    swSerial.write(bufferTX, 8);
    delay(200);
    bufferTX[2] = 0x00;
    swSerial.write(bufferTX, 8);
  }
  delay(30);

  // D578  
  if(keepAliveSend == 1)
  {
    swSerial.write(0x06);
  }
  //
}
