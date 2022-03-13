#include "console.h"
#include "font5x7.h"
console Console;
int console::begin()
{    
    Display.begin();    
    Controller.begin();
    Sound.begin();
    random_seed();
    return 0;
}

uint32_t console::random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}
uint32_t console::prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's 
}
void console::random_seed()
{
    // Need to find a source of random data.  This layer of BBG is meant to be 
    // hardware independent so I don't want to break that.  Will have to obtain 
    // a random seed from one of the lower layers of the BBG stack without introducing
    // additional complexity.  Hardware aware object choices: Controller, Display, Timer, Sound
    // An analogue input would be nice so this probably rules out the Display and Timer objects
    // leaving Sound and Controller.  Some variations of BBG may use an analogue joystick so 
    // perhaps a call to readAnalogue(chan_num) might be a good choice?  The channel number
    // can be defined in the controller class (as a static) and mapped appropriately within the
    // Controller object.  Will add up and scale a few reads to change more bits in the shift register
    shift_register = 0x12345678;
	return;
    while(shift_register == 0)
    {
        for (int i=1;i<17;i++)
            shift_register += Controller.readTemperature()<<i;
    }   
    Controller.stopADC();
}
void console::sleep(uint32_t ms)
{
	k_msleep(ms);
}
