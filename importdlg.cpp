#include "importdlg.h"

#include "Block.h"
#include "PlayerBlock.h"
#include "DesignBlock.h"
#include "decryptor.h"
#include "defs.h"

importDesignDlg::importDesignDlg(QString fileName, QWidget *parent) :
    QDialog(parent)
{
    mainWindow = (MainWindow *) parent;

    setWindowTitle("Select design");
    setMinimumSize(500,400);
    tw_designes = new QTableWidget(0,6,this);

    QStringList slist;
    slist << tr("Owner") << tr("Number") << tr("Name") << tr("Hull") << tr("Mass") << tr("Import");
    tw_designes -> setHorizontalHeaderLabels(slist);
    tw_designes -> setColumnWidth(0,50);
    tw_designes -> setColumnWidth(1,50);
    tw_designes -> setColumnWidth(2,100);
    tw_designes -> setColumnWidth(3,100);
    tw_designes -> setColumnWidth(4,40);
    tw_designes -> setColumnWidth(5,50);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    anonymity = new QGroupBox("Design anonymization",this);
    anonymity -> setCheckable(true);
    suppressOwner = new QCheckBox("Suppress design ownership",anonymity);
    suppressOwner -> setChecked(true);
    suppressDesNumber = new QCheckBox("Suppress design number",anonymity);
    suppressDesNumber -> setChecked(true);
    suppressName = new QCheckBox("Suppress design name",anonymity);
    suppressName -> setChecked(true);

    QVBoxLayout * anonymityLayout = new QVBoxLayout();
    anonymityLayout -> setSpacing(0);
    anonymityLayout -> setContentsMargins(6,6,6,6);
    anonymityLayout -> addWidget(suppressOwner);
    anonymityLayout -> addWidget(suppressDesNumber);
    anonymityLayout -> addWidget(suppressName);
    anonymity -> setLayout(anonymityLayout);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget(tw_designes);
    layout->addWidget(anonymity);
    layout->addWidget(buttonBox);
    setLayout(layout);



    char playerShipDesigns[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int count = 0;
    int player = 0;

    designCount = 0;

    QFile file(fileName);
    file.open(QFile::ReadOnly);
    qint64 size = file.size();

    qDebug() << "Loading file " << fileName << ", Size =" << size;

    Decryptor decryptor;
    unsigned char * data = new unsigned char[size];
    unsigned char * decryptedData = new unsigned char[size];
    int fileLength = file.read( (char*) data, size);


    // Index where we start to read the next block
    int offset = 0;
    while (offset < fileLength) {
        int blockType = (data[offset+1]>>2);
        int blockSize = data[offset] + (data[offset+1]&0x03) * 256;
        //qDebug("block type %d, size %d",blockType,blockSize);

        //--------------- DECRYPTION
        if(blockType == BlockType::FILE_HEADER) {
            FileHeaderBlock header(data);
            decryptor.initDecryption(header);
            //qDebug("GameId: %x", header.gameId);
            //qDebug("Version: %d.%d.%d", header.versionMajor, header.versionMinor, header.versionIncrement);
            //qDebug("Turn: %d; Year: %d", header.turn, header.turn + 2400);
            //qDebug("Player Number: %d; Displayed as: %d", header.playerNumber, header.playerNumber+1);
            //qDebug("Encryption Salt: %x", header.encryptionSalt);

        } else {
            for(int i = 0; i < blockSize; i+=4) {
                *(long*) (decryptedData+i) = *((long*) (data+offset+2+i)) ^ decryptor.starsRandom.nextRandom();
            }
        }
        //
        if(blockType == BlockType::PLAYER) {
            PlayerBlock player(decryptedData);
            //qDebug() << "Player " << (player.playerNumber+1) << "have" << player.shipDesigns << "designes";
            playerShipDesigns[player.playerNumber] = player.shipDesigns;
        }

        if(blockType == BlockType::DESIGN) {
            DesignBlock designBlock(decryptedData);
            if (designBlock.isStarbase) break;
            //qDebug("desN=%d hull=%d slotCnt=%d mass=%d nameLen=%d %h %h %s",
            //       design.designNumber, design.hullId, design.slotCount, design.mass, design.nameLen, design.name[0], design.name[1], design.name);
            while (count >= playerShipDesigns[player]) {
                count -= playerShipDesigns[player];
                player++;
            }
            int nameLen;
            char name[50];
            decryptor.decodeBytesForStarsString(designBlock.nameLen, designBlock.name, &nameLen, (unsigned char *) name);
            //qDebug() << "Design founded: owner" << (player+1) << "design slot" << (designBlock.designNumber+1) << "hullID" << designBlock.hullId << name;
            count++;

            if (designBlock.isFullDesign) {
                design[designCount].hullID = designBlock.hullId;
                design[designCount].owner = player+1;
                design[designCount].designNumber = designBlock.designNumber+1;
                for (int i = 0; i < designBlock.slotCount; i++) {
                    design[designCount].counts[i] = designBlock.slot[i].count;
                    if (designBlock.slot[i].category == 0) design[designCount].items[i] = 255;
                    else {
                        int category = 0;
                        for (int j = 0; j<13; j++) if (designBlock.slot[i].category == 1<<j) category = j;
                        static const int categoryShift[] = {186, 158, 148, 174, 16, 40, 52, 102, 110, 0, 87, 131, 120}; // MAGIC
                        design[designCount].items[i] = designBlock.slot[i].itemId + categoryShift[category];
                    }
                }
                design[designCount].designName = QString::fromLocal8Bit(name,nameLen);

                int row = tw_designes->rowCount();
                tw_designes -> insertRow(row);
                tw_designes -> setCellWidget(row, 0, new QLabel(QString::number(player+1)));
                tw_designes -> setCellWidget(row, 1, new QLabel(QString::number(designBlock.designNumber+1)));
                tw_designes -> setCellWidget(row, 2, new QLabel(QString::fromLocal8Bit(name,nameLen)));
                tw_designes -> setCellWidget(row, 3, new QLabel(mainWindow->shipHull[designBlock.hullId].name));
                tw_designes -> setCellWidget(row, 4, new QLabel(QString::number(mainWindow->designMass(design[designCount]))));
                QCheckBox * checkbox = new QCheckBox();
                checkbox -> setChecked(true);
                tw_designes -> setCellWidget(row, 5, checkbox);

                tw_designes -> resizeRowToContents(row);
                designCount++;
            }

        }

        offset+=blockSize+2;
    }
    delete[] data;
    delete[] decryptedData;

}

void importDesignDlg::accept()
{
    for (int i = 0; i < designCount ; i++) {
        if (((QCheckBox* )tw_designes -> cellWidget(i, 5))-> isChecked()) {
            // Anonymization
            if (anonymity->isChecked()) {
                if (suppressOwner -> isChecked()) design[i].owner = 0;
                if (suppressDesNumber -> isChecked()) design[i].designNumber = 0;
                if (suppressName -> isChecked()) design[i].designName = "";
            }

            // Match check
            bool match = false;
            for (int j = 0; j < mainWindow -> designCount; j++) {
                if (mainWindow -> isEqual(design[i], mainWindow -> design[j])) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                // Adding
                mainWindow -> design[mainWindow -> designCount] = design[i];
                mainWindow -> count[mainWindow -> designCount] = 0;
                mainWindow -> designCount++;
                if (mainWindow -> designCount >= mainWindow -> maxDesignCount) mainWindow -> increaseMaxDesignCount();
            }
        }
    }

    mainWindow -> fillTable();
    mainWindow -> fleetChanged();

    QDialog::accept();
}


