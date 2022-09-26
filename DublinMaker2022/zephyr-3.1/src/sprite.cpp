#include <stdint.h>
#include "console.h"
#include "sprite.h"
sprite::sprite(const uint16_t * img, uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
    X = x;
    Y = y;
    this->w=w;
    this->h=h;
    this->img=img;
    Visible = 0;      
}
sprite::sprite()
{
    Visible = 0;      
}
void sprite::show() {
    if (!Visible)
    {
        Console.Display.putImage(X,Y,w,h,(uint16_t *)img);
    }
    Visible=1;
}
void sprite::redraw() 
{
    if (Visible)
        show();
}
void sprite::hide() {
    if (Visible)
    {
        // assumes a black backgound (colour = 0)
        Console.Display.fillRectangle(X,Y,w,h,0);
    }
    Visible = 0;
}

void sprite::move(uint16_t x, uint16_t y) {
    if (Visible)
    {
        // Hide the object first
        hide();
        X = x;
        Y = y;
        // Now show it
        show();
    }
    else
    {
        X = x;
        Y = y;
    }
}
uint16_t sprite::within(uint16_t x, uint16_t y)
{
    if (Visible == 0)
        return 0;
    if ( (x>=X) && ( x < (X+w) ) )
    {
        if ( (y>=Y) && ( y < (Y+h) ) )
            return 1;
        else
            return 0;
        
    }
    else
        return 0;
    
}
uint16_t sprite::touching(uint16_t x, uint16_t y)
{
    // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    if (Visible == 0)
        return 0;
    if ( y == Y  )
    {  // top face?
        if ( (x>=X) && ( x < (X+w) ) )
            return 1;      
    }
    if ( x == X )
    {
        // left face
        if ( (y>=Y) && ( y < (Y+h) ) )
            return 2;
    }
    if ( y == (Y+h-1)  )
    {  // bottom face?
        if ( (x>=X) && ( x < (X+w) ) )
            return 3;      
    }
    if ( x == (X + w-1) )
    {
        // right face
        if ( (y>=Y) && ( y < (Y+h) ) )
            return 4;
    }
    
    return 0; // not touching
}
