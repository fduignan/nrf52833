#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include "mesh.h"
#include "display.h"
#include "console.h"
#include "controller.h"
#include "font5x7.h"
#include "realm.h"
// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
const char Baddies[]={'O','T','D','H'};
// The following is 4 sets of 4 damage types
const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
const byte FreezeSpellDamage[]={10,20,5,0};
const byte FireSpellDamage[]={20,10,5,0};
const byte LightningSpellDamage[]={15,10,25,0};
const byte BadGuyDamage[]={10,10,15,5};
int GameStarted = 0;
tPlayer thePlayer;
tRealm theRealm;
void delay(int len);
extern int	Blue_LED_Duty;
extern int	Red_LED_Duty;
extern char Name[MAX_NAME_LEN];
static int cell_size=SCREEN_WIDTH/MAP_WIDTH;

void printString(const char *s)
{
	
}
void eputc(char c)
{
}
void eputs(const char *s)
{
}
void printHex(uint16_t h)
{
}
void microrealms(void)
{
	uint16_t ch;
	int Dead=0;
	showHelp();	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);	
	showRealm(&theRealm,&thePlayer);	
	while (Dead>=0)
	{
		showPlayer(&thePlayer);
		ch = 0;
		do {
			ch = Console.Controller.getButtonState();
			k_msleep(50);
		}		
		while(ch == 0);
		switch (ch) {
			case Console.Controller.Up: {
				Dead = step('n',&thePlayer,&theRealm);
				break;
			}
			case Console.Controller.Down : {
				Dead = step('s',&thePlayer,&theRealm);
				break;

			}
			case Console.Controller.Right : {
				Dead = step('e',&thePlayer,&theRealm);
				break;
			}
			case Console.Controller.Left : {
				Dead = step('w',&thePlayer,&theRealm);
				break;
			}
		} // end switch
		k_msleep(100);
	} // end while
}
int step(char Direction,tPlayer *Player,tRealm *Realm)
{
	int new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'n' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'e' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'w' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks");
		showGameMessage("your path.");
		return 0;
	}
	// hide the current player location
	char s[2];
	s[1]=0;
	s[0]=Realm->map[Player->y][Player->x]; // restore anything we might have stood on that was not consumed
	if (s[0] == '.')
		s[0]=' ';
	Console.Display.printX2(s,1,3+Player->x*cell_size,1+Player->y*cell_size,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	
	Player->x = new_x;
	Player->y = new_y;
	// show the current player location
	s[0]='@';
	Console.Display.printX2(s,1,3+Player->x*cell_size,1+Player->y*cell_size,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0x0,0x0,0x0));
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			Console.Sound.playTone(500,50);
			showGameMessage("A smelly green");
			showGameMessage("Ogre appears");
			showGameMessage("before you");
			k_msleep(500); // slow scroll speed
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'T' :{
			Console.Sound.playTone(800,50);
			showGameMessage("An evil troll");
			showGameMessage("challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			Console.Sound.playTone(1200,50);
			showGameMessage("A smouldering");
			showGameMessage("Dragon blocks");
			showGameMessage("your way !");
			k_msleep(500); // slow scroll speed
			Consumed = doChallenge(Player,2);
			break;
		}
		case 'H' :{
			Console.Sound.playTone(1500,50);
			showGameMessage("A withered hag");			
			showGameMessage("cackles at you");			
			showGameMessage("wickedly");
			k_msleep(500); // slow scroll speed
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			Console.Sound.playTone(2000,50);
			showGameMessage("You find an elixer");
			showGameMessage("of health");
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			Console.Sound.playTone(2200,50);
			showGameMessage("You find a");
			showGameMessage("potion of");
			showGameMessage("strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{
			Console.Sound.playTone(2400,50);
			showGameMessage("You find a");
			showGameMessage("shiny golden");
			showGameMessage("nugget");
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			Console.Sound.playTone(2600,50);
			showGameMessage("You find a");
			showGameMessage("magic charm");
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Console.Sound.playTone(3000,50);
			Consumed = addWeapon(Player,random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			Console.Display.clear();
			showRealm(&theRealm,Player);
			showGameMessage("A door! You exit");
			showGameMessage("into a new realm");
			Console.Sound.playTone(1000,50);
			Console.Sound.playTone(2000,50);
		}
	}
	if (Consumed==1)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
	return Consumed;
	
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	showGameMessage("Press A to fight");
	ch=0;
	while(ch == 0)
	{
		ch = Console.Controller.getButtonState();
		k_msleep(50);
	}
	while(Console.Controller.getButtonState()!=0); // wait for release
	if (ch == Console.Controller.A)
	{		
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			showPlayer(Player);
			showGameMessage("Action? (up/down)");		
			// need to present a menu of actions.  Available choices will depend on player state
			k_msleep(200);
			ch = 0;
			char choices[6];
			int i;
			for (i=0;i<sizeof(choices);i++)
			{
				choices[i]=0;
			}
			i=0;
			if (Player->magic > ICE_SPELL_COST)
				choices[i]='I';
			i++;
			if (Player->magic > FIRE_SPELL_COST)
				choices[i]='F';
			i++;
			if (Player->magic > LIGHTNING_SPELL_COST)
				choices[i]='L';
			i++;
			if (Player->Weapon1)
			{				
				choices[i]='1';
				i++;
			}			
			if (Player->Weapon2)
			{
				choices[i]='2';
				i++;
			}
			choices[i]='P';			
			int Done=0;
			i=0;
			while(Done == 0)
			{
				ch = 0;
				while(ch == 0)
				{
					ch = Console.Controller.getButtonState();
					k_msleep(50);
				}
				if (ch == Console.Controller.A)
				{					
					Done=1;
				}
				if (ch == Console.Controller.Up)
				{
					i++;
					if (i>=sizeof(choices))
					{
						i=0;
					}
					if (choices[i]==0)
					{
						// gone past end
						i=0;
					}					
				}
				if (ch == Console.Controller.Down)
				{
					i--;
					if (i<0)
					{
						// find the last non-zero entry in the choices array
						i=sizeof(choices)-1;
						while(choices[i]==0)
							i--;
					}
				}
				switch (choices[i])
				{
					case 'I': {
						showGameMessage("Ice spell");
						break;
					}
					case 'F': {
						showGameMessage("Fire spell");
						break;
					}
					case 'L': {
						showGameMessage("Lightning spell");
						break;
					}
					case '1': {
						showGameMessage("Weapon 1");
						break;
					}
					case '2': {
						showGameMessage("Weapon 2");
						break;
					}
					case 'P': {
						showGameMessage("Punch");
						break;
					}
				}
			}
			ch=choices[i];

			switch (ch)
			{
				case 'i':
				case 'I':
				{
					Console.Sound.playTone(3000,100);					
					showGameMessage("FREEZE!");
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					showGameMessage("BURN!");
					Console.Sound.playTone(1500,100);
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					Console.Sound.playTone(1000,50);
					Console.Sound.playTone(2000,50);
					Console.Sound.playTone(3000,50);
					showGameMessage("ZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case '1':
				{
					Console.Sound.playTone(1000,50);
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					showGameMessage("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					Console.Sound.playTone(1000,50);
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					showGameMessage("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					Console.Sound.playTone(500,50);
					showGameMessage("Thump!");
					BadGuyHealth -= 1+random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					Console.Sound.playTone(200,50);
					showGameMessage("You fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = BadGuyDamage[BadGuyIndex]+random(5);
			setHealth(Player,Player->health - Damage);
			eputs("Health: you "); printHex(Player->health);
			eputs(", them " );printHex(BadGuyHealth);
			eputs("\r\n");
		}
		if (Player->health == 0)
		{ // You died
			showGameMessage("You are dead");		
			while(Console.Controller.getButtonState()==0)
			{
				k_msleep(100);
			}
			return -1;
		}
		else
		{ // You won!
			Player->wealth = 50 + random(50);			
			showGameMessage("You win!");
			showGameMessage("Their gold is yours");
			return 1;
		}	
	}
	else
	{
		showGameMessage("Our 'hero'");
		showGameMessage("chickens out");
		return 0;		
	}
	return 0;
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	showGameMessage("You find ");
	switch (Weapon)
	{
		case 1:
		{	
			showGameMessage("a mighty axe");
			Console.Sound.playTone(2000,50);
			Console.sleep(50);
			Console.Sound.playTone(500,50);
			Console.sleep(50);
			break;
		}
		case 2:
		{	
			Console.Sound.playTone(2000,50);
			Console.sleep(50);
			Console.Sound.playTone(500,50);
			Console.sleep(50);
			showGameMessage("a sword with");
			showGameMessage("mystical runes");
			break;
		}
		case 3:
		{	
			Console.Sound.playTone(2000,50);
			Console.sleep(50);
			Console.Sound.playTone(500,50);
			Console.sleep(50);
			showGameMessage("a bloody flail");
			break;
		}		
		default:
			printHex(Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		showGameMessage("You already");		
		showGameMessage("have two");
		showGameMessage("weapons");
		k_msleep(500);
		showGameMessage("(L) drop Weapon1");
		showGameMessage("(R) drop Weapon2");
		showGameMessage("(B) skip");
		c=0;
		while(c==0)
		{
			c = Console.Controller.getButtonState();
			k_msleep(50);
		}
		switch(c)
		{			
			case Console.Controller.B:{
				return 0; // don't pick up
			}
			case Console.Controller.Left:{
				Player->Weapon1 = Weapon;
				showGameMessage("Weapon 1");
				showGameMessage("Replaced");
				break;
			}
			case Console.Controller.Right:{
				Player->Weapon2 = Weapon;
				showGameMessage("Weapon 2");
				showGameMessage("Replaced");
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; break;
		case 1:return "Axe";break;
		case 2:return "Sword"; break;
		case 3:return "Flail"; break;
	}
	return "";
}

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = health;
}	
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	if (strength < 0)
		strength = 0;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *theRealm)
{
	// get the player name
	int index=0;
	byte x,y;

	// Initialize the player's attributes
	for (index=0;index<MAX_NAME_LEN;index++)
	{
		Player->name[index]=Name[index]; // terminate the name
	}
	setHealth(Player,100);
	Player->strength=50+random(50);
	Player->magic=50+random(50);	
	Player->wealth=10+random(10);
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=random(MAP_WIDTH);
		y=random(MAP_HEIGHT);
		
	} while(theRealm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}

void initRealm(tRealm *Realm)
{
	int x,y;
	int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = random(MAP_WIDTH);
	y = random(MAP_HEIGHT);
	Realm->map[y][x]='X';
}

void showRealm(tRealm *Realm,tPlayer *thePlayer)
{
	int x,y;
	char s[2];
	s[1]=0;
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			Console.Display.drawRectangle(x*cell_size,y*cell_size,cell_size,cell_size,Console.Display.RGBToWord(0,0xaf,0));
			s[0]=Realm->map[y][x];
			if (s[0] != '.')
			{
				Console.Display.printX2(s,1,3+x*cell_size,1+y*cell_size,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
			}
			if ( (x==thePlayer->x) && (y==thePlayer->y))
			{
				s[0]='@';
				Console.Display.printX2(s,1,3+x*cell_size,1+y*cell_size,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0x0,0x0,0x0));
			}
		}
	}
}

void showHelp()
{

	uint16_t row_pos=0;
	Console.Display.clear();
	
	Console.Display.printX2("Map Legend",sizeof("Map Legend")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 3+row_pos + (FONT_HEIGHT * 2);
	
	Console.Display.printX2("(T)roll, (O)gre",sizeof("(T)roll, (O)gre")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 5+row_pos + (FONT_HEIGHT * 2);
	Console.Display.printX2("(D)ragon, (H)ag",sizeof("(D)ragon, (H)ag")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 5+row_pos + (FONT_HEIGHT * 2);
	Console.Display.printX2("e(X)it",sizeof("e(X)it")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 5+row_pos + (FONT_HEIGHT * 2);
	Console.Display.printX2("(w)eapon, (g)old",sizeof("(w)eapon, (g)old")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 5+row_pos + (FONT_HEIGHT * 2);
	Console.Display.printX2("(m)agic, (s)trength",sizeof("(m)agic, (s)trength")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0x0,0x0,0x0));
	row_pos = 5+row_pos + (FONT_HEIGHT * 2);
	Console.Display.printX2("@=You",sizeof("@=You")-1,2,row_pos,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0x0,0x0,0x0));
	while(Console.Controller.getButtonState()==0);
	while(Console.Controller.getButtonState()!=0);
	Console.Display.clear();
}

void showGameMessage(const char *Msg)
{
#define  MAX_CHARS_PER_LINE ((SCREEN_WIDTH/(FONT_WIDTH*2))-3)
	int line_number;
	Console.Display.fillRectangle(0,1+ (1+MAP_HEIGHT)*cell_size,SCREEN_WIDTH,SCREEN_HEIGHT-(1+ MAP_HEIGHT*cell_size),Console.Display.RGBToWord(0,0,0));

	static char console_lines[3][MAX_CHARS_PER_LINE+1];
	int i;
	// scroll the lines
	for (line_number=1;line_number<3;line_number++)
	{
		for (i=0;i<MAX_CHARS_PER_LINE+1;i++)
		{
			console_lines[line_number-1][i] = console_lines[line_number][i];
		}
	}
	for (i=0;i<MAX_CHARS_PER_LINE+1;i++)
	{
		console_lines[2][i]=Msg[i];
	}
	int row_pos = 3+((MAP_HEIGHT+1)*cell_size);
	for (line_number=0;line_number < 3;line_number++)
	{
		int len=0;
		// how long is this message?
		while(console_lines[line_number][len]!=0)
			len++;
		Console.Display.printX2(console_lines[line_number],len,3,row_pos,Console.Display.RGBToWord(0xff,0xff,0xff),Console.Display.RGBToWord(0,0,0));		
		row_pos += 3+ (FONT_HEIGHT*2);
	}
}
void PrintPercent(uint8_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
     // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[2]; // Maximum value = 99
    if (Number > 99)
		Number = 99;    
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';    
    Console.Display.printX2(Buffer, 2, x, y, ForeColour, BackColour);
}
void showPlayer(tPlayer *thePlayer)
{
	int row_pos = 3+((MAP_HEIGHT)*cell_size);
	int col_pos = 0;
	Console.Display.printX2("H",1,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xf,0xf),Console.Display.RGBToWord(0,0,0));		
	col_pos += 2;
	PrintPercent(thePlayer->health,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xf,0xf),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	Console.Display.printX2(" G",2,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	PrintPercent(thePlayer->wealth,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xff,0xf),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	Console.Display.printX2(" M",2,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xf,0xff),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	PrintPercent(thePlayer->magic,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xff,0xf,0xff),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	Console.Display.printX2(" S",2,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xf,0xf,0xff),Console.Display.RGBToWord(0,0,0));		
	col_pos += 3;
	PrintPercent(thePlayer->strength,(col_pos*FONT_WIDTH*2)+1,row_pos,Console.Display.RGBToWord(0xf,0xf,0xff),Console.Display.RGBToWord(0,0,0));			
	return;
	
	eputs("\r\nName: ");
	printString(thePlayer->name);
	eputs("health: ");
	printHex(thePlayer->health);
	eputs("\r\nstrength: ");
	printHex(thePlayer->strength);
	eputs("\r\nmagic: ");
	printHex(thePlayer->magic);
	eputs("\r\nwealth: ");
	printHex(thePlayer->wealth);	
	eputs("\r\nLocation : ");
	printHex(thePlayer->x);
	eputs(" , ");
	printHex(thePlayer->y);	
	eputs("\r\nWeapon1 : ");
	printString(getWeaponName(thePlayer->Weapon1));
	eputs("Weapon2 : ");
	printString(getWeaponName(thePlayer->Weapon2));
}
char getUserInput()
{
	return 0;
}
unsigned random(unsigned range)
{	
	return Console.random(0,range);
}
void zap()
{
	
}
