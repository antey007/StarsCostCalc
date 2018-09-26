#include "filterdlg.h"

filterDlg::filterDlg(QWidget *parent) :
    QDialog(parent)
{
    mainWindow = (MainWindow *) parent;
    initView();
}

void filterDlg::initView()
{
    setWindowTitle("Filter");
    setMinimumWidth(500);

    tabWidget = new QTabWidget;
    hullTab = new QWidget(this);
    engineTab = new QWidget(this);
    battleTab = new QWidget(this);
    techReqTab = new QWidget(this);
    otherTab = new QWidget(this);

    tabWidget -> addTab (hullTab, "Hull");
    tabWidget -> addTab (engineTab, "Engines");
    tabWidget -> addTab (battleTab, "Weapon");
    tabWidget -> addTab (techReqTab, "Tech Req");
    tabWidget -> addTab (otherTab, "Other");

    QHBoxLayout * mainHLayout = new QHBoxLayout();

    initHullTab();
    initEngineTab();
    initBattleTab();
    initTechReqTab();
    initOtherTab();


    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Close, Qt::Vertical);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefault()));

    mainHLayout -> addWidget(tabWidget);
    mainHLayout -> addWidget(buttonBox);

    setLayout(mainHLayout);

    restoreDefault();

}

void filterDlg::initHullTab()
{
    QHBoxLayout * hullHLayout = new QHBoxLayout();
    QVBoxLayout * hullV1Layout = new QVBoxLayout();
    QVBoxLayout * hullV2Layout = new QVBoxLayout();
    //hullV1Layout -> setSpacing(0);
    for (int i = 0; i < mainWindow -> shipHullCount; i++) {
        hullCheckBox[i] = new QCheckBox(mainWindow -> shipHull[i].name);
        if (i<16) hullV1Layout -> addWidget(hullCheckBox[i]);
        else hullV2Layout -> addWidget(hullCheckBox[i]);
        connect(hullCheckBox[i], SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    }
    hullV1Layout -> addStretch();
    hullV2Layout -> addStretch();

    QVBoxLayout * hullGroupsVLayout = new QVBoxLayout();

    hullsAll    = new QCheckBox("All hull");
    hullsFr     = new QCheckBox("Freighters");
    hullsWar    = new QCheckBox("Warships");
    hullsPriv   = new QCheckBox("Cargo ships");
    hullsCol    = new QCheckBox("Colonizers");
    hullsBomb   = new QCheckBox("Bombers");
    hullsMines  = new QCheckBox("Miners");
    hullsFuel   = new QCheckBox("Fuel ships");
    hullsML     = new QCheckBox("Mine layers");
    hullsGP     = new QCheckBox("GP ships");
    hulls1Eng   = new QCheckBox("1 engine");
    hulls2Eng   = new QCheckBox("2 engines");
    hulls3Eng   = new QCheckBox("3 engines");
    hulls4Eng   = new QCheckBox("4-5 engines");

    hullGroupsVLayout -> addWidget(hullsAll);
    hullGroupsVLayout -> addWidget(hullsFr);
    hullGroupsVLayout -> addWidget(hullsWar);
    hullGroupsVLayout -> addWidget(hullsPriv);
    hullGroupsVLayout -> addWidget(hullsCol);
    hullGroupsVLayout -> addWidget(hullsBomb);
    hullGroupsVLayout -> addWidget(hullsMines);
    hullGroupsVLayout -> addWidget(hullsFuel);
    hullGroupsVLayout -> addWidget(hullsML);
    hullGroupsVLayout -> addWidget(hullsGP);
    hullGroupsVLayout -> addStretch();
    hullGroupsVLayout -> addWidget(hulls1Eng);
    hullGroupsVLayout -> addWidget(hulls2Eng);
    hullGroupsVLayout -> addWidget(hulls3Eng);
    hullGroupsVLayout -> addWidget(hulls4Eng);


    connect(hullsAll, SIGNAL(clicked()), this, SLOT(hullsAllClicked()));
    connect(hullsFr, SIGNAL(clicked()), this, SLOT(hullsFrClicked()));
    connect(hullsWar, SIGNAL(clicked()), this, SLOT(hullsWarClicked()));
    connect(hullsPriv, SIGNAL(clicked()), this, SLOT(hullsPrivClicked()));
    connect(hullsCol, SIGNAL(clicked()), this, SLOT(hullsColClicked()));
    connect(hullsBomb, SIGNAL(clicked()), this, SLOT(hullsBombClicked()));
    connect(hullsMines, SIGNAL(clicked()), this, SLOT(hullsMinesClicked()));
    connect(hullsFuel, SIGNAL(clicked()), this, SLOT(hullsFuelClicked()));
    connect(hullsML, SIGNAL(clicked()), this, SLOT(hullsMLClicked()));
    connect(hullsGP, SIGNAL(clicked()), this, SLOT(hullsGPClicked()));
    connect(hulls1Eng, SIGNAL(clicked()), this, SLOT(hulls1EngClicked()));
    connect(hulls2Eng, SIGNAL(clicked()), this, SLOT(hulls2EngClicked()));
    connect(hulls3Eng, SIGNAL(clicked()), this, SLOT(hulls3EngClicked()));
    connect(hulls4Eng, SIGNAL(clicked()), this, SLOT(hulls4EngClicked()));

    hullHLayout -> addLayout(hullV1Layout);
    hullHLayout -> addLayout(hullV2Layout);
    hullHLayout -> addLayout(hullGroupsVLayout);
    hullTab -> setLayout(hullHLayout);

}

void filterDlg::initEngineTab()
{
    QHBoxLayout * engineHLayout = new QHBoxLayout();
    QVBoxLayout * engineV1Layout = new QVBoxLayout();
    QVBoxLayout * engineV2Layout = new QVBoxLayout();

    for (int i = 0; i < 16; i++) {
        engineCheckBox[i] = new QCheckBox(mainWindow -> shipItem[i+186].name);
        engineV1Layout -> addWidget(engineCheckBox[i]);
        connect(engineCheckBox[i], SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    }
    engineCheckBox[16] = new QCheckBox("Not installed");
    engineV1Layout -> addWidget(engineCheckBox[16]);
    connect(engineCheckBox[16], SIGNAL(clicked()), mainWindow, SLOT(fillTable()));

    engineV1Layout -> addStretch();

    engineAll = new QCheckBox("All engines");
    engineV2Layout -> addWidget(engineAll);
    connect(engineAll, SIGNAL(clicked()), this, SLOT(engineAllClicked()));

    engineNormal = new QCheckBox("Normal engines");
    engineV2Layout -> addWidget(engineNormal);
    connect(engineNormal, SIGNAL(clicked()), this, SLOT(engineNormalClicked()));

    engineRamScoop = new QCheckBox("Ram scoop engines");
    engineV2Layout -> addWidget(engineRamScoop);
    connect(engineRamScoop, SIGNAL(clicked()), this, SLOT(engineRamScoopClicked()));

    engineW10 = new QCheckBox("Warp 10 is safe");
    engineV2Layout -> addWidget(engineW10);
    connect(engineW10, SIGNAL(clicked()), this, SLOT(engineW10Clicked()));

    engineV2Layout -> addStretch();

    engineHLayout -> addLayout(engineV1Layout);
    engineHLayout -> addLayout(engineV2Layout);
    engineTab -> setLayout(engineHLayout);
}

void filterDlg::initBattleTab()
{
    weaponGroupBox = new QGroupBox("Weapon type filter",this);
    weaponGroupBox -> setCheckable(true);
    unarmedCheckBox = new QCheckBox("Ships without beam weapons and torpedoes", weaponGroupBox);
    beamGroupBox = new QGroupBox("Ships with beam weapons", weaponGroupBox);
    beamGroupBox -> setCheckable(true);
    sapperCheckBox = new QCheckBox("Sappers", beamGroupBox);
    r0beamCheckBox = new QCheckBox("Range 0 weapon", beamGroupBox);
    r1beamCheckBox = new QCheckBox("Range 1 weapon", beamGroupBox);
    r2beamCheckBox = new QCheckBox("Range 2 weapon", beamGroupBox);
    r3beamCheckBox = new QCheckBox("Range 3 weapon", beamGroupBox);
    gatlingCheckBox = new QCheckBox("Gatling weapon", beamGroupBox);

    torpGroupBox = new QGroupBox("Ships with torpedoes", weaponGroupBox);
    torpGroupBox -> setCheckable(true);
    r4torpCheckBox = new QCheckBox("Range 4 torpedoes", torpGroupBox);
    r5torpCheckBox = new QCheckBox("Range 3 torpedoes", torpGroupBox);
    AMtorpCheckBox = new QCheckBox("Anti Matter torpedo (MT toy)", torpGroupBox);
    missileCheckBox = new QCheckBox("Missiles", torpGroupBox);

    connect(weaponGroupBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(unarmedCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(beamGroupBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(sapperCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r0beamCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r1beamCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r2beamCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r3beamCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(gatlingCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(torpGroupBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r4torpCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(r5torpCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(AMtorpCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(missileCheckBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));


    QVBoxLayout * battleTabVLayout = new QVBoxLayout();
    QVBoxLayout * weaponVLayout = new QVBoxLayout();
    QHBoxLayout * weaponHLayout = new QHBoxLayout();
    QVBoxLayout * beamVLayout = new QVBoxLayout();
    beamVLayout -> addWidget(sapperCheckBox);
    beamVLayout -> addWidget(r0beamCheckBox);
    beamVLayout -> addWidget(r1beamCheckBox);
    beamVLayout -> addWidget(r2beamCheckBox);
    beamVLayout -> addWidget(r3beamCheckBox);
    beamVLayout -> addWidget(gatlingCheckBox);
    beamGroupBox -> setLayout(beamVLayout);

    QVBoxLayout * torpVLayout = new QVBoxLayout();
    torpVLayout -> addWidget(r4torpCheckBox);
    torpVLayout -> addWidget(r5torpCheckBox);
    torpVLayout -> addWidget(AMtorpCheckBox);
    torpVLayout -> addWidget(missileCheckBox);
    torpGroupBox -> setLayout(torpVLayout);

    weaponHLayout -> addWidget(beamGroupBox);
    weaponHLayout -> addWidget(torpGroupBox);

    weaponVLayout -> addWidget(unarmedCheckBox);
    weaponVLayout -> addLayout(weaponHLayout);

    weaponGroupBox -> setLayout(weaponVLayout);


    battleTabVLayout -> addWidget(weaponGroupBox);
    battleTabVLayout -> addStretch();
    battleTab -> setLayout(battleTabVLayout);
}

void filterDlg::initTechReqTab()
{
    QVBoxLayout * techReqVLayout = new QVBoxLayout();
    QHBoxLayout * minTechReqHLayout = new QHBoxLayout();
    QHBoxLayout * maxTechReqHLayout = new QHBoxLayout();
    minTechReqGroupBox = new QGroupBox("Min tech req BIGGER THAN",this);
    minTechReqGroupBox -> setCheckable(true);
    maxTechReqGroupBox = new QGroupBox("Min tech req LESS THAN",this);
    maxTechReqGroupBox -> setCheckable(true);
    connect(minTechReqGroupBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));
    connect(maxTechReqGroupBox, SIGNAL(clicked()), mainWindow, SLOT(fillTable()));

    for (int i=0; i<6; i++)  {
        minTechReqSpinBox[i] = new QSpinBox(minTechReqGroupBox);
        minTechReqHLayout -> addWidget(minTechReqSpinBox[i]);
        minTechReqSpinBox[i] -> setMinimum(0);
        minTechReqSpinBox[i] -> setMaximum(26);
        minTechReqSpinBox[i] -> setValue(0);
        connect(minTechReqSpinBox[i], SIGNAL(valueChanged(int)), mainWindow, SLOT(fillTable()));
        maxTechReqSpinBox[i] = new QSpinBox(maxTechReqGroupBox);
        maxTechReqHLayout -> addWidget(maxTechReqSpinBox[i]);
        maxTechReqSpinBox[i] -> setMinimum(0);
        maxTechReqSpinBox[i] -> setMaximum(26);
        maxTechReqSpinBox[i] -> setValue(26);
        connect(maxTechReqSpinBox[i], SIGNAL(valueChanged(int)), mainWindow, SLOT(fillTable()));
    }
    minTechReqGroupBox -> setLayout(minTechReqHLayout);
    maxTechReqGroupBox -> setLayout(maxTechReqHLayout);
    minTechReqSpinBox[0] -> setPrefix("En:");
    minTechReqSpinBox[1] -> setPrefix("W:");
    minTechReqSpinBox[2] -> setPrefix("P:");
    minTechReqSpinBox[3] -> setPrefix("C:");
    minTechReqSpinBox[4] -> setPrefix("El:");
    minTechReqSpinBox[5] -> setPrefix("B:");

    maxTechReqSpinBox[0] -> setPrefix("En:");
    maxTechReqSpinBox[1] -> setPrefix("W:");
    maxTechReqSpinBox[2] -> setPrefix("P:");
    maxTechReqSpinBox[3] -> setPrefix("C:");
    maxTechReqSpinBox[4] -> setPrefix("El:");
    maxTechReqSpinBox[5] -> setPrefix("B:");

    techReqVLayout -> addWidget(minTechReqGroupBox);
    techReqVLayout -> addWidget(maxTechReqGroupBox);
    techReqVLayout -> addStretch();
    techReqTab -> setLayout(techReqVLayout);
}

void filterDlg::initOtherTab()
{
    QHBoxLayout * otherHLayout = new QHBoxLayout();
    QVBoxLayout * otherV1Layout = new QVBoxLayout();
    QVBoxLayout * otherV2Layout = new QVBoxLayout();


    otherHLayout -> addLayout(otherV1Layout);
    otherHLayout -> addLayout(otherV2Layout);
    otherTab -> setLayout(otherHLayout);
}

void filterDlg::restoreDefault() {
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        hullCheckBox[i] -> setChecked(true);
    hullsAll    -> setCheckState(Qt::Checked);
    hullsFr     -> setCheckState(Qt::Checked);
    hullsWar    -> setCheckState(Qt::Checked);
    hullsPriv   -> setCheckState(Qt::Checked);
    hullsCol    -> setCheckState(Qt::Checked);
    hullsBomb   -> setCheckState(Qt::Checked);
    hullsMines  -> setCheckState(Qt::Checked);
    hullsFuel   -> setCheckState(Qt::Checked);
    hullsML     -> setCheckState(Qt::Checked);
    hullsGP     -> setCheckState(Qt::Checked);
    hulls1Eng   -> setCheckState(Qt::Checked);
    hulls2Eng   -> setCheckState(Qt::Checked);
    hulls3Eng   -> setCheckState(Qt::Checked);
    hulls4Eng   -> setCheckState(Qt::Checked);

    for (int i = 0; i < 17; i++) // 16 engines + "not installed"
        engineCheckBox[i] -> setChecked(true);
    engineAll -> setChecked(true);
    engineNormal -> setChecked(true);
    engineRamScoop -> setChecked(true);
    engineW10 -> setChecked(true);

    weaponGroupBox -> setChecked(false);
    unarmedCheckBox -> setChecked(true);
    beamGroupBox -> setChecked(true);
    sapperCheckBox -> setChecked(true);
    r0beamCheckBox -> setChecked(true);
    r1beamCheckBox -> setChecked(true);
    r2beamCheckBox -> setChecked(true);
    r3beamCheckBox -> setChecked(true);
    gatlingCheckBox -> setChecked(true);

    torpGroupBox -> setChecked(true);
    r4torpCheckBox -> setChecked(true);
    r5torpCheckBox -> setChecked(true);
    AMtorpCheckBox -> setChecked(true);
    missileCheckBox -> setChecked(true);

    minTechReqGroupBox -> setChecked(false);
    maxTechReqGroupBox -> setChecked(false);
    for (int i=0; i<6; i++)  {
        minTechReqSpinBox[i] -> setValue(0);
        maxTechReqSpinBox[i] -> setValue(26);
    }

    mainWindow -> fillTable();
}

bool filterDlg::passed(cDesign design) {
    if (!hullCheckBox[design.hullID]->isChecked()) return false;

    if (design.items[0] == 255 || design.counts[0] == 0) {// engine not installed
        if (!engineCheckBox[16]->isChecked()) return false;
    } else {
        if (!engineCheckBox[design.items[0]-186]->isChecked()) return false;
    }

    if (minTechReqGroupBox -> isChecked() || maxTechReqGroupBox -> isChecked()) {
        int tech[6];
        mainWindow -> designMinTechReq(design, tech);
        for (int i = 0; i<6; i++) {
            if (minTechReqGroupBox -> isChecked() && tech[i] < minTechReqSpinBox[i]->value()) return false;
            if (maxTechReqGroupBox -> isChecked() && tech[i] > maxTechReqSpinBox[i]->value()) return false;
        }
    }

    if (weaponGroupBox -> isChecked()) {
        bool hasBeam = false;
        bool hasTorp = false;

        bool sapper = false;
        bool r0beam = false;
        bool r1beam = false;
        bool r2beam = false;
        bool r3beam = false;
        bool gatling = false;

        bool r4torp = false;
        bool r5torp = false;
        bool AMtorp = false;
        bool missile = false;

        bool hasEmptySlot = false;

        for (int slot = 1; slot < mainWindow->shipHull[design.hullID].slotCount; slot++) {
            if (design.counts[slot] == 0) {hasEmptySlot = true; continue;}
            int item = design.items[slot];
            if (item == 255) hasEmptySlot = true;
            if (item >= 16 && item <= 39) hasBeam = true;
            if (item >= 40 && item <= 51) hasTorp = true;
            if (item >= 40 && item <= 42) r4torp = true;
            if (item >= 43 && item <= 46) r5torp = true;
            if (item >= 48 && item <= 51) missile = true;
            switch (item) {
            case 22: case 28: case 35: sapper = true; break;
            case 20: case 26: case 32: r0beam = true; break;
            case 16: case 17: case 19: case 25: case 31: case 38: r1beam = true; break;
            case 21: case 27: case 33: case 39: r2beam = true; break;
            case 23: case 29: case 34: case 36: r3beam = true; break;
            case 18: case 24: case 30: case 37: gatling = true; break;
            case 47: AMtorp = true; break;
            }
        }
        bool passWeaponFilter = false;
        if (!hasBeam && !hasTorp) {
            if (unarmedCheckBox->isChecked() == true) passWeaponFilter = true;
        }
        if (beamGroupBox -> isChecked()) {
            if (sapperCheckBox -> isChecked() && sapper) passWeaponFilter = true;
            if (r0beamCheckBox -> isChecked() && r0beam) passWeaponFilter = true;
            if (r1beamCheckBox -> isChecked() && r1beam) passWeaponFilter = true;
            if (r2beamCheckBox -> isChecked() && r2beam) passWeaponFilter = true;
            if (r3beamCheckBox -> isChecked() && r3beam) passWeaponFilter = true;
            if (gatlingCheckBox -> isChecked() && gatling) passWeaponFilter = true;
        }
        if (torpGroupBox -> isChecked()) {
            if (r4torpCheckBox -> isChecked() && r4torp) passWeaponFilter = true;
            if (r5torpCheckBox -> isChecked() && r5torp) passWeaponFilter = true;
            if (AMtorpCheckBox -> isChecked() && AMtorp) passWeaponFilter = true;
            if (missileCheckBox -> isChecked() && missile) passWeaponFilter = true;
        }
        if (!passWeaponFilter) return false;

    }
    return true;
}



void filterDlg::hullsAllClicked() {
    bool state = hullsAll->isChecked();
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        hullCheckBox[i] -> setChecked(state);
    hullsAll    -> setChecked(state);
    hullsFr     -> setChecked(state);
    hullsWar    -> setChecked(state);
    hullsPriv   -> setChecked(state);
    hullsCol    -> setChecked(state);
    hullsBomb   -> setChecked(state);
    hullsMines  -> setChecked(state);
    hullsFuel   -> setChecked(state);
    hullsML     -> setChecked(state);
    hullsGP     -> setChecked(state);
    hulls1Eng   -> setChecked(state);
    hulls2Eng   -> setChecked(state);
    hulls3Eng   -> setChecked(state);
    hulls4Eng   -> setChecked(state);

    mainWindow -> fillTable();
}

void filterDlg::hullsFrClicked() {
    bool state = hullsFr->isChecked();
    for (int i = 0; i < 4; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsWarClicked() {
    bool state = hullsWar->isChecked();
    for (int i = 6; i < 11; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsPrivClicked() {
    bool state = hullsPriv->isChecked();
    for (int i = 11; i < 14; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsColClicked() {
    bool state = hullsCol->isChecked();
    for (int i = 14; i < 16; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsBombClicked() {
    bool state = hullsBomb->isChecked();
    for (int i = 16; i < 20; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsMinesClicked() {
    bool state = hullsMines->isChecked();
    for (int i = 20; i < 25; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsFuelClicked() {
    bool state = hullsFuel->isChecked();
    for (int i = 25; i < 27; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsMLClicked() {
    bool state = hullsML->isChecked();
    for (int i = 27; i < 29; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hullsGPClicked() {
    bool state = hullsGP->isChecked();
    for (int i = 29; i < 32; i++)
        hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hulls1EngClicked() {
    bool state = hulls1Eng->isChecked();
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        if (mainWindow -> shipHull[i].slot[0].maxCount == 1)
            hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hulls2EngClicked() {
    bool state = hulls2Eng->isChecked();
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        if (mainWindow -> shipHull[i].slot[0].maxCount == 2)
            hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hulls3EngClicked() {
    bool state = hulls3Eng->isChecked();
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        if (mainWindow -> shipHull[i].slot[0].maxCount == 3)
            hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::hulls4EngClicked() {
    bool state = hulls4Eng->isChecked();
    for (int i = 0; i < mainWindow -> shipHullCount; i++)
        if (mainWindow -> shipHull[i].slot[0].maxCount >= 4)
            hullCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}



void filterDlg::engineAllClicked()
{
    bool state = engineAll->isChecked();
    for (int i = 0; i < 16; i++)
        engineCheckBox[i] -> setChecked(state);
    engineNormal -> setChecked(state);
    engineRamScoop -> setChecked(state);
    engineW10 -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::engineNormalClicked()
{
    bool state = engineNormal->isChecked();
    for (int i = 1; i < 10; i++) {
        if (i == 2 || i == 8) continue;
        engineCheckBox[i] -> setChecked(state);
    }
    mainWindow -> fillTable();
}

void filterDlg::engineRamScoopClicked()
{
    bool state = engineRamScoop->isChecked();
    engineCheckBox[2] -> setChecked(state);
    for (int i = 10; i < 16; i++)
        engineCheckBox[i] -> setChecked(state);
    mainWindow -> fillTable();
}

void filterDlg::engineW10Clicked()
{
    bool state = engineW10->isChecked();
    engineCheckBox[7] -> setChecked(state);
    engineCheckBox[8] -> setChecked(state);
    engineCheckBox[9] -> setChecked(state);
    engineCheckBox[14] -> setChecked(state);
    engineCheckBox[15] -> setChecked(state);
    mainWindow -> fillTable();
}

