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
    for (unsigned int i=0; i<qMax(2, backgrounds.size()); i++) {
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


void QFPRDRFCS::insertALV7000File(const QStringList& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    int inputchannels=2;
    bool crossCorrelation=false;
    try {
        ALV7_analyze(filename.value(0, ""), mode, cc, runCount, crossCorrelation, inputchannels);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV700X file '%1':\n%2").arg(filename.value(0, "")).arg(E.what()));
        services->log_error(tr("Error while importing ALV700X file '%1':\n    %2\n").arg(filename.value(0, "")).arg(E.what()));

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

void QFPRDRFCS::insertFCS() {
    if (project) {
        QString alvf=tr("ALV-5000 file (*.asc)");
        QString alvf6=tr("ALV-6000 file (*.asc)");
        QString alvf7=tr("ALV-7000 file (*.asc)");
        QString flexf=tr("correlator.com files (*.sin)");
        QString zeisscf3=tr("Zeiss Confocor3 files (*.fcs)");
        QString asciif=tr("ASCII Data Files (*.txt *.dat *.csv)");
        QString albaf=tr("ISS Alba Files (*.csv)");
        QString diff4f=tr("diffusion4 correlation (*corr.dat)");
        QString olegf=tr("Oleg Kriechevsky's Binary format(*. *.dat)");
        QString currentFCSFileFormatFilter=settings->getQSettings()->value("fcs/current_fcs_format_filter", alvf).toString();
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select FCS Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              alvf+";;"+alvf6+";;"+alvf7+";;"+flexf+";;"+zeisscf3+";;"+asciif+";;"+albaf+";;"+diff4f+";;"+olegf, &currentFCSFileFormatFilter);
        //std::cout<<"filter: "<<currentFCSFileFormatFilter.toStdString()<<std::endl;
        if (files.size()>0) {
            settings->getQSettings()->setValue("fcs/current_fcs_format_filter", currentFCSFileFormatFilter);
            QMap<QString, QVariant> p;
            if (currentFCSFileFormatFilter==alvf) {
                p["FILETYPE"]="ALV5000";
                p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==alvf6){
               p["FILETYPE"]="ALV6000";
               p["CHANNEL"]=0;
               //qDebug() << "test 3";
            } else if (currentFCSFileFormatFilter==alvf7){
               p["FILETYPE"]="ALV7000";
               p["CHANNEL"]=0;
               //qDebug() << "test 3";
            } else if (currentFCSFileFormatFilter==flexf){
               p["FILETYPE"]="CORRELATOR.COM_SIN";
               p["CHANNEL"]=0;
               //qDebug() << "test 3";
            } else if (currentFCSFileFormatFilter==zeisscf3){
               p["FILETYPE"]="CONFOCOR3";
               p["CHANNEL"]=0;
               //qDebug() << "test 3";
            } else if (currentFCSFileFormatFilter==olegf){
               p["FILETYPE"]="OLEGKRIECHEVSKYBINARY";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==asciif) {
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
                if (files.size()>0) csvDlg->setFileContents(files[0]);
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
            } else if (currentFCSFileFormatFilter==albaf) {
                p["FILETYPE"]="ISS_ALBA";
            } else if (currentFCSFileFormatFilter==diff4f) {
                p["FILETYPE"]="DIFFUSION4_SIMRESULTS";
                p["CSV_SEPARATOR"]=",";
                p["CSV_COMMENT"]="#";
            }
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
                    if (currentFCSFileFormatFilter==alvf) {
                        insertALV5000File(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==alvf6) {
                       insertALV6000File(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==alvf7) {
                       insertALV7000File(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==olegf) {
                       insertOlegFile(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==albaf) {
                        insertALBAFile(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==diff4f) {
                        insertDiffusion4File(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==flexf) {
                        insertFLEX_SINFile(QStringList(*it), p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==zeisscf3) {
                        insertConfocor3File(QStringList(*it), p, paramsReadonly);
                    } else {
                        insertCSVFile(QStringList(*it), p, paramsReadonly);
                    }
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
        QString alvf=tr("ALV-5000 file (*.asc)");
        QString alvf6=tr("ALV-6000 file (*.asc)");
        QString alvf7=tr("ALV-7000 file (*.asc)");
        QString flexf=tr("correlator.com files (*.sin)");
        QString zeisscf3=tr("Zeiss Confocor3 files (*.fcs)");
        QString asciif=tr("ASCII Data Files (*.txt *.dat *.csv)");
        QString albaf=tr("ISS Alba Files (*.csv)");
        QString diff4f=tr("diffusion4 correlation (*corr.dat)");
        QString olegf=tr("Oleg Kriechevsky's Binary format(*. *.dat)");
        QString currentFCSFileFormatFilter=settings->getQSettings()->value("fcs/current_fcs_format_filter", alvf).toString();
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select FCS Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              alvf+";;"+alvf6+";;"+flexf+";;"+zeisscf3+";;"+asciif+";;"+albaf+";;"+diff4f+";;"+olegf, &currentFCSFileFormatFilter);
        //std::cout<<"filter: "<<currentFCSFileFormatFilter.toStdString()<<std::endl;
        if (files.size()>0) {
            settings->getQSettings()->setValue("fcs/current_fcs_format_filter", currentFCSFileFormatFilter);
            QMap<QString, QVariant> p;
            if (currentFCSFileFormatFilter==alvf) {
                p["FILETYPE"]="ALV5000";
                p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==alvf6){
               //qDebug() << "test 1";
               p["FILETYPE"]="ALV6000";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==alvf7){
               //qDebug() << "test 1";
               p["FILETYPE"]="ALV7000";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==olegf){
               p["FILETYPE"]="OLEGKRIECHEVSKYBINARY";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==flexf){
               p["FILETYPE"]="CORRELATOR.COM_SIN";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==zeisscf3){
               p["FILETYPE"]="CONFOCOR3";
               p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==asciif) {
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
                if (files.size()>0) csvDlg->setFileContents(files[0]);
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
                    settings->getQSettings()->setValue("fcs/csv_endswith", QString(csvDlg->get_startswith()));
                    settings->getQSettings()->setValue("fcs/csv_timefactor", csvDlg->get_timefactor());
                    settings->getQSettings()->setValue("fcs/csv_firstline", csvDlg->get_firstLine());
                    settings->getQSettings()->setValue("fcs/csv_mode", csvDlg->get_mode());
                    saveWidgetGeometry(*settings->getQSettings(), csvDlg, "fcs/csv_dialog.");
                } else {
                    services->setProgress(0);
                    return;
                }
            } else if (currentFCSFileFormatFilter==albaf) {
                p["FILETYPE"]="ISS_ALBA";
            } else if (currentFCSFileFormatFilter==diff4f) {
                p["FILETYPE"]="DIFFUSION4_SIMRESULTS";
                p["CSV_SEPARATOR"]=",";
                p["CSV_COMMENT"]="#";

            }
            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE"<<"CHANNEL"<<"CSV_SEPARATOR"<<"CSV_COMMENT"<<"CSV_STARTSWITH"<<"CSV_MODE"<<"CSV_FIRSTLINE"<<"CSV_ENDSWITH"<<"CSV_TIMEFACTOR";
            services->log_text(tr("loading %1 [%2] files ...\n").arg(files.size()).arg(currentFCSFileFormatFilter));

            if (currentFCSFileFormatFilter==alvf) {
                insertALV5000File(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==alvf6) {
                insertALV6000File(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==alvf7) {
                insertALV7000File(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==olegf) {
                insertOlegFile(files, p, paramsReadonly);
                //qDebug() << "test 2";
            } else if (currentFCSFileFormatFilter==albaf) {
                insertALBAFile(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==diff4f) {
                insertDiffusion4File(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==flexf) {
                insertFLEX_SINFile(files, p, paramsReadonly);
            } else if (currentFCSFileFormatFilter==zeisscf3) {
                insertConfocor3File(files, p, paramsReadonly);
            } else {
                insertCSVFile(files, p, paramsReadonly);
            }
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

Q_EXPORT_PLUGIN2(qfrdrfcs, QFPRDRFCS)
