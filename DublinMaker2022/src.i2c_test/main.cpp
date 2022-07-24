
#include <zephyr.h>
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <drivers/pwm.h>
#include "display.h"
#include "controller.h"
#include "console.h"
#include "sound.h"
#include "invaders.h"
#include "mesh.h"
#include "frame_buffer.h"

#include "sensirion_i2c.h"

#include "scd30.h"

void sao_test()
{
	/* P0.24 - test ok, 
	 * P0.13 - test ok,
	 * P0.18 - NOT OK
	 * POWER subsystem at offset 0x40000000
	 * PSELRESET0 at offset 200
	 * PSELRESET1 at offset 204
	 * P1.09 - test ok
	*/
	static const struct device *gpio0,*gpio1;
	uint32_t * pselreset0 = (uint32_t *)0x40000200;
	uint32_t * pselreset1 = (uint32_t *)0x40000204; 
	*pselreset0=0x80000000;
	*pselreset1=0x80000000;
	gpio0=device_get_binding("GPIO_0");
	gpio1=device_get_binding("GPIO_1");
	gpio_pin_configure(gpio0,18,GPIO_OUTPUT);
	
	printk("0 = %x ",*pselreset0);
	printk("1 = %x\n",*pselreset1);
	while(1)
	{
		gpio_pin_set(gpio0,18,1);
		k_msleep(100);
		gpio_pin_set(gpio0,18,0);
		k_msleep(100);
	}
	
}
extern console Console;
#define TILE_WIDTH 30
#define TILE_HEIGHT 30
#define SPRITE_WIDTH 30
#define SPRITE_HEIGHT 30
uint16_t Frame_Buffer[2*TILE_HEIGHT*2*TILE_WIDTH];
#include "char1.h"
#include "grass.h"
void drawTile(int row, int col)
{
	Console.Display.putImage(row*TILE_WIDTH,col*TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
}
void drawCharacterOld(int row, int col)
{
	Console.Display.putImage(row,col,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&char1[0]);
}
void drawTile2(int row, int col)
{
	Console.Display.fillRectangle(row*TILE_WIDTH,col*TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,0xffff);
}
void drawLandscape()
{

	int row,col;
	
	for (row=0;row<SCREEN_HEIGHT/TILE_HEIGHT;row++)
	{
		for (col=0;col<SCREEN_WIDTH/TILE_WIDTH;col++)
		{
			drawTile(row,col);
		}
	}
}

uint8_t dirty_flags[]={0,0,0,0,0,0,0,0};
void flagDirty(int row, int col)
{
	dirty_flags[row] |= (1 << col);
}
void flagClean(int row, int col)
{
	dirty_flags[row] &= ~(1 << col);
}
int isDirty(int row, int col)
{
	if (dirty_flags[row] & (1 << col))
		return 1;
	else
		return 0;
}
uint16_t fb_top_x, fb_top_y;
void check_for_overlap(uint16_t x,uint16_t y, uint16_t w, uint16_t h)
{
	// incoming arguments are the bounding rectangle for a sprite
	// This function checks to see if that sprite overlaps each
	// background screen tile.
	int row,col;
	uint16_t tlx,tly;
	int dirty;
	fb_top_x = 0xffff;
	fb_top_y = 0;
	for (row=0;row<SCREEN_HEIGHT/TILE_HEIGHT;row++)
	{
		for (col=0;col<SCREEN_WIDTH/TILE_WIDTH;col++)
		{
			dirty=0;
			// check top left corner
			if ((x >=row*TILE_WIDTH) && (y >= col*TILE_HEIGHT) &&
				((x) < (row+1)*TILE_WIDTH) && ((y) < (col+1)*TILE_HEIGHT) )
			{				
				dirty=1;
			}
			// check top right
			if (((x+w) >=row*TILE_WIDTH) && (y >= col*TILE_HEIGHT) &&
				((x+w) < (row+1)*TILE_WIDTH) && ((y) < (col+1)*TILE_HEIGHT) )
			{
				dirty=1;
			}
			// check bottom left corner
			if ((x >=row*TILE_WIDTH) && ((y+h) >= col*TILE_HEIGHT) &&
				((x) < (row+1)*TILE_WIDTH) && ((y+h) < (col+1)*TILE_HEIGHT) )
			{
				dirty=1;
			}
			// check bottom right
			if (((x+w) >=row*TILE_WIDTH) && ((y+w) >= col*TILE_HEIGHT) &&
				((x+w) < (row+1)*TILE_WIDTH) && ((y+w) < (col+1)*TILE_HEIGHT) )
			{
				dirty=1;
			}
			if (dirty)
			{
				flagDirty(row,col);
				tlx=row*TILE_WIDTH;
				tly=col*TILE_HEIGHT;
				if (fb_top_x > tlx)
				{
					fb_top_x = tlx;
				}
				if (fb_top_y < tly)
				{
					fb_top_y = tly;
				}
			}
		}
	}	
}
uint16_t fb_ap_x, fb_ap_y, fb_ap_w, fb_ap_h;
void openFBAperture(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	fb_ap_x = x;
	fb_ap_y = y;
	fb_ap_w = w;
	fb_ap_h = h;
}
void FBputImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t *img)
{
	openFBAperture(x,y,w,h);
}
void FBputImageTransparency(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t *img)
{
	openFBAperture(x,y,w,h);
}
void drawCharacter(int x, int y)
{
	
	// 1f00 = blue
	// 00f8 = red
	// e007 = green
	
	//Console.Display.fillRectangle(x,y,SPRITE_WIDTH,SPRITE_HEIGHT,4);	
	FBputImage(0,0,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		
	FBputImage(TILE_WIDTH,0,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		
	FBputImage(0,TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		
	FBputImageTransparency(TILE_WIDTH,TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
	
	FBputImageTransparency(x - fb_top_x, y - fb_top_y,SPRITE_WIDTH, SPRITE_HEIGHT,(uint16_t *)&char1[0]);
	
	Console.Display.putImage(fb_top_x,fb_top_y,TILE_WIDTH*2,TILE_HEIGHT*2,(uint16_t *)&Frame_Buffer[0]);
}
void moveCharacter()
{
	int x,y;
	int row,col;
	x = 0; y = 0;
	while(1)
	{

		for (row=0;row<SCREEN_HEIGHT/TILE_HEIGHT;row++)
		{
			for (col=0;col<SCREEN_WIDTH/TILE_WIDTH;col++)
			{
				if (isDirty(row,col))
				{
					drawTile(row,col);
					flagClean(row,col);
				}
			}
		}		
		drawCharacterOld(x,y);
		k_msleep(50);
		check_for_overlap(x,y,SPRITE_WIDTH,SPRITE_HEIGHT);
		x++;
		if (x >= (SCREEN_WIDTH-SPRITE_WIDTH))
			x=0;
		y++;
		if (y >= (SCREEN_HEIGHT-SPRITE_HEIGHT))
			y = 0;
		
	}
}
frame_buffer FrameBuffer;
void main(void)
{
	
	
	int ret;
	int count=0;		
	ret = Console.begin();	
	uint32_t co2_value;
	int err;
	uint8_t msg_count = 0;
	static const struct device *gpio0;
	//mesh_begin();
	gpio0=device_get_binding("GPIO_0");
	gpio_pin_configure(gpio0,2,GPIO_OUTPUT);

	if (ret < 0)
	{
		printk("\nError initializing console.  Error code = %d\n",ret);
		while(1);
	}	
	//playInvaders(0);		
 	//drawLandscape();	
	//moveCharacter();
	uint16_t char_x, char_y;
	char_x = char_y = 0;
	
    uint16_t interval_in_seconds = 2;
    float co2_ppm, temperature, relative_humidity;
    co2_ppm = 0.0;
    temperature = 0.0; 
    relative_humidity = 0.0;
        sensirion_i2c_select_bus(1); 
    sensirion_i2c_init();
    /* Busy loop for initialization, because the main loop does not work without
     * a sensor.
     */
    while (scd30_probe() != NO_ERROR) {
        printk("SCD30 sensor probing failed\n");
        sensirion_sleep_usec(1000000u);
    }
    printk("SCD30 sensor probing successful\n");

    scd30_set_measurement_interval(interval_in_seconds);
    sensirion_sleep_usec(20000u);
    scd30_start_periodic_measurement(0);

	
	while(0)
	{
		
		FrameBuffer.putImage(0,0,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		FrameBuffer.putImage(TILE_WIDTH,0,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		FrameBuffer.putImage(0,TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		FrameBuffer.putImage(TILE_WIDTH,TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&tile[0]);
		//printk("xy in fb = (%d,%d)",char_x % FRAMEBUFFER_WIDTH,char_y % FRAMEBUFFER_HEIGHT);
		//printk("fb locn = (%d,%d)\n",FRAMEBUFFER_WIDTH*(char_x / FRAMEBUFFER_WIDTH),FRAMEBUFFER_HEIGHT*(char_y/FRAMEBUFFER_HEIGHT));
		//FrameBuffer.putImage(char_x % FRAMEBUFFER_WIDTH,char_y % FRAMEBUFFER_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&char1[0]);		
 		//Console.Display.putImage(FRAMEBUFFER_WIDTH*(char_x / FRAMEBUFFER_WIDTH),FRAMEBUFFER_HEIGHT*(char_y/FRAMEBUFFER_HEIGHT),FRAMEBUFFER_WIDTH,FRAMEBUFFER_HEIGHT,(uint16_t *)&FrameBuffer.Frame_Buffer[0]);
		
		FrameBuffer.putImage(char_x % TILE_WIDTH,char_y % TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT,(uint16_t *)&char1[0]);		
 		Console.Display.putImage(TILE_WIDTH*(char_x / TILE_WIDTH),TILE_HEIGHT*(char_y/TILE_HEIGHT),FRAMEBUFFER_WIDTH,FRAMEBUFFER_HEIGHT,(uint16_t *)&FrameBuffer.Frame_Buffer[0]);
		char_x++;
		if (char_x >= (SCREEN_WIDTH-SPRITE_WIDTH))
			char_x=0;
		char_y++;
		if (char_y >= (SCREEN_HEIGHT-SPRITE_HEIGHT))
			char_y = 0;
		k_msleep(50);
	}
	
	while(1)
	{
		//printk("Hello World! %s\n", CONFIG_BOARD);
       //Display.drawRectangle(random(0,119),random(0,119),random(0,120),random(0,120),random(0,0xffff));                                
       //Console.Display.fillRectangle(Console.random(0,240),Console.random(0,240),Console.random(0,240),Console.random(0,240),Console.random(0,0xffff));                   
		Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,Console.random(0,0xffff));                   
		///Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0b1111111111111111);
		///k_msleep(100);
		//Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0b1111111111011111);                   
		//k_msleep(100);
       // display_drawCircle(random(0,240),random(0,240),random(0,135),random(0,0xffff));
        //Display.fillCircle(random(0,240),random(0,135),random(0,120),random(0,0xffff));

        count++;
        if (count >= 10) 
        {
      //      Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
			
		//	Console.Display.print("Starting over",sizeof("Starting over")-1,20,60,0xffff,0);
            count = 0;
		//	sendDMBMessage(msg_count++);

			//printk("restarting\n");
			if (Console.Controller.getButtonState())
			Console.Sound.playTone(500,40);
			printk("Buttons = %x\n",Console.Controller.getButtonState());
			
 		//	printk("Buttons = %d\r\n",Console.Controller.getButtonState());
			uint16_t data_ready = 0;
        err = scd30_get_data_ready(&data_ready);
        if (err) {
                printk("Error reading data_ready flag: %i\n", err);
        }
        if (data_ready)
        {
            
            err = scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
            if (err) {
                printk("error reading measurement\n");

            } else {
                printk("measured co2 concentration: %0.2f ppm, measured temperature: %0.2f degreeCelsius, measured humidity: %0.2f %%RH\n",  co2_ppm, temperature, relative_humidity);
            }
            co2_value = co2_ppm;
			printk("CO2 = %d\n",co2_value);			
			gpio_pin_set(gpio0,2,1);
			k_msleep(100);
			gpio_pin_set(gpio0,2,0);

        }

        }        
	}
}

