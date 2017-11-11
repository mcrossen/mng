#include "mngdefs.h"
#include "text.h"
#include "display.h"

extern volatile unsigned char _binary_lib_font_psf_start;

typedef struct {
	uint32_t magic;
	uint32_t version;
	uint32_t headersize;	/* offset of bitmaps in file */
	uint16_t flags;			/* original PSF2 has 32 bit flags */
	uint8_t hotspot_x;		/* addition to OS/Z */
	uint8_t hotspot_y;
	uint32_t numglyph;
	uint32_t bytesperglyph;
	uint32_t height;
	uint32_t width;
	uint8_t glyphs;
} __attribute__((packed)) font_t;

// current cursor position
int kx, ky;
// maximum coordinates
int maxx, maxy;

/**
 * display one literal unicode character
 */
void putc(char c) {
	font_t *font = (font_t*)&_binary_lib_font_psf_start;
	unsigned char *glyph = (unsigned char*)&_binary_lib_font_psf_start +
	 font->headersize + (c>0&&c<font->numglyph?c:0)*font->bytesperglyph;
	int offs = (ky * font->height * fb.scanline) + (kx * (font->width+1) * 4);
	int x,y, line,mask;
	int bytesperline=(font->width+7)/8;
	if(c=='\r') {
		kx=0;
	} else
	if(c=='\n') {
		kx=0; ky++;
	} else {
		for(y=0;y<font->height;y++){
			line=offs;
			mask=1<<(font->width-1);
			for(x=0;x<font->width;x++){
				*((uint32_t*)((uint64_t)fb.ptr + line))=((int)*glyph) & (mask)?TEXT_COLOR:0;
				mask>>=1;
				line+=4;
			}
			*((uint32_t*)((uint64_t)fb.ptr + line))=0;
			glyph+=bytesperline;
			offs+=fb.scanline;
		}
		kx++;
		if(kx>=maxx) {
			kx=0; ky++;
		}
	}
}

void font_init(uint32_t fbheight, uint32_t fbwidth) {
	font_t* font = (font_t*)&_binary_lib_font_psf_start;
	maxx=fb.width/(font->width+1);
	maxy=fb.height/font->height;
	kx=ky=0;
}

/**
 * display strings
 */
void print(char *string, int length) {
	int index;
	for (index = 0; index < length && *string; index++, string++) {
		putc(*string);
	}
}

void printString(char *string) {
	while(*string) {
		putc(*string++);
	}
}

void printNewLine() {
	putc((char)'\n');
}

void printChar(char c) {
	putc(c);
}
