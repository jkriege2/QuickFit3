/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "filters.h"
#include "ui_qf3filtereditor.h"

FilterDescription::FilterDescription() {
    manufacturer="";
    name=QObject::tr("invalid");
    type="";
    isValid=false;
}


QF3FilterEditor::QF3FilterEditor(const FilterDescription& filter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QF3FilterEditor)
{
    ui->setupUi(this);
    init(filter);
}

QF3FilterEditor::~QF3FilterEditor()
{
    delete ui;
}

void QF3FilterEditor::init(const FilterDescription& filter) {
    if (filter.isValid) {
        ui->edtManufacturer->setText(filter.manufacturer);
        ui->edtName->setText(filter.name);
        ui->cmbType->setEditText(filter.type);
    }
}

FilterDescription QF3FilterEditor::getData() {
    FilterDescription o;
    o.name=ui->edtName->text();
    o.manufacturer=ui->edtManufacturer->text();
    o.type=ui->cmbType->currentText();
    o.isValid=true;
    return o;
}

















QF3FilterComboboxNotifier* QF3FilterCombobox::m_notifier=NULL;


QF3FilterCombobox::QF3FilterCombobox(QWidget* parent):
    QWidget(parent)
{
    if (m_notifier==NULL) m_notifier=new QF3FilterComboboxNotifier(NULL);
    globalfilters="";
    localfilters="";

    hbl=new QHBoxLayout();
    setLayout(hbl);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    cmbFilters=new QFEnhancedComboBox(this);
    cmbFilters->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    cmbFilters->setMinimumContentsLength(10);
    cmbFilters->view()->setTextElideMode(Qt::ElideRight);
    connect(cmbFilters, SIGNAL(currentIndexChanged(int)), this, SLOT(currentFilterChanged(int)));
    hbl->addWidget(cmbFilters);

    btnAddFilter=new QToolButton(this);
    btnAddFilter->setToolTip(tr("add a new filter"));
    btnAddFilter->setIcon(QIcon(":/libqf3widgets/filter_add.png"));
    btnAddFilter->setEnabled(true);
    connect(btnAddFilter, SIGNAL(clicked()), this, SLOT(addFilter()));
    hbl->addWidget(btnAddFilter);
    btnDeleteFilter=new QToolButton(this);
    btnDeleteFilter->setToolTip(tr("delete the current filter"));
    btnDeleteFilter->setIcon(QIcon(":/libqf3widgets/filter_delete.png"));
    btnDeleteFilter->setEnabled(true);
    connect(btnDeleteFilter, SIGNAL(clicked()), this, SLOT(deleteFilter()));
    hbl->addWidget(btnDeleteFilter);
    btnEditFilter=new QToolButton(this);
    btnEditFilter->setToolTip(tr("edit the current filter"));
    btnEditFilter->setIcon(QIcon(":/libqf3widgets/filter_rename.png"));
    btnEditFilter->setEnabled(true);
    connect(btnEditFilter, SIGNAL(clicked()), this, SLOT(editFilter()));
    hbl->addWidget(btnEditFilter);
    labFilterDescription=new QLabel(this);
    hbl->addWidget(labFilterDescription, 1);


    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(loadFilters()));

}

QF3FilterCombobox::~QF3FilterCombobox() {

}

void QF3FilterCombobox::currentFilterChanged(int /*idx*/) {
    int iD=cmbFilters->currentIndex()-1;
    if (iD>=0 && iD<filters.size()) labFilterDescription->setText(tr(""));
    else labFilterDescription->setText("");
}


void QF3FilterCombobox::loadFilters() {
    filters=getFilterList(globalfilters, localfilters);
    QString currentO=cmbFilters->currentText();
    bool updt=updatesEnabled();
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    cmbFilters->clear();
    cmbFilters->addItem(QIcon(":/libqf3widgets/filter_none.png"), tr("--- none ---"));
    for (int i=0; i<filters.size(); i++) {
        FilterDescription o=filters[i];
        cmbFilters->addItem(getFilterIcon(o), o.name);
    }

    int i=cmbFilters->findText(currentO);
    if (i<0) i=0;
    cmbFilters->setCurrentIndex(i);
    //hbl->update();
    if (widVisible) setUpdatesEnabled(updt);
}

QList<FilterDescription> QF3FilterCombobox::getFilterList(QString globalfilters, QString localfilters) {
    QList<FilterDescription> filterList;

    QSettings inifileg(globalfilters, QSettings::IniFormat);
    QSettings inifile(localfilters, QSettings::IniFormat);
    // first read global config
    QStringList groups=inifile.childGroups();
    QStringList groupsg=inifile.childGroups();
    for (int i=0; i<groupsg.size(); i++) {
        if (!groups.contains(groupsg[i])) groups.append(groupsg[i]);
    }
    for (int i=0; i<groups.size(); i++) {
        QString g=groups[i];
        FilterDescription o;
        o.name=inifile.value(g+"/name", inifileg.value(g+"/name", "")).toString();
        o.manufacturer=inifile.value(g+"/manufacturer", inifileg.value(g+"/manufacturer", "")).toString();
        o.type=inifile.value(g+"/type", inifileg.value(g+"/type", "")).toString();
        o.isValid=true;
        filterList.append(o);
    }

    qSort(filterList);

    return filterList;
}

FilterDescription QF3FilterCombobox::getFilter(QString filter, QString globalfilters, QString localfilters) {
    QList<FilterDescription> f=getFilterList(globalfilters, localfilters);
    FilterDescription fd;
    for (int i=0; i<f.size(); i++) {
        if (f[i].name==filter) {
            return f[i];
        }
    }
    return fd;
}

QIcon QF3FilterCombobox::getFilterIcon(const FilterDescription &o) {
    if (!o.isValid) return QIcon(":/libqf3widgets/filter_invalid.png");
    if (o.type.contains("dichro")) return QIcon(":/libqf3widgets/filter_dichroic.png");
    else if (o.type.contains("polar")) {
        if (o.type.contains("split")) return QIcon(":/libqf3widgets/filter_splitterpol.png");
        else if (o.type.contains("circular") || o.type.contains("lambda") || o.type.contains("waveplate")) return QIcon(":/libqf3widgets/filter_pol_circ.png");
        else return QIcon(":/libqf3widgets/filter_pol_lin.png");
    } else if (o.type.contains("bandpass")||o.type.contains("band pass")) return QIcon(":/libqf3widgets/filter_bandpass.png");
    else if (o.type.contains("longpass")||o.type.contains("long pass")) return QIcon(":/libqf3widgets/filter_longpass.png");
    else if (o.type.contains("shortpass")||o.type.contains("short pass")) return QIcon(":/libqf3widgets/filter_shortpass.png");
    else if (o.type.contains("notch")) return QIcon(":/libqf3widgets/filter_notch.png");
    else if (o.type.contains("neutral density") || o.type.contains("neutral") || o.type.contains("grey")) {
        if (o.type.contains("split")) return QIcon(":/libqf3widgets/filter_splitterneutral.png");
        else return QIcon(":/libqf3widgets/filter_neutraldensity.png");
    } else if (o.type.contains("mirror")) return QIcon(":/libqf3widgets/filter_mirror.png");
    else return QIcon(":/libqf3widgets/filter.png");
    return QIcon(":/libqf3widgets/filter.png");
}

QIcon QF3FilterCombobox::getFilterIcon(QString filter, QString globalfilters, QString localfilters) {
    return getFilterIcon(getFilter(filter, globalfilters, localfilters));
}


void QF3FilterCombobox::setFilterINI(QString globalfilters, QString localfilters) {
    this->globalfilters=globalfilters;
    this->localfilters=localfilters;
    loadFilters();
}

void QF3FilterCombobox::storeFilters() {
    QSettings inifileg(globalfilters, QSettings::IniFormat);
    QSettings inifilel(localfilters, QSettings::IniFormat);
    QSettings* inifile=&inifileg;
    if (!localfilters.isEmpty()) {
        inifile=&inifilel;
    }
    if (inifile->isWritable()) {
        inifile->clear();
        for (int i=0; i<filters.size(); i++) {
            FilterDescription o=filters[i];
            QString g="filter"+QString::number(i);
            inifile->setValue(g+"/name", o.name);
            inifile->setValue(g+"/manufacturer", o.manufacturer);
            inifile->setValue(g+"/type", o.type);
        }
        emit m_notifier->emitUpdate();
    }
    loadFilters();
}

void QF3FilterCombobox::deleteFilter() {
    int i=cmbFilters->currentIndex()-1;
    if (i>=0 && i<filters.size()) {
        filters.removeAt(i);
    }
    storeFilters();
    loadFilters();
}


void QF3FilterCombobox::editFilter() {
    int i=cmbFilters->currentIndex()-1;
    if (i>=0 && i<filters.size()) {
        FilterDescription d=filters[i];

        QF3FilterEditor dlg(d, NULL);
        if (dlg.exec()==QDialog::Accepted) {
            filters[i]=dlg.getData();
        }

    }
    storeFilters();
    loadFilters();
}

void QF3FilterCombobox::addFilter() {
    FilterDescription d;
    d.name=tr("new filter");
    QF3FilterEditor dlg(d, NULL);
    if (dlg.exec()==QDialog::Accepted) {
        d=dlg.getData();
        while (filterExists(d.name)) {
            QMessageBox::critical(this, "Filter Editor", "A filter with the same name already exists. Please rename!", QMessageBox::Ok, QMessageBox::Ok);
            if (dlg.exec()!=QDialog::Accepted) return;
            d=dlg.getData();
        }
        //qDebug()<<d.name;
        filters.append(d);
        qSort(filters);
    }
    storeFilters();
    loadFilters();
}

FilterDescription QF3FilterCombobox::getFilterDescription(int i) const {
    if (i>0 && i<=filters.size())
        return filters[i-1];
    FilterDescription o;
    o.isValid=false;
    return o;
}

FilterDescription QF3FilterCombobox::filter() const {
    FilterDescription d;
    int i=cmbFilters->currentIndex()-1;
    if (i>=0 && i<filters.size()) return filters[i];
    return d;
}
bool QF3FilterCombobox::filterExists(QString name) const {
    for (int i=0; i<filters.size(); i++) {
        if (name==filters[i].name) return true;
    }
    return false;

}

void QF3FilterCombobox::loadSettings(QSettings& settings, QString property) {
    QString id=settings.value(property+"/id", settings.value(property, "").toString()).toString();

    if (id.isEmpty()) cmbFilters->setCurrentIndex(0);
    else {
        int idx=cmbFilters->findText(id);
        if (idx<0) {
            FilterDescription d;
            d.manufacturer=settings.value(property+"/manufacturer", "???").toString();
            d.name=settings.value(property+"/name", id).toString();
            d.type=settings.value(property+"/type", "").toString();
            filters.append(d);
            storeFilters();
            loadFilters();
        }
        idx=cmbFilters->findText(id);
        cmbFilters->setCurrentIndex(idx);
    }
}

void QF3FilterCombobox::saveSettings(QSettings& settings, QString property) {
    settings.setValue(property+"/id", cmbFilters->currentText());
    settings.setValue(property+"/manufacturer", filter().manufacturer);
    settings.setValue(property+"/name", filter().name);
    settings.setValue(property+"/type", filter().type);
}

void QF3FilterCombobox::loadSettings(QFManyFilesSettings &settings, QString property)
{
    QString id=settings.value(property+"/id", settings.value(property, "").toString()).toString();

    if (id.isEmpty()) cmbFilters->setCurrentIndex(0);
    else {
        int idx=cmbFilters->findText(id);
        if (idx<0) {
            FilterDescription d;
            d.manufacturer=settings.value(property+"/manufacturer", "???").toString();
            d.name=settings.value(property+"/name", id).toString();
            d.type=settings.value(property+"/type", "").toString();
            filters.append(d);
            storeFilters();
            loadFilters();
        }
        idx=cmbFilters->findText(id);
        cmbFilters->setCurrentIndex(idx);
    }
}

void QF3FilterCombobox::saveSettings(QFManyFilesSettings &settings, QString property)
{
    settings.setValue(property+"/id", cmbFilters->currentText());
    settings.setValue(property+"/manufacturer", filter().manufacturer);
    settings.setValue(property+"/name", filter().name);
    settings.setValue(property+"/type", filter().type);

}


void QF3FilterCombobox::setReadOnly(bool readonly) {
    cmbFilters->setReadOnly(readonly);
    btnAddFilter->setEnabled(!readonly);
    btnEditFilter->setEnabled(!readonly);
    btnDeleteFilter->setEnabled(!readonly);
}

void QF3FilterCombobox::setCurrentFilter(const QString &name) {
    cmbFilters->setCurrentIndex(0);
    for (int i=0; i<filters.size(); i++) {
        if (name==filters[i].name) {
            cmbFilters->setCurrentIndex(i+1);
            break;
        }
    }
}

QF3FilterCombobox *QF3DualViewWidget::getSplitterCombobox() const
{
    return fltSplitter;
}

QF3FilterCombobox *QF3DualViewWidget::getShortCombobox() const
{
    return fltShort;
}

QF3FilterCombobox *QF3DualViewWidget::getLongCombobox() const
{
    return fltLong;
}

FilterDescription QF3DualViewWidget::filterSplitter() const
{
    return fltSplitter->filter();
}

FilterDescription QF3DualViewWidget::filterShort() const
{
    return fltShort->filter();
}

FilterDescription QF3DualViewWidget::filterLong() const
{
    return fltLong->filter();
}

QF3DualViewWidget::Orientation QF3DualViewWidget::orientation() const
{
    return QF3DualViewWidget::Orientation(cmbDirection->currentIndex());
}

void QF3DualViewWidget::setReadOnly(bool ro)
{
    cmbDirection->setReadOnly(!ro);
    fltSplitter->setReadOnly(ro);
    fltShort->setReadOnly(ro);
    fltLong->setReadOnly(ro);
}

void QF3DualViewWidget::setFilterINI(QString globalfilters, QString localfilters)
{
    fltSplitter->setFilterINI(globalfilters, localfilters);
    fltShort->setFilterINI(globalfilters, localfilters);
    fltLong->setFilterINI(globalfilters, localfilters);
}

void QF3DualViewWidget::loadSettings(QSettings &settings, QString property)
{
    cmbDirection->setCurrentIndex(settings.value(property+"/orientation", 0).toInt());
    fltSplitter->loadSettings(settings, property+"/splitter/");
    fltShort->loadSettings(settings, property+"/short/");
    fltLong->loadSettings(settings, property+"/long/");
}

void QF3DualViewWidget::saveSettings(QSettings &settings, QString property)
{
    settings.setValue(property+"/orientation", cmbDirection->currentIndex());
    fltSplitter->saveSettings(settings, property+"/splitter/");
    fltShort->saveSettings(settings, property+"/short/");
    fltLong->saveSettings(settings, property+"/long/");
}

void QF3DualViewWidget::loadSettings(QFManyFilesSettings &settings, QString property)
{
    cmbDirection->setCurrentIndex(settings.value(property+"/orientation", 0).toInt());
    fltSplitter->loadSettings(settings, property+"/splitter/");
    fltShort->loadSettings(settings, property+"/short/");
    fltLong->loadSettings(settings, property+"/long/");
}

void QF3DualViewWidget::saveSettings(QFManyFilesSettings &settings, QString property)
{
    settings.setValue(property+"/orientation", cmbDirection->currentIndex());
    fltSplitter->saveSettings(settings, property+"/splitter/");
    fltShort->saveSettings(settings, property+"/short/");
    fltLong->saveSettings(settings, property+"/long/");
}

QF3DualViewWidget::QF3DualViewWidget(QWidget *parent):
    QWidget(parent)
{
    lay=new QGridLayout();
    setLayout(lay);
    QLabel* l=NULL;
    cmbDirection=new QFEnhancedComboBox(this);
    cmbDirection->addItem(QIcon(":/libqf3widgets/dvhor.png"), tr("horizontal"));
    cmbDirection->addItem(QIcon(":/libqf3widgets/dvver.png"), tr("vertical"));
    lay->addWidget(new QLabel(tr("<small>&orientation:</small>")),0,0);
    lay->addWidget(cmbDirection,0,1);
    fltSplitter=new QF3FilterCombobox(this);
    lay->addWidget(new QLabel(tr("<small>splitter:</small>")),0,2);
    lay->addWidget(fltSplitter,0,3);
    fltShort=new QF3FilterCombobox(this);
    lay->addWidget(new QLabel(tr("<small>short &lambda;:</small>")),1,0);
    lay->addWidget(fltShort,1,1);
    fltLong=new QF3FilterCombobox(this);
    lay->addWidget(new QLabel(tr("<small>long &lambda;:</small>")),1,2);
    lay->addWidget(fltLong,1,3);
    lay->setContentsMargins(0,0,0,0);
    lay->setVerticalSpacing(1);
    lay->setColumnStretch(1,1);
    lay->setColumnStretch(3,1);

}

QF3DualViewWidget::~QF3DualViewWidget()
{
}

