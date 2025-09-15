#include "config.h"
#include "morse.h"

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(CW_PIN, OUTPUT);
  pinMode(EOT_PIN, OUTPUT);
  digitalWrite(EOT_PIN, LOW);
}

void loop() {
  digitalWrite(EOT_PIN, LOW);

  play_message(CALL);
  send_word_space();

  play_message(TYPE);
  send_word_space();
  
  play_message(LOCATOR);
  delay(100);
  digitalWrite(EOT_PIN, HIGH);
  delay(5000);
}
