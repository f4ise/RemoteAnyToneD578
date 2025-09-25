#ifndef MORSE_H_
#define MORSE_H_

#include <Arduino.h>
#include "config.h"

// morse reference table
struct morse_char_t {
   char ch[7]; 
};
  
#define CW_DASH_LEN         3       // length of dash (in dots)
#define dot_length_ms       45      // keyer speed ((dot length mS)), 60 equates to 10 w.p.m., 45 equates to 15 w.p.m.

void send_dot(void);
void send_dash(void);

void send_letter_space(void);
void send_word_space(void);
void send_morse_char(char dotdash);
void play_message(String m);
int morse_lookup(char c);
void send_dot(void);
void send_dash(void);


#endif /* MORSE_H_ */