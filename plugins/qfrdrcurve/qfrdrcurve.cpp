/*
Copyright (c) 2015
	
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


#include "qfrdrcurve.h"
#include "qfrdrcurve_data.h"
#include "qfrdrcurve_dataeditor.h"
#include "qfdlgcsvparameters.h"

QFRDRCurvePlugin::QFRDRCurvePlugin(QObject* parent):
    QObject(parent)
{
    //constructor
}

QFRDRCurvePlugin::~QFRDRCurvePlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRCurvePlugin::createRecord(QFProject* parent) {
	// factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRCurveData)
    return new QFRDRCurveData(parent);
}


void QFRDRCurvePlugin::registerToMenu(QMenu* menu) {

    // create a sub-menu
    QMenu* m=menu->addMenu(QIcon(getIconFilename()), tr("XY-Curves"));

	// create menu entries to insert data with this type
    QAction* action=new QAction(QIcon(getIconFilename()), tr("Insert single-file curves"), parentWidget);
    action->setStatusTip(tr("Insert a new record, which represents one curve from one file"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRecord()));
    m->addAction(action);

    action=new QAction(QIcon(getIconFilename()), tr("Insert multi-file curves"), parentWidget);
    action->setStatusTip(tr("Insert a new record, which represents several curves form several files in one RDR"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertMultiFileRecord()));
    m->addAction(action);
}


void QFRDRCurvePlugin::insertRecord(bool multi_file) {
    if (project) {
        // file formats to import
        QStringList fileformats;
        fileformats<<tr("CSV Files (*.dat *.txt *.csv)");


        QString current_format_name=fileformats[0] ;
        // let the user select some files to import
        QStringList files = qfGetOpenFileNamesSet("curves/", parentWidget,
                              tr("Select Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              fileformats.join(";;"), &current_format_name);

        if (files.size()>0) { // if the user selected files

            // here we store some initial parameters
            QMap<QString, QVariant> initParams;

            // add all properties in initParams that will be readonly
            QStringList paramsReadonly;
            paramsReadonly<<"FILETYPE";


            int fomratID=fileformats.indexOf(current_format_name);
            // For CSV-files we ask the user to specify the file parameters:
            if (fomratID==0) {
                QFDlgCSVParameters* csvDlg=new QFDlgCSVParameters(NULL, ProgramOptions::getConfigValue("curves/column_separator_save", ",").toString(),
                                                                  ProgramOptions::getConfigValue("curves/decimal_separator_save", ".").toString(),
                                                                  ProgramOptions::getConfigValue("curves/comment_start_save", "#").toString(),
                                                                  ProgramOptions::getConfigValue("curves/header_start_save", "#!").toString());

                csvDlg->addColSelComboBox(tr("x-data column:"));
                csvDlg->addColSelComboBox(tr("y-data column:"));
                csvDlg->addColSelComboBox(tr("y-error column:"));

                csvDlg->setFileContents(files.value(0));
                if (csvDlg->exec()==QDialog::Accepted) {
                    // set whatever you want (FILETYPE is just an example)!
                    initParams["FILETYPE"]="CSV";

                    ProgramOptions::setConfigValue("curves/column_separator_save", QString(csvDlg->get_column_separator()));
                    ProgramOptions::setConfigValue("curves/decimal_separator_save", QString(csvDlg->get_decimal_separator()));
                    ProgramOptions::setConfigValue("curves/comment_start_save", QString(csvDlg->get_comment_start()));
                    ProgramOptions::setConfigValue("curves/header_start_save", QString(csvDlg->get_header_start()));

                    // here we save the configuration as parameters in the new RDR, which we can sue later to interpret the CSV-file
                    initParams["csv_column_separator"]=QString(csvDlg->get_column_separator());
                    initParams["decimal_separator"]=QString(csvDlg->get_decimal_separator());
                    initParams["comment_start"]=QString(csvDlg->get_comment_start());
                    initParams["header_start"]=QString(csvDlg->get_header_start());
                    initParams["column_x"]=csvDlg->getColSelComboBoxCurrentIndex(0);
                    initParams["column_y"]=csvDlg->getColSelComboBoxCurrentIndex(1);
                    initParams["column_yerr"]=csvDlg->getColSelComboBoxCurrentIndex(2);

                    paramsReadonly<<"csv_column_separator"<<"decimal_separator"<<"comment_start"<<"header_start"<<"column_x"<<"column_y"<<"column_yerr";
                } else {
                    // aborted by user!
                    delete csvDlg;
                    return;
                }
                delete csvDlg;
            }




            if (multi_file) {
                QString filename=files.value(0, "");
                QStringList fileNames;
                for (int i=0; i<files.size(); i++) {
                    fileNames<<QFileInfo(files[i]).fileName();
                }
                services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(current_format_name));
                // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                QFRawDataRecord* e=project->addRawData(getID(), tr("Multifile-XY-Curves: %1").arg(fileNames.join(", ")), files, initParams, paramsReadonly);
                if (e->error()) { // when an error occured: remove record and output an error message
                    project->deleteRawData(e->getID());
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                }
                settings->setCurrentRawDataDir(QFileInfo(files.value(0, "")).dir().absolutePath());
            } else {
                // now we iterate over all files and use QuickFit's progress bar interface (see plugin services)
                QStringList list = files;
                QStringList::Iterator it = list.begin();
                services->setProgressRange(0, list.size());
                services->setProgress(0);
                int i=0;
                while(it != list.end()) {
                    i++;
                    QString filename=*it;
                    if (QFile::exists(filename)) {
                        services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(current_format_name));
                        // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly);
                        if (e->error()) { // when an error occured: remove record and output an error message
                            project->deleteRawData(e->getID());
                            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        }
                        settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                        ++it;
                    }
                    services->setProgress(i);
                    QApplication::processEvents();
                }
                services->setProgress(0);
            }
        }
    }
}



Q_EXPORT_PLUGIN2(curves, QFRDRCurvePlugin)

