#include <Arduino.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "d578.h"
#include "dtmf.h"

SoftwareSerial swSerial(swRX, swTX);

uint8_t bufferTX[8] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
char cmdDTMF[4] = {0x00, 0x00, 0x00, 0x00};

uint16_t sendBeaconTimer = (60 * REPEAT);
boolean enaBeacon   = 1;

unsigned long lastSecond;

String VERSION = "0.3A";

#define INF   0
#define WAR   1
#define ERR   2

void Pressed(uint8_t bp);
void longPressed(uint8_t bp);
void pttPressed(boolean onoff);
void sendKeepAlive(void);

void logs(uint8_t level);
boolean checkSecond(void);

void setup() {
  // Serial Debug
  Serial.begin(dbgSpeed);
  Serial.println();
  Serial.print(F("-- REMOTE D578 - VERSION: "));
  Serial.println(VERSION);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(EOT_BEACON, INPUT);
  pinMode(SOT_BEACON, OUTPUT);

  digitalWrite(SOT_BEACON, LOW);

  // Serial D578
  swSerial.begin(swSPEED);

  // INIT DTMF
  initDTMF();
  Serial.println(F("-- Init.....DONE"));
}

void loop() {
  
  // BEACON
  uint8_t valEOT_BEACON = 0;
  uint8_t valSecond = 0;
  
  // DTMF
  cmdDTMF[0] = 0;
  cmdDTMF[0] = readDTMF();

  if((cmdDTMF[1] == 0) & (cmdDTMF[0] != 0))
  {        
    // Debug
    logs(INF);
    Serial.println(cmdDTMF[0]);

    switch (cmdDTMF[0])
    {
      case '0':
        Pressed(bp0);
        break;
      case '1':
        Pressed(bp1);
        break;    
      case '2':
        Pressed(bp2);
        break;      
      case '3':
        Pressed(bp3);
        break;
      case '4':
        Pressed(bp4);
        break;    
      case '5':
        Pressed(bp5);
        break;      
      case '6':
        Pressed(bp6);
        break;
      case '7':
        Pressed(bp7);
        break;    
      case '8':
        Pressed(bp8);
        break;      
      case '9':
        Pressed(bp9);
        break;
      case 'A':
        Pressed(bpA);
        break;      
      case 'B':
        Pressed(bpB);
        break;
      case 'C':
        Pressed(bpC);
        break;    
      case 'D':
        Pressed(bpD);
        break;      
      case '*':
        //Pressed(bpST);
        cmdDTMF[1] = '*';
        cmdDTMF[0] = 0;
        break;
      case '#':
        //Pressed(bpDH);
        cmdDTMF[1] = '#';
        cmdDTMF[0] = 0;
        break;

      default:
        break;
    }
  }

  if((cmdDTMF[1] != 0) & (cmdDTMF[0] != 0))
  {
    // Debug
    logs(INF);
    Serial.print(cmdDTMF[1]);
    Serial.println(cmdDTMF[0]);
    
    // Long press
    if(cmdDTMF[1] == '*')
    {
      switch (cmdDTMF[0])
      {
        case '0':
          enaBeacon = 0;
          logs(INF);
          digitalWrite(LED, LOW);
          Serial.println("BEACON DISABLE");
          break;   
        case '1':
          enaBeacon = 1;
          logs(INF);
          digitalWrite(LED, HIGH);
          Serial.println("BEACON ENABLE");
          sendBeaconTimer = (60 * REPEAT);
          break;
        case '2':
          Pressed(bpDN);
          break;
        case '3':
          Pressed(bpUP);
          break;            
        case '4':
          Pressed(bpSAB);
          break;          
        case 'A':
          longPressed(bpA);
          break;      
        case 'B':
          longPressed(bpB);
          break;
        case 'C':
          longPressed(bpC);
          break;    
        case 'D':
          longPressed(bpD);
          break;      
      }
    }
   
    cmdDTMF[1] = 0;
  }
  
  // D578 Keep Alive Packet 
  valSecond = checkSecond();

  if(valSecond == 1)
  {
    sendKeepAlive();
    sendBeaconTimer--;
    valSecond = 0;
  }

  // Send beacon
  if((sendBeaconTimer == 0) & (enaBeacon == 1))
  {
    logs(INF);
    Serial.println(F("SOT BEACON"));
    digitalWrite(SOT_BEACON, HIGH);

    delay(4000);
    cmdDTMF[0] = 0;
    cmdDTMF[1] = 0;

    //PTT ON
    pttPressed(1);
    logs(INF);
    Serial.print(F("PTT ON.."));    

    while(valEOT_BEACON == 0)
    {
      // D578 Keep Alive Packet
      valSecond = 0;
      valSecond = checkSecond();
      if(valSecond == 1)
      {
        sendKeepAlive();
        sendBeaconTimer--;
      }
      delay(30);
      valEOT_BEACON = digitalRead(EOT_BEACON); 
    }
    delay(100);

    // PTT OFF
    pttPressed(0);
    Serial.println(F("..PTT OFF"));
    delay(200);

    digitalWrite(SOT_BEACON, LOW);
    logs(INF);
    Serial.println(F("EOT BEACON"));
    sendBeaconTimer = (60 * REPEAT);
  }

  delay(30);
}

void Pressed(uint8_t bp)
{
  bufferTX[4] = bp;
  bufferTX[2] = 0x01;
  swSerial.write(bufferTX, 8);
  delay(200);
  bufferTX[2] = 0x00;
  swSerial.write(bufferTX, 8);
}

void longPressed(uint8_t bp)
{
  bufferTX[4] = bp;
  bufferTX[3] = 0x01;
  bufferTX[2] = 0x01;
  swSerial.write(bufferTX, 8);
  delay(200);
  bufferTX[3] = 0x00;
  bufferTX[2] = 0x00;
  swSerial.write(bufferTX, 8);
}

void pttPressed(boolean onoff)
{
  // Enable PTT
  bufferTX[4] = bpPTT;
  bufferTX[3] = 0;
  bufferTX[2] = 0;    
  
  if(onoff == 1)
  {
    bufferTX[1] = 0x01;    
  }
  else
  {
    bufferTX[1] = 0;
  }
  swSerial.write(bufferTX, 8);   
}

void sendKeepAlive(void)
{
  swSerial.write(0x06);
  // Serial.print('.');
}

void logs(uint8_t level)
{
  switch (level)
  {
  case 0:
    Serial.print(F("I: "));
    break;
  case 1:
    Serial.print(F("W: "));
    break;
  case 2:
    Serial.print(F("E: "));
    break;

  }
}

boolean checkSecond(void)
{
    if ((millis() - lastSecond) >= 1000)
    {
        lastSecond = millis();
        return 1;
    }
    else
    {
        return 0;
    }
}