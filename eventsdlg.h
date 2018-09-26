#ifndef EVENTSDLG_H
#define EVENTSDLG_H

#include <QDialog>
#include <QtWidgets>

#include "mainwindow.h"

class eventsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit eventsDlg(QString fileName, QWidget *parent = 0);

    MainWindow * mainWindow;

    QTableWidget * tw_events;

signals:
    
public slots:
    
};

#endif // EVENTSDLG_H
