#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtWidgets>

//#include <iostream>
//using namespace std;

#include "Block.h"
#include "PlayerBlock.h"
#include "DesignBlock.h"
#include "importdlg.h"
#include "costdynamicsdlg.h"
#include "eventsdlg.h"
#include "salvageanalyzerdlg.h"
#include "filterdlg.h"
#include "decryptor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // default tech costs
    playerResearchCosts[1] = playerResearchCosts[3] = 1; // Cheap       Weap and Con
    playerResearchCosts[0] = playerResearchCosts[4] = 2; // Normal      En and El
    playerResearchCosts[2] = playerResearchCosts[5] = 3; // Expensive   Prop and Bio

    //int tech[] = {26,26,26,26,26,26};
    //qDebug() << "techCost= " << researchCost(tech, playerResearchCosts);

    initData();
    initView();

    flagStopCountChanged = false;

    designCount = 0;
    maxDesignCount = 0;
    design = NULL;
    count = NULL;
    shown = NULL;
    increaseMaxDesignCount();

    maxDisplayedTechsCount = 0;
    displayedTechsCount = 0;
    displayedTechs = NULL;
    increaseMaxDisplayedTechsCount();


    filterDialog = new filterDlg(this);

    fleetChanged();
}

MainWindow::~MainWindow()
{
    if (design) {
        delete[] design;
        delete[] count;
        delete[] shown;
    }
}

void MainWindow::increaseMaxDesignCount()
{
    int newMaxDesignCount = maxDesignCount + 64;
    cDesign * newDesign = new cDesign[newMaxDesignCount];
    int * newCount = new int[newMaxDesignCount];
    int * newShown = new int[newMaxDesignCount];
    if (design) {
        for (int i = 0; i < maxDesignCount; i++) {
            newDesign[i] = design[i];
            newCount[i] = count[i];
            newShown[i] = shown[i];
        }
        //memcpy(newDesign, design, maxDesignCount*sizeof(cDesign));
        //memcpy(newCount, count, maxDesignCount*sizeof(int));
        //memcpy(newShown, shown, maxDesignCount*sizeof(int));
        delete[] design;
        delete[] count;
        delete[] shown;
    }
    design = newDesign;
    count = newCount;
    shown = newShown;
    maxDesignCount = newMaxDesignCount;
}

void MainWindow::increaseMaxDisplayedTechsCount()
{
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

void MainWindow::initData()
{
    shipHullCount = 0;

    QFile file("items.csv");
    if (!file.open(QFile::ReadOnly | QFile::Text) ) {
        QMessageBox::critical(this, tr("Critical error"), tr("File items.csv not found"));
    } else {
        //qDebug() << "Loading file items.csv ...";
        // Создаём поток для извлечения данных из файла
        QTextStream in(&file);
        // Считываем данные до конца файла
        while (!in.atEnd())
        {
            // ... построчно
            QString line = in.readLine();
            // учитываем, что строка разделяется запятой на колонки
            QStringList list = line.split(",");
            switch (list[0].toInt()) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                //qDebug() << "item " << list[2] << "\tmass" << list[10].toInt() << "\tres" << list[11].toInt();
                shipItem[shipItemCount].category = list[0].toInt();
                shipItem[shipItemCount].index = list[1].toInt();
                shipItem[shipItemCount].name = list[2].mid(1,list[2].length()-2);
                shipItem[shipItemCount].itemID = list[3].toInt();
                for (int i = 0; i<6; i++)
                    shipItem[shipItemCount].techReq[i] =  list[4+i].toInt();
                shipItem[shipItemCount].mass = list[10].toInt();
                shipItem[shipItemCount].costRes = list[11].toInt();
                for (int i = 0; i<3; i++)
                    shipItem[shipItemCount].costMinerals[i] = list[12+i].toInt();
                shipItem[shipItemCount].globalID = list[15].toInt();
                if (list[0].toInt() == 2)  { // beam weapon
                    shipItem[shipItemCount].weaponRange = list[16].toInt();
                    shipItem[shipItemCount].weaponPower = list[17].toInt();
                    shipItem[shipItemCount].weaponInit = list[18].toInt();
                    shipItem[shipItemCount].weaponType = list[19].toInt();
                }
                if (list[0].toInt() == 3)  { // torpedo
                    shipItem[shipItemCount].weaponRange = list[16].toInt();
                    shipItem[shipItemCount].weaponPower = list[17].toInt();
                    shipItem[shipItemCount].weaponInit = list[18].toInt();
                    shipItem[shipItemCount].weaponAccuracy = list[19].toInt();
                }

                shipItemCount ++;
                break;
            case 15:
                //qDebug() << "shipHull " << list[2];

                shipHull[shipHullCount].index = list[1].toInt();
                shipHull[shipHullCount].name = list[2].mid(1,list[2].length()-2);
                shipHull[shipHullCount].hullID = list[3].toInt();
                for (int i = 0; i<6; i++)
                    shipHull[shipHullCount].techReq[i] =  list[4+i].toInt();
                shipHull[shipHullCount].mass = list[10].toInt();
                shipHull[shipHullCount].costRes = list[11].toInt();
                for (int i = 0; i<3; i++)
                    shipHull[shipHullCount].costMinerals[i] = list[12+i].toInt();
                shipHull[shipHullCount].cargo = list[16].toInt();
                shipHull[shipHullCount].fuel = list[17].toInt();
                shipHull[shipHullCount].armor = list[18].toInt();
                shipHull[shipHullCount].engineCount = list[20].toInt();

                shipHull[shipHullCount].slotCount = list[51].toInt();
                for (int i = 0; i<shipHull[shipHullCount].slotCount; i++) {
                    shipHull[shipHullCount].slot[i].type = list[19+2*i].toInt();
                    shipHull[shipHullCount].slot[i].maxCount = list[20+2*i].toInt();
                    shipHull[shipHullCount].slot[i].place = list[56+i].toInt();
                }
                shipHull[shipHullCount].class1 = list[52].toInt() / 64;
                shipHull[shipHullCount].class2 = list[53].toInt();
                shipHull[shipHullCount].initiative = list[52].toInt() % 64;
                shipHull[shipHullCount].placeCargoBeg = list[55].toInt();
                shipHull[shipHullCount].placeCargoEnd = list[54].toInt();

                shipHullCount ++;
                break;
            case 16:
                //qDebug() << "starbaseHull " << list[2];
                break;
            }

        }
        file.close();
    }

    messageCount = 0;
    QFile msgFile("stars-msg.txt");
    if (!msgFile.open(QFile::ReadOnly | QFile::Text) ) {
        QMessageBox::critical(this, tr("Critical error"), tr("File stars-msg.txt not found"));
    } else {
        //qDebug() << "Loading file stars-msg.txt ...";
        QTextStream in(&msgFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split("\t");
            int msgID = list[0].toInt();
            message[msgID].msgID = msgID;
            message[msgID].args = list[1].toInt();
            message[msgID].msgText = list[2];
            //qDebug() << message[msgID].msgID << message[msgID].args << message[msgID].msgText;
        }
    }
}

void MainWindow::initView()
{
    //qDebug() << "MainWindow::initView()";
    setWindowTitle(tr("Ships cost calculator for Stars! 2018.09.12"));
    setMinimumSize(1024,430);

    QWidget * centralWidget = new QWidget(this);
    QHBoxLayout * mainHLayout = new QHBoxLayout();

    QVBoxLayout * leftVLayout = new QVBoxLayout();
    QHBoxLayout * leftbottom1HLayout = new QHBoxLayout();
    QHBoxLayout * leftbottom2HLayout = new QHBoxLayout();

    tw_designes = new QTableWidget(0,6,this);
    QStringList slist;
    slist.clear();
    slist << tr("Owner") << tr("Number") << tr("Name") << tr("Hull") << tr("Mass") << tr("Count");
    tw_designes -> setHorizontalHeaderLabels(slist);
    //tw_designes -> setColumnWidth(0,45);
    //tw_designes -> setColumnWidth(1,50);
    tw_designes -> setColumnWidth(2,100);
    tw_designes -> setColumnWidth(3,100);
    //tw_designes -> setColumnWidth(4,35);
    tw_designes -> setColumnWidth(5,50);
    tw_designes -> resizeColumnToContents(0);
    tw_designes -> resizeColumnToContents(1);
    tw_designes -> resizeColumnToContents(4);
    //tw_designes -> resizeColumnToContents(5);
    leftVLayout -> addWidget(tw_designes);

    label1 = new QLabel (this);
    label1 -> setAlignment(Qt::AlignCenter);
    QPushButton * filterButton = new QPushButton("Set filter",this);
    QPushButton * sortButton = new QPushButton("Sort",this);
    QPushButton * pb_newDesign = new QPushButton("New", this);
    QPushButton * pb_loadDesign = new QPushButton("Load", this);
    QPushButton * pb_importDesign = new QPushButton("Import", this);
    QPushButton * pb_saveLibrary = new QPushButton("Save library", this);

    leftbottom1HLayout -> addStretch();
    leftbottom2HLayout -> addStretch();
    leftbottom1HLayout -> addWidget(filterButton);
    connect(filterButton,SIGNAL(clicked()),this,SLOT(filter()));
    leftbottom1HLayout -> addWidget(sortButton);
    connect(sortButton,SIGNAL(clicked()),this,SLOT(sort()));
    leftbottom2HLayout -> addWidget(pb_newDesign);
    connect(pb_newDesign,SIGNAL(clicked()),this,SLOT(newDesign()));
    leftbottom2HLayout -> addWidget(pb_loadDesign);
    connect(pb_loadDesign,SIGNAL(clicked()),this,SLOT(loadDesign()));
    leftbottom2HLayout -> addWidget(pb_importDesign);
    connect(pb_importDesign,SIGNAL(clicked()),this,SLOT(importDesign()));
    leftbottom2HLayout -> addWidget(pb_saveLibrary);
    connect(pb_saveLibrary,SIGNAL(clicked()),this,SLOT(saveLibrary()));

    leftVLayout -> addWidget(label1);
    leftVLayout -> addLayout(leftbottom1HLayout);
    leftVLayout -> addLayout(leftbottom2HLayout);

    QHBoxLayout * righttopHLayout = new QHBoxLayout();
    QVBoxLayout * toolsVLayout = new QVBoxLayout();
    QGridLayout * modeLayout = new QGridLayout;
    QGridLayout * researchCostsLayout = new QGridLayout;
    QVBoxLayout * rightVLayout = new QVBoxLayout();
    QHBoxLayout * rightbottomHLayout = new QHBoxLayout();

    modeGroupBox = new QGroupBox("Mode",this);
    costModeRadioButton = new QRadioButton("Build cost",modeGroupBox);
    salvageModeRadioButton = new QRadioButton("Battle Salvage",modeGroupBox);
    scrapSpaceModeRadioButton = new QRadioButton("Scrap in space",modeGroupBox);
    scrapSpaceModeRadioButton -> setEnabled(false);
    scrapPlanetModeRadioButton = new QRadioButton("Scrap at planet",modeGroupBox);
    scrapPlanetModeRadioButton -> setEnabled(false);
    scrapStarbaseModeRadioButton = new QRadioButton("Scrap at starbase",modeGroupBox);
    scrapStarbaseModeRadioButton -> setEnabled(false);
    URLRT_ModeCheckBox = new QCheckBox("LRT UR",modeGroupBox);
    URLRT_ModeCheckBox -> setEnabled(false);
    costModeRadioButton->setChecked(true);

    modeLayout->addWidget(costModeRadioButton,0,0);
    modeLayout->addWidget(salvageModeRadioButton,1,0);
    modeLayout->addWidget(scrapSpaceModeRadioButton,2,0);
    modeLayout->addWidget(scrapPlanetModeRadioButton,0,1);
    modeLayout->addWidget(scrapStarbaseModeRadioButton,1,1);
    modeLayout->addWidget(URLRT_ModeCheckBox,2,1);
    modeGroupBox->setLayout(modeLayout);
    connect(costModeRadioButton,SIGNAL(clicked()), this, SLOT(fleetChanged()));
    connect(salvageModeRadioButton,SIGNAL(clicked()), this, SLOT(fleetChanged()));
    connect(scrapSpaceModeRadioButton,SIGNAL(clicked()), this, SLOT(fleetChanged()));
    connect(scrapPlanetModeRadioButton,SIGNAL(clicked()), this, SLOT(fleetChanged()));
    connect(scrapStarbaseModeRadioButton,SIGNAL(clicked()), this, SLOT(fleetChanged()));
    connect(URLRT_ModeCheckBox,SIGNAL(clicked()), this, SLOT(fleetChanged()));

    researchCostsGroupBox = new QGroupBox("Tech Costs",this);
    QString tch;
    QStringList items;
    researchCostsEn     = new QComboBox(this);
    tch = "Ener: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsEn -> addItems(items);
    researchCostsEn -> setCurrentIndex(playerResearchCosts[0]-1);
    researchCostsWeap   = new QComboBox(this);
    tch = "Weap: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsWeap -> addItems(items);
    researchCostsWeap -> setCurrentIndex(playerResearchCosts[1]-1);
    researchCostsProp   = new QComboBox(this);
    tch = "Prop: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsProp -> addItems(items);
    researchCostsProp -> setCurrentIndex(playerResearchCosts[2]-1);
    researchCostsCon    = new QComboBox(this);
    tch = "Cons: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsCon -> addItems(items);
    researchCostsCon -> setCurrentIndex(playerResearchCosts[3]-1);
    researchCostsEl     = new QComboBox(this);
    tch = "Elec: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsEl -> addItems(items);
    researchCostsEl -> setCurrentIndex(playerResearchCosts[4]-1);
    researchCostsBio    = new QComboBox(this);
    tch = "Bio: ";
    items.clear();
    items << tch+tr("Cheap") << tch+tr("Normal")  << tch+tr("Expensive");
    researchCostsBio -> addItems(items);
    researchCostsBio -> setCurrentIndex(playerResearchCosts[5]-1);

    connect(researchCostsEn,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));
    connect(researchCostsWeap,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));
    connect(researchCostsProp,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));
    connect(researchCostsCon,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));
    connect(researchCostsEl,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));
    connect(researchCostsBio,SIGNAL(currentIndexChanged(int)), this, SLOT(researchCostsChanged()));

    researchCostsLayout->addWidget(researchCostsEn,0,0);
    researchCostsLayout->addWidget(researchCostsWeap,1,0);
    researchCostsLayout->addWidget(researchCostsProp,2,0);
    researchCostsLayout->addWidget(researchCostsCon,0,1);
    researchCostsLayout->addWidget(researchCostsEl,1,1);
    researchCostsLayout->addWidget(researchCostsBio,2,1);
    researchCostsGroupBox->setLayout(researchCostsLayout);

    costDynamicsPushButton = new QPushButton("CostDynamics",this);
    eventsPushButton = new QPushButton("Events list",this);
    salvageAnalyzerPushButton = new QPushButton("Salvage",this);
    connect(costDynamicsPushButton, SIGNAL(clicked()), this, SLOT(costDynamicsPushButtonClicked()));
    connect(eventsPushButton, SIGNAL(clicked()), this, SLOT(eventsPushButtonClicked()));
    connect(salvageAnalyzerPushButton, SIGNAL(clicked()), this, SLOT(salvageAnalyzerPushButtonClicked()));

    toolsVLayout -> addWidget(costDynamicsPushButton);
    toolsVLayout -> addWidget(eventsPushButton);
    toolsVLayout -> addWidget(salvageAnalyzerPushButton);

    righttopHLayout ->addWidget(modeGroupBox);
    righttopHLayout ->addWidget(researchCostsGroupBox);
    righttopHLayout -> addLayout(toolsVLayout);
    righttopHLayout -> addStretch();

    tw_cost = new QTableWidget(0,13,this);
    //tw_cost -> setMaximumSize(500,16777215);
    //tw_cost -> setMinimumSize(500,250);
    slist.clear();
    slist << tr ("") << tr("En") << tr("Weap") << tr("Pro") << tr("Con") << tr("El") << tr("Bio") << tr("TechCost") << tr("Iron") << tr("Bor") << tr("Germ") << tr("Res") << tr("Sort");
    tw_cost -> setHorizontalHeaderLabels(slist);
    tw_cost -> setColumnWidth(0,30);
    tw_cost -> setColumnWidth(1,35);
    tw_cost -> setColumnWidth(2,40);
    tw_cost -> setColumnWidth(3,35);
    tw_cost -> setColumnWidth(4,35);
    tw_cost -> setColumnWidth(5,35);
    tw_cost -> setColumnWidth(6,35);
    tw_cost -> setColumnWidth(7,60);
    tw_cost -> setColumnWidth(8,45);
    tw_cost -> setColumnWidth(9,45);
    tw_cost -> setColumnWidth(10,45);
    tw_cost -> setColumnWidth(11,45);
    tw_cost -> setColumnWidth(12,40);

    pb_importTechLevel = new QPushButton("Import",this);
    pb_importTechLevel -> setEnabled(false);
    pb_importTechLevel -> setMaximumWidth(40);

    for (int i = 0; i< 6; i++) {
        spinbox_tech[i] = new QSpinBox(this);
        spinbox_tech[i] -> setMinimum(0);
        spinbox_tech[i] -> setMaximum(26);
        spinbox_tech[i] -> setValue(0);
        spinbox_tech[i] -> setMinimumWidth(50);
        connect(spinbox_tech[i], SIGNAL(valueChanged(int)), this, SLOT(techChanged()));
    }
    spinbox_tech[0] -> setPrefix("En:");
    spinbox_tech[1] -> setPrefix("W:");
    spinbox_tech[2] -> setPrefix("P:");
    spinbox_tech[3] -> setPrefix("C:");
    spinbox_tech[4] -> setPrefix("El:");
    spinbox_tech[5] -> setPrefix("B:");

    for (int i = 0; i< 4; i++) {
        label_cost[i] = new QLabel(this);
        label_cost[i] -> setAlignment(Qt::AlignCenter);
        label_cost[i] -> setAutoFillBackground(true);
    }
    label_cost[0] -> setPalette(QPalette(Qt::black,QColor(173,216,255),Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
    label_cost[1] -> setPalette(QPalette(Qt::black,QColor(128,255,128),Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
    label_cost[2] -> setPalette(QPalette(Qt::black,QColor(255,255,64),Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));

    pb_addRowToCostTable = new QPushButton("Add", this);
    pb_addRowToCostTable -> setMaximumWidth(30);
    connect(pb_addRowToCostTable,SIGNAL(clicked()),this, SLOT(addRowToCostTable()));

    rightVLayout -> addLayout(righttopHLayout);
    rightVLayout -> addWidget(tw_cost);
    rightVLayout -> addLayout(rightbottomHLayout);

    rightbottomHLayout -> addWidget(pb_importTechLevel);
    for (int i = 0; i< 6; i++) rightbottomHLayout -> addWidget(spinbox_tech[i]);
    for (int i = 0; i< 4; i++) rightbottomHLayout -> addWidget(label_cost[i]);
    rightbottomHLayout -> addWidget(pb_addRowToCostTable);

    mainHLayout -> addLayout(leftVLayout,4);
    mainHLayout -> addLayout(rightVLayout,5);

    centralWidget-> setLayout(mainHLayout);
    setCentralWidget(centralWidget);

}


int MainWindow::designMass(cDesign design)
{
    int mass = 0;
    mass += shipHull[design.hullID].mass;
    for (int slot = 0; slot < shipHull[design.hullID].slotCount; slot++)
        mass += (design.items[slot]!=255)?shipItem[design.items[slot]].mass*design.counts[slot]:0;
    return mass;
}

bool MainWindow::isEqual(cDesign design1, cDesign design2)
{
    if (design1.owner != design2.owner) return false;
    if (design1.hullID != design2.hullID) return false;
    int slotCount = shipHull[design1.hullID].slotCount;
    for (int slot = 0; slot < slotCount; slot++) {
        if (design1.counts[slot] != design2.counts[slot]) return false;
        if (design1.counts[slot] == 0) continue;
        if (design1.items[slot] != design2.items[slot]) return false;
    }
    return true;
}

void MainWindow::swapDesign(int i, int j)
{
    cDesign tmpDesign = design[i];
    design[i] = design[j];
    design[j] = tmpDesign;

    int tmpCount = count[i];
    count[i] = count[j];
    count[j] = tmpCount;

    int tmpShown = shown[i];
    shown[i] = shown[j];
    shown[j] = tmpShown;
}


void MainWindow::designMinTechReq(cDesign design, int tech[])
{
    for (int i = 0; i<6; i++)
        tech[i] = shipHull[design.hullID].techReq[i];
    for (int slot = 0; slot < shipHull[design.hullID].slotCount; slot++) {
        int itemID = design.items[slot];
        if (itemID == 255) continue;
        if (design.counts[slot] == 0) continue;
        for (int i = 0; i<6; i++)
            if (tech[i] < shipItem[itemID].techReq[i])
                tech[i] = shipItem[itemID].techReq[i];
    }
}

void MainWindow::fleetMinTechReq(int tech[])
{
    for (int i = 0; i<6; i++) tech[i] = 0;
    int dsnTech[] = {0,0,0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        if (shown[i] == 0) continue;
        if (count[i] == 0) continue;
        designMinTechReq(design[i], dsnTech);
        for (int i = 0; i<6; i++)
            if (tech[i] < dsnTech[i])
                tech[i] = dsnTech[i];
    }
}

void MainWindow::calculateCost(cDesign design, int count, int tech[], int cost[], int mode)
{
    for (int i = 0; i < 4; i++) cost[i] = 0;

    int K = 26;
    int sumT = 0;
    int minT = 26;
    for (int i = 0; i < 6 ; i++) {
        sumT += shipHull[design.hullID].techReq[i];
        if (minT > tech[i]) minT = tech[i];

        if (shipHull[design.hullID].techReq[i] == 0) continue;
        if (K > (tech[i] - shipHull[design.hullID].techReq[i]))
            K = (tech[i] - shipHull[design.hullID].techReq[i]);
    }
    if (sumT == 0) K = minT;
    if (K < 0) K = 0;
    double K1 = 1 - K*0.04; // TODO LRT Bleeding Edge
    if (K1 < 0.25) K1 = 0.25; // TODO LRT Bleeding Edge
    int itemCost[4] = {0,0,0,0};
    for (int i = 0; i < 3; i++) {
        itemCost[i] = ceil(K1 * shipHull[design.hullID].costMinerals[i] - 0.5);
        if (shipHull[design.hullID].costMinerals[i] > 0 && itemCost[i] == 0) itemCost[i] = 1;
    }
    itemCost[3] = ceil(K1 * shipHull[design.hullID].costRes - 0.5);
    if (shipHull[design.hullID].costRes > 0 && itemCost[3] == 0) itemCost[3] = 1;

    for (int i = 0; i < 4; i++) cost[i] += itemCost[i];

    //qDebug() << "---";
    //qDebug() << "Hull" << design.hullID << "K=" << K << "C=" << itemCost[0] << itemCost[1] << itemCost[2] << shipHull[design.hullID].name;
    //qDebug() << "(" << K1 * shipHull[design.hullID].costMinerals[0] << K1 * shipHull[design.hullID].costMinerals[1] << K1 * shipHull[design.hullID].costMinerals[2] << ")";

    for (int slot = 0; slot < shipHull[design.hullID].slotCount; slot++) {
        if (design.counts[slot] == 0) continue;
        int itemID = design.items[slot];
        if (itemID == 255) continue;

        int K = 26;
        int sumT = 0;
        int minT = 26;
        for (int i = 0; i < 6 ; i++) {
            sumT += shipItem[itemID].techReq[i];
            if (minT > tech[i]) minT = tech[i];

            if (shipItem[itemID].techReq[i] == 0) continue;
            if (K > (tech[i] - shipItem[itemID].techReq[i]))
                K = (tech[i] - shipItem[itemID].techReq[i]);
        }
        if (sumT == 0) K = minT;
        if (K < 0) K = 0;
        double K1 = 1 - K*0.04; // TODO LRT Bleeding Edge
        if (K1 < 0.25) K1 = 0.25; // TODO LRT Bleeding Edge
        int itemCost[4] = {0,0,0,0};
        for (int i = 0; i < 3; i++) {
            itemCost[i] = ceil(K1 * shipItem[itemID].costMinerals[i] - 0.5);
            if (shipItem[itemID].costMinerals[i] > 0 && itemCost[i] == 0) itemCost[i] = 1;
        }
        itemCost[3] = ceil(K1 * shipItem[itemID].costRes - 0.5);
        if (shipItem[itemID].costRes > 0 && itemCost[3] == 0) itemCost[3] = 1;

        for (int i = 0; i < 4; i++) cost[i] += itemCost[i]*design.counts[slot];
        //qDebug() << "Slot" << slot << "N=" << design.counts[slot] << "K=" << K << "C=" << itemCost[0] << itemCost[1] << itemCost[2] << shipItem[itemID].name;
        //qDebug() << "(" << K1 * shipItem[itemID].costMinerals[0] << K1 * shipItem[itemID].costMinerals[1] << K1 * shipItem[itemID].costMinerals[2] << ")";
    }
    for (int i = 0; i < 4; i++) cost[i] *= count;


    if (mode == 0) {
        if (costModeRadioButton->isChecked())               mode = 1;
        else if (salvageModeRadioButton->isChecked())       mode = 2;
        else if (scrapSpaceModeRadioButton->isChecked())    mode = 3;
        else if (scrapStarbaseModeRadioButton->isChecked()) mode = 4;
        else if (scrapPlanetModeRadioButton->isChecked())   mode = 5;
    }

    if (mode == 2) { for (int i = 0; i < 3; i++) cost[i] = salv(cost[i]); cost[3] = 0;}
}

int MainWindow::salv(int m) {
    return int((m+3)/6)+int((m+6)/12);
}

//courtesy Bob Martin, starsfaq.com, Henk Poell, Tiib, M.A, C.R et al
const static int baseCost[] = {0, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1385, 1804, 2244, 2705, 3187, 3690, 4214, 4759, 5325, 5912, 6520, 7149, 7799, 8470};

int MainWindow::researchCost(const int tech[], const int playerTechCosts[])
{
    int maxTech = 0;
    int c[4] = {0,0,0,0};
    // c[0] - count of techs with max
    // c[1] - count of cheap
    // c[2] - count of normal
    // c[3] - count of expensive

    for (int i = 0; i < 6; i++) {
        if (maxTech > tech[i]) continue;
        if (maxTech < tech[i]) {
            maxTech = tech[i];
            c[0] = c[1] = c[2] = c[3] = 0;
        }
        c[0] ++;
        c[playerTechCosts[i]] ++;
    }
    if (maxTech == 0) return 0;

    int sumTech = tech[0] + tech[1] + tech[2] + tech[3] + tech[4] + tech[5] - c[0];
    int cost1 = 0;
    // cheap - first
    cost1 += 5 * c[1] * (baseCost[maxTech] + sumTech);
    cost1 += 2.5 * c[1]*(c[1]-1); // 5 * ( c[1]*(c[1]-1)/2)

    // normal - second
    cost1 += 10 * c[2] * (baseCost[maxTech] + sumTech + c[1]);
    cost1 += 5 * c[2]*(c[2]-1); // 10 * ( c[2]*(c[2]-1)/2)

    // expensive - last
    cost1 += ceil(17.5 * c[3] * (baseCost[maxTech] + sumTech + c[1] + c[2]));
    cost1 += ceil(8.75 * c[3]*(c[3]-1)); // 17.5 * ( c[3]*(c[3]-1)/2)


    if (maxTech == 1) return cost1;

    int recursiveTech[6];
    for (int i = 0; i < 6; i++) {
        recursiveTech[i] = (tech[i] == maxTech)?(maxTech-1):tech[i];
    }
    return cost1 + researchCost(recursiveTech, playerTechCosts);

}

void MainWindow::newDesign()
{
    cDesign chaff;
    chaff.hullID = 5;
    chaff.owner = 1;
    chaff.designNumber = 1;
    chaff.designName = "Chaff";
    chaff.items[0] = 187;
    chaff.counts[0] = 1;
    chaff.items[1] = 255;
    chaff.counts[1] = 0;
    chaff.items[2] = 17;
    chaff.counts[2] = 1;
    chaff.items[3] = 255;
    chaff.counts[3] = 0;

    designDlg designDialog(chaff, this);
    //designDlg designDialog(this);

    designDialog.exec();
}

void MainWindow::techChanged()
{
    int tech[6] = {0,0,0,0,0,0};
    int fleetCost[4] = {0,0,0,0};
    for (int i = 0; i < 6; i++) tech[i] = spinbox_tech[i]->value();

    for (int i = 0; i < designCount; i++) {
        if (shown[i]) {
            int tokenCost[4] = {0,0,0,0};
            calculateCost(design[i], count[i], tech, tokenCost);
            for (int i = 0; i < 4; i++) fleetCost[i] += tokenCost[i];
        }
    }

    for (int i = 0; i < 4; i++) label_cost[i]->setNum(fleetCost[i]);
    pb_addRowToCostTable -> setEnabled(true);
}

//void MainWindow::addRowToCostTable()
//{
//    tw_cost->insertRow(tw_cost->rowCount()-1);
//    int row = tw_cost->rowCount()-2;
//    int tech[6] = {0,0,0,0,0,0};
//    int fleetCost[4] = {0,0,0,0};
//    for (int i = 0; i<6; i++) {
//        tech[i] = spinbox_tech[i]->value();
//        QLabel * label = new QLabel(QString::number(tech[i]),this);
//        label -> setAlignment(Qt::AlignCenter);
//        tw_cost->setCellWidget(row,i+1,label);
//    }
//    QLabel * techCostLabel = new QLabel(QString::number(researchCost(tech,playerResearchCosts)),this);
//    techCostLabel -> setAlignment(Qt::AlignCenter);
//    tw_cost->setCellWidget(row,7,techCostLabel);
//    for (int i = 0; i < designCount; i++) {
//        if (shown[i]) {
//            int tokenCost[4] = {0,0,0,0};
//            calculateCost(design[i], count[i], tech, tokenCost);
//            for (int i = 0; i < 4; i++) fleetCost[i] += tokenCost[i];
//        }
//    }
//    for (int i = 0; i<4; i++) {
//        QLabel * label = new QLabel(QString::number(fleetCost[i]),this);
//        label -> setAlignment(Qt::AlignCenter);
//        label -> setAutoFillBackground(true);
//        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
//        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
//        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
//        tw_cost->setCellWidget(row,i+8,label);
//    }
//    tw_cost -> resizeRowsToContents();
//    tw_cost -> resizeColumnsToContents();
//    pb_addRowToCostTable -> setEnabled(false);
//}

void MainWindow::addRowToCostTable()
{
    for (int i = 0; i<6; i++) {
        displayedTechs[displayedTechsCount].tech[i] = spinbox_tech[i]->value();
        displayedTechs[displayedTechsCount].techMax[i] = spinbox_tech[i]->value();
    }
    displayedTechs[displayedTechsCount].type = 1;
    displayedTechsCount ++;
    if (displayedTechsCount >= maxDisplayedTechsCount) increaseMaxDisplayedTechsCount();

    pb_addRowToCostTable -> setEnabled(false);
    fleetChanged();
}

void MainWindow::fillTable()
{
    QTime t;
    t.start();

    shownCount = 0;

    for (int i = 0; i< designCount; i++) {
        if (filterDialog -> passed(design[i])) {
            shownCount ++;
            shown[i] = shownCount;
        } else shown[i] = 0;
    }

    QProgressDialog progress("Processing...", "Abort", 0, designCount, this);
    progress.setMinimumDuration(1);
    progress.setWindowModality(Qt::WindowModal);

    int old = tw_designes -> rowCount();

    tw_designes -> setRowCount(shownCount);
    int row = 0;
    for (int i = 0; i< designCount; i++)
        if (shown[i]) {
            progress.setValue(i);

            if (row >= old) {
            //if (true) {
                QLabelModified * label0 = new QLabelModified(QString::number(design[i].owner));
                connect (label0, SIGNAL(doubleClicked()), this, SLOT(tableDoubleClicked()));
                tw_designes -> setCellWidget(row, 0, label0);

                QLabelModified * label1 = new QLabelModified(QString::number(design[i].designNumber));
                connect (label1, SIGNAL(doubleClicked()), this, SLOT(tableDoubleClicked()));
                tw_designes -> setCellWidget(row, 1, label1);

                QLabelModified * label2 = new QLabelModified(design[i].designName);
                connect (label2, SIGNAL(doubleClicked()), this, SLOT(tableDoubleClicked()));
                tw_designes -> setCellWidget(row, 2, label2);

                QLabelModified * label3 = new QLabelModified(shipHull[design[i].hullID].name);
                connect (label3, SIGNAL(doubleClicked()), this, SLOT(tableDoubleClicked()));
                tw_designes -> setCellWidget(row, 3, label3);

                QLabelModified * label4 = new QLabelModified(QString::number(designMass(design[i])));
                connect (label4, SIGNAL(doubleClicked()), this, SLOT(tableDoubleClicked()));
                tw_designes -> setCellWidget(row, 4, label4);

            } else {
                ((QLabelModified *) tw_designes -> cellWidget(row, 0)) -> setNum(design[i].owner);
                ((QLabelModified *) tw_designes -> cellWidget(row, 1)) -> setNum(design[i].designNumber);
                ((QLabelModified *) tw_designes -> cellWidget(row, 2)) -> setText(design[i].designName);
                ((QLabelModified *) tw_designes -> cellWidget(row, 3)) -> setText(shipHull[design[i].hullID].name);
                ((QLabelModified *) tw_designes -> cellWidget(row, 4)) -> setNum(designMass(design[i]));
            }

            flagStopCountChanged = true;
            if (row >= old) {
            //if (true) {
                QSpinBox * spinbox = new QSpinBox();
                spinbox -> setMaximum(9999);
                spinbox -> setValue(count[i]);
                //connect(spinbox,SIGNAL(editingFinished()), this, SLOT(countChanged()));
                //connect(spinbox,SIGNAL(editingFinished()), this, SLOT(fleetChanged()));
                connect(spinbox,SIGNAL(valueChanged(int)), this, SLOT(countChanged()));
                connect(spinbox,SIGNAL(valueChanged(int)), this, SLOT(fleetChanged()));
                tw_designes -> setCellWidget(row, 5, spinbox);
            } else {
                ((QSpinBox *) tw_designes -> cellWidget(row, 5)) -> setValue(count[i]);
            }
            flagStopCountChanged = false;

            row++;

        };

    tw_designes -> resizeRowsToContents();
    fleetChanged();
    qDebug("Time elapsed in MainWindow::fillTable(): %d ms", t.elapsed());
}

void MainWindow::tableDoubleClicked()
{
    //qDebug() << "row" << tw_designes->currentRow() << "column" << tw_designes -> currentColumn();
    int row = tw_designes->currentRow();

    int designN = -1;
    for (int i = 0; i< designCount; i++)
        if (shown[i] == row+1) designN = i;
    if (designN == -1) return;

    designDlg designDialog(design[designN], this, designN);
    designDialog.exec();
}

void MainWindow::sort()
{
    QStringList items;
    items << tr("Owner /\\")
          << tr("Owner \\/")
          << tr("Name /\\")
          << tr("Name \\/")
          << tr("Hull ID /\\")
          << tr("Hull ID \\/")
          << tr("Hull mass /\\")
          << tr("Hull mass \\/")
          << tr("Design mass /\\")
          << tr("Design mass \\/");
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Select sort order"), tr("Select sort order"), items, 0, false, &ok);

    if (!ok) return;
    if (item.isEmpty()) return;

    int index = items.indexOf(item);
    qDebug() << index;

    cDesign tmpDesign;
    int tmpCount;

    switch (index) {
    case 0: //"Owner /\\"
        for (int i = designCount-1; i>=0 ; i--)
            for (int j = i+1; j< designCount; j++)
                if (design[j-1].owner > design[j].owner)
                    swapDesign(j-1, j);
        break;
    case 1: //"Owner \\/"
        for (int i = 0; i < designCount-1 ; i++)
            for (int j = designCount-1; j>0 ; j--)
                if (design[j-1].owner < design[j].owner)
                    swapDesign(j-1, j);
        break;
    case 2: //"Name /\\"
        for (int i = designCount-1; i>=0 ; i--)
            for (int j = i+1; j< designCount; j++)
                if (design[j-1].designName > design[j].designName)
                    swapDesign(j-1, j);
        break;
    case 3: //"Name \\/"
        for (int i = 0; i < designCount-1 ; i++)
            for (int j = designCount-1; j>0 ; j--)
                if (design[j-1].designName < design[j].designName)
                    swapDesign(j-1, j);
        break;
    case 4: //"Hull ID /\\"
        for (int i = designCount-1; i>=0 ; i--)
            for (int j = i+1; j< designCount; j++)
                if (design[j-1].hullID > design[j].hullID)
                    swapDesign(j-1, j);
        break;
    case 5: //"Hull ID \\/"
        for (int i = 0; i < designCount-1 ; i++)
            for (int j = designCount-1; j>0 ; j--)
                if (design[j-1].hullID < design[j].hullID)
                    swapDesign(j-1, j);
        break;
    case 6: //"Hull mass /\\"
        for (int i = designCount-1; i>=0 ; i--)
            for (int j = i+1; j< designCount; j++)
                if (shipHull[design[j-1].hullID].mass > shipHull[design[j].hullID].mass)
                    swapDesign(j-1, j);
        break;
    case 7: //"Hull mass \\/"
        for (int i = 0; i < designCount-1 ; i++)
            for (int j = designCount-1; j>0 ; j--)
                if (shipHull[design[j-1].hullID].mass < shipHull[design[j].hullID].mass)
                    swapDesign(j-1, j);
        break;
    case 8: //"Design mass /\\"
        for (int i = designCount-1; i>=0 ; i--)
            for (int j = i+1; j< designCount; j++)
                if (designMass(design[j-1]) > designMass(design[j]))
                    swapDesign(j-1, j);
        break;
    case 9: //"Design mass \\/"
        for (int i = 0; i < designCount-1 ; i++)
            for (int j = designCount-1; j>0 ; j--)
                if (designMass(design[j-1]) < designMass(design[j]))
                    swapDesign(j-1, j);
        break;

    }
    fillTable();
    fleetChanged();
}

void MainWindow::filter()
{
    filterDialog->show();

}

void MainWindow::loadDesign()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select design file"), "", tr("Design file (*.des);;All files (*.*)"));
    if (fileName == NULL) return;
    QFile file(fileName);
    int fileSize = file.size();
    file.open(QFile::ReadOnly);

    char * data = new char[fileSize];
    file.read(data, fileSize);

    int p = 0;
    while (p<fileSize) {
        //qDebug() << "designCount=" << designCount;
        if (data[p+0] != 1) {
            QMessageBox::warning(this,"Error","Unexpected desing file version");
            return;
        }
        cDesign curDesign;
        curDesign.owner = data[p+1];
        curDesign.designNumber = data[p+2];
        curDesign.hullID = data[p+3];

        int slotCount = shipHull[data[p+3]].slotCount;
        for (int slot = 0; slot < slotCount; slot++) {
            curDesign.items[slot] = (unsigned char)data[p+slot*2+4];
            curDesign.counts[slot] = data[p+slot*2+5];
        }
        if (data[p+slotCount*2+4] > 0) {
            curDesign.designName = QString::fromLocal8Bit(data+p+slotCount*2+5,data[p+slotCount*2+4]);
        } else {
            //curDesign.designName = shipHull[data[p+3]].name + QString::number(designMass(curDesign));
            curDesign.designName = "";
        }


        // Match check
        bool match = false;
        for (int j = 0; j < designCount; j++) {
            if (isEqual(curDesign, design[j])) {
                match = true;
                break;
            }
        }

        if (!match) {
            // Adding
            design[designCount] = curDesign;
            count[designCount] = 0;
            designCount++;
            if (designCount >= maxDesignCount) increaseMaxDesignCount();
        }

        p += slotCount*2 + 5 + data[p+slotCount*2+4];
    };

    delete[] data;
    file.close();

    fillTable();
    fleetChanged();

}

void MainWindow::importDesign()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select one or more files"), "", tr("Stars files (*.m? *.m1? *.h? *.h1? *.hst);;All files (*.*)"));

    for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); it++) {
        QFileInfo fileInfo(*it);
        bool hstFile = false;
        if (fileInfo.suffix().toLower() == "hst") hstFile = true;
        bool ok = false;
        int player = fileInfo.suffix().toLower().right(fileInfo.suffix().length()-1).toInt(&ok);
        if (!hstFile && (!ok || player < 1 || player > 16))    continue;

        importDesignDlg dialog(*it, this);
        dialog.exec();
    }
}

void MainWindow::saveLibrary()
{
    QStringList items;
    items << tr("Save all design info")
          << tr("Save design name, don't save design owner")
          << tr("Save design owner, don't save design name")
          << tr("Don't save design owner and name");
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Select info to save"), tr("Select info to save"), items, 0, false, &ok);

    if (!ok) return;
    if (item.isEmpty()) return;

    int flags = items.indexOf(item);
    designDlg dialog(this);
    dialog.saveDesign(design, 0, designCount, flags);
    dialog.close();

}

void MainWindow::countChanged()
{
    //qDebug() << "designCount=" << designCount;
    if (flagStopCountChanged) return;
    for (int i = 0; i< designCount; i++) {
        if (shown[i]) {
            //qDebug() << design[i].designName << "shown[i]" << shown[i];
            count[i] = ((QSpinBox*) tw_designes -> cellWidget(shown[i]-1, 5))-> value();
        }
    }
}

void MainWindow::researchCostsChanged()
{
    playerResearchCosts[0] = researchCostsEn->currentIndex()+1;
    playerResearchCosts[1] = researchCostsWeap->currentIndex()+1;
    playerResearchCosts[2] = researchCostsProp->currentIndex()+1;
    playerResearchCosts[3] = researchCostsCon->currentIndex()+1;
    playerResearchCosts[4] = researchCostsEl->currentIndex()+1;
    playerResearchCosts[5] = researchCostsBio->currentIndex()+1;
    fleetChanged();
}

void MainWindow::costDynamicsPushButtonClicked()
{
    if (designCount == 0) {
        QMessageBox::critical(this, tr("Design not loaded"), tr("No design loaded"));
        return;
    }
    bool ok = false;
    for (int i = 0; i<designCount; i++)
        if (shown[i] > 0 && count[i] > 0) {
            ok = true;
            break;
        }
    if (!ok) {
        QMessageBox::critical(this, tr("Design not selected"), tr("To select design set positive count at left table"));
        return;
    }

    cCostDynamicsDlg dialog(this);
    dialog.exec();
}

void MainWindow::eventsPushButtonClicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select one or more files"), "", tr("Stars M files (*.m? *.m1? *.hst);;All files (*.*)"));

    for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); it++) {
        QFileInfo fileInfo(*it);
        bool hstFile = false;
        if (fileInfo.suffix().toLower() == "hst") hstFile = true;
        bool ok = false;
        int player = fileInfo.suffix().toLower().right(fileInfo.suffix().length()-1).toInt(&ok);
        if (!hstFile && (!ok || player < 1 || player > 16))    continue;

        eventsDlg dialog(*it, this);
        dialog.exec();
    }
}

void MainWindow::salvageAnalyzerPushButtonClicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select one or more files"), "", tr("Stars M files (*.m? *.m1? *.hst);;All files (*.*)"));

    for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); it++) {
        QFileInfo fileInfo(*it);
        bool hstFile = false;
        if (fileInfo.suffix().toLower() == "hst") hstFile = true;
        bool ok = false;
        int player = fileInfo.suffix().toLower().right(fileInfo.suffix().length()-1).toInt(&ok);
        if (!hstFile && (!ok || player < 1 || player > 16))    continue;

        salvageListDlg dialog(*it, this);
        dialog.exec();
    }

//    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), "", tr("Stars M files (*.m? *.m1? *.hst);;All files (*.*)"));
//    if (fileName == NULL) return;
//    QFileInfo fileInfo(fileName);
//    bool hstFile = false;
//    if (fileInfo.suffix().toLower() == "hst") hstFile = true;
//    bool ok = false;
//    int player = fileInfo.suffix().toLower().right(fileInfo.suffix().length()-1).toInt(&ok);
//    if (!hstFile && (!ok || player < 1 || player > 16)) {
//        QMessageBox::critical(this, tr("Unsuitable file"), tr("Unsuitable file"));
//        return;
//    }
//    salvageListDlg dialog(fileName, this);
//    dialog.exec();
}

void MainWindow::fleetChanged()
{
    if (flagStopCountChanged) return;
    int flMass = 0;
    int flCount = 0;
    int flSum = 0;
    for (int i = 0; i < designCount; i++) {
        if (shown[i] > 0) {
            if (count[i]) {
                flCount ++;
                flSum += count[i];
                flMass += designMass(design[i]) * count[i];
            }
        }
    }


    QString text;
    text = tr("Shown ")
            + QString::number(shownCount)
            + tr(" / ")
            + QString::number(designCount)
            + tr(" designes. Selected ")
            + QString::number(flSum)
            + tr(" ships of ")
            + QString::number(flCount)
            + tr(" designes (total ")
            + QString::number(flMass)
            + tr(" kt)");
    label1->setText(text);

    int minTech[6] = {0,0,0,0,0,0};
    fleetMinTechReq(minTech);
    int maxCost[4] = {0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        if (shown[i]) {
            int tokenCost[4] = {0,0,0,0};
            calculateCost(design[i], count[i], minTech, tokenCost);
            for (int j = 0; j < 4; j++) maxCost[j] += tokenCost[j];
        }
    }

    int maxTech[6] = {26,26,26,26,26,26};
    int minCost[4] = {0,0,0,0};
    for (int i = 0; i < designCount; i++) {
        if (shown[i]) {
            int tokenCost[4] = {0,0,0,0};
            calculateCost(design[i], count[i], maxTech, tokenCost);
            for (int j = 0; j < 4; j++) minCost[j] += tokenCost[j];
        }
    }

    // attempt to minimize maxTech
    int cost[4];
    for (int tech = 0 ; tech < 6; tech++) {
        do {
            maxTech[tech] --;
            if (maxTech[tech] < minTech[tech]) break;
            cost[0] = cost[1] = cost[2] = cost[3] = 0;
            for (int i = 0; i < designCount; i++) {
                if (shown[i]) {
                    int tokenCost[4] = {0,0,0,0};
                    calculateCost(design[i], count[i], maxTech, tokenCost);
                    for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
                }
            }
            if (cost[0] != minCost[0]) break;
            if (cost[1] != minCost[1]) break;
            if (cost[2] != minCost[2]) break;
            if (cost[3] != minCost[3]) break;
        } while (true);

        // revert to last %maxTech% with %cost% equal to %minCost%
        maxTech[tech] ++;
    }

    tw_cost->setRowCount(2+displayedTechsCount);
    tw_cost->setCellWidget(0,0,new QLabelModified("Min"));
    tw_cost->setCellWidget(1,0,new QLabelModified("Max"));
    for (int i = 0; i<6; i++) {
        QLabelModified * label = new QLabelModified(QString::number(minTech[i]),this);
        label -> setAlignment(Qt::AlignCenter);
       tw_cost->setCellWidget(0,i+1,label);
    }
    int iTechCost = researchCost(minTech,playerResearchCosts);
//    QTime t;
//    t.start();
//    for (int i=0; i<26; i++) {
//        minTech[0] = i;
//        for (int j=0; j<26; j++) {
//            minTech[1] = j;
//            for (int k=0; k<26; k++) {
//                minTech[2] = k;
//                for (int l=0; l<26; l++) {
//                    minTech[3] = l;
//                    iTechCost += techCost(minTech,playerTechCosts);
//                }
//            }
//        }
//    }
//    qDebug("Time elapsed: %d ms", t.elapsed());
    QLabelModified * minTechCostLabel = new QLabelModified(QString::number(iTechCost),this);
    minTechCostLabel -> setAlignment(Qt::AlignCenter);
    tw_cost->setCellWidget(0,7,minTechCostLabel);
    for (int i = 0; i<4; i++) {
        QLabelModified * label = new QLabelModified(QString::number(maxCost[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        tw_cost->setCellWidget(0,i+8,label);
    }

    for (int i = 0; i<6; i++) {
        QLabelModified * label = new QLabelModified(QString::number(maxTech[i])+QString(maxTech[i]==26?"":"+"),this);
        label -> setAlignment(Qt::AlignCenter);
        tw_cost->setCellWidget(1,i+1,label);
    }
    QLabelModified * maxTechCostLabel = new QLabelModified(QString::number(researchCost(maxTech,playerResearchCosts)),this);
    maxTechCostLabel -> setAlignment(Qt::AlignCenter);
    tw_cost->setCellWidget(1,7,maxTechCostLabel);
    for (int i = 0; i<4; i++) {
        QLabelModified * label = new QLabelModified(QString::number(minCost[i]),this);
        label -> setAlignment(Qt::AlignCenter);
        label -> setAutoFillBackground(true);
        if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
        if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
        if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
        tw_cost->setCellWidget(1,i+8,label);
    }

    for (int displayedTech = 0; displayedTech < displayedTechsCount; displayedTech++) {
        int row = displayedTech+2;
        QLabelModified * typeLabel = new QLabelModified(this);
        switch (displayedTechs[displayedTech].type) {
        case 1: typeLabel -> setText("MANUAL"); break;
        case 2: typeLabel -> setText("AUTO"); break;
        }
        typeLabel -> setAlignment(Qt::AlignCenter);

        tw_cost->setCellWidget(row,0,typeLabel);
        for (int i = 0; i<6; i++) {
            QString string = QString::number(displayedTechs[displayedTech].tech[i]);
            if (displayedTechs[displayedTech].techMax[i] != displayedTechs[displayedTech].tech[i])
                string += tr("-")+QString::number(displayedTechs[displayedTech].techMax[i]);
            QLabelModified * label = new QLabelModified(string,this);
            label -> setAlignment(Qt::AlignCenter);
            tw_cost->setCellWidget(row,i+1,label);
        }
        QLabelModified * maxTechCostLabel = new QLabelModified(QString::number(researchCost(displayedTechs[displayedTech].tech,playerResearchCosts)),this);
        maxTechCostLabel -> setAlignment(Qt::AlignCenter);
        tw_cost->setCellWidget(row,7,maxTechCostLabel);
        int cost[4] = {0,0,0,0};
        for (int i = 0; i < designCount; i++) {
            if (shown[i]) {
                int tokenCost[4] = {0,0,0,0};
                calculateCost(design[i], count[i], displayedTechs[displayedTech].tech, tokenCost);
                for (int j = 0; j < 4; j++) cost[j] += tokenCost[j];
            }
        }

        for (int i = 0; i<4; i++) {
            QLabelModified * label = new QLabelModified(QString::number(cost[i]),this);
            label -> setAlignment(Qt::AlignCenter);
            label -> setAutoFillBackground(true);
            if (i == 0) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(173,216,255)));
            if (i == 1) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(128,255,128)));
            if (i == 2) label -> setPalette(QPalette(Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,Qt::black,QColor(255,255,64)));
            tw_cost->setCellWidget(row,i+8,label);
        }
    }


    for (int i = 0; i< 6 ; i++) spinbox_tech[i] -> setMinimum(minTech[i]);

    tw_cost->resizeRowsToContents();
    tw_cost->resizeColumnsToContents();
    //tw_cost->resizeRowToContents(1);

    techChanged();
}

