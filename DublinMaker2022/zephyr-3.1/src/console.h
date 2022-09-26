#ifndef __console_h
#define __console_h
#include <stdint.h>
#include "display.h"
#include "controller.h"
#include "sound.h"
#define COLOUR(R,G,B) RGBTOWORD(R,G,B)

class console
{
public:    
    // various support functions 
    uint32_t random(uint32_t lower,uint32_t upper);
// public member objects
    
    controller Controller;
    display Display; // public but really should be used only internally (isolates the console from hardware dependencies)
    sound Sound;
// various functions that need to be declared but are handled internally (don't call them)    
    console(){}; // Default constructor does nothing.
    int begin(); // initialize console and sub objects (called from init.cpp on boot) 
    void sleep(uint32_t ms);
private:    

   	unsigned long shift_register;
    void random_seed(); // seed the PRBS generator with a 'random' value
    uint32_t prbs();    // get next prbs output
};
extern console Console;  // The console object is declared/created in console.cpp
#endif
