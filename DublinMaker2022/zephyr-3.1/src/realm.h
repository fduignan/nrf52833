// realm.h
// Some game constants
#include <stdint.h>
#define MAP_WIDTH 14
#define MAP_HEIGHT 10
#define MAX_WEAPONS 4


typedef unsigned char byte;
typedef struct {
	// The MSB of each entry specifies whether the contents of this
	// square are visible or not.
	uint8_t map[MAP_HEIGHT][MAP_WIDTH];
	
} tRealm;
typedef struct {
	char name[MAX_NAME_LEN+1];
	byte health;	
	byte strength;
	byte magic;
	byte wealth;
	byte x,y;
	byte Weapon1;
	byte Weapon2;
} tPlayer;

// Function prototypes
unsigned prbs();
unsigned random(unsigned range);
void showHelp();
void showGameMessage(const char *Msg);
char getUserInput();
void microrealms(void);
void initRealm(tRealm *Realm);
void showRealm(tRealm *Realm,tPlayer *thePlayer);
void initPlayer(tPlayer *Player,tRealm *Realm);
void showPlayer(tPlayer *thePlayer);
int step(char Direction,tPlayer *Player,tRealm *Realm);
void setHealth(tPlayer *Player,int health);
void setStrength(tPlayer *Player, byte strength);
int addWeapon(tPlayer *Player, int Weapon);
int doChallenge(tPlayer *Player, int BadGuyIndex);
const char *getWeaponName(int index);
void zap(void);
