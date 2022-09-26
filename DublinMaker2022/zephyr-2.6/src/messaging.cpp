#include "console.h"
#include "controller.h"
#include "mesh.h"
#include "osk.h"
#include "font5x7.h"

class ScrollingTextBox {
#define MAX_LINES 6	
public:
	ScrollingTextBox() {
		CursorRow = 0;
	}
	void clear()
	{
		CursorRow = 0;
	}
	void appendLine(char *Line);	
	void show();
private:
uint32_t CursorRow;
char Buffer[MAX_LINES][MAX_MESSAGE_LEN];
};

void ScrollingTextBox::appendLine(char *Line)
{
	if (CursorRow < MAX_LINES)
	{
		for (uint32_t c=0;c<MAX_MESSAGE_LEN;c++)
		{
			Buffer[CursorRow][c]=Line[c];
		}	
		CursorRow++;
	}
	else
	{
		// scroll the other lines 
		for (uint32_t r=1;r<MAX_LINES;r++)
		{
			for (uint32_t c=0;c<MAX_MESSAGE_LEN;c++)
			{
				Buffer[r-1][c]=Buffer[r][c];
			}
		}
		for (int c=0;c<MAX_MESSAGE_LEN;c++)
		{
			Buffer[MAX_LINES-1][c]=Line[c];
		}
	
	}
}
void ScrollingTextBox::show()
{
	Console.Display.fillRectangle(0,0,SCREEN_WIDTH,6*FONT_HEIGHT*2,Console.Display.RGBToWord(0,0,0));
	for (uint32_t r=0;r<CursorRow;r++)
	{
		int len=0;
		while(len < MAX_MESSAGE_LEN)
		{
			if (Buffer[r][len]==0)
				break;
			len++;
		}
		Console.Display.printX2((char *)&Buffer[r][0],len,0,r*(2*FONT_HEIGHT+3),Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0));
	}
}
ScrollingTextBox tb;
void ShowIncomingMessages(void)
{
	if (DMBMessageReceived)
	{
		int len=0;
		while(len < MAX_MESSAGE_LEN)
		{
			if (DMBMailBox.Message[len]==0)
				break;
			len++;
		}
		tb.appendLine((char *)DMBMailBox.Message);
		tb.show();
		
		DMBMessageReceived=0;
	}
}

void messaging(int other_address)
{
	osk OSK;
    uint16_t target=0;
    int Done=0;	
	mesh_clearReplay();
    Console.Display.clear();
	// Choose target node
	if (other_address == 0)
	{
		Console.Display.printX2("Target node: ", 13,0,0,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
		while(!Done)
		{
			Console.Display.printX2(target,0,1+FONT_HEIGHT*2, Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
			uint16_t key=0;
			while(key == 0)
			{
				key=Console.Controller.getButtonState();
				k_msleep(50);
			}
			switch(key)
			{
				case Console.Controller.Up :
				{
					target++;
					break;
				}
				case Console.Controller.Down :
				{
					target--;
					break;
				}
				case Console.Controller.A :
				{
					Done=1;
					break;
				}
				case Console.Controller.B :
				{
					Done=2;
					break;
				}
			}
			k_msleep(100);		
		}
	}
	else
	{
		target = other_address;
		Done=1;
	}
	if (Done==1) 
	{
		Done = 0;	
		while(!Done){
			Console.Display.clear();
			Console.Display.printX2("Target node: ", 13,0,0,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
			Console.Display.printX2(target,0,(FONT_HEIGHT*2)+1,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
			Done = 0;
			while (Done == 0)
			{
				// Enter messaging phase			
				OSK.show(&Console.Display, &Console.Controller);
				char word[MAX_MESSAGE_LEN];
				for (int w=0;w<MAX_MESSAGE_LEN;w++)
					word[w]=0;
				while (OSK.getUserInput(word,ShowIncomingMessages)>0)
				{
					dmb_message msg;
					for (int i=0;i<MAX_MESSAGE_LEN;i++)
					{
						msg.Message[i]=word[i];
						word[i]=0;
					}				
					sendDMBMessage(target, &msg);
					
				}
				Done=1;
			}
		}
	}
}
