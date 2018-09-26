#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "defs.h"
#include "designdlg.h"
#include "filterdlg.h"

class designDlg;
class filterDlg;
class QLabelModified : public QLabel
{
    Q_OBJECT

public:
    QLabelModified (QWidget * parent = 0, Qt::WindowFlags f = 0) : QLabel(parent, f) {};
    QLabelModified (const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0) : QLabel(text, parent, f) {};
    void mouseDoubleClickEvent(QMouseEvent *) {emit doubleClicked();};

signals:
    void doubleClicked();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int shipHullCount;
    cShipHull shipHull[40];

    int shipItemCount;
    cItem shipItem[300];

    int messageCount;
    cMessage message[400];

    int designCount;
    int maxDesignCount;
    cDesign * design;
    int * count;
    int * shown;
    int shownCount;

    void increaseMaxDesignCount();

    cTechs * displayedTechs;
    int displayedTechsCount;
    int maxDisplayedTechsCount;

    void increaseMaxDisplayedTechsCount();

    void initData();
    void initView();

    QTableWidget * tw_designes;

    QLabel * label1;

    filterDlg * filterDialog;

    QGroupBox * modeGroupBox;
    QRadioButton * costModeRadioButton;
    QRadioButton * salvageModeRadioButton;
    QRadioButton * scrapSpaceModeRadioButton;
    QRadioButton * scrapStarbaseModeRadioButton;
    QRadioButton * scrapPlanetModeRadioButton;
    QCheckBox * URLRT_ModeCheckBox;

    QGroupBox * researchCostsGroupBox;
    QComboBox * researchCostsEn;
    QComboBox * researchCostsWeap;
    QComboBox * researchCostsProp;
    QComboBox * researchCostsCon;
    QComboBox * researchCostsEl;
    QComboBox * researchCostsBio;

    QPushButton * costDynamicsPushButton;
    QPushButton * eventsPushButton;
    QPushButton * salvageAnalyzerPushButton;

    QTableWidget * tw_cost;

    QPushButton * pb_importTechLevel;
    QSpinBox * spinbox_tech[6];
    QLabel * label_cost[4];
    QPushButton * pb_addRowToCostTable;

    int designMass(cDesign design);
    bool isEqual(cDesign design1, cDesign design2);
    void swapDesign(int i, int j);
    void designMinTechReq(cDesign design, int tech[6]);
    void fleetMinTechReq(int tech[6]);
    void calculateCost(cDesign design, int count, int tech[6], int costs[4], int mode = 0);
    // mode: 1 - build cost
    // mode: 2 - battle salvage
    // mode: 3 - scrap in space
    // mode: 4 - scrap at planet
    // mode: 5 - scrap at starbase
    // mode: 0 - see mode in modeGroupBox

    int salv(int);

    // Calculate resources required to reach technology levels tech[6] with research costs cost[6]
    // each integer in cost[6] can take the values 1 (cheap), 2 (normal) and 3 (expensive)
    int researchCost(const int tech[6], const int playerResearchCosts[6]);

    int playerResearchCosts[6];

    bool flagStopCountChanged;

public slots:
    void newDesign();

    void fleetChanged();
    void techChanged();
    void addRowToCostTable();
    void fillTable();

    void tableDoubleClicked();

    void sort();
    void filter();

    void loadDesign();
    void importDesign();

    void saveLibrary();

    void countChanged();

    void researchCostsChanged();

    void costDynamicsPushButtonClicked();
    void eventsPushButtonClicked();
    void salvageAnalyzerPushButtonClicked();
};

#endif // MAINWINDOW_H
