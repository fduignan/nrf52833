#ifndef __display_h
#define __display_h
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
class display {
public:
	int begin();
	void command(uint8_t cmd);
	void data(uint8_t data);
	void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void putPixel(uint16_t x, uint16_t y, uint16_t colour);
	void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *Image);
	void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour);
	int iabs(int x); // simple integer version of abs for use by graphics functions        
	void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour);
	void fillRectangle(uint16_t x,uint16_t y,uint16_t width, uint16_t height, uint16_t colour);
	void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
	void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
	void print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
	void printX2(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
	void print(uint16_t number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
    void printX2(uint16_t number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
	uint16_t RGBToWord(uint16_t R, uint16_t G, uint16_t B);
	void drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
	void drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);    
	void hw_test(void);
	void setBrightness(uint32_t pcnt);
	void clear();
    uint32_t mystrlen(const char *s);

};
#endif
