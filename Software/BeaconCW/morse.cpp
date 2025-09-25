#include "morse.h"

morse_char_t MorseCode[] = {
    {'.', '-',  0,   0,   0},   // A
    {'-', '.', '.', '.',  0},   // B
    {'-', '.', '-', '.',  0},   // C
    {'-', '.', '.',  0,   0},   // D
    {'.',  0,   0,   0,   0},   // E
    {'.', '.', '-', '.',  0},   // F
    {'-', '-', '.',  0,   0},   // G
    {'.', '.', '.', '.',  0},   // H
    {'.', '.',  0,   0,   0},   // I
    {'.', '-', '-', '-',  0},   // J
    {'-', '.', '-',  0,   0},   // K
    {'.', '-', '.', '.',  0},   // L
    {'-', '-',  0,   0,   0},   // M
    {'-', '.',  0,   0,   0},   // N
    {'-', '-', '-',  0,   0},   // O
    {'.', '-', '-', '.',  0},   // P
    {'-', '-', '.', '-',  0},   // Q
    {'.', '-', '.',  0,   0},   // R
    {'.', '.', '.',  0,   0},   // S
    {'-',  0,   0,   0,   0},   // T
    {'.', '.', '-',  0,   0},   // U
    {'.', '.', '.', '-',  0},   // V
    {'.', '-', '-',  0,   0},   // W
    {'-', '.', '.', '-',  0},   // X
    {'-', '.', '-', '-',  0},   // Y
    {'-', '-', '.', '.',  0},   // Z
    {'-', '-', '-', '-', '-'},  // 0
    {'.', '-', '-', '-', '-'},  // 1
    {'.', '.', '-', '-', '-'},  // 2
    {'.', '.', '.', '-', '-'},  // 3
    {'.', '.', '.', '.', '-'},  // 4
    {'.', '.', '.', '.', '.'},  // 5
    {'-', '.', '.', '.', '.'},  // 6
    {'-', '-', '.', '.', '.'},  // 7
    {'-', '-', '-', '.', '.'},  // 8
    {'-', '-', '-', '-', '.'}   // 9
};


void play_message(String m)
{
// sends the message in string 'm' as CW, with inter letter and word spacing
// s is the speed to play at; if s == 0, use the current speed  
uint8_t i, j; 
  int n; 
  char buff[64];

  // use ch = m.charAt(index);
  m.toCharArray(buff, m.length()+1);

  for (i=0; i<m.length(); i++)
  {
    if(buff[i] == ' ') 
    {
       send_word_space(); 
    }
    else
    {
      if( (n = morse_lookup(buff[i])) == -1 )
      {
        // char not found, ignore it (but report it on Serial)
      }
      else
      {
        // char found, so send it as dots and dashes
        for(j=0; j<5; j++)
          send_morse_char(MorseCode[n].ch[j]);
        send_letter_space();  // send an inter-letter space
      }
    }
  }
}

int morse_lookup(char c)
// returns the index of parameter 'c' in MorseCode array, or -1 if not found
{
  if((c >= 'A') & (c <='Z')) {
    return (c - 'A');
  }
  if((c >= '0') & (c <='9')) {
    return ((c - 'A') + 26);
  }
  return -1;
}

void send_morse_char(char dotdash)
{
  if(dotdash == '.') {
    send_dot();
  }
  if(dotdash == '-') {
    send_dash();
  }    
}

void send_letter_space()
{
  delay(dot_length_ms * 2);  // wait for 2 dot periods
}

void send_word_space()
{
  delay(dot_length_ms * 6);  // wait for 6 dot periods
}

void send_dot()
{
  delay(dot_length_ms);  // wait for one dot period (space)
  tone(CW_PIN, TONE_CW);
  delay(dot_length_ms);
  noTone(CW_PIN);
}

void send_dash()
{
  delay(dot_length_ms);  // wait for one dot period (space)
  tone(CW_PIN, TONE_CW);
  delay((dot_length_ms * CW_DASH_LEN));
  noTone(CW_PIN);
}
