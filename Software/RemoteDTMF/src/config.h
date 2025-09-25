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
#define MSG0_CW     14
#define MSG1_CW     15
#define SOT_CW      8
#define EOT_CW      9
#define REPEAT      5       // Send Beacon X min.

// AUDIO AM
#define DURATION_AM 15      // Send Audio AM X sec.

// OPTO
#define PRES12V     3
#define CMDPTT      10

// SWITCH ANA
#define SW_CW       16
#define SW_AM       17




#endif