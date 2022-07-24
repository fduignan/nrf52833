#ifndef __MENU_H
#define __MENU_H
#include <stdint.h>
#include "display.h"
#include "controller.h"

// menu.h.  Implements a simple menuing system using a display and a controller object
class menu {
public:
	menu(display * Display, controller * Controller, const char **choices, uint32_t count)
	{
		this->Display = Display;
		this->Controller = Controller;
		this->choices = choices;
		this->count = count;
        choice=0;
	}
	void show();
	int getChoice(uint32_t timeout=0);
private:
	display * Display;
	controller * Controller;
	const char **choices;
	uint32_t count;
    uint32_t choice;
};

#endif
