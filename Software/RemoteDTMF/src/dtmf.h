#ifndef DTMF_H
#define DTMF_H

#include <stdint.h>

#include "config.h"

const char charDTMF[16] = {'D','1','2','3','4','5','6','7','8','9','0','*','#','A','B','C'};

void initDTMF(void);
char readDTMF(void);

void STQ_ISR(void);

#endif