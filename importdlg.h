#ifndef IMPORTDLG_H
#define IMPORTDLG_H

#include <QDialog>
#include <QtWidgets>

#include "mainwindow.h"

class importDesignDlg : public QDialog
{
    Q_OBJECT
public:
    explicit importDesignDlg(QString fileName, QWidget *parent = 0);

    MainWindow * mainWindow;

    QTableWidget * tw_designes;
    QGroupBox * anonymity;
    QCheckBox * suppressOwner;
    QCheckBox * suppressDesNumber;
    QCheckBox * suppressName;


    cDesign design[256];
    int designCount;

signals:
    
public slots:
    void accept();

};

#endif // IMPORTDLG_H
