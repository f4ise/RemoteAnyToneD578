#include "config.h"
#include "morse.h"

void sendBeacon(void);
void sendAck(void);

void setup() {

  pinMode(MSG0_CW, INPUT);
  pinMode(MSG1_CW, INPUT);
  pinMode(SOT_CW, INPUT);
  pinMode(CW_PIN, OUTPUT);
  pinMode(EOT_CW, OUTPUT);

  digitalWrite(EOT_CW, LOW);
}

void loop() {
  uint8_t valMessage = 0;
  valMessage = 0 | (digitalRead(MSG1_CW) << 1) | (digitalRead(MSG0_CW));
    
  if(digitalRead(SOT_CW) == 1) {
    switch(valMessage) {
      case 0:
        sendAck();
        break;
      case 1:
        sendBeacon();
        break;
      case 2:
        break;
      case 3:
        break;
    }

    digitalWrite(EOT_CW, HIGH);
    delay(100);
    digitalWrite(EOT_CW, LOW);
  }

  delay(200);
}

void sendBeacon(void) {
  play_message(CALL);
  send_word_space();
  play_message(TYPE);
  send_word_space();  
  play_message(LOCATOR);
}

void sendAck(void) {
  play_message(ACK);
}
