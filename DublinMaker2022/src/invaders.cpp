// Game that plays homage to classic arcarde games like Space Invaders and Galaga
#include <stdint.h>
#include "sprite.h"
#include "console.h"
#include "invaders.h"

void Explode(uint16_t X, uint16_t Y);
int FirePressed();
void playInvaders(uint16_t demo_mode)
{
  // Variables for the Invaders game
#define MAX_INVADERS 10
#define MAX_INVADER_LEVELS 4
  const uint16_t *IvdImage=DefaultInvaderImage;
       
  sprite  Invaders[MAX_INVADERS] = {
    { IvdImage, 3, 3, 10, 12   },
    { IvdImage, 23, 3, 10, 12  },
    { IvdImage, 43, 3, 10, 12  },
    { IvdImage, 63, 3, 10, 12  },
    { IvdImage, 83, 3, 10, 12  },
    { IvdImage, 103, 3, 10, 12 },
    { IvdImage, 123, 3, 10, 12 },
    { IvdImage, 143, 3, 10, 12 },
    { IvdImage, 163, 3, 10, 12 },
    { IvdImage, 183, 3, 10, 12 }
  };
  sprite  Defender(DefenderImage, SCREEN_WIDTH/2, SCREEN_HEIGHT-30, 10, 6);
#define MAX_DEFENDER_MISSILES 5
  sprite DefenderMissiles[MAX_DEFENDER_MISSILES] = {
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
  };
#define MAX_ATTACKER_MISSILES (MAX_INVADERS)
  sprite AttackerMissiles[MAX_ATTACKER_MISSILES] = {
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  }
  };
  uint8_t GameOver = 0;
  uint8_t DefenderMissileCounter = 0;
  uint8_t AttackerMissileCounter = 0;
  uint8_t InvaderCounter = 0;
  uint8_t DefenderCount = 5;
  int XDirectionChange = 0;
  int YDirectionChange = 0;
  int LevelComplete = 0;
  int Level = MAX_INVADER_LEVELS;
  uint16_t x_step = 2;
  uint16_t y_step = 1;
  uint8_t ActiveAttackerMissiles = 0;
  uint32_t demo_count=0;

  // Initialization for the Invaders game
  int Index;
  int Toggle = 1;

  Console.Display.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0);
  Console.Display.print("MCU Galaga", 10, 10, 40, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
  Console.Display.print("Only 5 missiles", 15, 10, 60, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
  Console.Display.print("can be in flight", 16, 10, 80, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
  Console.Display.print("at one time", 11, 10, 100, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
  Console.sleep(1000);  
  while (Level > 0)
  {
    LevelComplete = 0;
    GameOver = 0;
    Console.Display.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    for (Index = 0; Index < MAX_INVADERS-Level; Index++)
    {
      Invaders[Index].show();
    }     
    // Position the invaders at the top of the screen
    Invaders[0].move(0, 3);
    Invaders[1].move(15, 3);
    Invaders[2].move(30, 3);
    Invaders[3].move(45, 3);
    Invaders[4].move(60, 3);
    Invaders[5].move(75, 3);
    Invaders[6].move(90, 3);
    Invaders[7].move(105, 3);
    Invaders[8].move(120, 3);
    Invaders[9].move(135, 3);
    for (Index = 0; Index < MAX_ATTACKER_MISSILES;Index++)
      AttackerMissiles[Index].hide();    
    for (Index = 0; Index < MAX_DEFENDER_MISSILES;Index++)
      DefenderMissiles[Index].hide();          
    Console.Display.print("Level", 5, 5, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
    Console.Display.print(MAX_INVADER_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));    
    for (Index = DefenderCount; Index > 0; Index--) //  draw remaining lives
        Console.Display.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, Console.Display.RGBToWord(0xff, 0xf, 0xf));
    GameOver = 0;
    DefenderMissileCounter = 0;
    InvaderCounter = 0;
    XDirectionChange = 0;
    LevelComplete = 0;
    x_step = 2;    
    Defender.show();   
    while (!LevelComplete )
    {
      // Show the defender
      Defender.redraw();
      if (demo_mode )
      {
          if (Console.Controller.getButtonState()) // user pushed a button?
              return; // exit if in demo mode 
      }
      if (Console.Controller.getButtonState() & Console.Controller.Right)
      {
        // Move right
        if (Defender.getX() < (SCREEN_WIDTH - 11))
        {
          Defender.move(Defender.getX() + 2, Defender.getY());
        }
      }

      if (Console.Controller.getButtonState() & Console.Controller.Left)
      {
        // Move left
        if (Defender.getX() > 1)
        {
          Defender.move(Defender.getX() - 2, Defender.getY());
        }
      }
      if (FirePressed())
      {
        // User pressed Fire!
        // See if there is an empty missile-in-flight slot
        for (DefenderMissileCounter = 0; DefenderMissileCounter < MAX_DEFENDER_MISSILES; DefenderMissileCounter++)
        {
          if (DefenderMissiles[DefenderMissileCounter].visible() == 0)
          {
            // Found an empty slot, place a missile just above the centre of the defender
            DefenderMissiles[DefenderMissileCounter].show();
            DefenderMissiles[DefenderMissileCounter].move(Defender.getX() + Defender.getWidth() / 2 - DefenderMissiles[DefenderMissileCounter].getWidth() / 2, Defender.getY() - DefenderMissiles[DefenderMissileCounter].getHeight());
            break; // exit remaining 'for' loop cycles
          }
        }
      }
      // Update defender missiles
      for (DefenderMissileCounter = 0; DefenderMissileCounter < MAX_DEFENDER_MISSILES; DefenderMissileCounter++)
      {
        if ((DefenderMissiles[DefenderMissileCounter].getY() > 0) && (DefenderMissiles[DefenderMissileCounter].visible()))
        {
          DefenderMissiles[DefenderMissileCounter].move(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY() - 2);
          // Did any missile hit an invader?
          for (InvaderCounter = 0; InvaderCounter < MAX_INVADERS; InvaderCounter++)
          {
            if (Invaders[InvaderCounter].within(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY()))
            {
              // Missile meets invader!
              Invaders[InvaderCounter].hide();
              DefenderMissiles[DefenderMissileCounter].hide();
              // Do an explosion at that location
              Explode(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY());
            }
          }
        }
        else
        {
          if (DefenderMissiles[DefenderMissileCounter].visible())
            DefenderMissiles[DefenderMissileCounter].hide();
        }

      }

      // Update attacker missiles
      ActiveAttackerMissiles = MAX_ATTACKER_MISSILES;
      for (AttackerMissileCounter = 0; AttackerMissileCounter < MAX_ATTACKER_MISSILES; AttackerMissileCounter++)
      {
        if ((AttackerMissiles[AttackerMissileCounter].getY() < SCREEN_HEIGHT-20) && (AttackerMissiles[AttackerMissileCounter].visible()))
        {
          AttackerMissiles[AttackerMissileCounter].move(AttackerMissiles[AttackerMissileCounter].getX(), AttackerMissiles[AttackerMissileCounter].getY() + 2);

          // Did any attacker missile hit the defender?
          if (Defender.within(AttackerMissiles[AttackerMissileCounter].getX() + AttackerMissiles[AttackerMissileCounter].getWidth() / 2, AttackerMissiles[AttackerMissileCounter].getY() + AttackerMissiles[AttackerMissileCounter].getHeight() / 2))
          {
            Defender.hide();
            Explode(Defender.getX(), Defender.getY());
            DefenderCount --;
            AttackerMissiles[AttackerMissileCounter].hide(); // That missile has exploded
            if (DefenderCount == 0) 
            {   // All defenders are dead :(
                Console.Display.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
                Console.Display.print("GAME OVER", 9, 80, 100, Console.Display.RGBToWord(0xff, 0xff, 0xff), 0);
                Console.Display.print("Fire to restart", 15, 60, 120, Console.Display.RGBToWord(0xff, 0xff, 0), Console.Display.RGBToWord(0, 0, 0));
                if (!demo_mode)
                {
                    while (!FirePressed())
                        Console.sleep(10);
                    while (FirePressed());
                }
                return;
            }            
            Defender.show();
            Console.Display.fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);  // wipe out the remaining lives area
            for (Index = DefenderCount; Index > 0; Index--) //  draw remaining lives
                Console.Display.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, Console.Display.RGBToWord(0xff, 0xf, 0xf));
          }

        }
        else
        {
          if (AttackerMissiles[AttackerMissileCounter].visible())
            AttackerMissiles[AttackerMissileCounter].hide();
          if (ActiveAttackerMissiles > 0)
            ActiveAttackerMissiles--;

        }
      }
      // Update invaders
      XDirectionChange = 0;
      YDirectionChange = 0;
      LevelComplete = 1;
      for (InvaderCounter = 0; InvaderCounter < MAX_INVADERS; InvaderCounter++)
      {
        if (Invaders[InvaderCounter].visible())
        {
          if (ActiveAttackerMissiles < MAX_ATTACKER_MISSILES - Level)
          {
            // Spawn a new attacker missile
            AttackerMissileCounter = 0;
            uint8_t MissileFired = 0;
            while ( (AttackerMissileCounter < MAX_ATTACKER_MISSILES) && (!MissileFired))
            {
              if ((AttackerMissiles[AttackerMissileCounter].visible() == 0) && (Console.random(0,50)==0))
              {
                ActiveAttackerMissiles++;
                AttackerMissiles[AttackerMissileCounter].show();
              //  AttackerMissiles[AttackerMissileCounter].move(Invaders[InvaderCounter].getX() + Invaders[InvaderCounter].getWidth() / 2 - DefenderMissiles[DefenderMissileCounter].getWidth() / 2, Invaders[InvaderCounter].getY() + DefenderMissiles[DefenderMissileCounter].getHeight() / 2);
				AttackerMissiles[AttackerMissileCounter].move(Invaders[InvaderCounter].getX() + Invaders[InvaderCounter].getWidth() / 2 - DefenderMissiles[0].getWidth() / 2, Invaders[InvaderCounter].getY() + DefenderMissiles[0].getHeight() / 2);
                MissileFired = 1;
              }
              AttackerMissileCounter++;
            }
          }
          LevelComplete = 0; // didn't win yet, there are still invaders
          Invaders[InvaderCounter].move(Invaders[InvaderCounter].getX() + x_step, Invaders[InvaderCounter].getY() + y_step);
          if (Invaders[InvaderCounter].getX() >= (SCREEN_WIDTH - Invaders[InvaderCounter].getWidth()))
            XDirectionChange = 1;
          if (Invaders[InvaderCounter].getX() == 0)
            XDirectionChange = 1;
          if (Invaders[InvaderCounter].getY() > SCREEN_HEIGHT - 100)
          {
            YDirectionChange = 1;

            if (Invaders[InvaderCounter].getY() < 3)
            {
              YDirectionChange = 1;
            }

          }
        }
      }
      if (XDirectionChange) // Did an invader hit either edge?
        x_step = -x_step; // if so, then reverse direction
      if (YDirectionChange) // Did an invader hit either edge?
        y_step = -y_step; // if so, then reverse direction              
      Console.sleep(15);  // slow the game to make it playable
      
    } // End of while (!LevelComplete )
    if ((LevelComplete == 1) && (Level > 0))
    {
      Level--;
      Console.Display.print("Level", 5, 5, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));
      Console.Display.print(MAX_INVADER_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, Console.Display.RGBToWord(0xff, 0xff, 0xff), Console.Display.RGBToWord(0, 0, 0));

    }
  } // end of while (Level > 0)
  Console.Display.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Console.Display.RGBToWord(0, 0, 0xff));
  Console.Display.print("VICTORY!", 8, 40, 100, Console.Display.RGBToWord(0xff, 0xff, 0), Console.Display.RGBToWord(0, 0, 0xff));
  Console.Display.print("Fire to restart", 15, 8, 120, Console.Display.RGBToWord(0xff, 0xff, 0), Console.Display.RGBToWord(0, 0, 0xff));  
  while (!FirePressed())
      Console.sleep(10);
  while (FirePressed());
  return;
}

void Explode(uint16_t X, uint16_t Y)
{
  sprite Bang(ExplosionImage, X - 5, Y - 6, 10, 12);
  Bang.show();
  Console.Sound.playTone(2000,50);
  Console.sleep(50);
  Console.Sound.playTone(500,50);
  Console.sleep(50);
  Bang.hide();
}

int FirePressed()
{
    // return 1 if the user pressed 'Fire'
// Some debouncing of the button is performed    
    static int PreviousState = 0;
    if (Console.Controller.getButtonState() & Console.Controller.A)
    {
        if (PreviousState == 0)
        {
            PreviousState = 1;
            return 1;
        }
        else
            return 0;

    }
    else
    {
        PreviousState = 0;
        return 0;
    }
}
