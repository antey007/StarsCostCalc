#ifndef DEFS_H
#define DEFS_H

#include <QString>

struct cSlot {
    int maxCount;
    int type;
    int place;
};

struct cShipHull {
    int index;
    QString name;
    int hullID;
    int techReq[6];
    int mass;
    int costRes;
    int costMinerals[3];
    int cargo;
    int fuel;
    int armor;
    int engineCount;
    cSlot slot[16];
    int slotCount;

    int class1;
    int class2;
    int initiative;
    int placeCargoBeg;
    int placeCargoEnd;
};

struct cItem {
    int category;
    int index;
    QString name;
    int itemID;
    int techReq[6];
    int mass;
    int costRes;
    int costMinerals[3];
    int globalID;
    int weaponRange;
    int weaponPower;
    int weaponInit;
    int weaponType;
    int weaponAccuracy;

};

struct cDesign {
    int owner;
    int designNumber;
    int hullID;
    int items[16];
    int counts[16];
    QString designName;
};

static const QString hullShortNames[] = {"SmFr", "MdFr", "LaFr", "SupF",
                            "Sc",   "FF",   "DD",   "CC",  "BC",   "BB",    "Dn",
                            "Pvt",  "Rog",  "Gal",
                            "MCS",  "Col",
                            "MiB",  "B17",  "StB",  "B52",
                            "mdM",  "miM",  "Mine", "MxM", "UlM",
                            "Fuel", "SFX",
                            "MML",  "SML",
                            "Nub",  "mMor", "MM"};

/*
          Category
     1    Orbital,          9   0x0200
     2    BeamWeapon,       4   0x0010
     3    Torpedo,          5   0x0020
     4    Bomb,             6   0x0040
     5    Terraforming,
     6    Planetary,        10  0x0400
     7    MiningRobot,      7   0x0080
     8    MineLayer,        8   0x0100
     9    Mechanical,       12  0x1000
    10    Electrical,       11  0x0800
    11    Shield,           2   0x0004
    12    Scanner,          1   0x0002
    13    Armor,            3   0x0008
    14    Engine,           0   0x0001
    15    ShipHull,
    16    StarbaseHull
*/

struct cTechs {
    int type;
    int tech[6];
    int techMax[6];
};

struct cMessage {
    int msgID;
    int args;
    QString msgText;
};

#endif // DEFS_H
