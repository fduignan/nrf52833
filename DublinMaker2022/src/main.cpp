
#include <zephyr.h>
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <drivers/pwm.h>
#include <drivers/flash.h>
#include <storage/flash_map.h>
#include <fs/nvs.h>
//#include <stdlib.h>
#include "display.h"
#include "controller.h"
#include "console.h"
#include "sound.h"
#include "invaders.h"
#include "brici.h"
#include "mesh.h"
#include "battleship.h"
#include "messaging.h"
#include "frame_buffer.h"
#include "menu.h"
#include "osk.h"
#include "dmb_message_type.h"
#include "realm.h"
#include "logo_bricks.h"
extern console Console;

int doMainMenu()
{
	const char c1[]="Galaga";
	const char c2[]="Brici";
	const char c3[]="Battleship";
	const char c4[]="Microrealms";
    const char c5[]="Messaging";
	const char c6[]="Settings";
	const char c7[]="About";
	
	
	const char *choices[]={c1,c2,c3,c4,c5,c6,c7};
	menu Menu(&Console.Display, &Console.Controller, choices,sizeof(choices)/sizeof(const char *));
	Menu.show();	
	return Menu.getChoice(2000);
}

char Name[MAX_NAME_LEN];
uint16_t Address;

int load_settings(void);
int change_settings(void);
void drawLogo(uint32_t demo_mode);
void about(void);
void main(void)
{
	
	
	int ret;
	int count=0;		
	ret = Console.begin();		
    load_settings();
	mesh_begin(Address);
	if (ret < 0)
	{
		printk("\nError initializing console.  Error code = %d\n",ret);
		while(1);
	}
	
	uint16_t tones[]={A3,A5,E4,G4,A5};
	uint16_t times[]={150,150,150,150,250};
    Console.Sound.playMelody(tones, times,5);
	k_msleep(50);
	drawLogo(3);
	while(Console.Controller.getButtonState()==0);
    while(1) {
		Console.Display.clear();
		ret = doMainMenu();
		printk("Choice=%d\r\n",ret);
		if (ret==0)
		{
			playInvaders(0);
		}
		if (ret == 1)
		{
			ClassicBrici();
		}
		if (ret == 2)
		{
			// Settings
			battleship();
		}
		if (ret == 3)
		{
			// Settings
			microrealms();
		}
		if (ret ==4 )
		{
			messaging();
		}
		if (ret == 5)
		{
			change_settings();
		}
		if (ret == 6)
		{
			about();
		}

		if (ret < 0) // timeout
			drawLogo(3);

	}
	playInvaders(1);
 	
	while(1)
	{
		//printk("Hello World! %s\n", CONFIG_BOARD);
       //Display.drawRectangle(random(0,119),random(0,119),random(0,120),random(0,120),random(0,0xffff));                                
       //Console.Display.fillRectangle(Console.random(0,240),Console.random(0,240),Console.random(0,240),Console.random(0,240),Console.random(0,0xffff));                   
		Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,Console.random(0,0xffff));                   
		///Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0b1111111111111111);
		///k_msleep(100);
		//Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0b1111111111011111);                   
		//k_msleep(100);
       // display_drawCircle(random(0,240),random(0,240),random(0,135),random(0,0xffff));
        //Display.fillCircle(random(0,240),random(0,135),random(0,120),random(0,0xffff));

        count++;
        if (count >= 10) 
        {
      //      Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
			
		//	Console.Display.print("Starting over",sizeof("Starting over")-1,20,60,0xffff,0);
            count = 0;
		//	sendDMBMessage(msg_count++);

			//printk("restarting\n");
			if (Console.Controller.getButtonState())
			Console.Sound.playTone(1000,100);
			printk("Buttons = %x\n",Console.Controller.getButtonState());
			
 		//	printk("Buttons = %d\r\n",Console.Controller.getButtonState());
        }        
	}
}
static struct nvs_fs fs;
const struct device *flash_dev;

#define STORAGE_NODE DT_NODE_BY_FIXED_PARTITION_LABEL(storage)
#define FLASH_NODE DT_MTD_FROM_FIXED_PARTITION(STORAGE_NODE)
#define NAME_ID 1
#define ADDRESS_ID 2
int load_settings()
{
    int ret;
    struct flash_pages_info info;
    flash_dev = DEVICE_DT_GET(FLASH_NODE);
    if (!device_is_ready(flash_dev)) {
            printk("Flash device %s is not ready\n", flash_dev->name);
            return -1;
    }
    fs.offset = FLASH_AREA_OFFSET(storage);
    ret = flash_get_page_info_by_offs(flash_dev, fs.offset, &info);
    if (ret) {
            printk("Unable to get page info\n");
            return -2;
    }
    fs.sector_size = info.size;
    fs.sector_count = 2U;

    ret = nvs_init(&fs, flash_dev->name);
    if (ret) 
    {
        printk("Flash Init failed\n");
        return -3;
    }
    else
    {
        printk("Flash init success\n");
    }
    ret = nvs_read(&fs,NAME_ID,&Name,sizeof(Name));
    if (ret < 0)
    {
        Name[0]='D';
        Name[1]='U';
        Name[2]='B';
        Name[3]=0;
    }
    ret = nvs_read(&fs,ADDRESS_ID,&Address,sizeof(Address));
    if (ret < 0)
    {
        Address=1;
    }
    return 0;
    
}
int change_settings()
{
    int rc = 0;
	osk OSK;
	
    struct flash_pages_info info;

    flash_dev = DEVICE_DT_GET(FLASH_NODE);
    if (!device_is_ready(flash_dev)) {
            printk("Flash device %s is not ready\n", flash_dev->name);
            return -1;
    }
    fs.offset = FLASH_AREA_OFFSET(storage);
    rc = flash_get_page_info_by_offs(flash_dev, fs.offset, &info);
    if (rc) {
            printk("Unable to get page info\n");
            return -2;
    }
    fs.sector_size = info.size;
    fs.sector_count = 2U;

    rc = nvs_init(&fs, flash_dev->name);
    if (rc) 
    {
        printk("Flash Init failed\n");
        return -3;
    }
    else
    {
        printk("Flash init success\n");
    }
    rc = nvs_read(&fs,NAME_ID,&Name,sizeof(Name));
    Console.Display.clear();
    Console.Display.printX2("Badge name",10,0,0,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
    Console.Display.printX2(Name,Console.Display.mystrlen(Name),0,1+FONT_HEIGHT*2, Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
    OSK.show(&Console.Display, &Console.Controller);
    if (OSK.getUserInput(Name)>0)
    {
        rc=nvs_write(&fs,NAME_ID,&Name,sizeof(Name));
    }
    Console.Display.clear();
    Console.Display.printX2("Badge name",10,0,0,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
    Console.Display.printX2(Name,Console.Display.mystrlen(Name),0,1+FONT_HEIGHT*2, Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
    while(Console.Controller.getButtonState()); // wait for key release
    Console.Display.clear();
    int Done=0;
    Console.Display.printX2("Badge address",13,0,0,Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
    while(!Done)
    {
        Console.Display.printX2(Address,0,1+FONT_HEIGHT*2, Console.Display.RGBToWord(0xff,0xff,0), Console.Display.RGBToWord(0,0,0) );
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
                Address++;
                break;
            }
            case Console.Controller.Down :
            {
                Address--;
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
        k_msleep(50);
        if (Done == 1)
        {
            // new address entered
            rc=nvs_write(&fs,ADDRESS_ID,&Address,sizeof(Address));
        }
    }
    return 0;
}
void drawLogo(uint32_t demo_mode)
{
    brick Logo[LOGO_BRICK_COUNT];
    int Count;
    volatile int BrickDataIndex = 0;    
    uint16_t x,y,w,h;
    uint16_t r,g,b;
    uint16_t Colour;
    uint16_t Brick_Count = LOGO_BRICK_COUNT;
 // The logo is defined in the header file  logo_bricks.h
// The colours are defined in this file using just 2 bits for each colour component (r,g,b): total 6 bits of colour
// The following loop defines all of the bricks in memory and "expands" their colour definition to the 16 bits required by the display
    Console.Display.fillRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
    for (Count = 0; Count < LOGO_BRICK_COUNT; Count++)
    {        
        x = brickattribs[BrickDataIndex];
        y = brickattribs[BrickDataIndex + 1];
        w = brickattribs[BrickDataIndex + 2];
        h = brickattribs[BrickDataIndex + 3];
        Colour = brickattribs[BrickDataIndex + 4];
        r = (Colour & 0x30) >> 4;
        switch (r) {
            case 0: r=0;break;
            case 1: r=63;break;
            case 2: r=127;break;
            case 3: r=255;break;
        }
        g = (Colour & 0x0c) >> 2;
        switch (g) {
            case 0: g=0;break;
            case 1: g=63;break;
            case 2: g=127;break;
            case 3: g=255;break;
        }
        b = (Colour & 0x03);
        switch (b) {
            case 0: b=0;break;
            case 1: b=63;break;
            case 2: b=127;break;
            case 3: b=255;break;
        }
        Colour = COLOUR(r,g,b); // convert the 3 x 8 bit values to a single 16 bit value        
        Logo[Count].define(x,y,w,h,Colour);                           
        BrickDataIndex += 5;
        
    }   
    if (demo_mode == 0)
    {
        for (Count = 0; Count < LOGO_BRICK_COUNT; Count++)
        {
            Logo[Count].show();        
        }

    }
    if (demo_mode==1)
    {
        for (Count = 0; Count < LOGO_BRICK_COUNT; Count++)
        {
            Logo[Count].show();        
        }
        Brici(Logo,Brick_Count,SCREEN_WIDTH,1);
    }
    if (demo_mode==2)
    {
        sprite Invader = { DefaultInvaderImage, 3, 3, 10, 12};
        sprite AttackerMissile = { AttackerMissileImage, 0, 0, 5, 8  };
        Invader.show();
        Invader.move(Invader.getX()+Console.random(0,200),Invader.getY());
        int XDirectionChange = 0;
        int YDirectionChange = 0;
        int x_step = 2;
        int y_step = 1;
        for (Count = 0; Count < LOGO_BRICK_COUNT; Count++)
        {
            Logo[Count].move(Logo[Count].getX(),Logo[Count].getY()+120);
            Logo[Count].show();                    
        }
       
        for (Count = 0; Count < 4000; Count++)
        {
            if (Console.Controller.getButtonState()) // exit demo if the user presses a button
                return;
            XDirectionChange=0;
            YDirectionChange=0;
            Invader.move(Invader.getX() + x_step, Invader.getY() + y_step);
            if (Invader.getX() >= (SCREEN_WIDTH - Invader.getWidth()))
                XDirectionChange = 1;
            if (Invader.getX() == 0)
                XDirectionChange = 1;
            if (Invader.getY() > SCREEN_HEIGHT - 140)
            {
                YDirectionChange = 1;
                if (Invader.getY() < 3)
                {
                    YDirectionChange = 1;
                }
            }
            if (XDirectionChange) // Did an invader hit either edge?
                x_step = -x_step; // if so, then reverse direction
            if (YDirectionChange) // Did an invader hit either edge?
                y_step = -y_step; // if so, then reverse direction
            if (AttackerMissile.visible())
            {
                int16_t MissileTipX,MissileTipY;
                MissileTipX = AttackerMissile.getX()+AttackerMissile.getWidth() / 2;
                MissileTipY = AttackerMissile.getY()+AttackerMissile.getHeight() /2;
                for (int Index = 0; Index < LOGO_BRICK_COUNT; Index++)
                {
                    if (Logo[Index].visible())
                    {
                        if (Logo[Index].within(MissileTipX,MissileTipY))
                        {
                            Logo[Index].hide();
                            AttackerMissile.hide();
                           // Console.Sound.playTone(400,20);
                        }
                    }                                        
                }
                AttackerMissile.move(AttackerMissile.getX(),AttackerMissile.getY()+2);
                if (AttackerMissile.getY() > SCREEN_HEIGHT)
                    AttackerMissile.hide();
            }
            else
            {
                AttackerMissile.show();
                AttackerMissile.move(Invader.getX() + Invader.getWidth() / 2 - AttackerMissile.getWidth() / 2, Invader.getY() + AttackerMissile.getWidth() / 2);
            }
            k_msleep(10);
            
        }
    }
	if (demo_mode==3)
    { // Spire as a radio beacon
		for (Count = 0; Count < LOGO_BRICK_COUNT; Count++)
        {
			Logo[Count].move(Logo[Count].getX(),Logo[Count].getY()+60);
            Logo[Count].show();        
        }
		Console.Display.printX2("Address: ",9,0,SCREEN_HEIGHT - (6*FONT_HEIGHT),Console.Display.RGBToWord(0xff,0xff,0x03f),Console.Display.RGBToWord(0,0,0));		
		Console.Display.printX2(Address,10*FONT_WIDTH*2,SCREEN_HEIGHT - (6*FONT_HEIGHT),Console.Display.RGBToWord(0xff,0xff,0x03f),Console.Display.RGBToWord(0,0,0));
		DMBMessageReceived = 0;
		while(Console.Controller.getButtonState()==0)
		{
			Console.Display.putPixel(120,65,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(137,65,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(116,58,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(141,58,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(120,51,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(137,51,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(128,46,Console.Display.RGBToWord(0xff,0xff,0xff));
			Console.Display.putPixel(129,46,Console.Display.RGBToWord(0xff,0xff,0xff));
			k_msleep(100);
			Console.Display.putPixel(120,65,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(137,65,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(116,58,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(141,58,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(120,51,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(137,51,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(128,46,Console.Display.RGBToWord(0,0,0));
			Console.Display.putPixel(129,46,Console.Display.RGBToWord(0,0,0));
			k_msleep(100);
			if (DMBMessageReceived)
				messaging(DMBMessageSender);
		}
	}

}
void about()
{
	uint16_t row_pos = 0;
	Console.Display.clear();
	Console.Display.printX2("For software updates",sizeof("For software updates")-1,0,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0,0,0));
	row_pos += 2 + (FONT_HEIGHT*2);
	Console.Display.printX2("help with code",sizeof("help with code")-1,0,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0,0,0));
	row_pos += 2 + (FONT_HEIGHT*2);
	Console.Display.printX2("contact:",sizeof("contact:")-1,0,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0,0,0));
	row_pos += 2 + (FONT_HEIGHT*2);
	Console.Display.printX2("frank@ioprog.com",sizeof("frank@ioprog.com")-1,0,row_pos,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0,0,0));
	while(Console.Controller.getButtonState()==0)
		k_msleep(50);
	while(Console.Controller.getButtonState()!=0)
		k_msleep(50);
}
