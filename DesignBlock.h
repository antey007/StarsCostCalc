#ifndef DESIGNBLOCK_H
#define DESIGNBLOCK_H

#include "Block.h"

class Slot {
public:
    int category;
    int itemId;
    int count;
};

class DesignBlock : public Block {
public:
    bool isFullDesign;
    bool isTransferred;
    bool isStarbase;
    int designNumber;
    int hullId;
    int pic;
    int mass; // for full designs, this is calculated
    int armor;
    int slotCount;
    Slot slot[16];

    int nameLen;
    unsigned char name[32];

    DesignBlock()
        : Block()
    {
        typeId = BlockType::DESIGN;
    };


    DesignBlock(unsigned char * data)
        : Block()
    {
        typeId = BlockType::DESIGN;
        isFullDesign = (data[0] & 0x04) == 0x04;
        isTransferred = (data[1] & 0x80) == 0x80;
        isStarbase = (data[1] & 0x40) == 0x40;
        designNumber = (data[1] & 0x3C) >> 2;
        hullId = data[2] & 0xFF;
        pic = data[3] & 0xFF;
        int index;
        if (isFullDesign) {
            mass = 0;
            armor = *(short*)(data+4);
            slotCount = data[6] & 0xff;
            index = 17;
            for (int i = 0; i < slotCount; i++) {
                slot[i].category = *(short*)(data+index);
                slot[i].itemId = data[index+2];
                slot[i].count = data[index+3];
                index += 4;
            }
        } else {
            mass = *(short*)(data+4);
            armor = slotCount = 0;
            index = 6;
        }
        nameLen = data[index];
        char * ptr = (char*)data+index+1;
        memcpy(name, ptr, nameLen);
        name[nameLen] = 0;

    };

};

#endif // DESIGNBLOCK_H
