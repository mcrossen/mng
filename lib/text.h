#ifndef TEXT_H_
#define TEXT_H_
#include "mngdefs.h"

#define TEXT_COLOR 0xC0C0C0

// called by display_init
void font_init();

void print(char *string, int length);

void printString(char *string);

void printNewLine();

void printChar(char c);


#endif /* TEXT_H_ */
