#ifndef __OSK_H
#define __OSK_H
// on-screen keyboard
#include <stdint.h>
#include "display.h"
#include "controller.h"
#include "font5x7.h"
#include "menu.h"
#include "dmb_message_type.h"
#define OSK_ROWS 5
#define OSK_COLS 10
// use full screen width for keyboard
#define KEY_WIDTH (SCREEN_WIDTH/OSK_COLS)
// use only part of screen height for keyboard
#define KEY_HEIGHT (2*FONT_HEIGHT+2)


typedef void (*fptr)(void);

class osk 
{
public:
    osk(){}; // default constructor does nothing
    void show(display * Display, controller * Controller);
    //int getUserInput(char *input);
	int getUserInput(char *input,fptr UserPollingFunction=NULL);

private:
    void drawKey( char key, int col, int row, int xofs);
    void drawControlKeys(int col, int row);
    void drawInputTextBox();
    display * Display;
	controller * Controller;   
    
#define OSK_MAX_INPUT_LEN   MAX_MESSAGE_LEN
    char input_text[OSK_MAX_INPUT_LEN+1];
};
#endif
