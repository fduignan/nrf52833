// implementation for an on-screen keyboard.  A bit hacky but time was short.
#include "osk.h"
static const char row0[]="1234567890";
static const char row1[]="QWERTYUIOP";
static const char row2[]="ASDFGHJKL";
static const char row3[]="ZXCVBNM_";
void osk::show(display * Display, controller * Controller)
{
    
    
    this->Display = Display;
    this->Controller = Controller;
    
    uint32_t c;
    // zero out the input text buffer
    for (c=0;c<sizeof(input_text);c++)
    {
        input_text[c]=0;
    }
    
    drawInputTextBox();
    uint32_t r=9; // This pushes the on-screen keyboard about halfway down the screen
    for (c=0;c<sizeof(row0)-1;c++)
    {
        drawKey(row0[c],c,r,0);
    }
    r++;
    for (c=0;c<sizeof(row1)-1;c++)
    {
        drawKey(row1[c],c,r,0);
    }
    r++;
    for (c=0;c<sizeof(row2)-1;c++)
    {
        drawKey(row2[c],c,r,KEY_WIDTH/2);
    }
    r++;
    for (c=0;c<sizeof(row3)-1;c++)
    {
        drawKey(row3[c],c,r,KEY_WIDTH);
    }
    r++;
    drawControlKeys(2,r);
}
int osk::getUserInput(char *input,fptr UserPollingFunction)
{
    uint32_t col,row,xofs;
    int new_col, new_row;
    uint16_t user_button;
    int done = 0;
    row = 9;
    col = 0;
    new_row=row;
    new_col = col;
    xofs=0;
    char key_press='1';
    int input_index = 0;
	while(input_index < OSK_MAX_INPUT_LEN)
		input_text[input_index++]=0;
	input_index = 0;
    while(input[input_index] != 0)
    {
        input_text[input_index] = input[input_index];
        input_index++;
    }
    input_text[input_index]=0;
    drawInputTextBox();
    while(done==0)
    {
        switch (row) 
        {
            case 9: {
                xofs=0;
                break;
            }
            case 10: {
                xofs=0;
                break;
            }
            case 11: {
                xofs=KEY_WIDTH/2;
                break;
            }
            case 12: {
                xofs=KEY_WIDTH;
                break;
            }
            case 13: {
                xofs=KEY_WIDTH*2;
                break;
            }
        }
        // Show highlighted key
        if (row < 13)
            Display->drawRectangle(xofs+(col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH,KEY_HEIGHT,Display->RGBToWord(0xff,0,0));
        else
            Display->drawRectangle(xofs+(3*col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH*3,KEY_HEIGHT,Display->RGBToWord(0xff,0,0));
        do
        {   
            user_button = Controller->getButtonState();			
			// This section will lock up the user interface so if the user supplied a function that they need polled then call it.
			if (UserPollingFunction) 
				UserPollingFunction();
            k_msleep(20);
        } while(user_button == 0);
        switch(user_button) 
        {
            case Controller->Left : {
                new_col = col-1;
                break;
            }
            case Controller->Right : {
                new_col = new_col+1;
                break;
            }
            case Controller->Up : {
                if (row > 9)
                    new_row = row-1;
                break;
            }
            case Controller->Down : {
                if (row < 13)
                    new_row=row+1;
                break;
            }
            case (Controller->A) : {
                if (row==13)
                {  
                    if (col == 0)
                    {
                        // Done pressed
                        int n=0;
                        while(input_text[n] != 0)
                        {
                            input[n]=input_text[n];
                            n++;
                        } 
                        input[n]=0;
						Display->drawRectangle(xofs+(3*col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH*3,KEY_HEIGHT,Display->RGBToWord(0,0,0xff)); // unhighlight Done
                        return n;
                    }
                    if (col == 1)
                    {
                        // Back space pressed
                        if (input_index > 0)
                        {
                            input_index--;
                            key_press = 0;
                            input_text[input_index] = 0;
                        }
                    }
                }
                
                else if (input_index < OSK_MAX_INPUT_LEN)
                {
                    input_text[input_index]=key_press;
                    input_index++;
                }
                
                drawInputTextBox();
                break;
            }
            case (Controller->B) : {
                // user chose to abort
                return -1; 
            }
        }
        if (new_row == 9) 
        {
            if (new_col < 0)
                new_col = 9;
            if (new_col > 9)
                new_col = 0;
            key_press = row0[new_col];
        }
        if (new_row == 10)
        {
            if (new_col < 0)
                new_col = 9;
            if (new_col > 9)
                new_col = 0;
            key_press = row1[new_col];
        }
        if (new_row == 11)
        {
            if (new_col < 0)
                new_col = 8;
            if (new_col > 8)
                new_col = 0;
        key_press = row2[new_col];            
        }
        if (new_row == 12)
        {
            if (new_col < 0)
                new_col = 7;
            if (new_col > 7)
                new_col = 0;
            key_press = row3[new_col];
        }
        if (new_row == 13)
        {
            if (new_col < 0)
                new_col = 1;
            if (new_col > 1)
                new_col = 0;
        }
        if (row < 13)
            Display->drawRectangle(xofs+(col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH,KEY_HEIGHT,Display->RGBToWord(0,0,0xff));
        else
            Display->drawRectangle(xofs+(3*col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH*3,KEY_HEIGHT,Display->RGBToWord(0,0,0xff));
        row=new_row;
        col=new_col;
       
        while(Controller->getButtonState()) // wait for user to release the button
        {
            k_msleep(50); 
        }
        
    }
    return 0;
}
void osk::drawKey( char key, int col, int row, int xofs)
{
    Display->fillRectangle(xofs+(col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH,KEY_HEIGHT,Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->printX2(&key,1,6+xofs+(col*KEY_WIDTH),1+row*KEY_HEIGHT,Display->RGBToWord(0xff,0xff,0xff),Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->drawRectangle(xofs+(col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH,KEY_HEIGHT,Display->RGBToWord(0,0,0xff));

}
void osk::drawControlKeys(int col, int row)
{
    // Draws two keys : "Done", "Bksp"
    Display->fillRectangle((col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH * 3,KEY_HEIGHT,Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->printX2("Done",4,6+(col*KEY_WIDTH),1+row*KEY_HEIGHT,Display->RGBToWord(0xff,0xff,0xff),Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->drawRectangle((col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH*3,KEY_HEIGHT,Display->RGBToWord(0,0,0xff));
    
    col = col + 3;
    Display->fillRectangle((col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH * 3,KEY_HEIGHT,Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->printX2("Back",4,6+(col*KEY_WIDTH),1+row*KEY_HEIGHT,Display->RGBToWord(0xff,0xff,0xff),Display->RGBToWord(0x0f,0x0f,0x0f));
    Display->drawRectangle((col*KEY_WIDTH),row*KEY_HEIGHT,KEY_WIDTH*3,KEY_HEIGHT,Display->RGBToWord(0,0,0xff));

}
void osk::drawInputTextBox()
{
    int row=8;
    Display->fillRectangle(1,1+row*KEY_HEIGHT-2,KEY_WIDTH*10-3,KEY_HEIGHT-2,Display->RGBToWord(0,0,0));
    Display->printX2(input_text,Display->mystrlen(input_text),1,1+row*KEY_HEIGHT-2,Display->RGBToWord(0xff,0xff,0xff),Display->RGBToWord(0x0,0x0,0x0));   
    Display->drawRectangle(0,row*KEY_HEIGHT-2,KEY_WIDTH*10-1,KEY_HEIGHT,Display->RGBToWord(0xf,0xf,0xf));
}

