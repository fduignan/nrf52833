#ifndef __FRAME_BUFFER
#define __FRAME_BUFFER
#include <stdint.h>
#define TILE_WIDTH 30
#define TILE_HEIGHT 30
#define FRAMEBUFFER_WIDTH (2*TILE_WIDTH)
#define FRAMEBUFFER_HEIGHT (2*TILE_HEIGHT)
// A pixel with the value below is NOT drawn.  This enables transparency
#define TRANSPARENT_VALUE (0xffff)
class frame_buffer 
{
public:
	void openAperture(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	void put(uint16_t value);
	void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t c);
	void putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t *pImage);
	uint16_t Frame_Buffer[FRAMEBUFFER_WIDTH*FRAMEBUFFER_HEIGHT];
	
private:
	uint16_t fb_ap_tlx,fb_ap_tly,fb_ap_w,fb_ap_h,fb_x,fb_y;
	
};
#endif
