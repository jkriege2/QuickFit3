#include "objectives.h"
#include "ui_qf3objectiveeditor.h"

ObjectiveDescription::ObjectiveDescription() {
    manufacturer="";
    name="invalid";
    NA=1;
    magnification=1;
}


QF3ObjectiveEditor::QF3ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QF3ObjectiveEditor)
{
    ui->setupUi(this);
    init(objective);
}

QF3ObjectiveEditor::~QF3ObjectiveEditor()
{
    delete ui;
}

void QF3ObjectiveEditor::init(const ObjectiveDescription& objective) {
    ui->edtManufacturer->setText(objective.manufacturer);
    ui->edtName->setText(objective.name);
    ui->spinMagnification->setValue(objective.magnification);
    ui->spinNA->setValue(objective.NA);
}

ObjectiveDescription QF3ObjectiveEditor::getData() {
    ObjectiveDescription o;
    o.name=ui->edtName->text();
    o.manufacturer=ui->edtManufacturer->text();
    o.magnification=ui->spinMagnification->value();
    o.NA=ui->spinNA->value();
    return o;
}

















QF3ObjectiveComboboxNotifier* QF3ObjectiveCombobox::m_notifier=NULL;


QF3ObjectiveCombobox::QF3ObjectiveCombobox(QWidget* parent):
    QWidget(parent)
{
    if (m_notifier==NULL) m_notifier=new QF3ObjectiveComboboxNotifier(this);
    globalobjectives="";
    localobjectives="";

    hbl=new QHBoxLayout(this);
    setLayout(hbl);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    cmbObjective=new QEnhancedComboBox(this);
    cmbObjective->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(cmbObjective, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    hbl->addWidget(cmbObjective);

    btnAddObjective=new QToolButton(this);
    btnAddObjective->setToolTip(tr("add a new objective"));
    btnAddObjective->setIcon(QIcon(":/libqf3widgets/objective_add.png"));
    btnAddObjective->setEnabled(true);
    connect(btnAddObjective, SIGNAL(clicked()), this, SLOT(addObjective()));
    hbl->addWidget(btnAddObjective);
    btnDeleteObjective=new QToolButton(this);
    btnDeleteObjective->setToolTip(tr("delete an objective"));
    btnDeleteObjective->setIcon(QIcon(":/libqf3widgets/objective_delete.png"));
    btnDeleteObjective->setEnabled(true);
    connect(btnDeleteObjective, SIGNAL(clicked()), this, SLOT(deleteObjective()));
    hbl->addWidget(btnDeleteObjective);
    btnEditObjective=new QToolButton(this);
    btnEditObjective->setToolTip(tr("edit an objective"));
    btnEditObjective->setIcon(QIcon(":/libqf3widgets/objective_rename.png"));
    btnEditObjective->setEnabled(true);
    connect(btnEditObjective, SIGNAL(clicked()), this, SLOT(editObjective()));
    hbl->addWidget(btnEditObjective);
    labObjectiveDescription=new QLabel(this);
    hbl->addWidget(labObjectiveDescription, 1);


    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(loadObjectives()));

}

QF3ObjectiveCombobox::~QF3ObjectiveCombobox() {

}

void QF3ObjectiveCombobox::currentObjectiveChanged(int idx) {
    int iD=cmbObjective->currentIndex();
    if (iD>=0 && iD<objectives.size()) labObjectiveDescription->setText(tr("<small>magn.: %1x<br>NA: %2</small>").arg(objectives[iD].magnification).arg(objectives[iD].NA));
    else labObjectiveDescription->setText("");

    emit objectiveChanged();
}


void QF3ObjectiveCombobox::loadObjectives() {
    objectives.clear();
    QString currentO=cmbObjective->currentText();
    cmbObjective->clear();
    QSettings inifileg(globalobjectives, QSettings::IniFormat);
    QSettings inifile(localobjectives, QSettings::IniFormat);
    // first read global config
    QStringList groups=inifile.childGroups();
    QStringList groupsg=inifile.childGroups();
    for (int i=0; i<groupsg.size(); i++) {
        if (!groups.contains(groupsg[i])) groups.append(groupsg[i]);
    }
    for (int i=0; i<groups.size(); i++) {
        QString g=groups[i];
        ObjectiveDescription o;
        o.name=inifile.value(g+"/name", inifileg.value(g+"/name", "")).toString();
        o.manufacturer=inifile.value(g+"/manufacturer", inifileg.value(g+"/manufacturer", "")).toString();
        o.magnification=inifile.value(g+"/magnification", inifileg.value(g+"/magnification", 1)).toDouble();
        o.NA=inifile.value(g+"/na", 1).toDouble();
        objectives.append(o);
        cmbObjective->addItem(QIcon(":/libqf3widgets/objective.png"), o.name);
    }

    int i=cmbObjective->findText(currentO);
    if (i<0) i=0;
    cmbObjective->setCurrentIndex(i);
    hbl->update();

}

void QF3ObjectiveCombobox::setObjectivesINI(QString globalobjectives, QString localobjectives) {
    this->globalobjectives=globalobjectives;
    this->localobjectives=localobjectives;
    loadObjectives();
}

void QF3ObjectiveCombobox::storeObjectives() {
    QSettings inifileg(globalobjectives, QSettings::IniFormat);
    QSettings inifilel(localobjectives, QSettings::IniFormat);
    QSettings* inifile=&inifileg;
    if (!localobjectives.isEmpty()) {
        inifile=&inifilel;
    }
    if (inifile->isWritable()) {
        inifile->clear();
        for (int i=0; i<objectives.size(); i++) {
            ObjectiveDescription o=objectives[i];
            QString g="objective"+QString::number(i);
            inifile->setValue(g+"/name", o.name);
            inifile->setValue(g+"/manufacturer", o.manufacturer);
            inifile->setValue(g+"/magnification", o.magnification);
            inifile->setValue(g+"/na", o.NA);
        }
        emit m_notifier->emitUpdate();
    }
    loadObjectives();
}

void QF3ObjectiveCombobox::deleteObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        objectives.removeAt(i);
    }
    storeObjectives();
    loadObjectives();
}


void QF3ObjectiveCombobox::editObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        ObjectiveDescription d=objectives[i];

        QF3ObjectiveEditor dlg(d, NULL);
        if (dlg.exec()==QDialog::Accepted) {
            objectives[i]=dlg.getData();
        }

    }
    storeObjectives();
    loadObjectives();
}

void QF3ObjectiveCombobox::addObjective() {
    ObjectiveDescription d;
    d.name=tr("new objective");
    QF3ObjectiveEditor dlg(d, NULL);
    if (dlg.exec()==QDialog::Accepted) {
        d=dlg.getData();
        while (objectiveExists(d.name)) {
            QMessageBox::critical(this, "Objective Editor", "An objective with the same name already exists. Please rename!", QMessageBox::Ok, QMessageBox::Ok);
            if (dlg.exec()!=QDialog::Accepted) return;
            d=dlg.getData();
        }
        //qDebug()<<d.name;
        objectives.append(d);
    }
    storeObjectives();
    loadObjectives();
}

ObjectiveDescription QF3ObjectiveCombobox::getObjectiveDescription(int i) {
    return objectives[i];
}

ObjectiveDescription QF3ObjectiveCombobox::objective() {
    ObjectiveDescription d;
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

bool QF3ObjectiveCombobox::objectiveExists(QString name) {
    for (int i=0; i<objectives.size(); i++) {
        if (name==objectives[i].name) return true;
    }
    return false;

}


void QF3ObjectiveCombobox::loadSettings(QSettings& settings, QString property) {
    cmbObjective->setCurrentIndex(cmbObjective->findText(settings.value(property, "").toString()));
}

void QF3ObjectiveCombobox::saveSettings(QSettings& settings, QString property) {
    settings.setValue(property, cmbObjective->currentText());
}

void QF3ObjectiveCombobox::setReadOnly(bool readonly) {
    cmbObjective->setReadOnly(readonly);
    btnAddObjective->setEnabled(!readonly);
    btnEditObjective->setEnabled(!readonly);
    btnDeleteObjective->setEnabled(!readonly);
}
