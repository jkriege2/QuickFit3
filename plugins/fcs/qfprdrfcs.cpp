#include "qfprdrfcs.h"
#include <QtGui>
#include "qfrdrfcsdata.h"
#include "dlgcsvparameters.h"
#include "qfrdrfcsfitfunctionsimulator.h"
#include <QtXml>

QFPRDRFCS::QFPRDRFCS(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFPRDRFCS::~QFPRDRFCS()
{
    //dtor
}

QFRawDataRecord* QFPRDRFCS::createRecord(QFProject* parent) {
    return new QFRDRFCSData(parent);
}


void QFPRDRFCS::registerToMenu(QMenu* menu) {
    QMenu* smenu=menu->addMenu(QIcon(":/fcs_logo.png"), tr("FCS data"));
    QAction* actFCS=new QAction(QIcon(":/fcs_open.png"), tr("Open FCS Data from File"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new FCS data item from a specified file (CSV or ALV-5000 format)"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCS()));
    smenu->addAction(actFCS);
    QAction* actFCSSim=new QAction(QIcon(":/fcs_simulate.png"), tr("FCS Data from Fit Function"), parentWidget);
    connect(actFCSSim, SIGNAL(triggered()), this, SLOT(insertSimulated()));
    smenu->addAction(actFCSSim);
}


void QFPRDRFCS::insertALV5000File(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    unsigned int cc=1;
    QString mode="";
    unsigned int runCount=0;
    bool crossCorrelation=false;
    try {
        ALV_analyze(filename, mode, cc, runCount, crossCorrelation);
    } catch (std::exception& E) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV5000 file '%1':\n%2").arg(filename).arg(E.what()));
        services->log_error(tr("Error while importing ALV5000 file '%1':\n    %2\n").arg(filename).arg(E.what()));

    }
    for (unsigned int i=0; i<cc; i++) {
        QMap<QString, QVariant> p=paramValues;
        p["CHANNEL"]=i;
        QFRawDataRecord* e=project->addRawData(getID(), tr("%1 - CH%2").arg(QFileInfo(filename).fileName()).arg(i), QStringList(filename), p, paramReadonly);
        if (e->error()) {
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing ALV5000 file '%1' (channel %3/%4):\n%2").arg(filename).arg(e->errorDescription()).arg(i+1).arg(cc));
            services->log_error(tr("Error while importing ALV5000 file '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
}

void QFPRDRFCS::insertCSVFile(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}


void QFPRDRFCS::insertALBAFile(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFPRDRFCS::insertFCS() {
    if (project) {
        QString alvf=tr("ALV-5000 file (*.asc)");
        QString asciif=tr("ASCII Data Files (*.txt *.dat *.csv)");
        QString albaf=tr("ISS Alba Files (*.csv)");
        QString currentFCSFileFormatFilter=settings->getQSettings()->value("fcs/current_fcs_format_filter", alvf).toString();
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select FCS Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              alvf+";;"+asciif+";;"+albaf, &currentFCSFileFormatFilter);
        //std::cout<<"filter: "<<currentFCSFileFormatFilter.toStdString()<<std::endl;
        if (files.size()>0) {
            settings->getQSettings()->setValue("fcs/current_fcs_format_filter", currentFCSFileFormatFilter);
            QMap<QString, QVariant> p;
            if (currentFCSFileFormatFilter==alvf) {
                p["FILETYPE"]="ALV5000";
                p["CHANNEL"]=0;
            } else if (currentFCSFileFormatFilter==asciif) {
                p["FILETYPE"]="CSV_CORR";
                p["CSV_SEPARATOR"]=QString(",");
                p["CSV_COMMENT"]=QString("#");
                p["CSV_STARTSWITH"]=QString("");
                p["CSV_TIMEFACTOR"]=1.0;
                p["CSV_FIRSTLINE"]=1;

                dlgCSVParameters* csvDlg=new dlgCSVParameters(parentWidget, settings->getQSettings()->value("fcs/csv_startswith", "").toString(),
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
                    p["CSV_TIMEFACTOR"]=csvDlg->get_timefactor();
                    p["CSV_FIRSTLINE"]=csvDlg->get_firstLine();
                    settings->getQSettings()->setValue("fcs/csv_separator", QString(csvDlg->get_column_separator()));
                    settings->getQSettings()->setValue("fcs/csv_comment", QString(csvDlg->get_comment_start()));
                    settings->getQSettings()->setValue("fcs/csv_startswith", QString(csvDlg->get_startswith()));
                    settings->getQSettings()->setValue("fcs/csv_timefactor", csvDlg->get_timefactor());
                    settings->getQSettings()->setValue("fcs/csv_firstline", csvDlg->get_firstLine());
                    saveWidgetGeometry(*settings->getQSettings(), csvDlg, "fcs/csv_dialog.");
                } else {
                    services->setProgress(0);
                    return;
                }
            } else if (currentFCSFileFormatFilter==albaf) {
                p["FILETYPE"]="ISS_ALBA";
            }
            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE"<<"CHANNEL"<<"CSV_SEPARATOR"<<"CSV_COMMENT"<<"CSV_STARTSWITH"<<"CSV_TIMEFACTOR";
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
                        insertALV5000File(*it, p, paramsReadonly);
                    } else if (currentFCSFileFormatFilter==albaf) {
                        insertALBAFile(*it, p, paramsReadonly);
                    } else {
                        insertCSVFile(*it, p, paramsReadonly);
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

void QFPRDRFCS::insertSimulated() {
    QFRDRFCSFitFunctionSimulator* dlg=new QFRDRFCSFitFunctionSimulator(services, NULL);
    if (dlg->exec()) {
        QString CSV=dlg->getCSV();

        QMap<QString, QVariant> p;
        p["FILETYPE"]="INTERNAL";
        p["INTERNAL_CSV"]=CSV;
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE";
        QFRawDataRecord* e=project->addRawData(getID(), tr("Simulated FCS Model"), QStringList(), p, paramsReadonly);
        if (e->error()) {
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing simulated FCS curve:\n%1").arg(e->errorDescription()));
            services->log_error(tr("Error while importing simulated FCS curve:\n    %1\n").arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
        services->setProgress(0);
    }
}

Q_EXPORT_PLUGIN2(qfrdrfcs, QFPRDRFCS)
