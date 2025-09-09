#ifndef CONFIG_H
#define CONFIG_H

#define LED         LED_BUILTIN

#define dbgSpeed    115200

// UART D578
#define swTX        11
#define swRX        12
#define swSPEED     115200 

// MT8870
#define STQ         2
#define Q0          4
#define Q1          5
#define Q2          6
#define Q3          7

// BEACON
#define CW_PIN      8
#define CALL        "F1ZIC"
#define REPEAT      60
#define TONE_CW     700
#define MSG         "CQ CQ CQ DE F1ZIC/BEACON JN25TE"

#endif