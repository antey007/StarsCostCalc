#include "costdynamicsdlg.h"


cCostDynamicsDlg::cCostDynamicsDlg(QWidget *parent) :
    QDialog(parent)
{
    mainWindow = (MainWindow *) parent;
    initView();
    table0 = table1 = table2 = table3 = table4 = NULL;
    tableSize[0] = tableSize[1] = tableSize[2] = tableSize[3] = tableSize[4] = 0;

    processing();

}

cCostDynamicsDlg::~cCostDynamicsDlg()
{
    if (table0) delete[] table0;
    if (table1) delete[] table1;
    if (table2) delete[] table2;
    if (table3) delete[] table3;
    if (table4) delete[] table4;
}

void cCostDynamicsDlg::initView()
{
    cDesign design;
    for (int i = 0; i<mainWindow->designCount; i++)
        if (mainWindow->shown[i] && mainWindow->count[i] > 0) {
            design = mainWindow -> design[i];
            break;
        }

    if (design.designName.isEmpty()) name = "Unnamed " + mainWindow->shipHull[design.hullID].name + tr(" ") + QString::number(mainWindow->designMass(design));
    else name = design.designName;
    setWindowTitle(QString("Cost dynamics of design ")+name);
    setMinimumSize(600,512);

    QHBoxLayout * mainHLayout = new QHBoxLayout();
    QVBoxLayout * rightVLayout = new QVBoxLayout();

    costGraph = new cCostGraph(this);
    //costGraph -> setMinimumSize(600,500);


    QGroupBox * mineresGroupBox = new QGroupBox(this);
    ironRadioButton = new QRadioButton("Iron",mineresGroupBox);
    borRadioButton = new QRadioButton("Bor",mineresGroupBox);
    germRadioButton = new QRadioButton("Germ",mineresGroupBox);
    resRadioButton = new QRadioButton("Res",mineresGroupBox);
    sumRadioButton = new QRadioButton("Sum",mineresGroupBox);
    sumRadioButton -> setChecked(true);

    QGridLayout * mineresLayout = new QGridLayout;
    mineresLayout ->addWidget(ironRadioButton);
    mineresLayout ->addWidget(borRadioButton);
    mineresLayout ->addWidget(germRadioButton);
    mineresLayout ->addWidget(resRadioButton);
    mineresLayout ->addWidget(sumRadioButton);
    mineresGroupBox->setLayout(mineresLayout);
    connect(ironRadioButton,SIGNAL(clicked()), this, SLOT(mineresRadioButtonClicked()));
    connect(borRadioButton,SIGNAL(clicked()), this, SLOT(mineresRadioButtonClicked()));
    connect(germRadioButton,SIGNAL(clicked()), this, SLOT(mineresRadioButtonClicked()));
    connect(resRadioButton,SIGNAL(clicked()), this, SLOT(mineresRadioButtonClicked()));
    connect(sumRadioButton,SIGNAL(clicked()), this, SLOT(mineresRadioButtonClicked()));


    tableWidget = new QTableWidget(14,3, this);
    tableWidget -> setMaximumWidth(220);

    tableWidget -> horizontalHeader() -> hide();
    QStringList slist;
    slist.clear();
    slist << "" << tr("En") << tr("Weap") << tr("Pro") << tr("Con") << tr("El") << tr("Bio") << tr("TechCost") << tr("diff")
          << tr("Iron") << tr("Bor") << tr("Germ") << tr("Res") << tr("Sum");
    tableWidget -> setVerticalHeaderLabels(slist);
    tableWidget -> resizeRowsToContents();
    tableWidget -> resizeColumnsToContents();


    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    //connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    //QPushButton * plotCostDynamiscButton = new QPushButton(tr("Plot"));
    //buttonBox->addButton(plotCostDynamiscButton, QDialogButtonBox::ActionRole);
    //connect(plotCostDynamiscButton, SIGNAL(clicked()), this, SLOT(processing()));

    rightVLayout -> addWidget(mineresGroupBox);
    rightVLayout -> addWidget(tableWidget,1);
    rightVLayout -> addWidget(buttonBox);

    mainHLayout -> addWidget(costGraph, 1);
    mainHLayout -> addLayout(rightVLayout);

    setLayout(mainHLayout);
}

void cCostDynamicsDlg::processing()
{
    // не представляю, как эта проверка может сработать, ведь аналогичная проверка уже есть обработчике кнопки CostDinamics, но на всякий случай оставляю
    if (mainWindow->designCount == 0) {
        QMessageBox::critical(this, tr("Design not loaded"), tr("No design loaded"));
        return;
    }
    bool ok = false;
    for (int i = 0; i<mainWindow->designCount; i++)
        if (mainWindow->count[i] > 0) {
            ok = true;
            design = mainWindow -> design[i];
            break;
        }
    // и тут тоже не представляю, как эта проверка может сработать
    if (!ok) {
        QMessageBox::critical(this, tr("Design not selected"), tr("To select design set positive count at left table"));
        return;
    }

    int minTech[6];
    mainWindow -> fleetMinTechReq(minTech);
    // TODO set current tech level from mX-file
    int maxCost[5];
    mainWindow -> calculateCost(design, 1, minTech, maxCost, 1);

    int maxTech[] = {26,26,26,26,26,26};
    int minCost[4];
    mainWindow -> calculateCost(design, 1, maxTech, minCost, 1);

    // Processing speed optimization: round 1 (2018.08.21)
    // attempt to minimize maxTech
    int cost[4];
    for (int i = 0 ; i < 6; i++) {
        do {
            maxTech[i] --;
            if (maxTech[i] < minTech[i]) break;
            mainWindow -> calculateCost(design, 1, maxTech, cost, 1);
            if (cost[0] != minCost[0]) break;
            if (cost[1] != minCost[1]) break;
            if (cost[2] != minCost[2]) break;
            if (cost[3] != minCost[3]) break;
        } while (true);

        // revert to last %maxTech% with %cost% equal to %minCost%
        maxTech[i] ++;
    }
    qDebug() << "New maxTech:" << maxTech[0] << maxTech[1] << maxTech[2] << maxTech[3] << maxTech[4] << maxTech[5];


    // Processing speed optimization: round 2 (2018.08.22)
    // нахожу уровни технологий, которые можно пропустить
    int tech[6];
    int prevCost[4];
    bool skipTech[6][27];
    int skip[6] = {0,0,0,0,0,0}; // for debug
    int notSkip[6] = {1,1,1,1,1,1}; // for debug
    for (int i = 0 ; i < 6; i++) {
        for (int j = 0 ; j < 6; j++) tech[j] = maxTech[j];
        tech[i] = minTech[i];
        mainWindow -> calculateCost(design, 1, tech, prevCost, 1);
        skipTech[i][tech[i]] = false; // skipTech[i][minTech[i]] is always false
        ////notSkip[i]++; уже учтено при создании массива
        for (tech[i] = minTech[i]+1; tech[i] <= maxTech[i]; tech[i]++) {
            mainWindow -> calculateCost(design, 1, tech, cost, 1);
            skipTech[i][tech[i]] = (cost[0] == prevCost[0] && cost[1] == prevCost[1] && cost[2] == prevCost[2] && cost[3] == prevCost[3]); // if eqial - true - skip this level
            if (skipTech[i][tech[i]]) skip[i]++; else notSkip[i]++;
            mainWindow -> calculateCost(design, 1, tech, prevCost, 1);
        }
    }
    qDebug() << "skip: " << skip[0] << skip[1] << skip[2] << skip[3] << skip[4] << skip[5];
    qDebug() << "notSkip: " << notSkip[0] << notSkip[1] << notSkip[2] << notSkip[3] << notSkip[4] << notSkip[5];
    qDebug() << "Optimization: "
             << (skip[0]+notSkip[0])*(skip[1]+notSkip[1])*(skip[2]+notSkip[2])*(skip[3]+notSkip[3])*(skip[4]+notSkip[4])*(skip[5]+notSkip[5])
             << " -> "
             << notSkip[0]*notSkip[1]*notSkip[2]*notSkip[3]*notSkip[4]*notSkip[5];



    QProgressDialog progress("Processing...", "Abort", 0, (maxTech[0]+1-minTech[0])*(maxTech[1]+1-minTech[1]), this);
    progress.setMinimumDuration(1);
    progress.setWindowModality(Qt::WindowModal);


    if (table0) delete[] table0;
    if (table1) delete[] table1;
    if (table2) delete[] table2;
    if (table3) delete[] table3;
    if (table4) delete[] table4;

    maxCost[4] = maxCost[0]+maxCost[1]+maxCost[2]+maxCost[3];
    table0 = new cTechPoint[maxCost[0]+1];
    table1 = new cTechPoint[maxCost[1]+1];
    table2 = new cTechPoint[maxCost[2]+1];
    table3 = new cTechPoint[maxCost[3]+1];
    table4 = new cTechPoint[maxCost[4]+1];
    tableSize[0] = maxCost[0]+1;
    tableSize[1] = maxCost[1]+1;
    tableSize[2] = maxCost[2]+1;
    tableSize[3] = maxCost[3]+1;
    tableSize[4] = maxCost[4]+1;

    for (int i = 0; i <= maxCost[0] ; i++) table0[i].techCost = 0;
    for (int i = 0; i <= maxCost[1] ; i++) table1[i].techCost = 0;
    for (int i = 0; i <= maxCost[2] ; i++) table2[i].techCost = 0;
    for (int i = 0; i <= maxCost[3] ; i++) table3[i].techCost = 0;
    for (int i = 0; i <= maxCost[4] ; i++) table4[i].techCost = 0;

    //int tech[6];
    //int cost[4];
    int techCost;
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

                            techCost = mainWindow -> researchCost(tech, mainWindow -> playerResearchCosts);
                            mainWindow -> calculateCost(design, 1, tech, cost, 1);

                            if (table0[cost[0]].techCost==0 || table0[cost[0]].techCost > techCost) {
                                table0[cost[0]].techCost = techCost;
                                memcpy(table0[cost[0]].tech,tech,6*sizeof(int));
                            }
                            if (table1[cost[1]].techCost==0 || table1[cost[1]].techCost > techCost) {
                                table1[cost[1]].techCost = techCost;
                                memcpy(table1[cost[1]].tech,tech,6*sizeof(int));
                            }
                            if (table2[cost[2]].techCost==0 || table2[cost[2]].techCost > techCost) {
                                table2[cost[2]].techCost = techCost;
                                memcpy(table2[cost[2]].tech,tech,6*sizeof(int));
                            }
                            if (table3[cost[3]].techCost==0 || table3[cost[3]].techCost > techCost) {
                                table3[cost[3]].techCost = techCost;
                                memcpy(table3[cost[3]].tech,tech,6*sizeof(int));
                            }
                            cost[4] = cost[0]+cost[1]+cost[2]+cost[3];
                            if (table4[cost[4]].techCost==0 || table4[cost[4]].techCost > techCost) {
                                table4[cost[4]].techCost = techCost;
                                memcpy(table4[cost[4]].tech,tech,6*sizeof(int));
                            }

                        }
                    }
                }
            }
        }
    }

//    int sumTechCost[4] = {0,0,0,0};
//    for (int i = 0; i <= maxCost[0] ; i++) sumTechCost[0] += table0[i].techCost;
//    for (int i = 0; i <= maxCost[1] ; i++) sumTechCost[1] += table1[i].techCost;
//    for (int i = 0; i <= maxCost[2] ; i++) sumTechCost[2] += table2[i].techCost;
//    for (int i = 0; i <= maxCost[3] ; i++) sumTechCost[3] += table3[i].techCost;
//    qDebug() << "sumTechCost: " << sumTechCost[0] << sumTechCost[1] << sumTechCost[2] << sumTechCost[3];


    if (progress.wasCanceled()) {
        qDebug() << "Canceled";
        return;
    }
    qDebug() << "Done";

    repaint();
}

void cCostDynamicsDlg::mineresRadioButtonClicked()
{
    costGraph -> curPoint1 = -1;
    tableWidget -> setColumnCount(1);
    repaint();
}


cCostGraph::cCostGraph(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    costDynamicsDlg = (cCostDynamicsDlg *) parent;
    curPoint1 = -1;
}

cCostGraph::cCostGraph(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    costDynamicsDlg = (cCostDynamicsDlg *) parent;
    curPoint1 = -1;
}

void cCostGraph::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    QPainter p(this);

    QString mineres = "Cost of one "+(costDynamicsDlg->name) + ": ";
    table = NULL;
    tableSize = 0;
    if (costDynamicsDlg->ironRadioButton->isChecked()) {
        table = costDynamicsDlg -> table0;
        tableSize = costDynamicsDlg -> tableSize[0];
        mineres += "Iron";
    }
    if (costDynamicsDlg->borRadioButton->isChecked())  {
        table = costDynamicsDlg -> table1;
        tableSize = costDynamicsDlg -> tableSize[1];
        mineres += "Boranium";
    }
    if (costDynamicsDlg->germRadioButton->isChecked()) {
        table = costDynamicsDlg -> table2;
        tableSize = costDynamicsDlg -> tableSize[2];
        mineres += "Germanium";
    }
    if (costDynamicsDlg->resRadioButton->isChecked())  {
        table = costDynamicsDlg -> table3;
        tableSize = costDynamicsDlg -> tableSize[3];
        mineres += "Resource";
    }
    if (costDynamicsDlg->sumRadioButton->isChecked())  {
        table = costDynamicsDlg -> table4;
        tableSize = costDynamicsDlg -> tableSize[4];
        mineres += "Sum of resources and all minerals";
    }

    if (!table) return;


    maxX = 0;
    minX = 1e9;
    minY = 0;
    for (int i = 0; i<tableSize;i++) {
        if (table[i].techCost != 0) {
            if (maxX < table[i].techCost) maxX = table[i].techCost;
            if (minX > table[i].techCost) minX = table[i].techCost;
            if (minY == 0) minY = i;
        }
    }

    CX = double(width()-40) / double(maxX - minX);
    CY = double(height()-40) / double(tableSize - minY);

    p.setPen(Qt::black);
    p.drawLine(8,height()-28,8,8);
    p.drawLine(9,8,11,14);
    p.drawLine(8,8,6,14);
    p.drawText(10, 10, mineres);

    p.drawLine(8,height()-28,width()-8,height()-28);
    p.drawLine(width()-8,height()-27,width()-14,height()-24);
    p.drawLine(width()-8,height()-28,width()-14,height()-31);
    p.drawText(width()-150,height()-10,"Summary technology cost (res)");

    if (curPoint1 != -1) p.setPen(Qt::gray);
    for (int i = 0; i<tableSize;i++) {
        if (table[i].techCost != 0) {
            p.drawPoint( 9+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
            p.drawPoint( 9+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
            p.drawPoint( 9+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);
            p.drawPoint(10+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
            p.drawPoint(10+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
            p.drawPoint(10+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);
            p.drawPoint(11+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
            p.drawPoint(11+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
            p.drawPoint(11+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);
        }
    }


    if (curPoint1 == -1) return;
    if (curPoint1 >= tableSize) return;
    p.setPen(Qt::black);
    p.drawPoint( 9+CX*(table[curPoint1].techCost-minX), height()-29-(curPoint1-minY)*CY);
    p.drawPoint( 9+CX*(table[curPoint1].techCost-minX), height()-30-(curPoint1-minY)*CY);
    p.drawPoint( 9+CX*(table[curPoint1].techCost-minX), height()-31-(curPoint1-minY)*CY);
    p.drawPoint(10+CX*(table[curPoint1].techCost-minX), height()-29-(curPoint1-minY)*CY);
    p.drawPoint(10+CX*(table[curPoint1].techCost-minX), height()-30-(curPoint1-minY)*CY);
    p.drawPoint(10+CX*(table[curPoint1].techCost-minX), height()-31-(curPoint1-minY)*CY);
    p.drawPoint(11+CX*(table[curPoint1].techCost-minX), height()-29-(curPoint1-minY)*CY);
    p.drawPoint(11+CX*(table[curPoint1].techCost-minX), height()-30-(curPoint1-minY)*CY);
    p.drawPoint(11+CX*(table[curPoint1].techCost-minX), height()-31-(curPoint1-minY)*CY);

    p.drawText(12+CX*(table[curPoint1].techCost-minX), height()-28-(curPoint1-minY)*CY, "A");

    QLabel * label1 = new QLabel("A");
    label1 -> setAlignment(Qt::AlignCenter);
    costDynamicsDlg -> tableWidget -> setCellWidget(0,0, label1);

    for (int i = 0; i<6; i++) {
        QLabel * label = new QLabel(QString::number(table[curPoint1].tech[i]));
        label -> setAlignment(Qt::AlignCenter);
        costDynamicsDlg -> tableWidget -> setCellWidget(i+1,0, label);
    }

    QString str;
    if (table[curPoint1].techCost < 1000) str = QString::number(table[curPoint1].techCost);
    else if (table[curPoint1].techCost < 10000) str = QString::number(double(table[curPoint1].techCost)/1000.,'f',2)+"K";
    else if (table[curPoint1].techCost < 100000) str = QString::number(double(table[curPoint1].techCost)/1000.,'f',1)+"K";
    else if (table[curPoint1].techCost < 1000000) str = QString::number(double(table[curPoint1].techCost)/1000.,'f',0)+"K";
    else if (table[curPoint1].techCost < 10000000) str = QString::number(double(table[curPoint1].techCost)/1000000.,'f',2)+"M";
    else str = QString::number(double(table[curPoint1].techCost)/1000000.,'f',1)+"M";
    QLabel * label = new QLabel(str);
    label -> setAlignment(Qt::AlignCenter);
    costDynamicsDlg -> tableWidget -> setCellWidget(7,0, label);

    int cost[5];
    costDynamicsDlg -> mainWindow -> calculateCost(costDynamicsDlg->design, 1, table[curPoint1].tech, cost, 1);
    cost[4] = cost[0] + cost[1] + cost[2] + cost[3];
    for (int i = 0; i<5; i++) {
        QLabel * label = new QLabel(QString::number(cost[i]));
        label -> setAlignment(Qt::AlignCenter);
        costDynamicsDlg -> tableWidget -> setCellWidget(9+i,0, label);
    }

    bool * progeny = new bool [tableSize];
    int progenyCount = 0;
    for (int i = curPoint1-1; i>=0; i--) {
        progeny[i] = false;
        if (table[i].techCost == 0) continue;
        if (table[i].tech[0] < table[curPoint1].tech[0]) continue;
        if (table[i].tech[1] < table[curPoint1].tech[1]) continue;
        if (table[i].tech[2] < table[curPoint1].tech[2]) continue;
        if (table[i].tech[3] < table[curPoint1].tech[3]) continue;
        if (table[i].tech[4] < table[curPoint1].tech[4]) continue;
        if (table[i].tech[5] < table[curPoint1].tech[5]) continue;

        progeny[i] = true;
        progenyCount ++;
        for (int j = curPoint1-1; j > i; j--) {
            if (!progeny[j]) continue;
            if (table[i].tech[0] < table[j].tech[0]) continue;
            if (table[i].tech[1] < table[j].tech[1]) continue;
            if (table[i].tech[2] < table[j].tech[2]) continue;
            if (table[i].tech[3] < table[j].tech[3]) continue;
            if (table[i].tech[4] < table[j].tech[4]) continue;
            if (table[i].tech[5] < table[j].tech[5]) continue;
            progeny[i] = false;
            progenyCount --;
            break;
        }

        if (!progeny[i]) continue;

        costDynamicsDlg -> tableWidget -> setColumnCount(progenyCount+1);

        QColor color;
        switch (progenyCount) {
        case 1: color = Qt::blue; break;
        case 2: color = Qt::green; break;
        case 3: color = Qt::red; break;
        case 4: color = Qt::magenta; break;
        }

        p.setPen(color);
        p.drawPoint( 9+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
        p.drawPoint( 9+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
        p.drawPoint( 9+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);
        p.drawPoint(10+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
        p.drawPoint(10+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
        p.drawPoint(10+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);
        p.drawPoint(11+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY);
        p.drawPoint(11+CX*(table[i].techCost-minX), height()-30-(i-minY)*CY);
        p.drawPoint(11+CX*(table[i].techCost-minX), height()-31-(i-minY)*CY);

        char letter[2] = {'A'+progenyCount,0};
        p.drawText(12+CX*(table[i].techCost-minX), height()-29-(i-minY)*CY, letter);

        QLabel * label1 = new QLabel(letter);
        label1 -> setPalette(QPalette(color,Qt::white,color,color,color,color,Qt::white));
        label1 -> setAlignment(Qt::AlignCenter);
        costDynamicsDlg -> tableWidget -> setCellWidget(0,progenyCount, label1);

        for (int j = 0; j<6; j++) {
            QLabel * label = new QLabel();
            QString str = QString::number(table[i].tech[j]);
            if (table[i].tech[j] != table[curPoint1].tech[j]) str += "(+" + QString::number(table[i].tech[j]-table[curPoint1].tech[j]) + ")";
            label -> setText(str);
            label -> setAlignment(Qt::AlignCenter);
            costDynamicsDlg -> tableWidget -> setCellWidget(j+1,progenyCount, label);
        }

        QString str;
        if (table[i].techCost < 1000) str = QString::number(table[i].techCost);
        else if (table[i].techCost < 10000) str = QString::number(double(table[i].techCost)/1000.,'f',2)+"K";
        else if (table[i].techCost < 100000) str = QString::number(double(table[i].techCost)/1000.,'f',1)+"K";
        else if (table[i].techCost < 1000000) str = QString::number(double(table[i].techCost)/1000.,'f',0)+"K";
        else if (table[i].techCost < 10000000) str = QString::number(double(table[i].techCost)/1000000.,'f',2)+"M";
        else str = QString::number(double(table[i].techCost)/1000000.,'f',1)+"M";
        QLabel * label = new QLabel(str);
        label -> setAlignment(Qt::AlignCenter);
        costDynamicsDlg -> tableWidget -> setCellWidget(7,progenyCount, label);

        int delta = table[i].techCost - table[curPoint1].techCost;
        if (delta < 1000) str = QString::number(delta);
        else if (delta < 10000) str = QString::number(double(delta)/1000.,'f',2)+"K";
        else if (delta < 100000) str = QString::number(double(delta)/1000.,'f',1)+"K";
        else if (delta < 1000000) str = QString::number(double(delta)/1000.,'f',0)+"K";
        else if (delta < 10000000) str = QString::number(double(delta)/1000000.,'f',2)+"M";
        else str = QString::number(double(delta)/1000000.,'f',1)+"M";
        label = new QLabel(str);
        label -> setAlignment(Qt::AlignCenter);
        costDynamicsDlg -> tableWidget -> setCellWidget(8,progenyCount, label);

        int costB[5];
        costDynamicsDlg -> mainWindow -> calculateCost(costDynamicsDlg->design, 1, table[i].tech, costB, 1);
        costB[4] = costB[0] + costB[1] + costB[2] + costB[3];
        for (int j = 0; j<5; j++) {
            QLabel * label = new QLabel();
            QString str = QString::number(costB[j]);
            if (costB[j] != cost[j]) str += "(" + QString::number(costB[j]-cost[j]) + ")";
            label -> setText(str);
            label -> setAlignment(Qt::AlignCenter);
            costDynamicsDlg -> tableWidget -> setCellWidget(9+j,progenyCount, label);
        }



    }

    qDebug() << "progenyCount" << progenyCount;
    delete [] progeny;

    costDynamicsDlg -> tableWidget -> setColumnCount(progenyCount+1);
    costDynamicsDlg -> tableWidget -> resizeColumnsToContents();

}

void cCostGraph::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "cCostGraph::mousePressEvent(QMouseEvent *ev)";
    switch(ev->button()) {
    case Qt::LeftButton:
        if (!table) return;
        int best = -1;
        double minDist = 1e10;
        double dist;
        for (int i = 0; i<tableSize;i++) {
            if (table[i].techCost != 0) {
                dist = (ev->x()-10-int(CX*(table[i].techCost-minX)));
                dist = dist*dist;
                dist += (ev->y()-height()+30+int((i-minY)*CY))*(ev->y()-height()+30+int((i-minY)*CY));
                if (dist < minDist) {
                    minDist = dist;
                    best = i;
                    qDebug() << best << minDist;
                }
            }
        }
        if (minDist < 400) {
            curPoint1 = best;
        }
        break;
    }
    repaint();
    return;
}

void cCostGraph::mouseReleaseEvent(QMouseEvent *ev)
{
}


