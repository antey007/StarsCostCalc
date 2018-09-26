#include "eventsdlg.h"

#include "Block.h"
//#include "PlayerBlock.h"
//#include "DesignBlock.h"
#include "decryptor.h"
#include "defs.h"

eventsDlg::eventsDlg(QString fileName, QWidget *parent) :
    QDialog(parent)
{
    mainWindow = (MainWindow *) parent;

    setWindowTitle("Events in file " + fileName);
    setMinimumSize(1200,400);
    tw_events = new QTableWidget(0,4,this);

    QStringList slist;
    slist << tr("Type") << tr("GOTO button") <<  tr("Size") << tr("Text");
    tw_events -> setHorizontalHeaderLabels(slist);
    tw_events -> setColumnWidth(0,50);
    tw_events -> setColumnWidth(1,50);
    tw_events -> setColumnWidth(2,50);
    tw_events -> setColumnWidth(3,650);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget(tw_events);
    setLayout(layout);


    QFile file(fileName);
    file.open(QFile::ReadOnly);
    qint64 size = file.size();

    qDebug() << "Loading file " << fileName << ", Size =" << size;

    Decryptor decryptor;
    unsigned char * data = new unsigned char[size];
    unsigned char * decryptedData = new unsigned char[size];
    int fileLength = file.read( (char*) data, size);


    int blockType;
    int blockSize;
    int offset;
    int events = 0;

    for (offset = 0; offset < fileLength; offset+=blockSize+2) {
        blockType = (data[offset+1]>>2);
        blockSize = data[offset] + (data[offset+1]&0x03) * 256;
        //qDebug("block type %d, size %d",blockType,blockSize);

        //--------------- DECRYPTION
        if(blockType == BlockType::FILE_HEADER) {
            FileHeaderBlock header(data);
            decryptor.initDecryption(header);
            qDebug("GameId: %x", header.gameId);
            qDebug("Version: %d.%d.%d", header.versionMajor, header.versionMinor, header.versionIncrement);
            qDebug("Turn: %d; Year: %d", header.turn, header.turn + 2400);
            qDebug("Player Number: %d; Displayed as: %d", header.playerNumber, header.playerNumber+1);
            qDebug("Encryption Salt: %x", header.encryptionSalt);

        } else {
            for(int i = 0; i < blockSize; i+=4) {
                *(long*) (decryptedData+i) = *((long*) (data+offset+2+i)) ^ decryptor.starsRandom.nextRandom();
            }
        }

        if(blockType == BlockType::EVENTS) {
            int eventsOffset;
            int subblockType;
            int subblockSize;
            for (eventsOffset = 0; eventsOffset < blockSize; eventsOffset+=subblockSize) {
                subblockType = decryptedData[eventsOffset] + ((decryptedData[eventsOffset+1]&0x1)<<8);

                subblockSize = 4 + mainWindow->message[subblockType].args;
                for (int bit = 1; bit<8; bit++)
                    if (decryptedData[eventsOffset+1] & (1<<bit))
                        subblockSize ++;

                QString gotoString;
                int param1;
                int param2;
                switch(decryptedData[eventsOffset+3]>>5) {
                case 0:
                    param1 = decryptedData[eventsOffset+2] + ((decryptedData[eventsOffset+3]&0x5) << 8); // planet 0..1023
                    gotoString = tr("Planet ") + QString::number(param1+1);
                    break;
                case 2:
                    param1 = decryptedData[eventsOffset+2];     // battle ID 0..255
                    param2 = decryptedData[eventsOffset+3]&0x1F; // year    0..15
                    gotoString = tr("Battle ");
                    gotoString += QString::number(param1)+tr(" ") + QString::number(param2);
                    break;
                case 4:
                    param1 = decryptedData[eventsOffset+2] + ((decryptedData[eventsOffset+3]&0x1) << 8); // fleet 0..511
                    param2 = (decryptedData[eventsOffset+3]&0x1F)>>1;       // player 0..15
                    gotoString = tr("Fleet ") + QString::number(param1+1) + tr(" (player ") + QString::number(param2+1) + tr(")");
                    break;
                case 6:
                    param1 = decryptedData[eventsOffset+3]&0x1F;    //  category 0..15
                    // 0 - engine, 1 - scanner, 2 - shield, 3 - armor
                    // 4 - beam weapon, 5 - torp, 6 - bomb, 7 - mining robot
                    // 8 - minelayer, 9 - orbital
                    // 10, 14 - UNKNOWN
                    // 11 - electrical, 12 - mechanical, 13 - terraforming, 15 - planetary
                    param2 = decryptedData[eventsOffset+2];         //  technology 0..23
                    gotoString = tr("(F2) Tech browser ") + QString::number(param1) + tr("/") + QString::number(param2);
                    break;
                case 7:
                    if (decryptedData[eventsOffset+2] == 249) {
                        gotoString = tr("Battle summary report (?)");
                    } else if (decryptedData[eventsOffset+2] == 250) {
                        gotoString = tr("Deep space");
                    } else if (decryptedData[eventsOffset+2] == 251) {
                        gotoString = tr("Stars! serial number");
                    } else if (decryptedData[eventsOffset+2] == 252) {
                        gotoString = tr("(F10) Player scores");
                    } else if (decryptedData[eventsOffset+2] == 253) {
                        gotoString = tr("(F4) Ship design");
                    } else if (decryptedData[eventsOffset+2] == 254) {
                        gotoString = tr("(F5) Research");
                    } else if (decryptedData[eventsOffset+2] == 255) {
                        gotoString = tr("Button disabled");
                    } else {
                        gotoString = tr("--- UNKNOWN VALUE ---")+QString::number(decryptedData[eventsOffset+2])+tr(" ") + QString::number(decryptedData[eventsOffset+3]);
                    }
                    break;
                default:
                    gotoString = tr("--- UNKNOWN VALUE ---")+QString::number(decryptedData[eventsOffset+2])+tr(" ") + QString::number(decryptedData[eventsOffset+3]);
                    break;
                }

                QString message;
                QStringList list = mainWindow->message[subblockType].msgText.split("\\");
                message  += list[0];
                int argOffset = eventsOffset+4;
                int arg = 0;
                for (int part = 1; part < list.count(); part++, argOffset++, arg++) {
                    char argType;
                    argType = *(list[part].left(1).toLocal8Bit().data());
                    int num = decryptedData[argOffset];
                    if (decryptedData[eventsOffset+1] & (2<<arg)) {
                        argOffset++;
                        num += (decryptedData[argOffset]<<8);
                    }

                    double g = 0;

                    switch (argType) {
                    case 'i':
                        message += QString::number(num);
                        break;
                    case 'n':
                        message += tr("Space (") + QString::number(num);
                        argOffset++;
                        arg++;
                        num = decryptedData[argOffset];
                        if (decryptedData[eventsOffset+1] & (2<<arg)) {
                            argOffset++;
                            num += decryptedData[argOffset]<<8;
                        }
                        message += tr(", ") + QString::number(num) + tr(")");
                        break;
                    case 'I':
                        switch (num) {
                        case 0: message += tr("decreased"); break;
                        case 1: message += tr("increased"); break;
                        default: message += tr("(I")+QString::number(num)+tr(")"); break;
                        }
                        break;
                    case 'L':
                        message += tr("(player ")+QString::number(num+1)+tr(")");
                        break;
                    case 'M':
                        switch (num) {
                        case 0: message += tr("Standart"); break;
                        case 1: message += tr("Heavy"); break;
                        case 2: message += tr("(M2)"); break;
                        case 3: message += tr("(M3)"); break;
                        default: message += tr("(M")+QString::number(num)+tr(")"); break;
                        }
                        break;
                    case 'E':
                        switch (num>>8) {
                        case 0:
                            if ((num&0xFF) < 25) {
                                g = 1./(8.-0.24*(num&0xFF));
                            } else if ((num&0xFF) < 50) {
                                g = 25./(75.-(num&0xFF));
                            } else if ((num&0xFF) < 75) {
                                g = (num&0xFF)/25.-1.;
                            } else {
                                g = (num&0xFF)*0.24-16.;
                            }
                            message += QString::number(g) + tr("g"); break;
                        case 1: message += QString::number((num&0xFF)*4-200) + tr("C"); break;
                        case 2: message += QString::number(num&0xFF) + tr("mR"); break;
                        }
                        break;
                    case 'e':
                        switch (num) {
                        case 0: message += tr("Gravity"); break;
                        case 1: message += tr("Temperature"); break;
                        case 2: message += tr("Radiation"); break;
                        default: message += tr("(e")+QString::number(num)+tr(")"); break;
                        }
                        break;
                    case 'p':
                        message += tr("Planet ")+QString::number(num+1);
                        break;
                    case 's':
                        message += tr("Fleet ")+QString::number((num&0x1FF)+1);
                        break;
                    case 'z':
                        message += tr("(") + tr(((num>>4)&1)?"SB":"ship") + tr(" design ")+QString::number(num&0x0F)+tr(" name)");
                        break;
                    default:
                        message += tr("(")+argType+QString::number(num)+tr(")");
                        break;
                    }
                    message += list[part].right(list[part].length()-1);
                }

                tw_events -> insertRow(events);
                tw_events -> setCellWidget(events, 0, new QLabel(QString::number(subblockType)));
                tw_events -> setCellWidget(events, 1, new QLabel(gotoString));
                //tw_events -> setCellWidget(events, 2, new QLabel(QString::number(subblockSize)));
                tw_events -> setCellWidget(events, 2, new QLabel(QString::number(decryptedData[eventsOffset+1])));
                //tw_events -> setCellWidget(events, 3, new QLabel(mainWindow->message[subblockType].msgText));
                tw_events -> setCellWidget(events, 3, new QLabel(message));
                events++;
            }
        }
    }
    tw_events -> resizeColumnsToContents();
    tw_events -> resizeRowsToContents();

    delete[] data;
    delete[] decryptedData;
}
