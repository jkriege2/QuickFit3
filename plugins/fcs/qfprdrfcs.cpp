/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfprdrfcs.h"
#include <QtGui>
#include "qfrdrfcsdata.h"
#include "dlgcsvparameters.h"
#include <QtXml>
#include "flex_sin_tools.h"
#include "qfselectrdrdialog.h"
#include "qftablemodel.h"
#include "qenhancedtableview.h"
#include "qftabledelegate.h"
#include "qfdoubleedit.h"
#include "alv5000tools.h"
#include "alv6000tools.h"
#include "alv7000tools.h"
#include "confocor3tools.h"
#include "qfwizard.h"
#include "qfselectfileslistwidget.h"
#include "qffitfunctioncombobox.h"

QFPRDRFCS::QFPRDRFCS(QObject* parent):
    QObject(parent)
{
    //ctor
    dlgSimulator=NULL;
}

QFPRDRFCS::~QFPRDRFCS()
{
    if (dlgSimulator) {
        dlgSimulator->reject();
        delete dlgSimulator;
        dlgSimulator=NULL;
    }
}

QFRawDataRecord* QFPRDRFCS::createRecord(QFProject* parent) {
    return new QFRDRFCSData(parent);
}


void QFPRDRFCS::registerToMenu(QMenu* menu) {
    QMenu* smenu=menu->addMenu(QIcon(":/fcs_logo.png"), tr("FCS/DLS data"));
    QAction* actFCS=new QAction(QIcon(":/fcs_open.png"), tr("Open FCS/DLS Data from File"), parentWidget);
    actFCS->setStatusTip(tr("opens a file selection dialog and inserts a new FCS/DLS raw data record for each selected file"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCS()));
    smenu->addAction(actFCS);
    QAction* actMultiFCS=new QAction(QIcon(":/fcs_multifileopen.png"), tr("Insert Multi-File FCS/DLS Data"), parentWidget);
    actMultiFCS->setStatusTip(tr("opens a file selection dialog and inserts a single FCS/DLS raw data record containing all selected files"));
    connect(actMultiFCS, SIGNAL(triggered()), this, SLOT(insertMultiFileFCS()));
    smenu->addAction(actMultiFCS);
    QAction* actFCSSim=new QAction(QIcon(":/fcs_simulate.png"), tr("FCS/DLS data from fit function"), parentWidget);
    connect(actFCSSim, SIGNAL(triggered()), this, SLOT(openSimulator()));
    smenu->addAction(actFCSSim);
}

void QFPRDRFCS::init()
{
    QMenu* tmenu=services->getMenu("tools")->addMenu(QIcon(getIconFilename()), tr("FCS/DLS tools"));
    QAction* actBackground=new QAction(tr("set &background intensity in FCS/DLS datasets"), parentWidget);
    connect(actBackground, SIGNAL(triggered()), this, SLOT(setBackgroundInFCS()));
    tmenu->addAction(actBackground);
    QAction* actOffset=new QAction(tr("subtract &offset of correlation functions"), parentWidget);
    connect(actOffset, SIGNAL(triggered()), this, SLOT(correctOffset()));
    tmenu->addAction(actOffset);
    services->registerWizard("project_wizards", tr("FCS Wizard"), QIcon(getIconFilename()), this, SLOT(startFCSProjectWizard()));
}

void QFPRDRFCS::setBackgroundInFCS(const QVector<double> &backgrounds, const QVector<double> &background_sds, const QVector<bool> &background_set)
{
    QFMatchRDRFunctorSelectType* sel=new QFMatchRDRFunctorSelectType("fcs");
    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(sel,true, parentWidget);
    dlg->setWindowTitle(tr("set background intensities in FCS records ..."));
    dlg->setAllowMultiSelect(true);
    dlg->setAllowCreateNew(false);
    dlg->setOnlineHelp(services->getPluginHelpDirectory("fcs")+"/tutorial_background.html");

    QFTableModel* tab=new QFTableModel(dlg);
    tab->setReadonly(false);
    tab->clear();
    tab->setUndoEnabled(false);
    for (int i=0; i<qMax(2, backgrounds.size()); i++) {
        tab->setCellCreate(i, 0, backgrounds.value(i, 0.0));
        tab->setCellCreate(i, 1, background_sds.value(i, 0.0));
        tab->setCellCheckedRoleCreate(i, 2, background_set.value(i, true)?(Qt::Checked):(Qt::Unchecked));
        tab->setRowTitleCreate(i, tr("channel %1").arg(i+1));
    }
    tab->setColumnTitleCreate(0, tr("average"));
    tab->setColumnTitleCreate(1, tr("std. dev."));
    tab->setColumnTitleCreate(2, tr("set this channel"));

    QEnhancedTableView* tv=new QEnhancedTableView(dlg, true);
    tv->setItemDelegate(new QFTableDelegate(tv));
    tv->setModel(tab);
    dlg->addWidget(tr("backrgound intensities [Hz]:"), tv);

    if (dlg->exec()) {
        QList<QPointer<QFRawDataRecord> > sel=dlg->getSelectedRDRs();
        for (int i=0; i<sel.size(); i++) {
            QFRDRFCSData* fcs=qobject_cast<QFRDRFCSData*>(sel[i]);
            if (fcs) {
                for (int c=0; c<tab->rowCount(); c++) {
                    double cnt=tab->cell(c,0).toDouble();
                    double cnte=tab->cell(c, 1).toDouble();
                    if (tab->data(tab->index(c, 2), Qt::CheckStateRole).toInt()!=Qt::Unchecked) {
                        fcs->setQFProperty(QString("BACKGROUND_INTENSITY%1").arg(c+1), cnt, true, true);
                        fcs->setQFProperty(QString("BACKGROUND_INTENSITY_STD%1").arg(c+1), cnte, true, true);
                    }
                }
            }
        }
    }

}

void QFPRDRFCS::correctOffset()
{
    QFMatchRDRFunctorSelectType* sel=new QFMatchRDRFunctorSelectType("fcs");
    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(sel,true, parentWidget);
    dlg->setWindowTitle(tr("correct offset in FCS records ..."));
    dlg->setAllowMultiSelect(true);
    dlg->setAllowCreateNew(false);
    dlg->setOnlineHelp(services->getPluginHelpDirectory("fcs")+"/tutorial_offset.html");

    QFDoubleEdit* edtOffset=new QFDoubleEdit(dlg);
    edtOffset->setValue(0);
    edtOffset->setCheckBounds(false, false);
    dlg->addWidget(tr("offset:"), edtOffset);


    if (dlg->exec()) {
        QList<QPointer<QFRawDataRecord> > sel=dlg->getSelectedRDRs();
        for (int i=0; i<sel.size(); i++) {
            QFRDRFCSData* fcs=qobject_cast<QFRDRFCSData*>(sel[i]);
            if (fcs) {
                fcs->setQFProperty(QString("CF_CORRECT_OFFSET"), edtOffset->value(), true, true);
            }
        }
        if (QMessageBox::information(parentWidget, tr("corect correlation offset"), tr("The project needs to be saved and reloaded for the offset correction to take effect!\nReload project now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
            QFPluginServices::getInstance()->reloadCurrentProject();
        }

    }
}


void QFPRDRFCS::insertALV5000File(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    bool crossCorrelation=false;
    try {
        ALV_analyze(filename.value(0, ""), mode, cc, runCount, crossCorrelation);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV5000 file '%1':\n%2").arg(filename.value(0, "")).arg(E.what()));
        services->log_error(tr("Error while importing ALV5000 file '%1':\n    %2\n").arg(filename.value(0, "")).arg(E.what()));

    }
    for (unsigned int i=0; i<cc; i++) {
        QMap<QString, QVariant> p=paramValues;
        p["CHANNEL"]=i;
        QString role="ACF";
        if (!crossCorrelation) {
            role=(QString("ACF%1").arg(i));
        } else  {
            role=(QString("FCCS"));
            if (i>0) role=(QString("FCCS%10").arg(i));
        }

        QFRawDataRecord* e=project->addRawData(getID(), tr("%1 - %2").arg(QFileInfo(filename.value(0, "")).fileName()).arg(role), filename, p, paramReadonly);
        e->setRole(role);
        if (cc>1) e->setGroup(project->addOrFindRDRGroup(QFileInfo(filename.value(0, "")).fileName()));

        if (e->error()) {
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV5000 file '%1' (channel %3/%4):\n%2").arg(filename.value(0, "")).arg(e->errorDescription()).arg(i+1).arg(cc));
            services->log_error(tr("Error while importing ALV5000 file '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
}

void QFPRDRFCS::insertALV6000File(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    bool crossCorrelation=false;
    try {
        ALV6_analyze(filename.value(0, ""), mode, cc, runCount, crossCorrelation);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV6000 file '%1':\n%2").arg(filename.value(0, "")).arg(E.what()));
        services->log_error(tr("Error while importing ALV6000 file '%1':\n    %2\n").arg(filename.value(0, "")).arg(E.what()));

    }
    for (unsigned int i=0; i<cc; i++) {
        QMap<QString, QVariant> p=paramValues;
        p["CHANNEL"]=i;
        QString role="ACF";
        if (!crossCorrelation) {
            role=(QString("ACF%1").arg(i));
        } else  {
            role=(QString("FCCS"));
            if (i>0) role=(QString("FCCS%10").arg(i));
        }

        QFRawDataRecord* e=project->addRawData(getID(), tr("%1 - %2").arg(QFileInfo(filename.value(0, "")).fileName()).arg(role), filename, p, paramReadonly);
        e->setRole(role);
        if (cc>1) e->setGroup(project->addOrFindRDRGroup(QFileInfo(filename.value(0, "")).fileName()));

        if (e->error()) {
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV6000 file '%1' (channel %3/%4):\n%2").arg(filename.value(0, "")).arg(e->errorDescription()).arg(i+1).arg(cc));
            services->log_error(tr("Error while importing ALV6000 file '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
}

struct ALV700ConfigData {
    QString role;
    int channel;
    QList<int> columns;
    bool swapped;
};

void QFPRDRFCS::insertALV7000File(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    int inputchannels=2;
    bool crossCorrelation=false;
    bool autocorrelation=false;
    int firstchannel=0;
    QStringList pro=paramReadonly;
    pro<<"CHANNEL"<<"COLUMNS"<<"CHANNELS_SWAPPED";
    try {
        ALV7_analyze(filename.value(0, ""), mode, cc, runCount, crossCorrelation, autocorrelation, inputchannels, firstchannel);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV700X file '%1':\n%2").arg(filename.value(0, "")).arg(E.what()));
        services->log_error(tr("Error while importing ALV700X file '%1':\n    %2\n").arg(filename.value(0, "")).arg(E.what()));

    }
    QList<ALV700ConfigData> cdata;
    for (unsigned int i=0; i<cc; i++) {
        ALV700ConfigData d;
        d.channel=i+firstchannel;
        d.swapped=i>0;
        //if (cc<=1) d.channel=channel;
        d.role="ACF";
        if (autocorrelation) {
            d.role=QString("ACF%1").arg(d.channel);
            d.columns.clear();
            d.columns.append(i+1);
            // TODO: possibly add more by runCount
            cdata<<d;
        }
        if (crossCorrelation) {
            d.role=(QString("FCCS"));
            if (d.channel>0) d.role=(QString("FCCS%10").arg(d.channel));
            d.columns.clear();
            if (autocorrelation) {
                d.columns.append(i+cc+1);
            } else {
                d.columns.append(i+1);
            }
            // TODO: possibly add more by runCount

            cdata<<d;
        }
    }

    for (int i=0; i<cdata.size(); i++) {
        QMap<QString, QVariant> p=paramValues;
        p["CHANNEL"]=cdata[i].channel;
        p["COLUMNS"]=listToString(cdata[i].columns, false, false);
        p["CHANNELS_SWAPPED"]=cdata[i].swapped;
        QString role=cdata[i].role;
        QFRawDataRecord* e=project->addRawData(getID(), tr("%1 - %2").arg(QFileInfo(filename.value(0, "")).fileName()).arg(role), filename, p, pro);
        e->setRole(role);
        if (cc>1) e->setGroup(project->addOrFindRDRGroup(QFileInfo(filename.value(0, "")).fileName()));

        if (e->error()) {
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV700X file '%1' (channel %3/%4):\n%2").arg(filename.value(0, "")).arg(e->errorDescription()).arg(i+1).arg(cc));
            services->log_error(tr("Error while importing ALV700X file '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
}


void QFPRDRFCS::insertCSVFile(const QStringList &filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename.value(0, "")).fileName(), filename, paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename.value(0, "")).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}


void QFPRDRFCS::insertALBAFile(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename.value(0, "")).fileName(), filename, paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename.value(0, "")).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFPRDRFCS::insertDiffusion4File(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename.value(0, "")).fileName(), filename, paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename.value(0, "")).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFPRDRFCS::insertOlegFile(const QStringList &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly)
{
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename.value(0, "")).fileName(), filename, paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename.value(0, "")).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFPRDRFCS::insertConfocor3File(const QStringList &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly)
{
    unsigned int cc=1;

    Confocor3Tools reader;
    reader.loadFile(filename.value(0, ""));
    if (reader.wasError()) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ConfoCor3 file '%1':\n%2").arg(filename.value(0, "")).arg(reader.getLastErrors().join("\n")));
        services->log_error(tr("Error while importing ConfoCor3 file '%1':\n    %2\n").arg(filename.value(0, "")).arg(reader.getLastErrors().join("\n")));
    } else {
        const Confocor3Tools::ConfocorDataset& d=reader.getData();
        QMap<QString, QVariant> p=paramValues;
        //QStringList roles=d.getRoles();
        QStringList groups=d.getGroups();
        QList<int> kinetics=d.getKinetics();
        QList<int> positions=d.getPositions();
        QList<QVector<int> > reps=d.getRepetitions();
        QString name=QFileInfo(filename.value(0, "")).fileName();
        if (!d.name.isEmpty()) name=d.name;
        for (int r=0; r<reps.size(); r++) {
            if (reps[r].size()>0) {
                QStringList roles;
                for (int i=0; i<reps[r].size(); i++) {
                    QString rol=d.fcsdatasets[reps[r].value(i)].role;
                    if (!roles.contains(rol)) roles.append(rol);
                }
                for (int i=0; i<roles.size(); i++) {
                    QMap<QString, QVariant> p=paramValues;
                    QStringList pro=paramReadonly;
                    p["CONFOCOR3_ITEMS"]=listToString(reps[r], false, false);
                    p["CONFOCOR3_ROLE"]=roles[i];
                    p["CONFOCOR3_GROUP"]=groups.value(r, "");
                    pro<<"CONFOCOR3_ITEMS"<<"CONFOCOR3_ROLE"<<"CONFOCOR3_GROUP";
                    QFRawDataRecord* e=project->addRawData(getID(), tr("%1 (Pos=%3, Kin=%4) - %2").arg(name).arg(roles[i]).arg(positions.value(r, 0)).arg(kinetics.value(r, 0)), filename, p, pro);
                    e->setRole(roles[i]);
                    e->setGroup(project->addOrFindRDRGroup(QFileInfo(filename.value(0, "")).fileName()+"_"+groups.value(r, "")));
                    if (e->error()) {
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ConfoCor3 file '%1' (role: %3, group: %4):\n%2").arg(filename.value(0, "")).arg(e->errorDescription()).arg(roles[i]).arg(groups.value(r, "")));
                        services->log_error(tr("Error while importing ConfoCor3 file '%1' (role: %3, group: %4):\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()).arg(roles[i]).arg(groups.value(r, "")));
                        project->deleteRawData(e->getID());
                    }
                }
            }
        }
    }
}

void QFPRDRFCS::insertFLEX_SINFile(const QStringList &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly)
{
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    bool crossCorrelation=false;
    bool autoCorrelation=false;
    try {
        FLEX_analyze(filename.value(0, ""), mode, cc, runCount, crossCorrelation, autoCorrelation);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing CORRELATOR.COM .SIN file '%1':\n%2").arg(filename.value(0, "")).arg(E.what()));
        services->log_error(tr("Error while importing CORRELATOR.COM .SIN file '%1':\n    %2\n").arg(filename.value(0, "")).arg(E.what()));

    }
    for (unsigned int i=0; i<cc; i++) {
        QStringList roles;
        QList<int> col;
        if (autoCorrelation) {
           roles<<QString("ACF%1").arg(i);
           col<<i;
        }
        if (crossCorrelation) {
            if (i==0) roles<<QString("FCCS");
            else roles<<QString("FCCS%10").arg(i);
            if (!autoCorrelation) col<<i;
            else {
                col<<i+cc;
            }
        }

        for (int j=0; j<roles.size(); j++){
           QMap<QString, QVariant> p=paramValues;
           p["CHANNEL"]=i;
           p["FLEX_CF_COLUMN"]=col[j];
           QStringList rop=paramReadonly;
           rop<<"FLEX_CF_COLUMN";
           QFRawDataRecord* e=project->addRawData(getID(), tr("%1 - %2").arg(QFileInfo(filename.value(0, "")).fileName()).arg(roles[j]), filename, p, rop);
           e->setRole(roles[j]);
           if (cc>1) e->setGroup(project->addOrFindRDRGroup(QFileInfo(filename.value(0, "")).fileName()));

           if (e->error()) {
               QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing CORRELATOR.COM .SIN file '%1' (channel %3/%4):\n%2").arg(filename.value(0, "")).arg(e->errorDescription()).arg(i+1).arg(cc));
               services->log_error(tr("Error while importing CORRELATOR.COM .SIN file '%1':\n    %2\n").arg(filename.value(0, "")).arg(e->errorDescription()));
               project->deleteRawData(e->getID());
           }
       }
    }
}

const QString QFPRDRFCS_alvf=QObject::tr("ALV-5000 file (*.asc)");
const QString QFPRDRFCS_alvf6=QObject::tr("ALV-6000 file (*.asc)");
const QString QFPRDRFCS_alvf7=QObject::tr("ALV-7000 file (*.asc)");
const QString QFPRDRFCS_flexf=QObject::tr("correlator.com files (*.sin)");
const QString QFPRDRFCS_zeisscf3=QObject::tr("Zeiss Confocor3 files (*.fcs)");
const QString QFPRDRFCS_asciif=QObject::tr("ASCII Data Files (*.txt *.dat *.csv)");
const QString QFPRDRFCS_albaf=QObject::tr("ISS Alba Files (*.csv)");
const QString QFPRDRFCS_diff4f=QObject::tr("diffusion4 correlation (*corr.dat)");
const QString QFPRDRFCS_olegf=QObject::tr("Oleg Kriechevsky's Binary format(*. *.dat)");

QStringList QFPRDRFCS::getFCSFilters() const
{

    QStringList sl;
    sl<<QFPRDRFCS_alvf<<QFPRDRFCS_alvf6<<QFPRDRFCS_alvf7<<QFPRDRFCS_flexf<<QFPRDRFCS_zeisscf3<<QFPRDRFCS_asciif<<QFPRDRFCS_albaf<<QFPRDRFCS_diff4f<<QFPRDRFCS_olegf;
    return sl;
}

void QFPRDRFCS::setFCSFilterProperties(QMap<QString, QVariant> &p, const QString &currentFCSFileFormatFilter, const QString &filename)
{
    if (currentFCSFileFormatFilter==QFPRDRFCS_alvf) {
        p["FILETYPE"]="ALV5000";
        p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_alvf6){
       p["FILETYPE"]="ALV6000";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_alvf7){
       p["FILETYPE"]="ALV7000";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_flexf){
       p["FILETYPE"]="CORRELATOR.COM_SIN";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_zeisscf3){
       p["FILETYPE"]="CONFOCOR3";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_olegf){
       p["FILETYPE"]="OLEGKRIECHEVSKYBINARY";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_asciif) {
        p["FILETYPE"]="CSV_CORR";
        p["CSV_SEPARATOR"]=QString(",");
        p["CSV_COMMENT"]=QString("#");
        p["CSV_STARTSWITH"]=QString("");
        p["CSV_ENDSWITH"]=QString("");
        p["CSV_TIMEFACTOR"]=1.0;
        p["CSV_FIRSTLINE"]=1;
        p["CSV_MODE"]=0;

        dlgCSVParameters* csvDlg=new dlgCSVParameters(parentWidget, settings->getQSettings()->value("fcs/csv_mode", 0).toInt(),
                                                      settings->getQSettings()->value("fcs/csv_startswith", "").toString(),
                                                      settings->getQSettings()->value("fcs/csv_endswith", "").toString(),
                                                      settings->getQSettings()->value("fcs/csv_separator", ",").toString(),
                                                      settings->getQSettings()->value("fcs/csv_comment", "#").toString(),
                                                      settings->getQSettings()->value("fcs/csv_timefactor", 1.0).toDouble(),
                                                      settings->getQSettings()->value("fcs/csv_firstline", 1).toInt());
        loadWidgetGeometry(*settings->getQSettings(), csvDlg, QPoint(50,50), csvDlg->size(), QString("fcs/csv_dialog."));
        if (!filename.isEmpty() && QFile::exists(filename)) csvDlg->setFileContents(filename);
        if (csvDlg->exec()==QDialog::Accepted) {
            p["CSV_SEPARATOR"]=QString(csvDlg->get_column_separator());
            p["CSV_COMMENT"]=QString(csvDlg->get_comment_start());
            p["CSV_STARTSWITH"]=csvDlg->get_startswith();
            p["CSV_ENDSWITH"]=csvDlg->get_endswith();
            p["CSV_TIMEFACTOR"]=csvDlg->get_timefactor();
            p["CSV_FIRSTLINE"]=csvDlg->get_firstLine();
            p["CSV_MODE"]=csvDlg->get_mode();
            settings->getQSettings()->setValue("fcs/csv_separator", QString(csvDlg->get_column_separator()));
            settings->getQSettings()->setValue("fcs/csv_comment", QString(csvDlg->get_comment_start()));
            settings->getQSettings()->setValue("fcs/csv_startswith", QString(csvDlg->get_startswith()));
            settings->getQSettings()->setValue("fcs/csv_endswith", QString(csvDlg->get_endswith()));
            settings->getQSettings()->setValue("fcs/csv_timefactor", csvDlg->get_timefactor());
            settings->getQSettings()->setValue("fcs/csv_firstline", csvDlg->get_firstLine());
            settings->getQSettings()->setValue("fcs/csv_mode", csvDlg->get_mode());
            saveWidgetGeometry(*settings->getQSettings(), csvDlg, "fcs/csv_dialog.");
        } else {
            services->setProgress(0);
            return;
        }
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_albaf) {
        p["FILETYPE"]="ISS_ALBA";
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_diff4f) {
        p["FILETYPE"]="DIFFUSION4_SIMRESULTS";
        p["CSV_SEPARATOR"]=",";
        p["CSV_COMMENT"]="#";
    }
}

void QFPRDRFCS::loadFCSFilterFiles(const QStringList &files, const QString &currentFCSFileFormatFilter, const QMap<QString, QVariant> &p, const QStringList &paramsReadonly)
{
    if (currentFCSFileFormatFilter==QFPRDRFCS_alvf) {
        insertALV5000File(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_alvf6) {
        insertALV6000File(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_alvf7) {
        insertALV7000File(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_olegf) {
        insertOlegFile(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_albaf) {
        insertALBAFile(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_diff4f) {
        insertDiffusion4File(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_flexf) {
        insertFLEX_SINFile(files, p, paramsReadonly);
    } else if (currentFCSFileFormatFilter==QFPRDRFCS_zeisscf3) {
        insertConfocor3File(files, p, paramsReadonly);
    } else {
        insertCSVFile(files, p, paramsReadonly);
    }
}



void QFPRDRFCS::insertFCS() {
    if (project) {

        QString currentFCSFileFormatFilter=settings->getQSettings()->value("fcs/current_fcs_format_filter", getFCSFilters().value(0)).toString();
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select FCS Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              getFCSFilters().join(";;"), &currentFCSFileFormatFilter);
        if (files.size()>0) {
            settings->getQSettings()->setValue("fcs/current_fcs_format_filter", currentFCSFileFormatFilter);
            QMap<QString, QVariant> p;
            setFCSFilterProperties(p, currentFCSFileFormatFilter, files.value(0));


            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE"<<"CHANNEL"<<"CSV_SEPARATOR"<<"CSV_COMMENT"<<"CSV_STARTSWITH"<<"CSV_MODE"<<"CSV_FIRSTLINE"<<"CSV_ENDSWITH"<<"CSV_TIMEFACTOR";
            QStringList list = files;
            QStringList::Iterator it = list.begin();
            services->setProgressRange(0, list.size());
            services->setProgress(0);
            int i=0;
            while(it != list.end()) {
                i++;
                if (QFile::exists(*it)) {
                    //std::cout<<"loading "<<(*it).toStdString()<<std::endl;
                    services->log_text(tr("loading [%2] '%1' ...\n").arg(*it).arg(currentFCSFileFormatFilter));
                    loadFCSFilterFiles(QStringList(*it), currentFCSFileFormatFilter, p, paramsReadonly);

                    //std::cout<<"loading "<<(*it).toStdString()<<" ... done!\n";
                    settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                    //std::cout<<"loading "<<(*it).toStdString()<<" ... done ... done!\n";
                    ++it;
                }
                services->setProgress(i);
                QApplication::processEvents(QEventLoop::AllEvents, 50);
            }
            services->setProgress(0);
            //std::cout<<"loading done ...\n";
            //tvMain->expandToDepth(2);
        }

    }
}

void QFPRDRFCS::insertMultiFileFCS()
{
    if (project) {

        QString currentFCSFileFormatFilter=settings->getQSettings()->value("fcs/current_fcs_format_filter", getFCSFilters().value(0)).toString();
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select FCS Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              getFCSFilters().join(";;"), &currentFCSFileFormatFilter);
        //std::cout<<"filter: "<<currentFCSFileFormatFilter.toStdString()<<std::endl;
        if (files.size()>0) {
            settings->getQSettings()->setValue("fcs/current_fcs_format_filter", currentFCSFileFormatFilter);
            QMap<QString, QVariant> p;
            setFCSFilterProperties(p, currentFCSFileFormatFilter, files.value(0));

            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE"<<"CHANNEL"<<"CSV_SEPARATOR"<<"CSV_COMMENT"<<"CSV_STARTSWITH"<<"CSV_MODE"<<"CSV_FIRSTLINE"<<"CSV_ENDSWITH"<<"CSV_TIMEFACTOR";
            services->log_text(tr("loading %1 [%2] files ...\n").arg(files.size()).arg(currentFCSFileFormatFilter));

            loadFCSFilterFiles(files, currentFCSFileFormatFilter, p, paramsReadonly);

            //std::cout<<"loading "<<(*it).toStdString()<<" ... done!\n";
            settings->setCurrentRawDataDir(QFileInfo(files.value(0, "")).absolutePath());
            services->setProgress(0);
            //std::cout<<"loading done ...\n";
            //tvMain->expandToDepth(2);
        }

    }
}

void QFPRDRFCS::openSimulator()
{
    if (!dlgSimulator) {
        dlgSimulator=new QFRDRFCSFitFunctionSimulator(services, NULL);
        connect(dlgSimulator, SIGNAL(accepted()), this, SLOT(insertSimulated()));
        connect(dlgSimulator, SIGNAL(insertRecord()), this, SLOT(insertSimulated()));
    }
    dlgSimulator->show();
    dlgSimulator->raise();

}

void QFPRDRFCS::insertSimulated() {
    if (dlgSimulator) {
        QString CSV=dlgSimulator->getCSV();

        QMap<QString, QVariant> p_sim=dlgSimulator->getParams();
        QMap<QString, QVariant> p;
        p["FILETYPE"]="INTERNAL";
        p["INTERNAL_CSV"]=CSV;
        p["INTERNAL_CSVMODE"]="tccc";
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE"<<"INTERNAL_CSV"<<"INTERNAL_CSVMODE";
        QString title=tr("Simulated FCS/DLS Model: %1").arg(p_sim["model_function"].toString());

        bool ok=true;
        title=QInputDialog::getText(NULL, tr("FCS/DLS simulator"), tr("enter a name for the new record ..."), QLineEdit::Normal, title, &ok);

        if (ok) {
        QMapIterator<QString, QVariant> it(p_sim);
            while (it.hasNext()) {
                it.next();
                p[it.key().toUpper()]=it.value();
                paramsReadonly<<it.key().toUpper();
            }

            QFRawDataRecord* e=project->addRawData(getID(), title, QStringList(), p, paramsReadonly);
            if (e->error()) {
                QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing simulated FCS/DLS curve:\n%1").arg(e->errorDescription()));
                services->log_error(tr("Error while importing simulated FCS/DLS curve:\n    %1\n").arg(e->errorDescription()));
                project->deleteRawData(e->getID());
            }
            services->setProgress(0);
        }
    }
}

struct FCSProjectWizardEvalSettingsData {
    QFFormWizardPage* evalprops;
    QFComboBoxWizardPage* meastype;
    QFCheckboxListWizardPage* evals;
    QFFitFunctionComboBox* cmbFitFunc;
    QDoubleSpinBox* spinWxy;
    QDoubleSpinBox* spinWz;
    QDoubleSpinBox* spinGamma;
    QDoubleSpinBox* spinA;
    QDoubleSpinBox* spinDLSAngle;
    QDoubleSpinBox* spinDLSN;
    QDoubleSpinBox* spinDLSLambda;

};


void QFPRDRFCS::startFCSProjectWizard()
{
    QFWizard* wiz=new QFWizard(parentWidget);
    wiz->setWindowTitle(tr("FCS Project Wizard"));
    wiz->addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to load a set of FCS or DLS correlation curves from files and select the appropriate mode of data evaluation. It will finally set up a complete QuickFit 3.0 project with all the data you entered."),
                                      wiz));
    QFSelectFilesWizardPage* selfiles;
    wiz->addPage(selfiles=new QFSelectFilesWizardPage(tr("FCS/DLS data files ...")));
    selfiles->setSubTitle(tr("Select one or more FCS/DLS data files, that you want to add to the project. You can click on '+' to add files, '-' to remove files and use the arrow buttons to change the order of the files in the list."));
    selfiles->setFilters(getFCSFilters());
    selfiles->setSettingsIDs("fcs/last_fcswizard_dir", "fcs/current_fcs_format_filter");
    selfiles->setAddOnStartup(false);


    QFComboBoxWizardPage* meastype;
    wiz->addPage(meastype=new QFComboBoxWizardPage(tr("Instrument used for measurmeent")));
    meastype->setSubTitle(tr("Select the type of instrument, you used for the data acquisition."));
    QStringList measurementTypes;
    measurementTypes<<tr("confocal FCS (single-spot)");
    measurementTypes<<tr("TIR FCS (single-spot)");
    measurementTypes<<tr("dynamic light scattering (DLS)");
    measurementTypes<<tr("SPIM-FCS (camera)");
    measurementTypes<<tr("TIR-FCS (camera)");
    meastype->setItems(measurementTypes);
    meastype->setLabel(tr("instrument type:"));

    QFCheckboxListWizardPage* evals;
    wiz->addPage(evals=new QFCheckboxListWizardPage(tr("Evaluations")));
    evals->setUserPreviousPage(meastype);
    evals->setEnableable(true);
    evals->setEnableCheckbox(tr("also add evaluation objects?"), true);
    evals->addItem("FCS/DLS fit");
    evals->addItem("FCS/DLS Maximum Entropy (MaxEnt) Distribution analysis");
    evals->addItem("FCS Mean Squared Displacement (MSD) evaluation");
    evals->setChecked(0,true);


    FCSProjectWizardEvalSettingsData evsetData;
    QFFormWizardPage* evalprops;
    wiz->addPage(evalprops=new QFFormWizardPage(tr("Evaluation Properties")));
    evsetData.evalprops=evalprops;
    evsetData.meastype=meastype;
    evsetData.evals=evals;
    evsetData.cmbFitFunc=new QFFitFunctionComboBox(evalprops);
    evalprops->addRow("<i>fit function</i> ", evsetData.cmbFitFunc);
    evsetData.spinWxy=new QDoubleSpinBox(evalprops);
    evsetData.spinWxy->setSuffix(" nm");
    evsetData.spinWxy->setRange(0,100000);
    evsetData.spinWxy->setValue(250);
    evsetData.spinWxy->setDecimals(1);
    evalprops->addRow("<i>1/e<sup>2</sup> focus width w<sub>xy</sub></i> = ", evsetData.spinWxy);
    evsetData.spinWz=new QDoubleSpinBox(evalprops);
    evsetData.spinWz->setSuffix(" nm");
    evsetData.spinWz->setRange(0,100000);
    evsetData.spinWz->setValue(1200);
    evsetData.spinWz->setDecimals(1);
    evalprops->addRow("<i>1/e<sup>2</sup> focus height w<sub>z</sub></i> = ", evsetData.spinWz);
    evsetData.spinGamma=new QDoubleSpinBox(evalprops);
    evsetData.spinGamma->setSuffix("");
    evsetData.spinGamma->setRange(0.01,100000);
    evsetData.spinGamma->setValue(8);
    evsetData.spinGamma->setDecimals(2);
    evalprops->addRow("<i>axial ratio &gamma;</i> = ", evsetData.spinGamma);
    evsetData.spinA=new QDoubleSpinBox(evalprops);
    evsetData.spinA->setSuffix(" nm");
    evsetData.spinA->setRange(0,100000);
    evsetData.spinA->setValue(400);
    evsetData.spinA->setDecimals(1);
    evalprops->addRow("<i>pixel size a = ", evsetData.spinA);
    evsetData.spinDLSAngle=new QDoubleSpinBox(evalprops);
    evsetData.spinDLSAngle->setSuffix(" °");
    evsetData.spinDLSAngle->setRange(0,100000);
    evsetData.spinDLSAngle->setValue(90);
    evsetData.spinDLSAngle->setDecimals(1);
    evalprops->addRow("<i>scattering angle = ", evsetData.spinDLSAngle);
    evsetData.spinDLSN=new QDoubleSpinBox(evalprops);
    evsetData.spinDLSN->setSuffix("");
    evsetData.spinDLSN->setRange(0,100000);
    evsetData.spinDLSN->setValue(1.33);
    evsetData.spinDLSN->setDecimals(4);
    evalprops->addRow("<i>refractive index = ", evsetData.spinDLSN);
    evsetData.spinDLSLambda=new QDoubleSpinBox(evalprops);
    evsetData.spinDLSLambda->setSuffix(" nm");
    evsetData.spinDLSLambda->setRange(0,100000);
    evsetData.spinDLSLambda->setValue(488);
    evsetData.spinDLSLambda->setDecimals(1);
    evalprops->addRow("<i>wavelength = ", evsetData.spinDLSLambda);


    QFTextWizardPage* last;
    wiz->addPage(last=new QFTextWizardPage(tr("Finalize"),
                                           tr("You completed this wizard and all selected files, as well as all selected evaluations will be added to the project.\n\nThe data files will be added as \"raw data records (RDR)\" in the upper partof the project and the evaluations as additional items below that. By double-clicking on any project item, you can open a new window, which displays the data, or allows you to perform the desired evaluation."),
                                      wiz));
    last->setFinalPage(true);
    evals->setNextPageIfAllDisabled(last);
    evals->setNextPageIfDisabled(last);


    meastype->setUserOnValidateArgument(&evsetData);
    connect(meastype, SIGNAL(onValidateA(QWizardPage*,void*)), this, SLOT(FCSProjectWizardValidateIntrument(QWizardPage*,void*)));

    if (wiz->exec()) {

        QStringList files=selfiles->files();
        QStringList filters=selfiles->fileFilters();

        //qDebug()<<"OK"<<files<<filters;
        if (files.size()>0) {


            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE"<<"CHANNEL"<<"CSV_SEPARATOR"<<"CSV_COMMENT"<<"CSV_STARTSWITH"<<"CSV_MODE"<<"CSV_FIRSTLINE"<<"CSV_ENDSWITH"<<"CSV_TIMEFACTOR";
            QStringList list = files;
            QStringList::Iterator it = list.begin();
            services->setProgressRange(0, list.size());
            services->setProgress(0);
            int i=0;
            QMap<QString, QMap<QString, QVariant> > ps;

            while(it != list.end()) {
                if (QFile::exists(*it)) {
                    QMap<QString, QVariant> p;
                    if (ps.contains( filters.value(i))) p=ps[filters.value(i)];
                    else setFCSFilterProperties(p, filters.value(i), files.value(i));
                    ps[filters.value(i)]=p;
                    //std::cout<<"loading "<<(*it).toStdString()<<std::endl;
                    services->log_text(tr("loading [%2] '%1' ...\n").arg(*it).arg(filters.value(i)));
                    loadFCSFilterFiles(QStringList(*it), filters.value(i), p, paramsReadonly);

                    //std::cout<<"loading "<<(*it).toStdString()<<" ... done!\n";
                    settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                    //std::cout<<"loading "<<(*it).toStdString()<<" ... done ... done!\n";
                }
                ++it;
                i++;
                services->setProgress(i);
                QApplication::processEvents(QEventLoop::AllEvents, 50);
            }
            services->setProgress(0);
            //std::cout<<"loading done ...\n";
            //tvMain->expandToDepth(2);
        }
        for (int i=0; i<3; i++) {
            if (evals->getChecked(i)) {
                QFEvaluationItem* ev=NULL;
                if (i==0) ev=project->addEvaluation("fcs_fit", "FCS Fit");
                else if (i==1) ev=project->addEvaluation("fcs_maxent", "FCS MaxEnt Evaluation");
                else if (i==2 && meastype->currentItem()!=2) ev=project->addEvaluation("fcs_msd", "FCS MSD Evaluation");
                if (ev) {
                    //if (i==0) ev->setQFProperty("PRESET_FIT_MODEL", evsetData.cmbFitFunc->currentFitFunctionID());
                    if (i==0) ev->setPresetProperty("PRESET_FIT_MODEL", evsetData.cmbFitFunc->currentFitFunctionID());
                    else if (i==1) {
                        ev->setPresetProperty("PRESET_MODEL", 0);
                        if (meastype->currentItem()==1) ev->setPresetProperty("PRESET_MODEL", 2);
                        else if (meastype->currentItem()==2) ev->setPresetProperty("PRESET_MODEL", 3);
                        else if (meastype->currentItem()==3) ev->setPresetProperty("PRESET_MODEL", 5);
                        else if (meastype->currentItem()==4) ev->setPresetProperty("PRESET_MODEL", 6);
                    } else if (i==2) {
                        ev->setPresetProperty("PRESET_MODEL", 1);
                        if (meastype->currentItem()==1) ev->setPresetProperty("PRESET_MODEL", 0);
                        else if (meastype->currentItem()==3) ev->setPresetProperty("PRESET_MODEL", 4);
                        else if (meastype->currentItem()==4) ev->setPresetProperty("PRESET_MODEL", 5);
                    }
                    if (meastype->currentItem()==2) {
                        if (i==0) ev->setName("DLS Fit");
                        else if (i==1) ev->setName("DLS MaxEnt Fit");
//                        ev->setQFProperty("PRESET_DLS_ANGLE", evsetData.spinDLSAngle->value());
//                        ev->setQFProperty("PRESET_REFRACTIVE_INDEX", evsetData.spinDLSN->value());
//                        ev->setQFProperty("PRESET_WAVELENGTH", evsetData.spinDLSLambda->value());
                        ev->setPresetProperty("preset_dls_angle", evsetData.spinDLSAngle->value());
                        ev->setPresetProperty("preset_refractive_index", evsetData.spinDLSN->value());
                        ev->setPresetProperty("preset_wavelength", evsetData.spinDLSLambda->value());
                    } else if (meastype->currentItem()==1) {
//                        ev->setQFProperty("PRESET_FOCUS_WIDTH", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_focus_width", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_maxent_wxy", evsetData.spinWxy->value());
                    } else if (meastype->currentItem()==3) {
//                        ev->setQFProperty("PRESET_FOCUS_WIDTH", evsetData.spinWxy->value());
//                        ev->setQFProperty("PRESET_PIXEL_WIDTH", evsetData.spinA->value());
//                        ev->setQFProperty("PRESET_FOCUS_HEIGHT", evsetData.spinWz->value());
                        ev->setPresetProperty("preset_focus_width", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_pixel_width", evsetData.spinA->value());
                        ev->setPresetProperty("preset_focus_height", evsetData.spinWz->value());
                        ev->setPresetProperty("preset_focus_hieght", evsetData.spinWz->value());
                        ev->setPresetProperty("preset_maxent_wxy", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_pixel_size", evsetData.spinA->value());
                    } else if (meastype->currentItem()==4) {
//                        ev->setQFProperty("PRESET_FOCUS_WIDTH", evsetData.spinWxy->value());
//                        ev->setQFProperty("PRESET_PIXEL_WIDTH", evsetData.spinA->value());
                        ev->setPresetProperty("preset_focus_width", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_pixel_width", evsetData.spinA->value());
                        ev->setPresetProperty("preset_pixel_size", evsetData.spinA->value());
                        ev->setPresetProperty("preset_maxent_wxy", evsetData.spinWxy->value());
                    } else {
//                        ev->setQFProperty("PRESET_FOCUS_STRUCT_FAC", evsetData.spinGamma->value());
//                        ev->setQFProperty("PRESET_FOCUS_WIDTH", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_focus_struct_fac", evsetData.spinGamma->value());
                        ev->setPresetProperty("preset_focus_width", evsetData.spinWxy->value());
                        ev->setPresetProperty("preset_maxent_wxy", evsetData.spinWxy->value());
                    }
                }
            }
        }
    }
    delete wiz;
}

void QFPRDRFCS::FCSProjectWizardValidateIntrument(QWizardPage *page, void *fitprops)
{
    //qDebug()<<"FCSProjectWizardValidateIntrument"<<page<<fitprops;
    FCSProjectWizardEvalSettingsData* fp=(FCSProjectWizardEvalSettingsData*)fitprops;
    if (fp) {
        switch (fp->meastype->currentItem()) {
            default:
            case 0: // confocal
                fp->evalprops->setWidgetVisible(fp->cmbFitFunc, fp->evals->getChecked(0));
                fp->cmbFitFunc->updateFitFunctions("fcs_");
                fp->cmbFitFunc->setCurrentFitFunction("fcs_diff");
                fp->evalprops->setWidgetVisible(fp->spinWxy, true);
                fp->spinWxy->setValue(250);
                fp->evalprops->setWidgetVisible(fp->spinWz, false);
                fp->evalprops->setWidgetVisible(fp->spinGamma, true);
                fp->evalprops->setWidgetVisible(fp->spinA, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSAngle, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSN, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSLambda, false);
                break;


            case 1: // TIR
                fp->evalprops->setWidgetVisible(fp->cmbFitFunc, fp->evals->getChecked(0));
                fp->cmbFitFunc->updateFitFunctions("fcs_");
                fp->cmbFitFunc->setCurrentFitFunction("fcs_tir_diffe2_diffbright");
                fp->spinWxy->setValue(600);
                fp->spinA->setValue(400);
                fp->evalprops->setWidgetVisible(fp->spinWxy, true);
                fp->evalprops->setWidgetVisible(fp->spinWz, false);
                fp->evalprops->setWidgetVisible(fp->spinGamma, false);
                fp->evalprops->setWidgetVisible(fp->spinA, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSAngle, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSN, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSLambda, false);
                break;

            case 2: // DLS
                fp->evalprops->setWidgetVisible(fp->cmbFitFunc, fp->evals->getChecked(0));
                fp->cmbFitFunc->updateFitFunctions("dls_");
                fp->cmbFitFunc->setCurrentFitFunction("dls_g2");
                fp->evalprops->setWidgetVisible(fp->spinWxy, false);
                fp->evalprops->setWidgetVisible(fp->spinWz, false);
                fp->evalprops->setWidgetVisible(fp->spinGamma, false);
                fp->evalprops->setWidgetVisible(fp->spinA, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSAngle, true);
                fp->evalprops->setWidgetVisible(fp->spinDLSN, true);
                fp->evalprops->setWidgetVisible(fp->spinDLSLambda, true);
                break;

            case 3: // SPIM
                fp->evalprops->setWidgetVisible(fp->cmbFitFunc, fp->evals->getChecked(0));
                fp->cmbFitFunc->updateFitFunctions("fcs_");
                fp->cmbFitFunc->setCurrentFitFunction("fcs_spim_diffe2_newveff");
                fp->spinWxy->setValue(600);
                fp->spinWz->setValue(1200);
                fp->spinA->setValue(400);
                fp->evalprops->setWidgetVisible(fp->spinWxy, true);
                fp->evalprops->setWidgetVisible(fp->spinWz, true);
                fp->evalprops->setWidgetVisible(fp->spinGamma, false);
                fp->evalprops->setWidgetVisible(fp->spinA, true);
                fp->evalprops->setWidgetVisible(fp->spinDLSAngle, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSN, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSLambda, false);
                break;

            case 4: // TIR
                fp->evalprops->setWidgetVisible(fp->cmbFitFunc, fp->evals->getChecked(0));
                fp->cmbFitFunc->updateFitFunctions("fcs_");
                fp->cmbFitFunc->setCurrentFitFunction("fcs_tir_diffe2_diffbright");
                fp->spinWxy->setValue(600);
                fp->spinA->setValue(400);
                fp->evalprops->setWidgetVisible(fp->spinWxy, true);
                fp->evalprops->setWidgetVisible(fp->spinWz, false);
                fp->evalprops->setWidgetVisible(fp->spinGamma, false);
                fp->evalprops->setWidgetVisible(fp->spinA, true);
                fp->evalprops->setWidgetVisible(fp->spinDLSAngle, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSN, false);
                fp->evalprops->setWidgetVisible(fp->spinDLSLambda, false);
                break;

        }
    }
}



Q_EXPORT_PLUGIN2(qfrdrfcs, QFPRDRFCS)

