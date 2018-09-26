#ifndef SALVAGEANALYZERDLG_H
#define SALVAGEANALYZERDLG_H

#include <QDialog>
#include <QtWidgets>

#include "mainwindow.h"

class MainWindow;
class salvageListDlg : public QDialog
{
    Q_OBJECT
public:
    explicit salvageListDlg(QString fileName, QWidget *parent = 0);
    MainWindow * mainWindow;

    void initView();
    void initData(QString fileName);

    QTableWidget * tw_salvage;
    QCheckBox * hideCrustedCheckBox;
    QCheckBox * hideEmptyCheckBox;

    void loadCoordFromEventsSubblock(int * X, int * Y, unsigned char * subblock);

public slots:
    void hideCrustedSalvage();
    void salvageAnalyzer();
};

#define MAXDESIGNCOUNT 64
class salvageAnalysezDlg : public QDialog
{
    Q_OBJECT
public:
    explicit salvageAnalysezDlg(int row, QWidget *parent = 0);

    MainWindow * mainWindow;
    void initView();

    int player;
    int minerals[3];

    int designCount;
    cDesign design[MAXDESIGNCOUNT];
    int count[MAXDESIGNCOUNT];

    int minTech[6];
    int maxTech[6];
    bool skipTech[6][27];

    int minPassTech[6];
    int maxPassTech[6];

    cTechs * displayedTechs;
    int displayedTechsCount;
    int maxDisplayedTechsCount;

    void increaseMaxDisplayedTechsCount();

    QTableWidget * tw_designes;
    QLabel * label1;

    QTableWidget * tw_cost_short;
    QLabel * label2;
    QTableWidget * tw_cost_full;
    QLabel * label3;
    QPushButton * phase1Button;
    QPushButton * phase2Button;

public slots:
    void fleetChanged();
    void countChanged();

    void processingPhase1();
    void processingPhase2();

};


#endif // SALVAGEANALYZERDLG_H
