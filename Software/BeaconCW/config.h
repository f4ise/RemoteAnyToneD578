#ifndef CONFIG_H_
#define CONFIG_H_

#define LED         LED_BUILTIN

// Definition IO ATTINY85
#define MSG0_CW    PB0
#define MSG1_CW    PB1
#define SOT_CW     PB2
#define CW_PIN     PB3
#define EOT_CW     PB4

// BEACON
#define TONE_CW     700
#define CALL        "F1ZIC"
#define TYPE        "BEACON"
#define LOCATOR     "JN25VA"
#define ACK         "K"

#endif /* CONFIG_H_ */