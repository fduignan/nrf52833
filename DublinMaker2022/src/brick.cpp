    #include <stdint.h>
#include "console.h"
#include "sprite.h"
#include "brick.h"

brick::brick( uint16_t x, uint16_t y, uint16_t w, uint16_t h,  uint16_t fillcolour)
{
    
    this->Visible = 0;        
    define(x,y,w,h,fillcolour);
}
brick::brick()
{
    this->Visible = 0;
}

void brick::define( uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t fillcolour)
{       
    this->fillcolour = fillcolour;
    this->Visible = 0;    
    this->w = w;
    this->h = h;
    this->X = x;
    this->Y = y;    
}

void brick::show()
{
    //if (!Visible)
    {                
        Console.fillRectangle(X,Y,w,h,fillcolour);           
    }
    Visible=1;
    
}

void brick::hide()
{
    if (Visible)
    {
        // assumes a black backgound (colour = 0)
        Console.fillRectangle(X,Y,w,h,0);
    }
    Visible = 0;
}
