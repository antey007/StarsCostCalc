#ifndef DESIGNDLG_H
#define DESIGNDLG_H

#include <QWizard>
#include <QtWidgets>

#include "defs.h"
#include "mainwindow.h"

class categorySelect : public QComboBox
{
    Q_OBJECT

public:
    categorySelect(int newSlot, QWidget * parent = 0)
        :QComboBox(parent)
    {
        hullSlot = newSlot;
        connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_indexChanged(int)));
        for (int i = 0; i<16; i++) categories[i] = -1;
    }
    ~categorySelect() {};

private:
    int hullSlot;
    int categories[16];

public:
    void setCategory(int index, int category) {
        if (index < 16 && index >= 0) categories[index] = category;
        else qDebug() << "Category index (" << index << ") out of bounds";
    }
    int getCategory(int index) { return categories[index];}

    void setHullSlot(int newSlot){ hullSlot = newSlot; }

signals:
    void categoryChanged(int, int);
public slots:
    void on_indexChanged(int index) { emit categoryChanged(hullSlot, categories[index]); }

};


class cItemSelect : public QComboBox
{
    Q_OBJECT

public:
    cItemSelect(int newCategory, QWidget * parent = 0)
        :QComboBox(parent)
    {
        category = newCategory;
        for (int i = 0; i<50; i++) items[i] = 255;
    }
    ~cItemSelect() {};

private:
    int category;
    int items[50];

public:
    void setCategory(int newCategory) { category = newCategory; }
    int getCategory() { return category; }
    void setItem(int index, int item) {
        if (index < 50 && index >= 0) items[index] = item;
        else qDebug() << "Item index (" << index << ") out of bounds";
    }
    int getItem(int index) {return items[index];}

};

class MainWindow;
class designDlg : public QDialog
{
    Q_OBJECT
public:
    explicit designDlg(QWidget *parent = 0);
    explicit designDlg(cDesign dsgn, QWidget *parent = 0, int designRow = -1);

    void initView();

    MainWindow * mainWindow;

    QSpinBox * ownerSpinbox;
    QSpinBox * designNumberSpinbox;
    QComboBox * shipHullCombobox;
    QLineEdit * designNameLineedit;
    QLabel * massLabel;
    QSpinBox * countSpinBox;

    QTableWidget * designTableWidget;

    QLabel * passFilterLabel;
    QDialogButtonBox * buttonBox;

    void fillForm (cDesign design);
    void fillTable (cDesign design);
    void fillItemComboBox (int hullSlot, int category);

    cDesign currentDesign();
    int dsgnRow;

signals:
    
public slots:
    void on_shipHull_changed(int);
    void on_itemCategory_changed(int, int);
    void designChanged();

    void saveCurrentDesign();
    void loadCurrentDesign();

    void saveDesign(cDesign * design, QString fileName = NULL, int count = 1, int flags = 0);
    void loadDesign(cDesign * design);

    void accept();
    void apply();
    void remove();

};

#endif // DESIGNDLG_H
