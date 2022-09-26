#include <stdint.h>
#include "menu.h"
#include "font5x7.h"
void menu::show()
{
	//Display->clear();
    int len;
    uint16_t w,h;
    w=h=0;
	for(uint32_t i=0;i< this->count;i++)
	{
		
        len=Display->mystrlen(this->choices[i]);
        if (i == choice)
        {
            Display->printX2(this->choices[i],len,FONT_WIDTH*2,i*((FONT_HEIGHT*2)+1),Display->RGBToWord(0xff,0xff,0),0);
        }
        else
        {
            Display->printX2(this->choices[i],len,FONT_WIDTH*2,i*((FONT_HEIGHT*2)+1),Display->RGBToWord(0xff,0xff,0xff),0);
        }
        len=len*(FONT_WIDTH*2+1);
        if (w < len)
            w = len;
	}
	h=this->count*(FONT_HEIGHT*2+1);
	Display->drawRectangle(0,0,w+FONT_WIDTH*5,h,Display->RGBToWord(0xff,0,0));
}
int menu::getChoice(uint32_t timeout)
{
	uint32_t loop_count=0;
	choice = 0;
	int Done=0;
	while(Done == 0)
	{
		int state=0;
		while(state==0)
		{	
			k_msleep(50); // debounce and sleep a bit			
			state=Controller->getButtonState();
			loop_count +=50;
			if (timeout)
			{
				if (loop_count>=timeout)
				{
					return -1;
				}
			}
		}
		switch (state)
		{
			
			case Controller->Down: {
                // remove highlight from previous selection
                Display->fillRectangle(0,(choice*FONT_HEIGHT*2)+1,FONT_WIDTH,FONT_HEIGHT*2,Display->RGBToWord(0,0,0));
				choice = (choice + 1) % this->count;
                show();
                // highlight current selection
                Display->fillRectangle(0,(choice*FONT_HEIGHT*2)+1,FONT_WIDTH,FONT_HEIGHT*2,Display->RGBToWord(0xff,0xff,0));  
				loop_count=0;
				break;
			}
			case Controller->Up: {
                // remove highlight from previous selection
                Display->fillRectangle(0,(choice*FONT_HEIGHT*2)+1,FONT_WIDTH,FONT_HEIGHT*2,Display->RGBToWord(0,0,0));
                if (choice > 0)
				{
                    choice = (choice - 1);
				}
				else
                {
                    choice = this->count - 1;
                }
                show();
                // highlight current selection				
                Display->fillRectangle(0,(choice*FONT_HEIGHT*2)+1,FONT_WIDTH,FONT_HEIGHT*2,Display->RGBToWord(0xff,0xff,0));
				loop_count=0;
				break;
			}
			case Controller->A: {
				// OK
				Done = 1;
				break;
			}
			case Controller->B: {
				// Cancel
				Done = -1;
				break;
			}
		}
		while(Controller->getButtonState()) // wait for key release
		{
			k_msleep(50);
		}
	}
	if (Done == 1)
		return choice;
	else
		return -1;
}
