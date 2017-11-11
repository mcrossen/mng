#include "mngdefs.h"
#include "display.h"
#include "text.h"
fb_t fb;
volatile uint32_t  __attribute__((aligned(16))) mbox[36];

#define PAGE_SIZE		4096
#define MMIO_BASE       0x3F000000
#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile uint32_t*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile uint32_t*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile uint32_t*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile uint32_t*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile uint32_t*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile uint32_t*)(VIDEOCORE_MBOX+0x20))
#define MBOX_REQUEST    0
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8
// framebuffer pixel format, only 32 bits supported
#define FB_ARGB   0
#define FB_RGBA   1
#define FB_ABGR   2
#define FB_BGRA   3

/* mailbox functions */
void mbox_write(uint8_t ch, volatile uint32_t *mbox) {
	do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
	*MBOX_WRITE = (((uint32_t)((uint64_t)mbox)&~0xF) | (ch&0xF));
}

uint32_t mbox_read(uint8_t ch) {
	uint32_t r;
	while(1) {
		do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
		r=*MBOX_READ;
		if((uint8_t)(r&0xF)==ch)
			return (r&~0xF);
	}
}

uint8_t mbox_call(uint8_t ch, volatile uint32_t *mbox) {
	mbox_write(ch,mbox);
	return mbox_read(ch)==(uint32_t)((uint64_t)mbox) && mbox[1]==MBOX_RESPONSE;
}



// maximum coordinates
int maxx, maxy;

/**
 * Get a linear frame buffer
 */
int GetLFB(uint32_t width, uint32_t height) {

	//query natural width, height if not given
	if(width==0 && height==0) {
		mbox[0] = 8*4;
		mbox[1] = MBOX_REQUEST;
		mbox[2] = 0x40003;  //get phy wh
		mbox[3] = 8;
		mbox[4] = 8;
		mbox[5] = 0;
		mbox[6] = 0;
		mbox[7] = 0;
		if(mbox_call(MBOX_CH_PROP,mbox) && mbox[5]!=0) {
			width=mbox[5];
			height=mbox[6];
		}
	}
	//if we already have a framebuffer, release it
	if(fb.ptr!=NULL) {
		mbox[0] = 8*4;
		mbox[1] = MBOX_REQUEST;
		mbox[2] = 0x48001;  //release buffer
		mbox[3] = 8;
		mbox[4] = 8;
		mbox[5] = (uint32_t)(((uint64_t)fb.ptr));
		mbox[6] = 0;
		mbox[7] = 0;
		mbox_call(MBOX_CH_PROP,mbox);
	}
	//check minimum resolution
	if(width<800 || height<600) {
		width=800; height=600;
	}
	mbox[0] = 35*4;
	mbox[1] = MBOX_REQUEST;

	mbox[2] = 0x48003;  //set phy wh
	mbox[3] = 8;
	mbox[4] = 8;
	mbox[5] = width;        //FrameBufferInfo.width
	mbox[6] = height;       //FrameBufferInfo.height

	mbox[7] = 0x48004;  //set virt wh
	mbox[8] = 8;
	mbox[9] = 8;
	mbox[10] = width;       //FrameBufferInfo.virtual_width
	mbox[11] = height;      //FrameBufferInfo.virtual_height

	mbox[12] = 0x48009; //set virt offset
	mbox[13] = 8;
	mbox[14] = 8;
	mbox[15] = 0;           //FrameBufferInfo.x_offset
	mbox[16] = 0;           //FrameBufferInfo.y.offset

	mbox[17] = 0x48005; //set depth
	mbox[18] = 4;
	mbox[19] = 4;
	mbox[20] = 32;          //FrameBufferInfo.depth

	mbox[21] = 0x48006; //set pixel order
	mbox[22] = 4;
	mbox[23] = 4;
	mbox[24] = 0;       //RGB, not BGR preferably

	mbox[25] = 0x40001; //get framebuffer, gets alignment on request
	mbox[26] = 8;
	mbox[27] = 8;
	mbox[28] = PAGE_SIZE;    //FrameBufferInfo.pointer
	mbox[29] = 0;           //FrameBufferInfo.size

	mbox[30] = 0x40008; //get pitch
	mbox[31] = 4;
	mbox[32] = 4;
	mbox[33] = 0;           //FrameBufferInfo.pitch

	mbox[34] = 0;       //Arnold Schwarzenegger

	if(mbox_call(MBOX_CH_PROP,mbox) && mbox[20]==32 && mbox[27]==(MBOX_RESPONSE|8) && mbox[28]!=0) {
		mbox[28]&=0x3FFFFFFF;
		fb.width=mbox[5];
		fb.height=mbox[6];
		fb.scanline=mbox[33];
		fb.ptr=(void*)((uint64_t)mbox[28]);
		fb.size=mbox[29];
		fb.type=mbox[24]?FB_ABGR:FB_ARGB;
		font_init(fb.width, fb.height);
		return 1;
	}
	return 0;
}

// set up a framebuffer so that we can write on screen
void display_init() {
	if(!GetLFB(0, 0)) {
		//TODO: error
	}
}
