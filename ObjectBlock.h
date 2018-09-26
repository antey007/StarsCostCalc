#ifndef OBJECTBLOCK_H
#define OBJECTBLOCK_H

#include "Block.h"

class ObjectBlock : public Block {
public:

    int count;
    int objectId;
    int number;
    int owner;
    int type; // 0 = minefield, 1 = packet/salvage, 2 = wormhole, 3 = MT
    int x, y;

    //For MT
    int xDest, yDest;
    int warp;
    int metBits;
    int itemBits;
    int turnNo;

    //For minefields
    int mineType;
    long mineCount;

    //For wormholes
    int wormholeId;
    int targetId;

    //For packet/salvage
    int Destination;
    int NA2;
    int NA3;
    int NA4;
    int ironium, boranium, germanium;

    ObjectBlock()
        :Block()
    {
        typeId = BlockType::OBJECT;
    };

    ObjectBlock(unsigned char * data)
        :Block()
    {
        typeId = BlockType::OBJECT;

        if (size == 2) {
            count = *(short*)(data);
            return;
        }
        count = -1;
        objectId = *(short*)(data);
        number = objectId & 0x01FF;
        owner = (objectId & 0x1E00) >> 9;
        type = objectId >> 13;
        if (type == 0) { // minefield
            x = *(short*)(data+2);
            y = *(short*)(data+4);
            mineCount = *(long*)(data + 6);
            mineType = data[12];
        }
        else if (type == 1){ // packet or salvage
            x = *(short*)(data+2);
            y = *(short*)(data+4);
            Destination = data[6] + ((data[7] & 3) << 8);
            NA2 = data[7] >> 2;
            warp = 4 + (NA2 >> 2);
            ironium = *(short*)(data+8);
            boranium = *(short*)(data+10);
            germanium = *(short*)(data+12);
            NA3 = data[14];
            NA4 = data[15];
            turnNo = *(short*)(data+16);
        }
        else if (type == 2){ // wormhole
            x = *(short*)(data+2);
            y = *(short*)(data+4);
            wormholeId = *(short*)(data+4) >> 12;
            int beenThrough = *(short*)(data+8);
            int canSee = *(short*)(data+10);
            targetId = *(short*)(data+12) >> 12;
        }
        else if (type == 3){ // MT
            x = *(short*)(data+2);
            y = *(short*)(data+4);
            xDest = *(short*)(data+6);
            yDest = *(short*)(data+8);
            warp = data[10] >> 4;
            metBits = *(short*)(data+12);
            itemBits = *(short*)(data+14);
            turnNo = *(short*)(data+16);
        }

    }
};

#endif // OBJECTBLOCK_H
