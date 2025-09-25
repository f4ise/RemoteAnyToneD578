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

String VERSION = "0.4A";

#define INF   0
#define WAR   1
#define ERR   2

void sendBeacon(void);
void sendAM(void);
void sendAck(void);

void Pressed(uint8_t bp);
void longPressed(uint8_t bp);
void pttPressed(boolean onoff);
void sendKeepAlive(void);

void taskDTMF(void);

void logs(uint8_t level);
boolean checkSecond(void);

void setup() {
  // Serial Debug
  Serial.begin(dbgSpeed);
  Serial.println();
  Serial.print(F("-- REMOTE D578 - VERSION: "));
  Serial.println(VERSION);

  // Config IO GEN CW
  pinMode(EOT_CW, INPUT);
  pinMode(SOT_CW, OUTPUT);
  pinMode(MSG0_CW, OUTPUT);
  pinMode(MSG1_CW, OUTPUT);
  // Init IO GEN CW
  digitalWrite(SOT_CW, LOW);
  digitalWrite(MSG0_CW, LOW);
  digitalWrite(MSG1_CW, LOW);

  // Config IO Misc
  pinMode(LED, OUTPUT);
  pinMode(CMDPTT, OUTPUT);
  pinMode(PRES12V, INPUT);
  pinMode(SW_CW, OUTPUT);
  pinMode(SW_AM, OUTPUT);
  // Init IO Misc  
  digitalWrite(LED, HIGH);
  digitalWrite(CMDPTT, LOW);
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, LOW);

  // Serial D578
  swSerial.begin(swSPEED);

  // INIT DTMF
  initDTMF();
  Serial.println(F("-- Init.....DONE"));
}

void loop() {

  
  if(digitalRead(PRES12V) == 0)
  {
    // Presence 12V
    digitalWrite(LED, HIGH);
  }
  else
  {
    // Absence 12V
    digitalWrite(LED, LOW);
  }

  taskDTMF();

  // D578 Keep Alive Packet 
  if(checkSecond() == 1)
  {
    sendKeepAlive();
    sendBeaconTimer--;
  }

  sendBeacon();

  delay(30);
}

void taskDTMF(void)
{
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
        sendAck();
        break;      
      case 'B':
        Pressed(bpB);
        sendAck();
        break;
      case 'C':
        Pressed(bpC);
        sendAck();
        break;    
      case 'D':
        Pressed(bpD);
        sendAck();
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
          //digitalWrite(LED, LOW);
          Serial.println("BEACON DISABLE");
          sendAck();
          break;   
        case '1':
          enaBeacon = 1;
          logs(INF);
          //digitalWrite(LED, HIGH);
          Serial.println("BEACON ENABLE");
          sendBeaconTimer = (60 * REPEAT);
          sendAck();
          break;
        case '2':
          Pressed(bpDN);
          sendAck();
          break;
        case '3':
          Pressed(bpUP);
          sendAck();
          break;            
        case '4':
          Pressed(bpSAB);
          sendAck();
          break;          
        case 'A':
          longPressed(bpA);
          sendAck();
          break;      
        case 'B':
          longPressed(bpB);
          delay(500);
          longPressed(bpB);
          delay(500);
          sendAck();
          delay(100);
          sendAM();
          //longPressed(bpB);
          //delay(500);
          longPressed(bpB);
          delay(500);
          longPressed(bpB);
          break;
        case 'C':
          longPressed(bpC);
          sendAck();
          break;    
        case 'D':
          longPressed(bpD);
          sendAck();
          break;      
      }
    }
   
    cmdDTMF[1] = 0;
  }
  
}

void sendBeacon(void)
{
  // Send beacon
  if((sendBeaconTimer == 0) & (enaBeacon == 1))
  {
    logs(INF);
    Serial.println(F("SOT BEACON"));

    cmdDTMF[0] = 0;
    cmdDTMF[1] = 0;

    //PTT ON
    pttPressed(1);
    logs(INF);
    delay(1000);
    Serial.print(F("PTT ON.."));
    digitalWrite(MSG0_CW, HIGH);
    digitalWrite(SOT_CW, HIGH);

    while(digitalRead(EOT_CW) == 0)
    {
      // D578 Keep Alive Packet
      if(checkSecond() == 1)
      {
        sendKeepAlive();
      }
      delay(30);
    }

    // PTT OFF
    digitalWrite(SOT_CW, LOW);
    pttPressed(0);
    Serial.println(F("..PTT OFF"));
    logs(INF);
    Serial.println(F("EOT BEACON"));
    sendBeaconTimer = (60 * REPEAT);
  }
}

void sendAM(void)
{ 
  uint8_t duration = 0;
  // Send AUDIO AM
  logs(INF);
  Serial.println(F("Send AM"));

  //PTT ON TELCO

  logs(INF);
  Serial.print(F("PTT ON..."));
  digitalWrite(CMDPTT, HIGH);
  delay(500);
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, HIGH);
  delay(100);

  for(duration = 0; duration < (DURATION_AM * 2); duration++)
  {
    // D578 Keep Alive Packet
    if(checkSecond() == 1)
    {
      sendKeepAlive();
    }
    delay(500);
  }

  // PTT OFF
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, LOW);
  digitalWrite(CMDPTT, LOW);
  Serial.println(F("...PTT OFF"));
  logs(INF);
  Serial.println(F("EOT AM"));
}

void sendAck(void)
{
  // Send beacon
  logs(INF);
  Serial.println(F("Send ACK"));

  //PTT ON TELCO

  logs(INF);
  Serial.print(F("PTT ON..."));
  digitalWrite(CMDPTT, HIGH);
  delay(500);
  digitalWrite(SW_CW, HIGH);
  digitalWrite(SW_AM, LOW);
  delay(100);
  digitalWrite(MSG0_CW, LOW);
  digitalWrite(SOT_CW, HIGH);

  while(digitalRead(EOT_CW) == 0)
  {
    // D578 Keep Alive Packet
    if(checkSecond() == 1)
    {
      sendKeepAlive();
    }
    delay(30);
  }

  // PTT OFF
  digitalWrite(SOT_CW, LOW);
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, LOW);
  digitalWrite(CMDPTT, LOW);
  Serial.println(F("...PTT OFF"));
  logs(INF);
  Serial.println(F("EOT ACK"));
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