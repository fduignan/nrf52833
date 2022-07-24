#ifndef __BATTLESHIP_H
#define __BATTLESHIP_H

#define CARRIER_COUNT 1
#define BATTLESHIP_COUNT 1
#define SUBMARINE_COUNT 2
#define PATROL_BOAT_COUNT 2

#define CARRIER_SQUARES 5
#define BATTLESHIP_SQUARES 4
#define SUBMARINE_SQUARES 3
#define PATROL_BOAT_SQUARES 2

#define MY_SHIP_NORMAL 1
#define MY_SHIP_HIT 2
#define THEIR_SHIP_NORMAL 0x81
#define THEIR_SHIP_HIT 0x82
class ship {
public:
    ship(){};
    void place(int len, int row, int col, int orientation);
    int contains(int row, int col);
    int overlaps(int len, int row, int col, int orientation);
private:
    uint8_t row,col;
    uint8_t len;
    uint8_t orientation;
    uint32_t state;
};

void battleship();

#endif
