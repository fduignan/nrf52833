#include <sys/printk.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/pwm.h>
#include <stdio.h>
#include "display.h"
#include "font5x7.h"

// Configuration for the SPI port.  Note the 32MHz clock speed possible only on SPI 3
// Pin usage by SPI bus defined in app.overlay.
static const struct spi_config cfg = {
	.frequency = 32000000,
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |  SPI_MODE_CPOL | SPI_MODE_CPHA,
	.slave = 0,
};
static const struct device *spi_display;
static const struct device *gpio0, *gpio1;		

// Control line usage
// The RESET_PORT_BIT is on GPIO0
#define RESET_PORT_BIT 18
// The DC_PORT_BIT is on GPIO0
#define DC_PORT_BIT 13

// The BACKLIGHT control is on GPIO0
#define BACKLIGHT_PORT_BIT 24
static const struct device *blk_pwm;


void ResetLow()
{
	gpio_pin_set(gpio0,RESET_PORT_BIT,0);
}
void ResetHigh()
{
	gpio_pin_set(gpio0,RESET_PORT_BIT,1);
}
void DCLow()
{
	gpio_pin_set(gpio0,DC_PORT_BIT,0);
}
void DCHigh()
{
	gpio_pin_set(gpio0,DC_PORT_BIT,1);
}
void BacklightHigh()
{
	uint32_t Period = (uint32_t)16000000/ (uint32_t)1000;
	pwm_pin_set_cycles(blk_pwm, BACKLIGHT_PORT_BIT, Period,Period,0);
}
void BacklightLow()
{
	uint32_t Period = (uint32_t)16000000/ (uint32_t)1000;
	pwm_pin_set_cycles(blk_pwm, BACKLIGHT_PORT_BIT, Period,0,0);
}
void display::setBrightness(uint32_t pcnt)
{
	volatile uint32_t *Prescaler = (volatile uint32_t *)(0x4001C000+0x50c);
	*Prescaler = 6;
	
	uint32_t Period = (uint32_t)16000000/ (uint32_t)1000;
	uint32_t HighTime = (Period * pcnt)/100;
	pwm_pin_set_cycles(blk_pwm, BACKLIGHT_PORT_BIT, Period,HighTime,0);
}
void display::hw_test()
{
	// Test routine
	while(1)
	{
		// Insert calls here to the various
		// control lines to test them.
		// Also send some SPI data
		ResetLow();
		k_msleep(100);		
		ResetHigh();
		k_msleep(100);
	}
}
int display::begin()
{
	int ret;
	// Set up the SPI interface
	spi_display = device_get_binding("SPI_3");
	if (spi_display==NULL)
	{
		printf("Error acquiring SPI interface\n");
		return -1;
	}
	else
	{
		printf("Got SPI interface\n");
		if (!device_is_ready(spi_display))
		{
			printf("Device is not ready\n");
			return -2;
		}
	}	
	// Now configure the GPIO's for use as DC and Reset
	// The MISO pin (not used) will be reassigned as Reset
	gpio0=device_get_binding("GPIO_0");
	if (gpio0 == NULL)
	{
		printf("Error acquiring GPIO 0 interface\n");
		return -3;
	}
	gpio1=device_get_binding("GPIO_1");
	if (gpio1 == NULL)
	{
		printf("Error acquiring GPIO 1 interface\n");
		return -4;
	}
	ret = gpio_pin_configure(gpio0,RESET_PORT_BIT,GPIO_OUTPUT);
	ret = gpio_pin_configure(gpio0,DC_PORT_BIT,GPIO_OUTPUT);
	//ret = gpio_pin_configure(gpio0,BACKLIGHT_PORT_BIT,GPIO_OUTPUT);
	blk_pwm = device_get_binding("PWM_1");
	if (blk_pwm == NULL)
	{
		printk("Error acquiring PWM\r\n");
		return -1;
		
	}
//	hw_test();		
	k_msleep(25);	
	k_msleep(1);
	ResetHigh();
	k_msleep(25);
	ResetLow();
	k_msleep(25);
	ResetHigh();
	k_msleep(25);
	this->command(0x1); // Software reset
	k_msleep(150);
	this->command(0x11); // exit sleep
	k_msleep(250);
	this->command(0x3a);// Set colour mode        
	this->data(0x55); // 16bits / pixel @ 64k colors 5-6-5 format 
	k_msleep(25);
	this->command(0x36); // RGB Format
	this->data(0x08); // // RGB Format, rows are on the long axis. Change to 0x40 to enable portrait mode,0x60 for landscape
	k_msleep(25);
	this->command(0x51); // maximum brightness
    k_msleep(25);
	this->command(0x21);    // display inversion off (datasheet is incorrect on this point)
    this->command(0x13);    // partial mode off                 
    this->command(0x29);    // display on
	k_msleep(25);
	this->command(0x2c);   // put display in to write mode
	
	this->fillRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);  // black out the screen
   // BacklightHigh();
	setBrightness(70);
	return 0;
}
void display::command(uint8_t cmd)
{
	DCLow();
	struct spi_buf tx_buf = {.buf = &cmd, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	spi_write(spi_display, &cfg, &tx_bufs);
}

void display::data(uint8_t data)
{
	DCHigh();
	struct spi_buf tx_buf = {.buf = &data, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	spi_write(spi_display, &cfg, &tx_bufs);
}
void display::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display    

	this->command(0x2A); // Set X limits    	
    this->data(x1>>8);
    this->data(x1&0xff);        
    this->data(x2>>8);
    this->data(x2&0xff);
    
    this->command(0x2B);// Set Y limits
    this->data(y1>>8);
    this->data(y1&0xff);        
    this->data(y2>>8);
    this->data(y2&0xff);    
        
    this->command(0x2c); // put display in to data write mode

}

void display::fillRectangle(uint16_t x,uint16_t y,uint16_t width, uint16_t height, uint16_t colour)
{
	// This routine breaks the filled area up into sections and fills these.
	// This allows it to make more efficient use of the control lines and SPI bus
#define PIXEL_CACHE_SIZE 64
	static uint16_t fill_cache[PIXEL_CACHE_SIZE]; // use this to speed up writes
	uint32_t pixelcount = height * width;
	uint32_t blockcount = pixelcount / PIXEL_CACHE_SIZE;
	
	this->openAperture(x, y, x + width - 1, y + height - 1);
	DCHigh();
	struct spi_buf tx_buf = {.buf = &colour, .len = 2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   

	if (blockcount)
	{
		for (int p=0;p<PIXEL_CACHE_SIZE;p++)
		{
			fill_cache[p]=colour;
		}
	}
	while(blockcount--)
	{
		tx_buf.buf=fill_cache;
		tx_buf.len = PIXEL_CACHE_SIZE*2;
		spi_write(spi_display, &cfg, &tx_bufs);
	}

	pixelcount = pixelcount % PIXEL_CACHE_SIZE;
	while(pixelcount--) 
	{
		tx_buf.buf = &colour;
		tx_buf.len = 2;		
		spi_write(spi_display, &cfg, &tx_bufs);
	}	
}
void display::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    this->openAperture(x, y, x + 1, y + 1);
	struct spi_buf tx_buf = {.buf = &colour, .len = 2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
	DCHigh();
	spi_write(spi_display, &cfg, &tx_bufs);

}
void display::putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *Image)
{
	this->openAperture(x, y, x + width - 1, y + height - 1);
	struct spi_buf tx_buf = {.buf = Image, .len = width*height*2};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};   
	DCHigh();
	spi_write(spi_display, &cfg, &tx_bufs);

}
int display::iabs(int x) // simple integer version of abs for use by graphics functions        
{
	if (x < 0)
		return -x;
	else
		return x;
}
void display::drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  
  int y = y0;

  for (int x=x0; x <= x1;x++)
  {
    this->putPixel(x,y,Colour);    
    if (D > 0)
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
    
  }
}
void display::drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
        // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }  
  int D = 2*dx - dy;
  int x = x0;

  for (int y=y0; y <= y1; y++)
  {
    this->putPixel(x,y,Colour);
    if (D > 0)
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}
void display::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
    if ( iabs(y1 - y0) < iabs(x1 - x0) )
    {
        if (x0 > x1)
        {
            this->drawLineLowSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            this->drawLineLowSlope(x0, y0, x1, y1, Colour);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            this->drawLineHighSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            this->drawLineHighSlope(x0, y0, x1, y1, Colour);
        }
        
    }    
}
void display::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    this->drawLine(x,y,x+w,y,Colour);
    this->drawLine(x,y,x,y+h,Colour);
    this->drawLine(x+w,y,x+w,y+h,Colour);
    this->drawLine(x,y+h,x+w,y+h,Colour);
}
void display::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles    
    while (x >= y)
    {
        this->putPixel(x0 + x, y0 + y, Colour);
        this->putPixel(x0 + y, y0 + x, Colour);
        this->putPixel(x0 - y, y0 + x, Colour);
        this->putPixel(x0 - x, y0 + y, Colour);
        this->putPixel(x0 - x, y0 - y, Colour);
        this->putPixel(x0 - y, y0 - x, Colour);
        this->putPixel(x0 + y, y0 - x, Colour);
        this->putPixel(x0 + x, y0 - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// Similar to drawCircle but fills the circle with lines instead
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles        
    while (x >= y)
    {
        this->drawLine(x0 - x, y0 + y,x0 + x, y0 + y, Colour);        
        this->drawLine(x0 - y, y0 + x,x0 + y, y0 + x, Colour);        
        this->drawLine(x0 - x, y0 - y,x0 + x, y0 - y, Colour);        
        this->drawLine(x0 - y, y0 - x,x0 + y, y0 - x, Colour);        

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::printX2(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
	#define Scale 2
	// This function draws each character individually scaled up by a factor of 2.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
	
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT*Scale * Scale];

    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[((Row*Scale) * FONT_WIDTH*Scale) + (Col*Scale)] = ForeColour;
					TextBox[((Row*Scale) * FONT_WIDTH*Scale) + (Col*Scale)+1] = ForeColour;
					TextBox[(((Row*Scale)+1) * FONT_WIDTH*Scale) + (Col*Scale)] = ForeColour;
					TextBox[(((Row*Scale)+1) * FONT_WIDTH*Scale) + (Col*Scale)+1] = ForeColour;
                }
                else
                {
                    TextBox[((Row*Scale) * FONT_WIDTH*Scale) + (Col*Scale)] = BackColour;
					TextBox[((Row*Scale) * FONT_WIDTH*Scale) + (Col*Scale)+1] = BackColour;
					TextBox[(((Row*Scale)+1) * FONT_WIDTH*Scale) + (Col*Scale)] = BackColour;
					TextBox[(((Row*Scale)+1) * FONT_WIDTH*Scale) + (Col*Scale)+1] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        this->putImage(x, y, FONT_WIDTH*Scale, FONT_HEIGHT*Scale, (uint16_t *)TextBox);
        x = x + FONT_WIDTH*Scale + 2;
    }
}
void display::print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
	// This function draws each character individually.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT];
    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = ForeColour;
                }
                else
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        this->putImage(x, y, FONT_WIDTH, FONT_HEIGHT, (uint16_t *)TextBox);
        x = x + FONT_WIDTH + 2;
    }
}
void display::print(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
     // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[5]; // Maximum value = 65535
    Buffer[4] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[3] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[2] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';
    print(Buffer, 5, x, y, ForeColour, BackColour);
}
void display::printX2(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
     // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[5]; // Maximum value = 65535
    Buffer[4] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[3] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[2] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';
    printX2(Buffer, 5, x, y, ForeColour, BackColour);
}
uint16_t display::RGBToWord(uint16_t R, uint16_t G, uint16_t B)
{
    uint16_t rvalue = 0;
    rvalue += G >> 5;
    rvalue += (G & (0b111)) << 13;
    rvalue += (R >> 3) << 8;
    rvalue += (B >> 3) << 3;
    return rvalue;
}
void display::clear()
{
	this->fillRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0x0000);  // black out the screen
}
uint32_t display::mystrlen(const char *s)
{
	uint32_t count=0;
	while(*s)
	{
		s++;
		count++;
	}
	return count;
}
