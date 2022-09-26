#ifndef __SPRITE_H
#define __SPRITE_H
#include <stdint.h>
#include "console.h"
extern console Console;
class sprite {
public:
    sprite();
    sprite( const uint16_t * image, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    virtual void show();
    virtual void hide();
    void redraw();    
    void move(uint16_t x, uint16_t y); // move the sprite (if the sprite is hidden it remains so)
    uint16_t within(uint16_t x, uint16_t y); // returns 1 if the specified point is within the sprite
    // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    uint16_t touching(uint16_t x, uint16_t y); 
    inline uint16_t getX() {
        return X;
    }
    inline uint16_t getY() {
        return Y;
    }
    inline uint8_t getWidth() {
        return w;
    }
    inline uint8_t getHeight() {
        return h;
    }
    inline uint16_t visible() {    
        return Visible;
    }
    
protected:   
    uint16_t w,h;
    const uint16_t *img;
    uint16_t X,Y;
    uint8_t Visible;    
};
#endif
