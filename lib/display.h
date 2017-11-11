#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "mngdefs.h"

typedef struct {
	uint8_t *ptr;     // framebuffer pointer and dimensions
	uint8_t type;
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint32_t scanline;
} fb_t;

extern fb_t fb;

void display_init();

#endif /* DISPLAY_H_ */
