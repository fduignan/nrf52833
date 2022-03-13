#include <stdint.h>
#include "console.h"
#include "sprite.h"
class brick: public sprite {
public:
    brick( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolour);    
    brick();
    void define(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t fillcolour);    
    virtual void show();
    virtual void hide();
private:   
    uint16_t fillcolour;    

};



