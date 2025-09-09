#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "d578.h"
#include "dtmf.h"
#include "morse.h"

SoftwareSerial swSerial(swRX, swTX);

uint8_t bufferTX[8] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
char cmdDTMF[4] = {0x00, 0x00, 0x00, 0x00};

uint16_t sendBeacon = REPEAT;

String VERSION = "0.2A";

void setup() {
  // Serial Debug
  Serial.begin(dbgSpeed);
  Serial.println();
  Serial.print(F("-- REMOTE D578 - VERSION: "));
  Serial.println(VERSION);

  pinMode(LED, OUTPUT);

  // Serial D578
  swSerial.begin(swSPEED);

  // INIT DTMF
  initDTMF();
  Serial.println(F("-- Init.....DONE"));
}

void loop() {
  
  // DTMF
  boolean keepAliveSend = 0;
  cmdDTMF[0] = 0;

  cmdDTMF[0] = readDTMF();
  keepAliveSend = keepAlive();

  if((cmdDTMF[1] == 0) & (cmdDTMF[0] != 0))
  {    
    
    // Debug
    Serial.print(F("I: "));
    Serial.println(cmdDTMF[0]);

    switch (cmdDTMF[0])
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
        //bufferTX[4]sendBeacon = bpST;
        cmdDTMF[1] = '*';
        cmdDTMF[0] = 0;
        break;
      case '#':
        //bufferTX[4] = bpDH;
        cmdDTMF[1] = '#';
        cmdDTMF[0] = 0;
        break;

      default:
        break;
    }
    if(cmdDTMF[0] != 0)
    {
      bufferTX[2] = 0x01;
      swSerial.write(bufferTX, 8);
      delay(200);
      bufferTX[2] = 0x00;
      swSerial.write(bufferTX, 8);
    }
  }

  if((cmdDTMF[1] != 0) & (cmdDTMF[0] != 0))
  {
    // Debug
    Serial.print(F("I: "));
    Serial.print(cmdDTMF[1]);
    Serial.println(cmdDTMF[0]);
    
    // Long press
    if(cmdDTMF[1] == '*')
    {
      switch (cmdDTMF[0])
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
   
    cmdDTMF[1] = 0;
  }
  
  // D578 Keep Alive Packet 
  if(keepAliveSend == 1)
  {
    swSerial.write(0x06);
    // sendBeacon--;
  }

  // Send beacon
  // if(sendBeacon == 0)
  // {
  //   detachInterrupt(digitalPinToInterrupt(STQ));
  //   cmdDTMF[0] = 0;
  //   cmdDTMF[1] = 0;

  //   bufferTX[4] = bpPTT;
  //   //bufferTX[4] = 0x1F;
  //   bufferTX[3] = 0;
  //   bufferTX[2] = 0;    
  //   bufferTX[1] = 0x01;
  //   swSerial.write(bufferTX, 8);   
  //   Serial.print(F("PTT ON "));     
    
  //   for(uint8_t i = 0; i <= 5; i++)
  //   {
  //     swSerial.write(0x06);
  //     tone(CW_PIN, TONE_CW, 1000);
  //     noTone(CW_PIN);
  //     delay(500);
  //   }

  //   bufferTX[1] = 0;
  //   swSerial.write(bufferTX, 8);    
  //   Serial.println(F("PTT OFF"));
  //   delay(200);

  //   sendBeacon = REPEAT;
  //   attachInterrupt(digitalPinToInterrupt(STQ), STQ_ISR, FALLING);
  // }

  delay(30);
}

