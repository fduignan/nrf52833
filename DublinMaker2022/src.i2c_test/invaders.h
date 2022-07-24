// invaders.h
#include "console.h"
#define RGBToWord( R,  G,  B)  (  ((R&0xf8) << (11-3)) | ((G&0xfc) << (5-2)) | ((B&0xf8)>>3) )
void playInvaders(uint16_t demo_mode);
//  An invader is shown below
/*     
    ..........
    ....##....
    ...####...
    .##X##X##.
    .##X##X##.
    .########.
    .########.
    ..##..##..
    ..##..##..
    .##....##.
    ###....###
    ..........
*/
// Numeric declaration of this invader

#ifndef WHI
#define BGND COLOUR(0,0,0)
#define WHI COLOUR(255,255,255)
#define GRN COLOUR(0,255,0)
#define YEL COLOUR(255,255,0)
#define BLUE COLOUR(0,0,255)
#endif 
#define IVDR COLOUR(0xff,0xff,0x00)
const uint16_t DefaultInvaderImage[] = { 
    BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,
    BGND,BGND,BGND,BGND,IVDR,IVDR,BGND,BGND,BGND,BGND,
    BGND,BGND,BGND,IVDR,IVDR,IVDR,IVDR,BGND,BGND,BGND,
    BGND,IVDR,IVDR,BGND,IVDR,IVDR,BGND,IVDR,IVDR,BGND,
    BGND,IVDR,IVDR,BGND,IVDR,IVDR,BGND,IVDR,IVDR,BGND,
    BGND,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,BGND,
    BGND,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,IVDR,BGND,
    BGND,BGND,IVDR,IVDR,BGND,BGND,IVDR,IVDR,BGND,BGND,
    BGND,BGND,IVDR,IVDR,BGND,BGND,IVDR,IVDR,BGND,BGND,
    BGND,IVDR,IVDR,BGND,BGND,BGND,BGND,IVDR,IVDR,BGND,
    IVDR,IVDR,IVDR,BGND,BGND,BGND,BGND,IVDR,IVDR,IVDR,
    BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND 
    };   

/* 
 The player Defender Ship is as follows:
 ..........
 ....##....
 ....##....
 .########.
 ##########
 ##########
 */
// Numeric declaration of Defender
const uint16_t DefenderImage[] = { 
    BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,BGND,
    BGND,BGND,BGND,BGND,WHI ,WHI ,BGND,BGND,BGND,BGND,
    BGND,BGND,BGND,BGND,WHI ,WHI ,BGND,BGND,BGND,BGND,
    BGND,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,BGND,
    WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,
    WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,WHI ,
};



/* The defender missile image is as follows 
    ..#..
    .###.
    .###.
    .###.
    .###.
    #####
    #####
    ..#..
*/

const uint16_t DefenderMissileImage[]= { 
    BGND,BGND,GRN ,BGND,BGND, 
    BGND,GRN ,GRN ,GRN ,BGND, 
    BGND,GRN ,GRN ,GRN ,BGND, 
    BGND,GRN ,GRN ,GRN ,BGND, 
    BGND,GRN ,GRN ,GRN ,BGND, 
    GRN ,GRN ,GRN ,GRN ,GRN , 
    GRN ,GRN ,GRN ,GRN ,GRN , 
    BGND,BGND,YEL ,BGND,BGND  
};

/* The attacker missile image is as follows 
    ..#..
    .###.
    .###.
    .###.
    .###.
    #####
    #####
    ..#..
*/

const uint16_t AttackerMissileImage[]= { 
    BGND,BGND,YEL ,BGND,BGND, 
    BLUE,BLUE ,BLUE ,BLUE ,BLUE, 
    BLUE,BLUE ,BLUE ,BLUE ,BLUE, 
    BGND,BLUE ,BLUE ,BLUE ,BGND, 
    BGND,BLUE ,BLUE ,BLUE ,BGND, 
    BGND ,BLUE ,BLUE ,BLUE ,BGND,
    BGND ,BLUE ,BLUE ,BLUE ,BGND,
    BGND,BGND,BLUE ,BGND,BGND     
};

// The explosion image is as follows
/*     
    ....##....
    #...##...#
    .#..##..#.
    ..#.##.#..
    ...####...
    ##########
    ##########
    ...####...
    ..#.##.#..
    .#..##..#.
    #...##...#
    ....##....
*/                    
// Numeric declaration of Explosion
#define EXPL COLOUR(0xff,0x00,0x00)
const uint16_t ExplosionImage[] = { \
    BGND,BGND,BGND,BGND,EXPL,EXPL,BGND,BGND,BGND,BGND,
    EXPL,BGND,BGND,BGND,EXPL,EXPL,BGND,BGND,BGND,EXPL,
    BGND,EXPL,BGND,BGND,EXPL,EXPL,BGND,BGND,EXPL,BGND,
    BGND,BGND,EXPL,BGND,EXPL,EXPL,BGND,EXPL,BGND,BGND,
    BGND,BGND,BGND,EXPL,EXPL,EXPL,EXPL,BGND,BGND,BGND,
    EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,
    EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,EXPL,
    BGND,BGND,BGND,EXPL,EXPL,EXPL,EXPL,BGND,BGND,BGND,
    BGND,BGND,EXPL,BGND,EXPL,EXPL,BGND,EXPL,BGND,BGND,
    BGND,EXPL,BGND,BGND,EXPL,EXPL,BGND,BGND,EXPL,BGND,
    EXPL,BGND,BGND,BGND,EXPL,EXPL,BGND,BGND,BGND,EXPL,
    BGND,BGND,BGND,BGND,EXPL,EXPL,BGND,BGND,BGND,BGND,
    };      
    
