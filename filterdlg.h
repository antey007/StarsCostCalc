#ifndef FILTERDLG_H
#define FILTERDLG_H

#include <QDialog>
#include "mainwindow.h"

class MainWindow;

class filterDlg : public QDialog
{
    Q_OBJECT
public:
    explicit filterDlg(QWidget *parent = 0);
    MainWindow * mainWindow;

    void initView();
    void initHullTab();
    void initEngineTab();
    void initBattleTab();
    void initTechReqTab();
    void initOtherTab();

    QTabWidget * tabWidget;
    QWidget * hullTab;
    QWidget * engineTab;
    QWidget * battleTab;
    QWidget * techReqTab;
    QWidget * otherTab;


    QCheckBox * hullCheckBox[40];
    QCheckBox * hullsAll;
    QCheckBox * hullsFr;
    QCheckBox * hullsWar;
    QCheckBox * hullsPriv;
    QCheckBox * hullsCol;
    QCheckBox * hullsBomb;
    QCheckBox * hullsMines;
    QCheckBox * hullsFuel;
    QCheckBox * hullsML;
    QCheckBox * hullsGP;
    QCheckBox * hulls1Eng;
    QCheckBox * hulls2Eng;
    QCheckBox * hulls3Eng;
    QCheckBox * hulls4Eng;

    QCheckBox * engineCheckBox[20];
    QCheckBox * engineAll;
    QCheckBox * engineNormal;
    QCheckBox * engineRamScoop;
    QCheckBox * engineW10;


    QGroupBox * weaponGroupBox;
    QCheckBox * unarmedCheckBox;

    QGroupBox * beamGroupBox;
    QCheckBox * sapperCheckBox;
    QCheckBox * r0beamCheckBox;
    QCheckBox * r1beamCheckBox;
    QCheckBox * r2beamCheckBox;
    QCheckBox * r3beamCheckBox;
    QCheckBox * gatlingCheckBox;

    QGroupBox * torpGroupBox;
    QCheckBox * r4torpCheckBox;
    QCheckBox * r5torpCheckBox;
    QCheckBox * AMtorpCheckBox;
    QCheckBox * missileCheckBox;


    QGroupBox * minTechReqGroupBox;
    QSpinBox * minTechReqSpinBox[6];
    QGroupBox * maxTechReqGroupBox;
    QSpinBox * maxTechReqSpinBox[6];


    bool passed(cDesign design);
    
signals:
    
public slots:
    void restoreDefault();
    void hullsAllClicked();
    void hullsFrClicked();
    void hullsWarClicked();
    void hullsPrivClicked();
    void hullsColClicked();
    void hullsBombClicked();
    void hullsMinesClicked();
    void hullsFuelClicked();
    void hullsMLClicked();
    void hullsGPClicked();
    void hulls1EngClicked();
    void hulls2EngClicked();
    void hulls3EngClicked();
    void hulls4EngClicked();

    void engineAllClicked();
    void engineNormalClicked();
    void engineRamScoopClicked();
    void engineW10Clicked();


};

#endif // FILTERDLG_H
