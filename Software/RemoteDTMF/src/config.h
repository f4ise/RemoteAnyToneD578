#ifndef CONFIG_H
#define CONFIG_H

#define VERSION     "0.5A"

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
#define timeoutDTMF 10      // Timeout DTMF

// BEACON
#define MSG0_CW     14
#define MSG1_CW     15
#define SOT_CW      8
#define EOT_CW      9

// AUDIO AM
#define DURATION_AM 15      // Send Audio AM X sec.

// OPTO
#define PRES12V1     3
#define PRES12V2     3      // TBD IO
#define CMDPTT      10

// SWITCH ANA
#define SW_CW       16
#define SW_AM       17

// Logs
#define INF   0
#define WAR   1
#define ERR   2

#endif