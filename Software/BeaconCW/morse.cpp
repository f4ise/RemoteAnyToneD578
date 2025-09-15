#include "morse.h"

morse_char_t MorseCode[] = {
    {'A', '.', '-',  0,   0,   0,   0},
    {'B', '-', '.', '.', '.',  0,   0},
    {'C', '-', '.', '-', '.',  0,   0},
    {'D', '-', '.', '.',  0,   0,   0},
    {'E', '.',  0,   0,   0,   0,   0},
    {'F', '.', '.', '-', '.',  0,   0},
    {'G', '-', '-', '.',  0,   0,   0},
    {'H', '.', '.', '.', '.',  0,   0},
    {'I', '.', '.',  0,   0,   0,   0},
    {'J', '.', '-', '-', '-',  0,   0},
    {'K', '-', '.', '-',  0,   0,   0},
    {'L', '.', '-', '.', '.',  0,   0},
    {'M', '-', '-',  0,   0,   0,   0},
    {'N', '-', '.',  0,   0,   0,   0},
    {'O', '-', '-', '-',  0,   0,   0},
    {'P', '.', '-', '-', '.',  0,   0},
    {'Q', '-', '-', '.', '-',  0,   0},
    {'R', '.', '-', '.',  0,   0,   0},
    {'S', '.', '.', '.',  0,   0,   0},
    {'T', '-',  0,   0,   0,   0,   0},
    {'U', '.', '.', '-',  0,   0,   0},
    {'V', '.', '.', '.', '-',  0,   0},
    {'W', '.', '-', '-',  0,   0,   0},
    {'X', '-', '.', '.', '-',  0,   0},
    {'Y', '-', '.', '-', '-',  0,   0},
    {'Z', '-', '-', '.', '.',  0,   0},
    {'0', '-', '-', '-', '-', '-',  0},
    {'1', '.', '-', '-', '-', '-',  0},
    {'2', '.', '.', '-', '-', '-',  0},
    {'3', '.', '.', '.', '-', '-',  0},
    {'4', '.', '.', '.', '.', '-',  0},
    {'5', '.', '.', '.', '.', '.',  0},
    {'6', '-', '.', '.', '.', '.',  0},
    {'7', '-', '-', '.', '.', '.',  0},
    {'8', '-', '-', '-', '.', '.',  0},
    {'9', '-', '-', '-', '-', '.',  0},
    {'/', '-', '.', '.', '-', '.',  0}
    // {'/', '-', '.', '.', '-', '.',  0},
    // {'?', '.', '.', '-', '-', '.', '.'},
    // {'.', '.', '-', '.', '-', '.', '-'},
    // {',', '-', '-', '.', '.', '-', '-'},
    // {'(', '-', '.', '-', '.', '-', 0},
    // {')', '.', '-', '.', '-', '.', 0}
};

void send_letter_space()
{
  delay(dot_length_ms * 4);  // wait for 3 dot periods
}


void send_word_space()
{
  delay(dot_length_ms * 7);  // wait for 6 dot periods
}


void send_morse_char(char c)
{
  // 'c' is a '.' or '-' char, so send it 
  if(c == '.')
    send_dot();
  else 
    if (c == '-') 
      send_dash();
}


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
        for(j=1; j<7; j++)
          send_morse_char(MorseCode[n].ch[j]);
        send_letter_space();  // send an inter-letter space
      }
    }
  }
}

int morse_lookup(char c)
// returns the index of parameter 'c' in MorseCode array, or -1 if not found
{
  unsigned int i;
  for(i=0; i<sizeof(MorseCode); i++)
  {
    if(c == MorseCode[i].ch[0])
      return i;
  }
  return -1; 
}

void send_dot()
{
  digitalWrite(LED, HIGH);
  delay(dot_length_ms);  // wait for one dot period (space)
  tone(CW_PIN, TONE_CW);
  delay(dot_length_ms);
  noTone(CW_PIN);
  digitalWrite(LED, LOW);
}

void send_dash()
{
  digitalWrite(LED, HIGH);
  delay(dot_length_ms);  // wait for one dot period (space)
  tone(CW_PIN, TONE_CW);
  delay((dot_length_ms * CW_DASH_LEN));
  noTone(CW_PIN);
  digitalWrite(LED, LOW);
}
