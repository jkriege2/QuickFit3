/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfrdrphotoncounts.h"
#include "qfrdrphotoncounts_data.h"
#include "qfrdrphotoncounts_dataeditor.h"
#include "qftools.h"

QFRDRPhotonCountsPlugin::QFRDRPhotonCountsPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
}

QFRDRPhotonCountsPlugin::~QFRDRPhotonCountsPlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRPhotonCountsPlugin::createRecord(QFProject* parent) {
	// factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRPhotonCountsData)
    return new QFRDRPhotonCountsData(parent);
}


void QFRDRPhotonCountsPlugin::registerToMenu(QMenu* menu) {
	// create menu entries to insert data with this type
    QAction* action=new QAction(QIcon(getIconFilename()), tr("insert photon counts"), parentWidget);
    action->setStatusTip(tr("Insert a new record containing multi-channel photon counts"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRecord()));
    menu->addAction(action);
}


void QFRDRPhotonCountsPlugin::insertRecord() {
    if (project) {
        // file format to import
        QString format_name_csv=tr("Photon Counts CSV files (*.dat *.txt *.csv)");
        QString format_name_bin=tr("Photon Counts binary files (*.dat)");
        QStringList format_names, format_names_short;
        format_names<<format_name_csv<<format_name_bin;
        format_names_short<<tr("CSV File")<<tr("binary files");
        format_names_short<<tr("unknown file format");
        // look into INI which was the last used format 
        QString current_format_name=settings->getQSettings()->value("photoncounts/current_format_filter", format_name_csv).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Photon Count Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                                               format_names.join(";;"), &current_format_name);
        // store the format we just used
		settings->getQSettings()->setValue("photoncounts/current_format_filter", current_format_name); 
		// here we store some initial parameters
        QMap<QString, QVariant> initParams;
		// set whatever you want (FILETYPE is just an example)!
        initParams["FILETYPE"]="NONE";
        int formatID=format_names.indexOf(current_format_name);
        if (current_format_name==format_name_csv) {
            initParams["FILETYPE"]="CSV";
        } else if (current_format_name==format_name_bin) {
            initParams["FILETYPE"]="BINARY";
        }

		// add all properties in initParams that will be readonly
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE";
		
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
                services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(format_names_short.value(formatID, "")));
                if (initParams.value("FILETYPE", "NONE").toString()!="NONE") {
					// insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
					QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly);
					if (e->error()) { // when an error occured: remove record and output an error message
                        QMessageBox::critical(parentWidget, tr("QuickFit"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
						services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        project->deleteRawData(e->getID());
                    }
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



Q_EXPORT_PLUGIN2(photoncounts, QFRDRPhotonCountsPlugin)
