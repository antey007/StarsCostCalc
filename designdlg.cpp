#include "designdlg.h"
#include "importdlg.h"

designDlg::designDlg(QWidget *parent) :
    QDialog(parent)
{
    dsgnRow = -1;
    mainWindow = (MainWindow*) parent;
    initView();
}

designDlg::designDlg(cDesign design, QWidget *parent, int designRow)
{
    dsgnRow = designRow;
    mainWindow = (MainWindow*) parent;
    initView();
    fillForm(design);
    fillTable(design);
}

void designDlg::initView()
{
    setWindowTitle((dsgnRow==-1)?"New design":"Edit design");
    setMinimumSize(520,420);

    QVBoxLayout * mainVLayout = new QVBoxLayout();
    QHBoxLayout * topHLayout = new QHBoxLayout();
    QHBoxLayout * bottomHLayout = new QHBoxLayout();

    QLabel * ownerLabel = new QLabel("Owner",this);
    ownerSpinbox = new QSpinBox(this);
    ownerSpinbox -> setMinimum(0);
    ownerSpinbox -> setMaximum(255);
    ownerSpinbox -> setValue(0);

    QLabel * designNumberLabel = new QLabel("Design number",this);
    designNumberSpinbox = new QSpinBox(this);
    designNumberSpinbox -> setMinimum(0);
    designNumberSpinbox -> setMaximum(255);
    designNumberSpinbox -> setValue(0);

    designTableWidget = new QTableWidget(0,6,this);
    shipHullCombobox = new QComboBox(this);
    shipHullCombobox ->setMaxVisibleItems(mainWindow->shipHullCount);

    for (int i = 0; i < mainWindow->shipHullCount; i++) {
        shipHullCombobox ->addItem(mainWindow->shipHull[i].name);
    }
    shipHullCombobox -> setCurrentIndex(0);
    connect(shipHullCombobox,SIGNAL(currentIndexChanged(int)), this, SLOT(on_shipHull_changed(int)));
    connect(shipHullCombobox,SIGNAL(currentIndexChanged(int)), this, SLOT(designChanged()));

    QLabel * nameLabel = new QLabel("Name",this);
    designNameLineedit = new QLineEdit("Noname", this);

    massLabel = new QLabel("Mass",this);

    QLabel * countLabel = new QLabel("Count",this);
    countSpinBox = new QSpinBox(this);
    countSpinBox -> setMinimum(0);
    countSpinBox -> setMaximum(9999);
    countSpinBox -> setValue((dsgnRow==-1)?1:mainWindow->count[dsgnRow]);

    topHLayout -> addWidget(ownerLabel);
    topHLayout -> addWidget(ownerSpinbox);
    topHLayout -> addWidget(designNumberLabel);
    topHLayout -> addWidget(designNumberSpinbox);
    topHLayout -> addWidget(nameLabel);
    topHLayout -> addWidget(designNameLineedit);
    topHLayout -> addWidget(shipHullCombobox);
    topHLayout -> addWidget(massLabel);
    topHLayout -> addWidget(countLabel);
    topHLayout -> addWidget(countSpinBox);

    designTableWidget -> setMinimumWidth(500);
    QStringList slist;
    slist << tr("Slot type") << tr("Category") << tr("Item") << tr("Max") << tr("Count") << tr("Reserved");
    designTableWidget -> setHorizontalHeaderLabels(slist);
    designTableWidget -> setColumnWidth(0,100);
    designTableWidget -> setColumnWidth(1,80);
    designTableWidget -> setColumnWidth(2,160);
    designTableWidget -> setColumnWidth(3,30);
    designTableWidget -> setColumnWidth(4,40);
    designTableWidget -> setColumnWidth(5,60);

    passFilterLabel = new QLabel(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    QPushButton * loadDesignButton = new QPushButton(tr("Load from file"));
    buttonBox->addButton(loadDesignButton, QDialogButtonBox::ActionRole);
    connect(loadDesignButton, SIGNAL(clicked()), this, SLOT(loadCurrentDesign()));

    QPushButton * saveDesignButton = new QPushButton(tr("Save to file"));
    buttonBox->addButton(saveDesignButton, QDialogButtonBox::ActionRole);
    connect(saveDesignButton, SIGNAL(clicked()), this, SLOT(saveCurrentDesign()));

    if (dsgnRow != -1) {
        QPushButton * applyButton = new QPushButton(tr("Apply"));
        buttonBox->addButton(applyButton, QDialogButtonBox::ActionRole);
        connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

        QPushButton * removeButton = new QPushButton(tr("Remove"));
        buttonBox->addButton(removeButton, QDialogButtonBox::ActionRole);
        connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    }

    bottomHLayout -> addWidget(passFilterLabel);
    bottomHLayout -> addWidget(buttonBox);

    mainVLayout -> addLayout(topHLayout);
    mainVLayout -> addWidget(designTableWidget);
    mainVLayout -> addLayout(bottomHLayout);
    setLayout(mainVLayout);
    //qDebug() << "designDlg::initView() end";

}

void designDlg::fillForm(cDesign design)
{
    //qDebug() << "designDlg::fillForm(cDesign design)" << design.hullID << design.designName;
    ownerSpinbox->setValue(design.owner);
    designNumberSpinbox->setValue(design.designNumber);
    //if (design.hullID != combobox_shipHull->currentIndex())
        shipHullCombobox->setCurrentIndex(design.hullID);
    fillTable(design);
    designNameLineedit->setText(design.designName);
}

void designDlg::fillTable(cDesign design)
{
    //qDebug() << "designDlg::fillTable(cDesign design)" << design.hullID << design.designName;
    designTableWidget->setRowCount(0);
    designTableWidget->setRowCount(mainWindow->shipHull[design.hullID].slotCount);
    for (int slot = 0; slot< mainWindow->shipHull[design.hullID].slotCount; slot++) {
        // ------------ column 0 (Slot type) ----------------
        QString text;
        switch (mainWindow->shipHull[design.hullID].slot[slot].type) {
        case 0:         text = QString("Empty");                    break;
        case 1:         text = QString("Engine");                   break;
        case 2:         text = QString("Scanner");                  break;
        case 4:         text = QString("Shield");                   break;
        case 8:         text = QString("Armor");                    break;
        case 12:        text = QString("Shield/Armor");             break;
        case 48:        text = QString("Weapon");                   break;
        case 52:        text = QString("Shield/Weapon");            break;
        case 64:        text = QString("Bomb");                     break;
        case 128:       text = QString("MiningRobot");              break;
        case 256:       text = QString("MineLayer");                break;
        case 512:       text = QString("Orbital");                  break;
        case 1024:      text = QString("Planetary");                break;
        case 2048:      text = QString("Electrical");               break;
        case 4096:      text = QString("Mechanical");               break;
        case 6144:      text = QString("Elect/Mech");               break;
        case 6146:      text = QString("Scan/Elect/Mech");          break;
        case 6148:      text = QString("Shield/Elect/Mech");        break;
        case 6154:      text = QString("Scan/Arm/El/Mech");         break;
        case 6400:      text = QString("MineLayer/El/Mech");        break;
        case 6462:      text = QString("General Purpose");          break;
        default:        text = QString::number(mainWindow->shipHull[design.hullID].slot[slot].type);    break;
        }
        designTableWidget -> setCellWidget(slot, 0, new QLabel (text));

        // ------------ column 1 (Category) ----------------
        categorySelect * catSelect = new categorySelect(slot, this);
        // формируем список допустимых классов - зависит только от каркаса, не зависит от загружаемого дизайна
        int count = 0;
        for (int j=0; j<13; j++) { // итератор по битам в описании слота каркаса
            if (mainWindow->shipHull[design.hullID].slot[slot].type>>j&1) {
                //catSelect->setCategory(count,j); // запоминаем, что в этом объекте строка count соответствует классу j
                switch(j) {
                case 0:     catSelect->addItem(QString("Engine"));      catSelect->setCategory(count,14);     break;
                case 1:     catSelect->addItem(QString("Scanner"));     catSelect->setCategory(count,12);     break;
                case 2:     catSelect->addItem(QString("Shield"));      catSelect->setCategory(count,11);     break;
                case 3:     catSelect->addItem(QString("Armor"));       catSelect->setCategory(count,13);     break;
                case 4:     catSelect->addItem(QString("BeamWeapon"));  catSelect->setCategory(count, 2);     break;
                case 5:     catSelect->addItem(QString("Torpedo"));     catSelect->setCategory(count, 3);     break;
                case 6:     catSelect->addItem(QString("Bomb"));        catSelect->setCategory(count, 4);     break;
                case 7:     catSelect->addItem(QString("MiningRobot")); catSelect->setCategory(count, 7);     break;
                case 8:     catSelect->addItem(QString("MineLayer"));   catSelect->setCategory(count, 8);     break;
                case 9:     catSelect->addItem(QString("Orbital"));     catSelect->setCategory(count, 1);     break;
                case 10:    catSelect->addItem(QString("Planetary"));   catSelect->setCategory(count, 6);     break;
                case 11:    catSelect->addItem(QString("Electrical"));  catSelect->setCategory(count,10);     break;
                case 12:    catSelect->addItem(QString("Mechanical"));  catSelect->setCategory(count, 9);     break;
                }
                count++;
            }
        }
        // а если класс единственный - отключаем возможность изменения
        if (count == 1) catSelect -> setDisabled(true);

        // выбираем нужный класс - зависит от загружаемого дизайна
        int cat;
        if (design.items[slot] != 255) {
            cat = mainWindow->shipItem[design.items[slot]].category;
            for (int i = 0; i<16; i++) {
                if(catSelect->getCategory(i) == cat) {
                    catSelect->setCurrentIndex(i);
                }
            }
        } else {
            catSelect->setCurrentIndex(0);
            cat = catSelect->getCategory(0);
        }

        designTableWidget -> setCellWidget(slot, 1, catSelect);
        connect(catSelect,SIGNAL(categoryChanged(int, int)), this, SLOT(on_itemCategory_changed(int, int)));

        // ------------ column 2 (Item) ----------------

        /*switch(cat) {
        case 0:      fillItemComboBox(slot,14);     break;
        case 1:      fillItemComboBox(slot,12);     break;
        case 2:      fillItemComboBox(slot,11);     break;
        case 3:      fillItemComboBox(slot,13);     break;
        case 4:      fillItemComboBox(slot, 2);     break;
        case 5:      fillItemComboBox(slot, 3);     break;
        case 6:      fillItemComboBox(slot, 4);     break;
        case 7:      fillItemComboBox(slot, 7);     break;
        case 8:      fillItemComboBox(slot, 8);     break;
        case 9:      fillItemComboBox(slot, 1);     break;
        case 10:     fillItemComboBox(slot, 6);     break;
        case 11:     fillItemComboBox(slot,10);     break;
        case 12:     fillItemComboBox(slot, 9);     break;
        }*/

        fillItemComboBox(slot, cat);

        cItemSelect * is = (cItemSelect*) designTableWidget->cellWidget(slot,2);
        for (int i = 0; i<is->count(); i++) {
            if(is->getItem(i) == design.items[slot]) is->setCurrentIndex(i);
        }


        // ------------ column 3 (Max) ----------------
        QLabel * label = new QLabel (QString::number(mainWindow->shipHull[design.hullID].slot[slot].maxCount));
        label -> setAlignment(Qt::AlignCenter);
        designTableWidget -> setCellWidget(slot, 3, label);

        // ------------ column 4 (Count) ----------------
        QSpinBox * sb_count = new QSpinBox(this);
        sb_count->setMinimum(0);
        sb_count->setMaximum(mainWindow->shipHull[design.hullID].slot[slot].maxCount);
        sb_count->setValue(design.counts[slot]);
        designTableWidget->setCellWidget(slot, 4, sb_count);
        connect (sb_count,SIGNAL(valueChanged(int)),this, SLOT(designChanged()));

        designTableWidget->resizeRowToContents(slot);
    }
    designChanged();
    //mainWindow->fleetChanged();
}

void designDlg::fillItemComboBox(int hullSlot, int category)
{
    cItemSelect * itemSelect = new cItemSelect(category,this);
    itemSelect->setMaxVisibleItems(30);

    int index = 0;
    for (int i = 0; i< mainWindow->shipItemCount; i++) {
        if (mainWindow->shipItem[i].category == category) {
            itemSelect->addItem(mainWindow->shipItem[i].name);
            itemSelect->setItem(index,i);
            index++;
        }
    }
    designTableWidget -> setCellWidget(hullSlot, 2, itemSelect);
    connect(itemSelect,SIGNAL(currentIndexChanged(int)), this, SLOT(designChanged()));
    designChanged();
}

cDesign designDlg::currentDesign()
{
    cDesign design;
    design.owner = ownerSpinbox->value();
    design.designNumber = designNumberSpinbox->value();
    design.hullID = shipHullCombobox->currentIndex();
    for (int slot = 0; slot < mainWindow->shipHull[design.hullID].slotCount; slot++) {
        cItemSelect * is = (cItemSelect*) designTableWidget->cellWidget(slot,2);
        design.items[slot] = (is)?(is->getItem(is->currentIndex())):255; // item

        QSpinBox * sb_count = (QSpinBox*) designTableWidget->cellWidget(slot,4);
        design.counts[slot] = (sb_count)?sb_count->value():0; // count
    }
    design.designName = designNameLineedit->text();
    return design;
}

void designDlg::on_shipHull_changed(int index)
{
    cDesign emptyDesign;
    emptyDesign.owner = 0;
    emptyDesign.designNumber = 0;
    emptyDesign.designName = QString("NEW");
    emptyDesign.hullID = index;
    for (int i = 0; i< 16; i++) {
        emptyDesign.items[i] = 255;
        emptyDesign.counts[i] = 0;
    }
    fillTable(emptyDesign);
    //mainWindow->fleetChanged();
}

void designDlg::on_itemCategory_changed(int slot, int category)
{
    //qDebug() << slot << category;
    fillItemComboBox(slot, category);
}

void designDlg::designChanged()
{
    massLabel -> setText("Mass " + QString::number(mainWindow -> designMass(currentDesign())) + " kt");
    bool passed = mainWindow -> filterDialog -> passed(currentDesign());
    passFilterLabel -> setText(tr("This design ") + (passed?tr("passed"):tr("failed")) + tr(" the filter conditions"));
    QColor color = passed?Qt::green:Qt::red;
    passFilterLabel -> setPalette(QPalette(color,Qt::white,color,color,color,color,Qt::white));
}

void designDlg::saveCurrentDesign()
{
    saveDesign(&currentDesign());
}

void designDlg::loadCurrentDesign()
{
    cDesign design = currentDesign();
    loadDesign(&design);
    fillForm(design);
    fillTable(design);
    //mainWindow->fleetChanged();
    return;
}

void designDlg::saveDesign(cDesign * design, QString fileName, int count, int flags)
{
    bool suppressOwner = flags & 1;
    bool suppressName = flags & 2;
    //qDebug() "flags" << flags << "suppressOwner" << suppressOwner << "suppressName" << suppressName;

    if (fileName == NULL) {
        QClipboard *clipboard = QApplication::clipboard();
        QString text;
        if (count == 1) {
            text = hullShortNames[design[0].hullID] + QString::number(mainWindow->designMass(design[0])) + "_" + design[0].designName;
        } else {
            text = "library_" + QString::number(count);
        }
        clipboard->setText(text);

        fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Design file (*.des);;All files (*.*)"));
        if (fileName == NULL) return;
    }
    QFile file(fileName);
    file.open(QFile::WriteOnly);

    int slotCount;
    char * data = new char[250];

    for (int i = 0; i< count; i++) {
        slotCount = mainWindow->shipHull[design[i].hullID].slotCount;
        data[0] = 1; // desing file version
        data[1] = suppressOwner?0:(design[i].owner);
        data[2] = suppressOwner?0:(design[i].designNumber);
        data[3] = design[i].hullID;
        for (int slot = 0; slot< slotCount; slot++) {
            data[slot*2+4] = design[i].items[slot]; // item
            data[slot*2+5] = design[i].counts[slot]; // count
        }
        QString designName = design[i].designName;
        //qDebug() << design[i].designName;
        if (suppressName) designName = "";
        data[slotCount*2+4] = designName.length();
        memcpy(data+slotCount*2+5, designName.toLocal8Bit(), designName.length());

        file.write(data,5+2*slotCount+designName.length());
    }
    delete[] data;

    file.close();
}

void designDlg::loadDesign(cDesign * design)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select design file"), "", tr("Design file (*.des);;All files (*.*)"));
    if (fileName == NULL) return;
    QFile file(fileName);
    file.open(QFile::ReadOnly);

    char * data = new char[250];
    file.read(data, 250);

    if (data[0] != 1) {
        QMessageBox::warning(this,"Error","Unexpected desing file version");
        return;
    }
    design->owner = data[1];
    design->designNumber = data[2];
    design->hullID = data[3];

    int slotCount = mainWindow->shipHull[data[3]].slotCount;
    for (int slot = 0; slot < slotCount; slot++) {
        design->items[slot] = (unsigned char)data[slot*2+4];
        design->counts[slot] = data[slot*2+5];
    }
    design->designName = QString::fromLocal8Bit(data+slotCount*2+5,data[slotCount*2+4]);

    delete[] data;
    file.close();
}

void designDlg::accept()
{
    QDialog::accept();

    if (dsgnRow == -1) {
        // Match check
        bool match = false;
        for (int j = 0; j < mainWindow -> designCount; j++) {
            if (mainWindow -> isEqual(currentDesign(), mainWindow -> design[j])) {
                match = true;
                break;
            }
        }

        if (!match) {
            // Adding
            mainWindow -> design[mainWindow -> designCount] = currentDesign();
            mainWindow -> count[mainWindow -> designCount] = countSpinBox->value();
            mainWindow -> designCount++;
            if (mainWindow -> designCount >= mainWindow -> maxDesignCount) mainWindow -> increaseMaxDesignCount();
        }
    } else {
        mainWindow -> design[dsgnRow] = currentDesign();
        mainWindow -> count[dsgnRow] = countSpinBox->value();

    }
    mainWindow -> fillTable();
    mainWindow -> fleetChanged();

    close();
}

void designDlg::apply()
{
    mainWindow -> design[dsgnRow] = currentDesign();
    mainWindow -> count[dsgnRow] = countSpinBox->value();

    mainWindow -> fillTable();
    mainWindow -> fleetChanged();
}

void designDlg::remove()
{
    int ret = QMessageBox::question(this, "Remove design", "Remove this design from table?", tr("Remove"), tr ("Cancel"));
    qDebug () << ret;
    if (ret == 1) return;

    for (int j = dsgnRow; j < mainWindow -> designCount-1; j++) {
        mainWindow -> design[j] = mainWindow -> design[j+1];
        mainWindow -> count[j] = mainWindow -> count[j+1];
    }
    mainWindow -> designCount--;

    mainWindow -> fillTable();
    mainWindow -> fleetChanged();

    close();

}

