#include <stdint.h>
#include "console.h"

#include "sprite.h"
#include "brick.h"
#include "brici.h"
#define DEMO_HITS 10
void ClassicBrici()
{   
    #define Brick_Width 30
    #define Brick_Height 10
    #define BRICK_COUNT ( (3*SCREEN_WIDTH) / Brick_Width )
    Console.Display.fillRectangle(0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,0);
    brick Bricks[BRICK_COUNT];
    uint16_t RowCount=0;
    uint32_t Brick_Count = BRICK_COUNT;
    uint32_t Index=0;
    uint16_t x,y,w,h;
    uint16_t r,g,b;
    uint16_t Colour;
    for (Index=0;Index < Brick_Count/3; Index++)
    {
        r=255;
        b=64;
        g=255;
        x=RowCount*Brick_Width;
        y=50;
        Bricks[Index].define(x,y,Brick_Width,Brick_Height,Console.Display.RGBToWord(r,g,b));
        RowCount++;
    }
    RowCount = 0;
    for (Index=Brick_Count/3;Index < (2*Brick_Count)/3; Index++)
    {
        r=64;
        b=64;
        g=255;
        x=RowCount*Brick_Width;
        y=50+Brick_Height;
        Bricks[Index].define(x,y,Brick_Width,Brick_Height,Console.Display.RGBToWord(r,g,b));
        RowCount++;
    }
    RowCount = 0;
    for (Index=(2*Brick_Count)/3;Index < Brick_Count; Index++)
    {
        r=64;
        b=255;
        g=64;
        x=RowCount*Brick_Width;
        y=50+2*Brick_Height;
        Bricks[Index].define(x,y,Brick_Width,Brick_Height,Console.Display.RGBToWord(r,g,b));
        RowCount++;
    }
    Brici(Bricks,Brick_Count,30,0);
}
void Brici(brick *Bricks, uint16_t Brick_Count, uint16_t BatWidth,uint16_t demo_mode)
{
    
    
    brick Bat(0, SCREEN_HEIGHT-20, BatWidth, 3,Console.Display.RGBToWord(255,255,255));    
    brick Ball(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3, 3,Console.Display.RGBToWord(255,255,255));
#define MAX_BRICI_LEVELS 4
    int Level = MAX_BRICI_LEVELS;
    int LevelComplete = 0;
    int8_t BallCount = 5;
    uint8_t Index;
    int8_t BallXVelocity = 1;
    int8_t BallYVelocity = 1;   
    uint32_t BatHits=0;

    while (Level > 0)
    {
        for (Index = 0; Index < Brick_Count; Index++)
        {
            Bricks[Index].show();
            if (!demo_mode)
                Console.Display.drawRectangle(Bricks[Index].getX(),Bricks[Index].getY(),Bricks[Index].getWidth(),Bricks[Index].getHeight(),0);
        }
        Ball.move(Console.random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
        if (Console.random(0,2) > 0)
            BallXVelocity = 1;
        else
            BallXVelocity = -1;
        LevelComplete = 0;
        BallYVelocity = -1;        
        for (Index = BallCount; Index > 0; Index--)
            Console.Display.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, Console.Display.RGBToWord(0xff, 0xf, 0xf));        
        Ball.show();
        Bat.show();
        Console.Display.print("Level", 5, 5, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
        Console.Display.print(MAX_BRICI_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
        
        while (!LevelComplete)
        {            
            if (Console.Controller.getButtonState() & Console.Controller.Right)
            {
                // Move right
                if (Bat.getX() < (SCREEN_WIDTH - Bat.getWidth()))
                {
                    Bat.move(Bat.getX() + 2, Bat.getY()); // Move the bat faster than the ball
                }
            }
            
            if (Console.Controller.getButtonState() & Console.Controller.Left)
            {
                // Move left
                if (Bat.getX() > 0)
                {
                    Bat.move(Bat.getX() - 2, Bat.getY()); // Move the bat faster than the ball
                }
            }
            
            if (Bat.touching(Ball.getX() + Ball.getWidth() / 2, Ball.getY() + Ball.getHeight() / 2))
            { // Ball bounced off the bat
                BallYVelocity = -BallYVelocity; 
                if (!demo_mode)
                    Console.Sound.playTone(200,20);
                if (demo_mode) 
                {
                    if (Console.Controller.getButtonState())
                    {
                        // Someone pushed a button - get out of demo mode
                        return;
                    }
                    BatHits++;                
                    if (BatHits >= DEMO_HITS)
                    {
                       return;      
                    }
                }
            }                        
            Ball.move(Ball.getX() + BallXVelocity, Ball.getY() + BallYVelocity); // update ball position
            if (Ball.getX() == 2) // bounced off left side?
            {
                BallXVelocity = -BallXVelocity;
                if (!demo_mode)
                    Console.Sound.playTone(400,20);
            }
            if (Ball.getX() == SCREEN_WIDTH - 2) // bounced off right side?
            {
                BallXVelocity = -BallXVelocity;
                if (!demo_mode)
                    Console.Sound.playTone(400,20);
            }
            if (Ball.getY() == 2)  // bounced off top? (behind the block)
            {
                BallYVelocity = -BallYVelocity;
                if (!demo_mode)
                    Console.Sound.playTone(400,20);
            }
            
            if (Ball.getY() >= Bat.getY() + Bat.getHeight() + 2)  // Did the ball go behind the bat?
            {
                BallCount--;  // Lost a ball!
                if (BallCount == 0) // Any left?
                {
                    // Nope: hard luck, game over
                    Console.Display.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
                    Console.Display.print("GAME OVER", 9, 80, 100, Console.Display.RGBToWord(0xff, 0xff, 0xff), 0);
                    Console.Display.print("Press A to restart", 15, 60, 120, Console.Display.RGBToWord(0xff, 0xff, 0), Console.Display.RGBToWord(0, 0, 0));
                    while (!(Console.Controller.getButtonState() & Console.Controller.A))
                        k_msleep(10);
                    return;
                }
                // start a new ball moving in a random way
                if (Console.random(0,100) & 1)
                    BallXVelocity = 1;
                else
                    BallXVelocity = -1;
                BallYVelocity = -1;
                Ball.move(Console.random(10, SCREEN_WIDTH - 10), Console.random(90, 120));
                Console.Display.fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);  // wipe out the remaining lives area
                for (Index = BallCount; Index > 0; Index--) //  draw remaining lives
                    Console.Display.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, Console.Display.RGBToWord(0xff, 0xf, 0xf));
                
            }
            LevelComplete = 1;
            // Check to see if the ball has hit any of the blocks
            for (Index = 0; Index < Brick_Count; Index++)
            {
                if (Bricks[Index].visible()) // any blocks left?
                    LevelComplete = 0; // If yes then the level is not complete
                    
                int touch = 0;
                // Check all 4 corners of the ball for touching
                touch += Bricks[Index].touching(Ball.getX(), Ball.getY());
                if (touch == 0)
                    touch = Bricks[Index].touching(Ball.getX()+2, Ball.getY());
                if (touch == 0)
                    touch = Bricks[Index].touching(Ball.getX(), Ball.getY()+2);
                if (touch == 0)
                    touch = Bricks[Index].touching(Ball.getX()+2, Ball.getY()+2);
                if (touch)
                { // Block hit so hide it.
                    Bricks[Index].hide();  
                    if ( (touch == 1) || (touch == 3) )
                        BallYVelocity = -BallYVelocity;
                    if ( (touch == 2) || (touch == 4) )
                        BallXVelocity = -BallXVelocity;
                    if (!demo_mode)
                        Console.Sound.playTone(1000,20);
                }
            }
            // No Blocks left, Move to next level.
            if ((LevelComplete == 1) && (Level > 0))
            {
                Level--;
                Ball.hide();
                Console.Display.print("Level", 5, 5, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
                Console.Display.print(MAX_BRICI_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));                
            }
            if (!demo_mode)
            {
                k_msleep(Level+5); // Slow the game to human speed                
            }
            else
            {
                k_msleep(5); // Slow the game to human speed                
            }
            Bat.redraw(); // redraw bat as it might have lost a pixel due to collisions          
        }
    }
}
