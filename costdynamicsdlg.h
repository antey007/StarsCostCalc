#ifndef COSTDYNAMICSDLG_H
#define COSTDYNAMICSDLG_H

#include <QDialog>
#include <QtWidgets>

#include "mainwindow.h"

class cTechPoint {
public:
    int techCost;
    int tech[6];
};

class cCostDynamicsDlg;
class cCostGraph : public QLabel {

public:
    cCostGraph (QWidget * parent = 0, Qt::WindowFlags f = 0);
    cCostGraph (const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
    cCostDynamicsDlg * costDynamicsDlg;

    void paintEvent(QPaintEvent* e);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    int minX, maxX;
    cTechPoint * table;
    int tableSize;
    int minY;

    double CX, CY;

    int curPoint1;
};


class cCostDynamicsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit cCostDynamicsDlg(QWidget *parent = 0);
    ~cCostDynamicsDlg();
    void initView();

    MainWindow * mainWindow;
    cDesign design;
    QString name;

    cCostGraph * costGraph;

    QRadioButton * ironRadioButton;
    QRadioButton * borRadioButton;
    QRadioButton * germRadioButton;
    QRadioButton * resRadioButton;
    QRadioButton * sumRadioButton;

    QDialogButtonBox * buttonBox;
    QTableWidget * tableWidget;


    cTechPoint * table0;
    cTechPoint * table1;
    cTechPoint * table2;
    cTechPoint * table3;
    cTechPoint * table4;
    int tableSize[5];

signals:
    
public slots:
    void processing();
    void mineresRadioButtonClicked();
    
};

#endif // COSTDYNAMICSDLG_H
