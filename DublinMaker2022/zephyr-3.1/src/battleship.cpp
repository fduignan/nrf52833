#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include "mesh.h"
#include "display.h"
#include "console.h"
#include "controller.h"
#include "battleship.h"
#include "font5x7.h"
void ship::place(int len, int row, int col, int orientation)
{
    this->len = len;
    this->row = row;
    this->col = col;
    this->orientation = orientation;
    
    state = 0;
}
int ship::contains(int row, int col)
{
    // returns 1 if the ship occupies the square at this row, column
    // otherwise returns 0
    int return_value = 0;
    if (orientation == 0)
    {
        // Horizontal
        if (this->row == row) 
        {
            // same row.
            if ( (col >= this->col) && (col < (this->col+this->len)))
                return_value = 1; // it is within
        }
    }
    else
    {
        // Vertical
        if (this->col == col)
        {
            if ( (row >= this->row) && (row < (this->row+this->len)))
                return_value = 1; // it is within
        }
    }
    return return_value; // not within the bounds of this ship
}
int ship::overlaps(int len, int row, int col, int orientation)
{
    // returns 1 if the ship described by the parameters overlaps
    // this ship - uses for initial ship placement.
    // otherwise returns 0
    int return_value = 0;
    if (orientation == 0)
    {
        // Horizontal
        for (int count = 0; count < len; count++)
        {
            if (this->contains(row,col+count))
                return_value = 1;
        }
    }
    else
    {
        // Vertical
        for (int count = 0; count < len; count++)
        {
            if (this->contains(row+count,col))
                return_value = 1;
        }
    }
    return return_value;
}
/* each square can have the following properties:
 * Empty
 * Your ship (ship_id with particular bit pattern for you/them)
 * Their ship
 * Your ship hit
 * Their ship hit
 * uint8_t my_ship_map[10][10];
 */

void battleship()
{
    int row,col;
    uint8_t map[10][10];
    int ship_counter = 0;
    int Done=0;
    uint16_t target=0;
    int my_ship_hit_counter = 0;
    int their_ship_hit_counter = 0;
    int total_hits;
    total_hits = (CARRIER_COUNT*CARRIER_SQUARES) + (BATTLESHIP_COUNT*BATTLESHIP_SQUARES)+(SUBMARINE_COUNT*SUBMARINE_SQUARES)+(PATROL_BOAT_COUNT*PATROL_BOAT_SQUARES); 
	mesh_clearReplay();

    Console.Display.clear();
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

    ship Ships[CARRIER_COUNT+BATTLESHIP_COUNT+SUBMARINE_COUNT+PATROL_BOAT_COUNT];
    Console.Display.clear();
    
    for (row=0;row<10;row++)
    {
        Console.Display.drawLine(0,row*(SCREEN_HEIGHT/10),SCREEN_WIDTH,row*(SCREEN_HEIGHT/10),Console.Display.RGBToWord(0xf,0xf,0xff));
    }
    for (col=0;col<10;col++)
    {
        Console.Display.drawLine(col*(SCREEN_WIDTH/10),0,col*(SCREEN_WIDTH/10),SCREEN_HEIGHT,Console.Display.RGBToWord(0xf,0xf,0xff));
    }
    Console.Display.drawLine(0,SCREEN_HEIGHT-1,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,Console.Display.RGBToWord(0xf,0xf,0xff));
    Console.Display.drawLine(SCREEN_WIDTH-1,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,Console.Display.RGBToWord(0xf,0xf,0xff));
    for (row=0;row<10;row++)
    {
        for (col=0;col<10;col++)
            map[row][col]=0;
    }
    // start placing ships
    // Aircraft carrier first as it will be hardest to position.
    int orientation = Console.random(0,2);
    if (orientation == 0)
    {
        // Horizontal
        col=Console.random(0,CARRIER_SQUARES);
        row=Console.random(0,10);
        for (int i=0;i<CARRIER_SQUARES;i++)
            map[row][col+i]=MY_SHIP_NORMAL;
    }
    else
    {        
        col=Console.random(0,10);
        row=Console.random(0,CARRIER_SQUARES);
        for (int i=0;i<CARRIER_SQUARES;i++)
            map[row+i][col]=MY_SHIP_NORMAL;
    }
    Ships[ship_counter].place(CARRIER_SQUARES,row,col,orientation);
    ship_counter++;
    
    // Place Battleships
    int len = BATTLESHIP_SQUARES;    
    for (int ship_class_count = 0; ship_class_count < BATTLESHIP_COUNT; ship_class_count++)
    {
        Done = 0;    
        while( !Done )
        {
            // Attempt to get an empty part of the board;
            orientation = Console.random(0,2);
            if (orientation == 0)
            {
                // Horizontal
                col=Console.random(0,len);
                row=Console.random(0,10);
            }
            else
            {        
                col=Console.random(0,10);
                row=Console.random(0,len);
            }
            int invalid = 0;
            for (int ship_index=0; ship_index < ship_counter; ship_index++)
            {
                if (Ships[ship_index].overlaps(len,row,col,orientation))
                    invalid = 1;
                
            }
            if (invalid == 0)
                Done = 1;
        }
        if (orientation == 0)
        {
            for (int i=0;i<len;i++)
                map[row][col+i]=MY_SHIP_NORMAL;
        }
        else
        {
            for (int i=0;i<len;i++)
                map[row+i][col]=MY_SHIP_NORMAL;
        }
        Ships[ship_counter].place(BATTLESHIP_SQUARES,row,col,orientation);
        ship_counter++;
    }
    
    // Place submarines
    len = SUBMARINE_SQUARES;
    for (int ship_class_count = 0; ship_class_count < SUBMARINE_COUNT; ship_class_count++)
    {
        Done = 0;    
        while( !Done )
        {
            // Attempt to get an empty part of the board;
            orientation = Console.random(0,2);
            if (orientation == 0)
            {
                // Horizontal
                col=Console.random(0,len);
                row=Console.random(0,10);
            }
            else
            {        
                col=Console.random(0,10);
                row=Console.random(0,len);
            }
            int invalid = 0;
            for (int ship_index=0; ship_index < ship_counter; ship_index++)
            {
                if (Ships[ship_index].overlaps(len,row,col,orientation))
                    invalid = 1;
                
            }
            if (invalid == 0)
                Done = 1;
        }
        if (orientation == 0)
        {
            for (int i=0;i<len;i++)
                map[row][col+i]=MY_SHIP_NORMAL;
        }
        else
        {
            for (int i=0;i<len;i++)
                map[row+i][col]=MY_SHIP_NORMAL;
        }
        Ships[ship_counter].place(SUBMARINE_SQUARES,row,col,orientation);
        ship_counter++;
    }

    // Place Patrol boats
    len = PATROL_BOAT_SQUARES;    
    for (int ship_class_count = 0; ship_class_count < PATROL_BOAT_COUNT; ship_class_count++)
    {
        Done = 0;    
        while( !Done )
        {
            // Attempt to get an empty part of the board;
            orientation = Console.random(0,2);
            if (orientation == 0)
            {
                // Horizontal
                col=Console.random(0,len);
                row=Console.random(0,10);
            }
            else
            {        
                col=Console.random(0,10);
                row=Console.random(0,len);
            }
            int invalid = 0;
            for (int ship_index=0; ship_index < ship_counter; ship_index++)
            {
                if (Ships[ship_index].overlaps(len,row,col,orientation))
                    invalid = 1;
                
            }
            if (invalid == 0)
                Done = 1;
        }
        if (orientation == 0)
        {
            for (int i=0;i<len;i++)
                map[row][col+i]=MY_SHIP_NORMAL;
        }
        else
        {
            for (int i=0;i<len;i++)
                map[row+i][col]=MY_SHIP_NORMAL;
        }
        Ships[ship_counter].place(PATROL_BOAT_SQUARES,row,col,orientation);
        ship_counter++;
    }
    
    
    
    uint16_t x,y,old_row,old_col;
    x=y=old_row=old_col=0;
    Done = 0;
    while(!Done)
    {
        
        // Hide the old dot
        x = old_col * (SCREEN_WIDTH/10)+SCREEN_WIDTH/20;
        y = old_row * (SCREEN_WIDTH/10)+SCREEN_WIDTH/20;
        Console.Display.fillCircle(x,y,8,Console.Display.RGBToWord(0,0,0));
        for (int map_row=0;map_row<10;map_row++)
        {
            for (int map_col=0;map_col < 10; map_col++)
            {
                if (map[map_row][map_col]==MY_SHIP_NORMAL)
                {
                    Console.Display.fillRectangle(map_col*(SCREEN_WIDTH/10),map_row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0xff,0xff,0xff));
                }
                else if (map[map_row][map_col]==MY_SHIP_HIT)
                {
                    Console.Display.fillRectangle(map_col*(SCREEN_WIDTH/10),map_row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0xff,0xf,0xf));
                }
                else if (map[map_row][map_col]==THEIR_SHIP_NORMAL)
                {
                    Console.Display.fillRectangle(map_col*(SCREEN_WIDTH/10),map_row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0x0,0xff,0xff));                    
                }
                else if (map[map_row][map_col]==THEIR_SHIP_HIT)
                {
                    Console.Display.fillRectangle(map_col*(SCREEN_WIDTH/10),map_row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0xff,0x0,0xff));                    
                }
            }
        }        
        // show the new dot        
        x = col * (SCREEN_WIDTH/10)+SCREEN_WIDTH/20;
        y = row * (SCREEN_WIDTH/10)+SCREEN_WIDTH/20;
        Console.Display.fillCircle(x,y,8,Console.Display.RGBToWord(0x1f,0x1f,0xff));
        old_row = row;
        old_col = col;
        uint16_t key=0;
        while(key == 0)
        {
            key=Console.Controller.getButtonState();
            if (DMBGameMessageReceived)
            {
                // incoming message from the other player.
                uint16_t target_row, target_col, hit;
                DMBGameMessageReceived = 0;
                hit = 0;
                target_row=DMBGameMailBox.Message[0];
                target_col=DMBGameMailBox.Message[1];
                printk("Target %d, %d \n",target_row,target_col);
                for (int ship_counter = 0; ship_counter < (CARRIER_COUNT+BATTLESHIP_COUNT+SUBMARINE_COUNT+PATROL_BOAT_COUNT); ship_counter++)
                {
                    if (Ships[ship_counter].contains(target_row,target_col))  
                    {
                        hit = 1;
                        if (map[target_row][target_col] != MY_SHIP_HIT)
                        {
                            my_ship_hit_counter++;
                        }
                        if (my_ship_hit_counter >= total_hits)
                        {
                            Console.Display.clear();
                            Console.Display.printX2("You lose!",9,5,20,Console.Display.RGBToWord(0xff,0xf,0xf),Console.Display.RGBToWord(0,0,0));
							Done=1;
                        }
                        map[target_row][target_col]=MY_SHIP_HIT;
                        // Fill in the square with the MY_SHIP_HIT colour  
                        Console.Display.fillRectangle(target_col*(SCREEN_WIDTH/10),target_row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0xff,0xf,0xf));
                    }
                }
                dmb_message response;
                response.Message[0]=hit;
                
                sendDMBGameResponseMessage(target,&response);
            }
            k_msleep(50);
        }
        
        switch(key)
        {
            case Console.Controller.Up :
            {
                row--;
                break;
            }
            case Console.Controller.Down :
            {
                row++;
                break;
            }
            case Console.Controller.Left :
            {
                col--;
                break;
            }
            case Console.Controller.Right :
            {
                col++;
                break;
            }
            case Console.Controller.A :
            {
                // Send a message to the opponent with row/col and wait for a reply (with timeout)
                DMBGameMessageResponseReceived = 0;
                dmb_message msg;
                msg.Message[0]=row;
                msg.Message[1]=col;
                sendDMBGameMessage(target,&msg);
                // wait for a reply
                uint32_t Timeout=100;
                while( (!DMBGameMessageResponseReceived) && (Timeout--))
                    k_msleep(10);
                if (DMBGameMessageResponseReceived)
                {                    
                    if (DMBGameResponseMailBox.Message[0]==1) 
                    {
                        // It's a hit 
                        if (map[row][col] != THEIR_SHIP_HIT)
                        {
                            their_ship_hit_counter++;
                        }
                        if (their_ship_hit_counter >= total_hits)
                        {
                            Console.Display.clear();
                            Console.Display.printX2("You win!",8,5,20,Console.Display.RGBToWord(0xff,0xff,0),Console.Display.RGBToWord(0,0,0));
							while(Console.Controller.getButtonState()==0)
								k_msleep(50);
							return;
							
                        }
                        map[row][col]=THEIR_SHIP_HIT;
                        Console.Display.fillRectangle(col*(SCREEN_WIDTH/10),row*(SCREEN_HEIGHT/10),SCREEN_WIDTH/10,SCREEN_HEIGHT/10,Console.Display.RGBToWord(0xff,0x0,0xff));     
                    }
                    else
                    {
                       // if (map[row][col] != 0)
                        {
                            map[row][col]=THEIR_SHIP_NORMAL;
                        }
                    }
                }
                else 
                {
                    printk("Message response timeout\n");
                }
                break;
            }
        }
        k_msleep(50);
        if (row < 0)
            row=0;
        if (col < 0)
            col = 0;
        if (row > 9)
            row=9;
        if (col > 9)
            col=9;
       
    }
    while(Console.Controller.getButtonState()==0)
	{
		k_msleep(50);
	}
	return;
    
}
