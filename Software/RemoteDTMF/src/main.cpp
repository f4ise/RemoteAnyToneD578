#include <Arduino.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include <ErriezSerialTerminal.h>
#include <SparkFunBME280.h>
#include <DS3231.h>
#include <PCF8575.h>

#include "config.h"
#include "d578.h"
#include "dtmf.h"

// RTC Clock
DS3231 rtc;

// Temperature Sensor
BME280 SensorInt;

// PIO Extension
PCF8575 pio(0x20);

// DTMF
void taskDTMF(void);

uint8_t bufferTX[8] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
char cmdDTMF[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t dtmfTimer = 0;

// CW/AM
void sendBeacon(void);
void sendAM(void);
void sendAck(char ack);

uint8_t repeatBeacon;
uint16_t sendBeaconTimer;
boolean enaBeacon = 0;

// AT-D578
SoftwareSerial swSerial(swRX, swTX);

char Pressed(uint8_t bp);
char longPressed(uint8_t bp);
char pttPressed(boolean onoff);
void sendKeepAlive(void);

// Terminal
SerialTerminal term('\n', ' ');

void cmdHelp();
void cmdGet();
void cmdSet();
void cmdTemp();
void cmdVersion();
void cmdSave();
void cmdUnknown(const char *command);
void printConsoleChar(void);

// Misc
void logs(uint8_t level);
void checkSecond(void);
void getDateStuff(byte& year, byte& month, byte& date, byte& dOW, byte& hour, byte& minute, byte& second);

unsigned long lastSecond;

// --  PROGRAM  ----------------------------------------------------------
void setup() {
  
  int addr = 0;

  // Serial Debug
  Serial.begin(dbgSpeed);

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
  pinMode(PRES12V1, INPUT);
  //pinMode(PRES12V2, INPUT);     // TBD IO
  pinMode(LED, OUTPUT);
  pinMode(CMDPTT, OUTPUT);
  pinMode(SW_CW, OUTPUT);
  pinMode(SW_AM, OUTPUT);
  // Init IO Misc  
  digitalWrite(LED, HIGH);
  digitalWrite(CMDPTT, LOW);
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, LOW);

  // Init I2C
  Wire.begin();

  // Init BME280
  SensorInt.setI2CAddress(0x76);
  if(SensorInt.beginI2C() == false)
  {
    Serial.println(F("Sensor Int connect failed"));
  }    

  // Init PIO
  pio.begin();

  // Init EEPROM
  EEPROM.begin();  //Initialize EEPROM
  enaBeacon = EEPROM.read(addr);
  addr++;
  repeatBeacon = EEPROM.read(addr);
  
  // Serial D578
  swSerial.begin(swSPEED);

  // INIT DTMF
  initDTMF();
  
  // Init Beacon
  sendBeaconTimer = (60 * repeatBeacon);

  // Init Serial Terminal
  Serial.println();
  Serial.println(F("-- Remote DTMF AT-D578 --"));
  Serial.println(F("type '?' to display usage."));
  Serial.println();
  printConsoleChar();
  term.setDefaultHandler(cmdUnknown);
  term.addCommand("?", cmdHelp);
  term.addCommand("get", cmdGet);
  term.addCommand("set", cmdSet);
  term.addCommand("temp", cmdTemp);
  term.addCommand("sav", cmdSave);
  term.addCommand("v", cmdVersion);
  term.setSerialEcho(true);
  term.setPostCommandHandler(printConsoleChar);
}

void loop() {
  
  taskDTMF();

  sendBeacon();

  checkSecond();  

  term.readSerial();

  delay(10);
}

// --  DTMF  -------------------------------------------------------------
void taskDTMF(void)
{
  // DTMF
  cmdDTMF[0] = 0;
  cmdDTMF[0] = readDTMF();
  
  if((cmdDTMF[1] == 0) & (cmdDTMF[0] != 0))
  {        
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
        sendAck(Pressed(bpA));
        break;      
      case 'B':
        sendAck(Pressed(bpB));
        break;
      case 'C':
        sendAck(Pressed(bpC));
        break;    
      case 'D':
        sendAck(Pressed(bpD));
        break;      
      case '*':
        //Pressed(bpST);
        cmdDTMF[1] = '*';
        cmdDTMF[0] = 0;
        dtmfTimer = timeoutDTMF;
        break;
      case '#':
        //Pressed(bpDH);
        cmdDTMF[1] = '#';
        cmdDTMF[0] = 0;
        dtmfTimer = timeoutDTMF;
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
          digitalWrite(LED, LOW);
          logs(INF);
          Serial.println("BEACON DISABLE");
          sendAck('K');
          break;   
        case '1':
          enaBeacon = 1;
          digitalWrite(LED, HIGH);
          logs(INF);
          Serial.println("BEACON ENABLE");
          sendBeaconTimer = (60 * repeatBeacon);
          sendAck('K');
          break;
        case '2':
          sendAck(Pressed(bpDN));
          break;
        case '3':
          sendAck(Pressed(bpUP));
          break;            
        case '4':
          sendAck(Pressed(bpSAB));
          break;          
        case 'A':
          sendAck(longPressed(bpA));
          break;      
        case 'B':
          longPressed(bpB);
          delay(200);
          sendAck(longPressed(bpB));
          delay(200);
          sendAM();
          //longPressed(bpB);
          //delay(200);
          longPressed(bpB);
          delay(200);
          sendAck(longPressed(bpB));
          break;
        case 'C':
          sendAck(longPressed(bpC));
          break;    
        case 'D':
          sendAck(longPressed(bpD));
          break;      
      }
    }
   
    cmdDTMF[1] = 0;
  }
  
  // Timeout DTMF for no reception after * or #
  if((cmdDTMF[1] != 0) & (cmdDTMF[0] == 0) & (dtmfTimer == 0))
  {
    cmdDTMF[1] = 0;
    logs(WAR);
    Serial.println(F("Timeout receive DTMF"));
  }

  delay(30);
}

// --  AUDIO CW/AM  ------------------------------------------------------
void sendBeacon(void)
{
  // Send beacon
  if((sendBeaconTimer == 0) & (enaBeacon == 1))
  {
    logs(INF);
    Serial.println(F("Send Beacon"));

    cmdDTMF[0] = 0;
    cmdDTMF[1] = 0;

    //PTT ON
    pttPressed(1);
    delay(1000);
    // logs(INF);
    // Serial.print(F("PTT ON.."));
    digitalWrite(MSG0_CW, HIGH);
    digitalWrite(SOT_CW, HIGH);

    while(digitalRead(EOT_CW) == 0)
    {
      checkSecond();
      delay(30);
    }

    // PTT OFF
    digitalWrite(SOT_CW, LOW);
    pttPressed(0);
    // Serial.println(F("..PTT OFF"));
    // logs(INF);
    // Serial.println(F("EOT BEACON"));
    sendBeaconTimer = (60 * repeatBeacon);
  }
}

void sendAM(void)
{ 
  uint8_t duration = 0;
  // Send AUDIO AM
  logs(INF);
  Serial.println(F("Send AM"));

  //PTT ON TELCO
  // logs(INF);
  // Serial.print(F("PTT ON..."));
  digitalWrite(CMDPTT, HIGH);
  delay(500);
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, HIGH);
  delay(100);

  for(duration = 0; duration < (DURATION_AM * 2); duration++)
  {
    checkSecond();
    delay(500);
  }

  // PTT OFF
  digitalWrite(SW_CW, LOW);
  digitalWrite(SW_AM, LOW);
  digitalWrite(CMDPTT, LOW);
  // Serial.println(F("...PTT OFF"));
  // logs(INF);
  // Serial.println(F("EOT AM"));
}

void sendAck(char ack)
{
  if((ack == 0xaa) | (ack == 'K'))
  {
    // Send beacon
    logs(INF);
    Serial.println(F("Send ACK"));

    //PTT ON TELCO
    // logs(INF);
    // Serial.print(F("PTT ON..."));
    digitalWrite(CMDPTT, HIGH);
    delay(800);
    digitalWrite(SW_CW, HIGH);
    digitalWrite(SW_AM, LOW);
    delay(100);
    digitalWrite(MSG0_CW, LOW);
    digitalWrite(SOT_CW, HIGH);

    while(digitalRead(EOT_CW) == 0)
    {
      checkSecond();
      delay(30);
    }

    // PTT OFF
    digitalWrite(SOT_CW, LOW);
    digitalWrite(SW_CW, LOW);
    digitalWrite(SW_AM, LOW);
    digitalWrite(CMDPTT, LOW);
    // Serial.println(F("...PTT OFF"));
    // logs(INF);
    // Serial.println(F("EOT ACK"));
  }
  else
  {
    Serial.print(F("Valeur ACK: "));
    Serial.println(ack, HEX);
  }
}

// --  COMMAND D578  -----------------------------------------------------
char Pressed(uint8_t bp)
{
  char ackD578 = 0;
  
  bufferTX[4] = bp;
  bufferTX[2] = 0x01;
  swSerial.write(bufferTX, 8);  
  delay(200);  
  bufferTX[2] = 0x00;
  swSerial.write(bufferTX, 8);
  
  if(swSerial.available() > 0)
  {
    ackD578 = swSerial.read();
  }
  
  Serial.print(F("Val ack D578: "));
  Serial.println(ackD578, HEX);

  if(ackD578 == 0xaa)
  {
    return ackD578;
  }
  else
  {
    return 0;
  }  
}

char longPressed(uint8_t bp)
{
  char ackD578 = 0;
  
  bufferTX[4] = bp;
  bufferTX[3] = 0x01;
  bufferTX[2] = 0x01;
  swSerial.write(bufferTX, 8);
  delay(200);  
  bufferTX[3] = 0x00;
  bufferTX[2] = 0x00;
  swSerial.write(bufferTX, 8);
  
  if(swSerial.available() > 0)
  {
    ackD578 = swSerial.read();
  }
  
  Serial.print(F("Val ack D578: "));
  Serial.println(ackD578, HEX);

  if(ackD578 == 0xaa)
  {
    return ackD578;
  }
  else
  {
    return 0;
  }  
}

char pttPressed(boolean onoff)
{
  char ackD578 = 0;
  
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
  
  if(swSerial.available() > 0)
  {
    ackD578 = swSerial.read();
  }
  
  Serial.print(F("Val ack D578: "));
  Serial.println(ackD578, HEX);

  if(ackD578 == 0xaa)
  {
    return ackD578;
  }
  else
  {
    return 0;
  }  
}

void sendKeepAlive(void)
{
  swSerial.write(0x06);
}

// --  COMMAND TERMINAL  -------------------------------------------------
void cmdHelp()
{
    // Print usage
    Serial.println(F("Serial terminal usage:"));
    Serial.println(F("  ?                   Print this help"));
    Serial.println(F("  get                 Get Parameters"));
    Serial.println(F("  set <eb> <rb> <dt>  Set Parameters"));
    Serial.println(F("  temp                Get Temperature"));
    Serial.println(F("  sav                 Save Parameters"));
    Serial.println(F("  v                   Print Version"));
}

void cmdGet()
{
	bool CenturyBit = false;
  bool h12Flag;
  bool pmFlag;

  Serial.print(F("Enable Beacon: "));
  Serial.println(enaBeacon);
  Serial.print(F("Repeat Beacon: "));
  Serial.println(repeatBeacon);

  // Get date
	Serial.print(rtc.getDate(), DEC);
	Serial.print("/");
	Serial.print(rtc.getMonth(CenturyBit), DEC);
	Serial.print("/");
  Serial.print(rtc.getYear(), DEC);
	Serial.print(' ');
	
  // Get Time
	Serial.print(rtc.getHour(h12Flag, pmFlag), DEC);
	Serial.print(":");
	Serial.print(rtc.getMinute(), DEC);
	Serial.print(":");
	Serial.print(rtc.getSecond(), DEC);
 
	// Add AM/PM indicator
	if (h12Flag) {
		if (pmFlag) {
			Serial.println(F(" PM"));
		} else {
			Serial.println(F(" AM"));
		}
	} else {
		Serial.println(F(" 24H"));
	}
}

void cmdSet()
{
  int val;
  char *arg;

  byte year;
  byte month;
  byte date;
  byte dOW;
  byte hour;
  byte minute;
  byte second;

  arg = term.getNext();
  if (arg == NULL)
  {
    Serial.println(F("DEC value not specified."));
    return;
  }
  else
  {
    if (sscanf(arg, "%d", &val) != 1)
    {
      Serial.println(F("Cannot convert DEC value."));
      return;
    }
  }
  enaBeacon = val;
  Serial.print(F("Set Enable Beacon: "));
  Serial.println(enaBeacon, DEC);
    
  arg = term.getNext();
  if (arg == NULL)
  {
    Serial.println(F("DEC value not specified."));
    return;
  }
  else
  {
    if (sscanf(arg, "%d", &val) != 1)
    {
      Serial.println(F("Cannot convert DEC value."));
      return;
    }
  }
  repeatBeacon = val;
  sendBeaconTimer = (60 * repeatBeacon);
  Serial.print(F("Set Repeat Beacon: "));
  Serial.println(repeatBeacon, DEC);

  arg = term.getNext();
  if (arg == NULL)
  {
    Serial.println(F("Date & Time not specified."));
    return;
  }
  else
  {
    // TODO : Finaliser la mise a jour de l'heure / date et AL1 AL2 pour Demmarage / Arret relays
    getDateStuff(year, month, date, dOW, hour, minute, second);
        
    rtc.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h
        
    rtc.setYear(year);
    rtc.setMonth(month);
    rtc.setDate(date);
    rtc.setDoW(dOW);
    rtc.setHour(hour);
    rtc.setMinute(minute);
    rtc.setSecond(second);
  }

}

void cmdSave()
{
  int addr = 0;  

  // write to EEPROM.
  EEPROM.write(addr, enaBeacon); 
  addr++;
  EEPROM.write(addr, repeatBeacon);    
  Serial.println("Save Configuration");

}

void cmdTemp()
{
  Serial.print(F("Humidite: "));
  Serial.print(SensorInt.readFloatHumidity(), 1);
  Serial.println(F("%"));

  Serial.print(F("Pression: "));
  Serial.print((SensorInt.readFloatPressure() / 100), 1);
  Serial.println(F(" hPa"));

  Serial.print(F("Temperature: "));
  Serial.print(SensorInt.readTempC(), 1);
  Serial.println(F("°"));
}

void cmdVersion()
{
    // Print usage
    Serial.print(F("VERSION: "));
    Serial.println(VERSION);
}

void cmdUnknown(const char *command)
{
    // Print unknown command
    Serial.print(F("Unknown command: "));
    Serial.print(command);
    Serial.println(F(", '?' for help"));
}

void printConsoleChar(void)
{
    Serial.print(F("> "));
}

// --  MISC  -------------------------------------------------------------
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

void checkSecond(void)
{
  if ((millis() - lastSecond) >= 1000)
  {
    lastSecond = millis();
    sendKeepAlive();
    sendBeaconTimer--;
    if(dtmfTimer > 0 )
    {
      dtmfTimer--;
    }
  }
}

void getDateStuff(byte& year, byte& month, byte& date, byte& dOW, byte& hour, byte& minute, byte& second)
{
    // Call this if you notice something coming in on
    // the serial port. The stuff coming in should be in
    // the order YYMMDDwHHMMSS, with an 'x' at the end.
    boolean gotString = false;
    char inChar;
    byte temp1, temp2;
    char inString[20];
    
    byte j=0;
    while (!gotString) {
        if (Serial.available()) {
            inChar = Serial.read();
            inString[j] = inChar;
            j += 1;
            if (inChar == 'x') {
                gotString = true;
            }
        }
    }
    Serial.println(inString);
    // Read year first
    temp1 = (byte)inString[0] -48;
    temp2 = (byte)inString[1] -48;
    year = temp1*10 + temp2;
    // now month
    temp1 = (byte)inString[2] -48;
    temp2 = (byte)inString[3] -48;
    month = temp1*10 + temp2;
    // now date
    temp1 = (byte)inString[4] -48;
    temp2 = (byte)inString[5] -48;
    date = temp1*10 + temp2;
    // now Day of Week
    dOW = (byte)inString[6] - 48;
    // now hour
    temp1 = (byte)inString[7] -48;
    temp2 = (byte)inString[8] -48;
    hour = temp1*10 + temp2;
    // now minute
    temp1 = (byte)inString[9] -48;
    temp2 = (byte)inString[10] -48;
    minute = temp1*10 + temp2;
    // now second
    temp1 = (byte)inString[11] -48;
    temp2 = (byte)inString[12] -48;
    second = temp1*10 + temp2;
}