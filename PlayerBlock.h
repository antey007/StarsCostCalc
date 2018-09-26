#ifndef PLAYERBLOCK_H
#define PLAYERBLOCK_H

#include "Block.h"

class PlayerBlock : public Block {
public:

    int playerNumber;
    int shipDesigns;
    int planets;
    int fleets;
    int starbaseDesigns;
    int logo;
    bool fullDataFlag;
    //char fullDataSize;
    //char fullDataBytes[256];
    //char playerRelations[16]; // 0 neutral, 1 friend, 2 enemy
    //char nameBytes[16];
    char byte7;

    PlayerBlock()
        :Block()
    {
        typeId = BlockType::PLAYER;
    };

    PlayerBlock(unsigned char * data)
        :Block()
    {
        typeId = BlockType::PLAYER;
        playerNumber = data[0];
        shipDesigns = data[1];
        planets = (data[2]) + ((data[3] & 0x03) << 8);
        fleets = (data[4]) + ((data[5] & 0x03) << 8);
        starbaseDesigns = ((data[5] & 0xF0) >> 4);
        logo = ((data[6] & 0xFF) >> 3);
        fullDataFlag = (data[6] & 0x04) != 0;
        byte7 = data[7];

        //TODO load all data
    };

};


#endif // PLAYERBLOCK_H
