#include "frame_buffer.h"
void frame_buffer::openAperture(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    fb_ap_tlx = x;
    fb_ap_tly = y;
    fb_ap_w = w;
    fb_ap_h = h;    
    fb_x = fb_ap_tlx; // current cursor posn
    fb_y = fb_ap_tly;
}
void frame_buffer::put(uint16_t value)
{
	if (value != TRANSPARENT_VALUE)
	{
		uint16_t index=fb_x+(fb_y*FRAMEBUFFER_WIDTH);
		if (index < (FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT))
			Frame_Buffer[index]=value;
	}
    fb_x ++;
    if (fb_x >= (fb_ap_w+fb_ap_tlx))
    {
        fb_x = fb_ap_tlx;
        fb_y ++;
        if (fb_y >= (fb_ap_h+fb_ap_tly))
        {
            fb_y = fb_ap_tly;
        }
    }
    
}
void frame_buffer::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t c)
{
    openAperture(x,y,w,h);
    for (int i=0;i<w*h;i++)
	{		
        put(c);
	}    
}
void frame_buffer::putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t *pImage)
{
    openAperture(x,y,w,h);
    for (int i=0;i<w*h;i++)
	{
		put(pImage[i]);
	}
    
}
