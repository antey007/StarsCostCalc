#include "salvageanalyzerdlg.h"

#include "importdlg.h"
#include "Block.h"
#include "PlayerBlock.h"
#include "ObjectBlock.h"
#include "decryptor.h"
#include "defs.h"

salvageListDlg::salvageListDlg(QString fileName, QWidget *parent) :
    QDialog(parent)
{
    mainWindow = (MainWindow *) parent;
    setWindowTitle(tr("Select salvage from file ")+fileName);
    initView();
    initData(fileName);

    hideCrustedSalvage();
}

void salvageListDlg::initView()
{
    setMinimumSize(600,400);

    tw_salvage = new QTableWidget(0,12,this);
    QStringList slist;
    slist << tr("Owner") << tr("Number") << tr("    X    ") << tr("    Y    ") << tr("Iron") << tr("Bor") << tr("Germ")
          << tr("Decay") << tr("NA4") << tr("turnNo") << tr("hit MF") << tr("Battle");
    tw_salvage -> setHorizontalHeaderLabels(slist);
    tw_salvage -> resizeColumnsToContents();
    //tw_salvage -> setColumnWidth(2,50);
    //tw_salvage -> setColumnWidth(3,50);

    hideCrustedCheckBox = new QCheckBox("Hide crusted salvage");
    hideCrustedCheckBox -> setChecked(true);
    connect(hideCrustedCheckBox, SIGNAL(toggled(bool)), this, SLOT(hideCrustedSalvage()));

    hideEmptyCheckBox = new QCheckBox("Hide empty salvage");
    hideEmptyCheckBox -> setChecked(true);
    connect(hideEmptyCheckBox, SIGNAL(toggled(bool)), this, SLOT(hideCrustedSalvage()));

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    QVBoxLayout * layout = new QVBoxLayout();
    QHBoxLayout * checkBoxLayout = new QHBoxLayout();
    checkBoxLayout -> addWidget(hideCrustedCheckBox);
    checkBoxLayout -> addWidget(hideEmptyCheckBox);
    layout -> addWidget(tw_salvage);
    layout -> addLayout(checkBoxLayout);
    layout -> addWidget(buttonBox);
    setLayout(layout);
}

void salvageListDlg::initData(QString fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    qint64 size = file.size();

    qDebug() << "Loading file " << fileName << ", Size =" << size;
    Decryptor decryptor;
    unsigned char * data = new unsigned char[size];
    unsigned char * decryptedData = new unsigned char[size];
    int fileLength = file.read( (char*) data, size);

    int row = 0;

    int * mfSalvX = new int[1000];
    int * mfSalvY = new int[1000];
    int mfSalvCrdCount = 0;

    int * battleX = new int[1000];
    int * battleY = new int[1000];
    int battleCrdCount = 0;


    int blockType;
    int blockSize;
    for (int offset = 0; offset < fileLength; offset+=blockSize+2) {
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


                int X, Y;

                switch (subblockType) {
                case 199: // \s has been stopped in a \L \M mine field at \n. Your fleet has taken \i damage points and \i of your ships were destroyed.
                case 200: // \X\F has been annihilated in a \L \M mine field at \n.
                case 203: // \s has been stopped in your \M mine field at \n. Your mines have inflicted \i damage points and you have destroyed \i of the ships.
                case 204: // \X\s has been annihilated in your \M mine field at \n.
                case 351: // \F has been annihilated in a \L \M mine field at \n.
                case 353: // \s has taken damage from a detonating \L \M mine field at \n. Your fleet has taken \i damage points and \i of your ships were destroyed.
                case 354: // \F has been annihilated in your \M mine field at \n.
                case 356: // \s has been damaged by your detonating \M mine field at \n. Your mines have inflicted \i damage points and you have destroyed \i of the ships.
                    loadCoordFromEventsSubblock(&X, &Y, decryptedData+eventsOffset);
                    qDebug() << "salvageListDlg::loadCoordFromEventsSubblock " << subblockType << subblockSize << X << Y;
                    if (X == 65535) break;
                    mfSalvX[mfSalvCrdCount] = X;
                    mfSalvY[mfSalvCrdCount] = Y;
                    if (mfSalvCrdCount<1000) mfSalvCrdCount++;
                    break;

                case 126:
                case 145:
                case 146:
                case 147:
                case 148:
//                case 149:
                case 150:
                case 151:
                case 152:
                case 153:
                case 154:
                case 155:
                case 156:
                case 157:
                case 158:
                case 159:
                case 160:
                case 161:
                case 162:
                case 163:
                case 164:
                case 165:
                case 166:
                case 167:
                case 168:
                case 275:
                case 276:
                case 277:
                case 278:
                    loadCoordFromEventsSubblock(&X, &Y, decryptedData+eventsOffset);
                    qDebug() << "salvageListDlg::loadCoordFromEventsSubblock " << subblockType << subblockSize << X << Y;
                    if (X == 65535) break;
                    battleX[battleCrdCount] = X;
                    battleY[battleCrdCount] = Y;
                    if (battleCrdCount<1000) battleCrdCount++;
                    break;

                }
            }
            qDebug() << mfSalvCrdCount  << battleCrdCount;
        }


        if(blockType == BlockType::OBJECT) {
            ObjectBlock objectBlock(decryptedData);
            if (objectBlock.type != 1) continue; // not packet or salvage
            if (objectBlock.Destination != 1023) continue;// not salvage
            //if (objectBlock.turnNo != 0) continue;// not fresh

            tw_salvage -> insertRow(row);
            tw_salvage -> setCellWidget(row, 0, new QLabelModified(QString::number(objectBlock.owner+1)));
            tw_salvage -> setCellWidget(row, 1, new QLabelModified(QString::number(objectBlock.number)));
            tw_salvage -> setCellWidget(row, 2, new QLabelModified(QString::number(objectBlock.x)));
            tw_salvage -> setCellWidget(row, 3, new QLabelModified(QString::number(objectBlock.y)));
            tw_salvage -> setCellWidget(row, 4, new QLabelModified(QString::number(objectBlock.ironium)));
            tw_salvage -> setCellWidget(row, 5, new QLabelModified(QString::number(objectBlock.boranium)));
            tw_salvage -> setCellWidget(row, 6, new QLabelModified(QString::number(objectBlock.germanium)));
            tw_salvage -> setCellWidget(row, 7, new QLabelModified(QString::number(objectBlock.NA3)));
            tw_salvage -> setCellWidget(row, 8, new QLabelModified(QString::number(objectBlock.NA4)));
            tw_salvage -> setCellWidget(row, 9, new QLabelModified(QString::number(objectBlock.turnNo)));
            for (int i = 0; i< 10; i++) {
                ((QLabelModified *) (tw_salvage->cellWidget(row, i))) -> setAlignment(Qt::AlignCenter);
                connect((QLabelModified *) (tw_salvage->cellWidget(row, i)),SIGNAL(doubleClicked()),this,SLOT(salvageAnalyzer()));
            }
            for (int i = 0; i < mfSalvCrdCount; i++) {
                if (mfSalvX[i] != objectBlock.x) continue;
                if (mfSalvY[i] != objectBlock.y) continue;
                tw_salvage -> setCellWidget(row, 10, new QLabelModified("yes"));
                ((QLabelModified *) (tw_salvage->cellWidget(row, 10))) -> setAlignment(Qt::AlignCenter);
                connect((QLabelModified *) (tw_salvage->cellWidget(row, 10)),SIGNAL(doubleClicked()),this,SLOT(salvageAnalyzer()));
            }
            for (int i = 0; i < battleCrdCount; i++) {
                if (battleX[i] != objectBlock.x) continue;
                if (battleY[i] != objectBlock.y) continue;
                tw_salvage -> setCellWidget(row, 11, new QLabelModified("yes"));
                ((QLabelModified *) (tw_salvage->cellWidget(row, 11))) -> setAlignment(Qt::AlignCenter);
                connect((QLabelModified *) (tw_salvage->cellWidget(row, 11)),SIGNAL(doubleClicked()),this,SLOT(salvageAnalyzer()));
            }

            tw_salvage -> resizeRowToContents(row);
            row++;
        }
    }
    delete[] mfSalvX;
    delete[] mfSalvY;
    delete[] battleX;
    delete[] battleY;

    delete[] data;
    delete[] decryptedData;

    importDesignDlg dialog1(fileName, mainWindow);
    dialog1.anonymity -> setChecked(false);
    dialog1.accept();

    QFileInfo fileinfo(fileName);

    QString hFile = fileName.left(fileName.length()-fileinfo.suffix().length())+QString("h")+fileName.right(fileinfo.suffix().length()-1);
    //qDebug() << hFile;
    importDesignDlg dialog2(hFile, mainWindow);
    dialog2.anonymity -> setChecked(false);
    dialog2.accept();

}

void salvageListDlg::loadCoordFromEventsSubblock(int * X, int * Y, unsigned char * subblock)
{
    int subblockType = subblock[0] + ((subblock[1]&0x1)<<8);

    int subblockSize = 4 + mainWindow->message[subblockType].args;
    for (int bit = 1; bit<8; bit++)
        if (subblock[1] & (1<<bit))
            subblockSize ++;

    QStringList list = mainWindow->message[subblockType].msgText.split("\\");
    int argOffset = 4;
    int arg = 0;
    for (int part = 1; part < list.count(); part++, argOffset++, arg++) {
        char argType;
        argType = *(list[part].left(1).toLocal8Bit().data());

        *X = subblock[argOffset];
        if (subblock[1] & (2<<arg)) {
            argOffset++;
            *X += (subblock[argOffset]<<8);
        }

        if (argType=='n') {
            argOffset++;
            arg++;
            *Y = subblock[argOffset];
            if (subblock[1] & (2<<arg)) {
                argOffset++;
                *Y += subblock[argOffset]<<8;
            }
            break;
        }
    }
}

void salvageListDlg::hideCrustedSalvage()
{
    for (int row = 0; row< tw_salvage-> rowCount(); row++) {
        bool hide = false;
        if (hideCrustedCheckBox->isChecked() && ((QLabel *) (tw_salvage->cellWidget(row, 9))) -> text() != "0") hide = true;
        if (hideEmptyCheckBox->isChecked()
                && ((QLabel *) (tw_salvage->cellWidget(row, 4)))->text()=="0"
                && ((QLabel *) (tw_salvage->cellWidget(row, 5)))->text()=="0"
                && ((QLabel *) (tw_salvage->cellWidget(row, 6)))->text()=="0") hide = true;

        tw_salvage -> setRowHidden(row, hide);
    }
}

void salvageListDlg::salvageAnalyzer()
{
    int row = tw_salvage->currentRow();
    salvageAnalysezDlg salvageAnalysezDialog(row, this);
    salvageAnalysezDialog.exec();

}


salvageAnalysezDlg::salvageAnalysezDlg(int row, QWidget *parent) :
    QDialog(parent)
{
    salvageListDlg * listDialog = (salvageListDlg *) parent;
    mainWindow = listDialog -> mainWindow;

    player = ((QLabelModified *) (listDialog -> tw_salvage->cellWidget(row, 0))) -> text().toInt();
    minerals[0] = ((QLabelModified *) (listDialog -> tw_salvage->cellWidget(row, 4))) -> text().toInt();
    minerals[1] = ((QLabelModified *) (listDialog -> tw_salvage->cellWidget(row, 5))) -> text().toInt();
    minerals[2] = ((QLabelModified *) (listDialog -> tw_salvage->cellWidget(row, 6))) -> text().toInt();

    //qDebug() << player << minerals[0] << minerals[1] << minerals[2];

    initView();

    maxDisplayedTechsCount = 0;
    displayedTechsCount = 0;
    displayedTechs = NULL;
    increaseMaxDisplayedTechsCount();

    for (int i = 0; i < 6; i++) {
        minPassTech[i] = 0;
        maxPassTech[i] = 26;
        for (int j = 0; j<=26; j++)
            skipTech[i][j] = true;
    }

    fleetChanged();

}

void salvageAnalysezDlg::initView()
{
    setWindowTitle("Salvage analyzer");
    setMinimumSize(1000,450);

    tw_designes = new QTableWidget(0,6,this);
    QStringList slist;
    slist.clear();
    slist << tr("Owner") << tr("Number") << tr("Name") << tr("Hull") << tr("Mass") << tr("Count");
    tw_designes -> setHorizontalHeaderLabels(slist);
    tw_designes -> resizeColumnToContents(0);
    tw_designes -> resizeColumnToContents(1);
    tw_designes -> setColumnWidth(2,100);
    tw_designes -> setColumnWidth(3,100);
    tw_designes -> resizeColumnToContents(4);
    tw_designes -> setColumnWidth(5,50);

    designCount = 0;
    for (int i = 0; i < mainWindow->designCount; i++) {
        if (mainWindow->shown[i] == 0) continue;
        if (mainWindow->design[i].owner == player) {
            design[designCount] = mainWindow->design[i];
            count[designCount] = mainWindow->count[i];
            designCount++;
            if (designCount == MAXDESIGNCOUNT) {
                QMessageBox::critical(this, tr("Warning"), tr("designCount == MAXDESIGNCOUNT"));
                break;
            }
        }
    }
    for (int i = 0; i < designCount; i++) {

        tw_designes -> insertRow(i);
        tw_designes -> setCellWidget(i, 0, new QLabel(QString::number(design[i].owner)));
        tw_designes -> setCellWidget(i, 1, new QLabel(QString::number(design[i].designNumber)));
        tw_designes -> setCellWidget(i, 2, new QLabel(design[i].designName));
        tw_designes -> setCellWidget(i, 3, new QLabel(mainWindow->shipHull[design[i].hullID].name));
        tw_designes -> setCellWidget(i, 4, new QLabel(QString::number(mainWindow->designMass(design[i]))));

        QSpinBox * spinbox = new QSpinBox();
        spinbox -> setMaximum(9999);
        spinbox -> setValue(count[i]);
        tw_designes -> setCellWidget(i, 5, spinbox);
        connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(countChanged()));
        connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(fleetChanged()));
    }
    tw_designes -> resizeRowsToContents();

    label1 = new QLabel(this);


    tw_cost_short = new QTableWidget(0,11,this);
    slist.clear();
    slist << tr ("") << tr("En") << tr("Weap") << tr("Pro") << tr("Con") << tr("El") << tr("Bio") << tr("TechCost") << tr("Iron") << tr("Bor") << tr("Germ");
    tw_cost_short -> setHorizontalHeaderLabels(slist);
    tw_cost_short -> resizeColumnsToContents();
    tw_cost_short -> setColumnHidden(7,true);
    tw_cost_short -> setMaximumHeight(120);


    label2 = new QLabel("Full list of possible tech combinations (first 100 rows) :",this);

    tw_cost_full = new QTableWidget(0,11,this);
    slist.clear();
    slist << tr ("") << tr("En") << tr("Weap") << tr("Pro") << tr("Con") << tr("El") << tr("Bio") << tr("TechCost") << tr("Iron") << tr("Bor") << tr("Germ");
    tw_cost_full -> setHorizontalHeaderLabels(slist);
    tw_cost_full -> resizeColumnsToContents();
    tw_cost_full -> setColumnHidden(7,true);

    label3 = new QLabel("You must set the exact fleet composition.",this);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    phase1Button = new QPushButton(tr("Phase 1"));
    buttonBox->addButton(phase1Button, QDialogButtonBox::ActionRole);
    connect(phase1Button, SIGNAL(clicked()), this, SLOT(processingPhase1()));

    phase2Button = new QPushButton(tr("Phase 2"));
    buttonBox->addButton(phase2Button, QDialogButtonBox::ActionRole);
    connect(phase2Button, SIGNAL(clicked()), this, SLOT(processingPhase2()));

    QHBoxLayout * mainLayout = new QHBoxLayout();
    QVBoxLayout * leftLayout = new QVBoxLayout();
    QVBoxLayout * rightLayout = new QVBoxLayout();

    leftLayout -> addWidget(tw_designes);
    leftLayout -> addWidget(label1);
    rightLayout -> addWidget(tw_cost_short);
    rightLayout -> addWidget(label2);
    rightLayout -> addWidget(tw_cost_full);
    rightLayout -> addWidget(label3);
    rightLayout -> addWidget(buttonBox);
    mainLayout -> addLayout(leftLayout);
    mainLayout -> addLayout(rightLayout);
    setLayout(mainLayout);
}

void salvageAnalysezDlg::increaseMaxDisplayedTechsCount()
{
    qDebug() << "salvageAnalysezDlg::increaseMaxDisplayedTechsCount(): maxDisplayedTechsCount=" << maxDisplayedTechsCount+64;
    int newMaxDisplayedTechsCount = maxDisplayedTechsCount + 64;
    cTechs * newDisplayedTechs = new cTechs[newMaxDisplayedTechsCount];
    if (displayedTechs) {
        for (int i = 0; i < maxDisplayedTechsCount; i++) {
            newDisplayedTechs[i] = displayedTechs[i];
        }
        delete[] displayedTechs;
    }
    displayedTechs = newDisplayedTechs;
    maxDisplayedTechsCount = newMaxDisplayedTechsCount;
}

void salvageAnalysezDlg::fleetChanged()
{
    int flMass = 0;
    int flCount = 0;
    int flSum = 0;
    for (int i = 0; i < designCount; i++) {
        if (count[i]) {
            flCount ++;
            flSum += count[i];
            flMass += mainWindow -> designMass(design[i]) * count[i];
        }
    }

    QString text;
    text = tr("Total ")
            + QString::number(designCount)
            + tr(" designes. Selected ")
            + QString::number(flSum)
            + tr(" ships of ")
            + QString::number(flCount)
            + tr(" designes (total ")
            + QString::number(flMass)
            + tr(" kt)");
    label1->setText(text);

    minTech[0] = minTech[1] = minTech[2] = minTech[3] = minTech[4] = minTech[5] = 0;

    int dsnTech[] = {0,0,0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        if (count[i] == 0) continue;
        mainWindow -> designMinTechReq(design[i], dsnTech);
        for (int i = 0; i<6; i++)
            if (minTech[i] < dsnTech[i])
                minTech[i] = dsnTech[i];
    }


    int maxSlvCost[4] = {0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        int tokenSlvCost[4] = {0,0,0,0};
        mainWindow -> calculateCost(design[i], count[i], minTech, tokenSlvCost, 2);
        for (int j = 0; j < 4; j++) maxSlvCost[j] += tokenSlvCost[j];
    }

    maxTech[0] = maxTech[1] = maxTech[2] = maxTech[3] = maxTech[4] = maxTech[5] = 26;
    int minSlvCost[4] = {0,0,0,0};
    int minCost[4] = {0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        int tokenSlvCost[4] = {0,0,0,0};
        mainWindow -> calculateCost(design[i], count[i], maxTech, tokenSlvCost, 2);
        for (int j = 0; j < 4; j++) minSlvCost[j] += tokenSlvCost[j];
        int tokenCost[4] = {0,0,0,0};
        mainWindow -> calculateCost(design[i], count[i], maxTech, tokenCost, 1);
        for (int j = 0; j < 4; j++) minCost[j] += tokenCost[j];
    }

    // attempt to minimize maxTech
//    for (int tech = 0 ; tech < 6; tech++) {
//        do {
//            maxTech[tech] --;
//            if (maxTech[tech] < minTech[tech]) break;
//            int cost[4] = {0,0,0,0};
//            for (int i = 0; i < designCount; i++) {
//                int tokenCost[4] = {0,0,0,0};
//                mainWindow -> calculateCost(design[i], count[i], maxTech, tokenCost, 1);
//                for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
//            }
//            if (cost[0] != minCost[0]) break;
//            if (cost[1] != minCost[1]) break;
//            if (cost[2] != minCost[2]) break;
//            //if (cost[3] != minCost[3]) break;
//        } while (true);

//        // revert to last %maxTech% with %cost% equal to %minCost%
//        maxTech[tech] ++;
//    }




    bool phase1ButtonEnabled = true;
    tw_cost_short->setRowCount(4);
    tw_cost_short->setCellWidget(0,0,new QLabel("Minerals in salvage"));
    tw_cost_short->setCellWidget(1,0,new QLabel("Min tech / max cost"));
    tw_cost_short->setCellWidget(2,0,new QLabel("Max tech / min cost"));
    tw_cost_short->setCellWidget(3,0,new QLabel("Suitable techs"));

    for (int i = 0; i<6; i++) {
        QLabel * label = new QLabel("-",this);
        label -> setAlignment(Qt::AlignCenter);
       tw_cost_short->setCellWidget(0,i+1,label);
    }
    for (int i = 0; i<3; i++) {
        QLabel * label = new QLabel(QString::number(minerals[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        tw_cost_short->setCellWidget(0,i+8,label);
    }

    for (int i = 0; i<6; i++) {
        QLabel * label = new QLabel(QString::number(minTech[i]),this);
        label -> setAlignment(Qt::AlignCenter);
       tw_cost_short->setCellWidget(1,i+1,label);
    }
    for (int i = 0; i<3; i++) {
        QLabel * label = new QLabel(QString::number(maxSlvCost[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        if (maxSlvCost[i] < minerals[i]) phase1ButtonEnabled = false;
        tw_cost_short->setCellWidget(1,i+8,label);
    }


    for (int i = 0; i<6; i++) {
        QLabel * label = new QLabel(QString::number(maxTech[i])+QString(maxTech[i]==26?"":"+"),this);
        label -> setAlignment(Qt::AlignCenter);
        tw_cost_short->setCellWidget(2,i+1,label);
    }
    for (int i = 0; i<3; i++) {
        QLabel * label = new QLabel(QString::number(minSlvCost[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        if (minSlvCost[i] > minerals[i]) phase1ButtonEnabled = false;
        tw_cost_short->setCellWidget(2,i+8,label);
    }

    for (int i = 0; i<6; i++) {
        QString string = QString::number(minPassTech[i]);
        if (minPassTech[i] != maxPassTech[i])
            string += tr("-")+QString::number(maxPassTech[i]);
        QLabelModified * label = new QLabelModified(string,this);
        label -> setAlignment(Qt::AlignCenter);
       tw_cost_short->setCellWidget(3,i+1,label);
    }
    for (int i = 0; i<3; i++) {
        QLabel * label = new QLabel(QString::number(minerals[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        tw_cost_short->setCellWidget(3,i+8,label);
    }


    int rowCount = displayedTechsCount;
    if (rowCount > 100) rowCount = 100;
    tw_cost_full->setRowCount(rowCount);
    for (int displayedTech = 0; displayedTech < rowCount; displayedTech++) {
        int row = displayedTech;
        QLabelModified * typeLabel = new QLabelModified(this);
        switch (displayedTechs[displayedTech].type) {
        case 1: typeLabel -> setText("MANUAL"); break;
        case 2: typeLabel -> setText("   AUTO    "); break;
        }
        typeLabel -> setAlignment(Qt::AlignCenter);

        tw_cost_full->setCellWidget(row,0,typeLabel);
        for (int i = 0; i<6; i++) {
            QString string = QString::number(displayedTechs[displayedTech].tech[i]);
            if (displayedTechs[displayedTech].techMax[i] != displayedTechs[displayedTech].tech[i])
                string += tr("-")+QString::number(displayedTechs[displayedTech].techMax[i]);
            QLabelModified * label = new QLabelModified(string,this);
            label -> setAlignment(Qt::AlignCenter);
            tw_cost_full->setCellWidget(row,i+1,label);
        }
        int cost[4] = {0,0,0,0};
        for (int i = 0; i < designCount; i++) {
            int tokenCost[4] = {0,0,0,0};
            mainWindow -> calculateCost(design[i], count[i], displayedTechs[displayedTech].tech, tokenCost, 2);
            for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
        }

        for (int i = 0; i<4; i++) {
            QLabelModified * label = new QLabelModified(QString::number(cost[i]),this);
            label -> setAlignment(Qt::AlignCenter);
            label -> setAutoFillBackground(true);
            if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
            if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
            if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
            tw_cost_full->setCellWidget(row,i+8,label);
        }
    }


    tw_cost_short->resizeRowsToContents();
    tw_cost_short->resizeColumnsToContents();

    tw_cost_full->resizeRowsToContents();
    tw_cost_full->resizeColumnsToContents();

    phase1Button -> setEnabled(phase1ButtonEnabled);
    QColor color = phase1ButtonEnabled?Qt::black:Qt::red;
    label3 -> setPalette(QPalette(color,Qt::white,color,color,color,color,Qt::white));

}

void salvageAnalysezDlg::countChanged()
{
    for (int i = 0; i< designCount; i++) {
        count[i] = ((QSpinBox*) tw_designes -> cellWidget(i, 5))-> value();
    }
}

void salvageAnalysezDlg::processingPhase1()
{
    int freedomDegree = 1;
    for (int i = 0; i<6; i++) freedomDegree*=(maxTech[i]+1-minTech[i]);
    qDebug() << "Freedom degree 1 =" << freedomDegree;

    // Processing speed optimization: round 2 (2018.08.22)
    // нахожу уровни технологий, которые можно пропустить
    int tech[6];
    for (int i = 0 ; i < 6; i++) {
        for (int j = 0 ; j < 6; j++) tech[j] = maxTech[j];
        tech[i] = minTech[i];
        int prevCost[4] = {0,0,0,0};
        for (int ii = 0; ii < designCount; ii++) {
            int tokenCost[4] = {0,0,0,0};
            mainWindow -> calculateCost(design[ii], count[ii], tech, tokenCost, 1);
            for (int jj = 0; jj < 4; jj++) prevCost[jj] += tokenCost[jj];
        }
        skipTech[i][minTech[i]] = false; // skipTech[i][minTech[i]] is always false
        for (tech[i] = minTech[i]+1; tech[i] <= maxTech[i]; tech[i]++) {
            int cost[4] = {0,0,0,0};
            for (int ii = 0; ii < designCount; ii++) {
                int tokenCost[4] = {0,0,0,0};
                mainWindow -> calculateCost(design[ii], count[ii], tech, tokenCost, 1);
                for (int jj = 0; jj < 4; jj++) cost[jj] += tokenCost[jj];
            }
            skipTech[i][tech[i]] = (cost[0] == prevCost[0] && cost[1] == prevCost[1] && cost[2] == prevCost[2]); // if eqial - true - skip this level
            for (int jj = 0; jj < 4; jj++) prevCost[jj] = cost[jj];
        }
    }

    freedomDegree = 1;
    int a[6];
    for (int i = 0; i<6; i++) {
        a[i] = 0;
        for (int j = minTech[i]; j <= maxTech[i]; j++)
            if (!skipTech[i][j]) a[i]++;
        freedomDegree*=a[i];
    }
    qDebug() << "Freedom degree 2 =" << freedomDegree << "(" << a[0] << a[1] << a[2] << a[3] << a[4] << a[5] << ")";

    qDebug() << "minTech=" << minTech[0] << minTech[1] << minTech[2] << minTech[3] << minTech[4] << minTech[5];
    qDebug() << "maxTech=" << maxTech[0] << maxTech[1] << maxTech[2] << maxTech[3] << maxTech[4] << maxTech[5];

    for (int tech = 0 ; tech < 6; tech++) {
        int curTech[6];
        for (int i = 0; i<6; i++) curTech[i] = maxTech[i];
        for (curTech[tech] = minTech[tech]; curTech[tech]<= maxTech[tech]; curTech[tech]++) {
            int cost[4] = {0,0,0,0};
            for (int ii = 0; ii < designCount; ii++) {
                int tokenCost[4] = {0,0,0,0};
                mainWindow -> calculateCost(design[ii], count[ii], curTech, tokenCost, 2);
                for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
            }
            if (cost[0] > minerals[0]) minTech[tech] = curTech[tech]+1;
            if (cost[1] > minerals[1]) minTech[tech] = curTech[tech]+1;
            if (cost[2] > minerals[2]) minTech[tech] = curTech[tech]+1;
        }
    }

    for (int tech = 0 ; tech < 6; tech++) {
        int curTech[6];
        for (int i = 0; i<6; i++) curTech[i] = minTech[i];
        for (curTech[tech] = maxTech[tech]; curTech[tech]>= minTech[tech]; curTech[tech]--) {
            int cost[4] = {0,0,0,0};
            for (int ii = 0; ii < designCount; ii++) {
                int tokenCost[4] = {0,0,0,0};
                mainWindow -> calculateCost(design[ii], count[ii], curTech, tokenCost, 2);
                for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
            }
            if (cost[0] < minerals[0]) maxTech[tech] = curTech[tech]-1;
            if (cost[1] < minerals[1]) maxTech[tech] = curTech[tech]-1;
            if (cost[2] < minerals[2]) maxTech[tech] = curTech[tech]-1;
        }
    }

    freedomDegree = 1;
    for (int i = 0; i<6; i++) {
        a[i] = 0;
        for (int j = minTech[i]; j <= maxTech[i]; j++)
            if (!skipTech[i][j]) a[i]++;
        freedomDegree*=a[i];
    }
    qDebug() << "Freedom degree 3 =" << freedomDegree << "(" << a[0] << a[1] << a[2] << a[3] << a[4] << a[5] << ")";

    qDebug() << "minTech=" << minTech[0] << minTech[1] << minTech[2] << minTech[3] << minTech[4] << minTech[5];
    qDebug() << "maxTech=" << maxTech[0] << maxTech[1] << maxTech[2] << maxTech[3] << maxTech[4] << maxTech[5];
}

void salvageAnalysezDlg::processingPhase2()
{
    QProgressDialog progress("Processing...", "Abort", 0, (maxTech[0]+1-minTech[0])*(maxTech[1]+1-minTech[1]), this);
    progress.setMinimumDuration(1);
    progress.setWindowModality(Qt::WindowModal);

    int A1 = 0;
    int A2 = 0;
    displayedTechsCount = 0;

    for (int i = 0; i < 6; i++) {
        minPassTech[i] = 26;
        maxPassTech[i] = 0;
    }

    int tech[6] = {0,0,0,0,0,0};
    for (tech[0] = minTech[0]; tech[0]<=maxTech[0]; tech[0]++) {
        if (skipTech[0][tech[0]]) continue;    // Processing speed optimization
        for (tech[1] = minTech[1]; tech[1]<=maxTech[1]; tech[1]++) {
            progress.setValue((tech[0]-minTech[0])*(maxTech[1]+1-minTech[1])+(tech[1]-minTech[1]));
            if (progress.wasCanceled()) break;
            if (skipTech[1][tech[1]]) continue;    // Processing speed optimization
            for (tech[2] = minTech[2]; tech[2]<=maxTech[2]; tech[2]++) {
                if (skipTech[2][tech[2]]) continue;    // Processing speed optimization
                for (tech[3] = minTech[3]; tech[3]<=maxTech[3]; tech[3]++) {
                    if (skipTech[3][tech[3]]) continue;    // Processing speed optimization
                    for (tech[4] = minTech[4]; tech[4]<=maxTech[4]; tech[4]++) {
                        if (skipTech[4][tech[4]]) continue;    // Processing speed optimization
                        for (tech[5] = minTech[5]; tech[5]<=maxTech[5]; tech[5]++) {
                            if (skipTech[5][tech[5]]) continue;    // Processing speed optimization

                            A1 ++;
                            int cost[4] = {0,0,0,0};
                            for (int ii = 0; ii < designCount; ii++) {
                                int tokenCost[4] = {0,0,0,0};
                                mainWindow -> calculateCost(design[ii], count[ii], tech, tokenCost, 2);
                                for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
                            }

                            if (cost[0] == minerals[0] && cost[1] == minerals[1] && cost[2] == minerals[2]) {
                                A2++;
                                for (int ii = 0; ii < 6; ii++) {
                                    if (minPassTech[ii] > tech[ii]) minPassTech[ii] = tech[ii];
                                    if (maxPassTech[ii] < tech[ii]) maxPassTech[ii] = tech[ii];
                                }

                                for (int i = 0; i<6; i++) {
                                    displayedTechs[displayedTechsCount].tech[i] = tech[i];
                                    displayedTechs[displayedTechsCount].techMax[i] = tech[i];
                                    for (int j = tech[i]+1; j <= 26; j++) {
                                        if (skipTech[i][j] == true)
                                            displayedTechs[displayedTechsCount].techMax[i] = j;
                                        else
                                            break;
                                    }
                                }
                                displayedTechs[displayedTechsCount].type = 2;
                                displayedTechsCount ++;
                                if (displayedTechsCount >= maxDisplayedTechsCount) increaseMaxDisplayedTechsCount();
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i<6; i++) {
        for (int j = maxPassTech[i]+1; j <= 26; j++) {
            if (skipTech[i][j] == true)
                maxPassTech[i] = j;
            else
                break;
        }
    }

    for (int i = 0; i < 6; i++) {
        if (minTech[i] > minPassTech[i]) minTech[i] = minPassTech[i];
        if (maxTech[i] < maxPassTech[i]) maxTech[i] = maxPassTech[i];
    }

    qDebug() << A1 << A2;
    qDebug() << "minPassTech=" << minPassTech[0] << minPassTech[1] << minPassTech[2] << minPassTech[3] << minPassTech[4] << minPassTech[5];
    qDebug() << "maxPassTech=" << maxPassTech[0] << maxPassTech[1] << maxPassTech[2] << maxPassTech[3] << maxPassTech[4] << maxPassTech[5];

    fleetChanged();
}

