/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfrdrimagestack.h"
#include "qfrdrimagestack_data.h"
#include "qfrdrimagestack_dataeditor.h"

QFRDRImageStackPlugin::QFRDRImageStackPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
}

QFRDRImageStackPlugin::~QFRDRImageStackPlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRImageStackPlugin::createRecord(QFProject* parent) {
    // factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRImageStackData)
    return new QFRDRImageStackData(parent);
}


void QFRDRImageStackPlugin::registerToMenu(QMenu* menuMain) {
    // create menu entries to insert data with this type
    QMenu* menu=menuMain->addMenu(QIcon(getIconFilename()), tr("image &stacks"));
    QAction* action=new QAction(QIcon(":/image_stack/singlefile_stack.png"), tr("single-file image stack(s)"), parentWidget);
    action->setStatusTip(tr("Insert each image file as image stack with one channel"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertSingleFileImageStack()));
    menu->addAction(action);
    action=new QAction(QIcon(":/image_stack/singlefile_dvh_stack.png"), tr("single-file, horizontal DualView image stack"), parentWidget);
    action->setStatusTip(tr("Insert each image file as a dual-channel image-stack, where each frame is split horozontally (left=channel 0, right=channel 1)"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertSingleFileHDualViewImageStack()));
    menu->addAction(action);
    action=new QAction(QIcon(":/image_stack/singlefile_dvv_stack.png"), tr("single-file, vertical DualView image stack"), parentWidget);
    action->setStatusTip(tr("Insert each image file as a dual-channel image-stack, where each frame is split vertically (top=channel 0, bottom=channel 1)"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertSingleFileVDualViewImageStack()));
    menu->addAction(action);
    menu->addSeparator();
    action=new QAction(QIcon(":/image_stack/multifile_stack.png"), tr("multi-file image stack"), parentWidget);
    action->setStatusTip(tr("Insert several image files as channels into a single image stack"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertMultiFileImageStack()));
    menu->addAction(action);
}


void QFRDRImageStackPlugin::insertSingleFileImageStack() {
    if (project) {
        // file format to import
        QStringList format_names=QFRDRImageStackData::getImageFilterList(services);
        QStringList format_descriptions=QFRDRImageStackData::getImageFormatNameList(services);
        QStringList reader_id=QFRDRImageStackData::getImageReaderIDList(services);
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("image_stack/current_format_filter", format_names.value(0,"")).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Image File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              format_names.join(";;"), &current_format_name);
        // store the format we just used
        settings->getQSettings()->setValue("image_stack/current_format_filter", current_format_name);
        // here we store some initial parameters
        QMap<QString, QVariant> initParams;
        // set whatever you want (FILETYPE is just an example)!
        QString format_id=reader_id.value(format_names.indexOf(current_format_name), "");
        initParams["FILETYPE"]=format_id;
        initParams["STACKTYPE"]="ONEFILEPERSTACK";

        // add all properties in initParams that will be readonly
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE"<<"STACKTYPE";

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
                services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(format_descriptions.value(format_names.indexOf(current_format_name), "")));
                if (!format_id.isEmpty()) {
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly, QStringList("image"));
                    if (e->error()) { // when an error occured: remove record and output an error message
                        project->deleteRawData(e->getID());
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
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



void QFRDRImageStackPlugin::insertMultiFileImageStack() {
    if (project) {
        // file format to import
        QStringList format_names=QFRDRImageStackData::getImageFilterList(services);
        QStringList format_descriptions=QFRDRImageStackData::getImageFormatNameList(services);
        QStringList reader_id=QFRDRImageStackData::getImageReaderIDList(services);
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("image_stack/current_format_filter", format_names.value(0,"")).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Image File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              format_names.join(";;"), &current_format_name);
        // store the format we just used
        settings->getQSettings()->setValue("image_stack/current_format_filter", current_format_name);
        // here we store some initial parameters
        QMap<QString, QVariant> initParams;
        // set whatever you want (FILETYPE is just an example)!
        QString format_id=reader_id.value(format_names.indexOf(current_format_name), "");
        initParams["FILETYPE"]=format_id;
        initParams["STACKTYPE"]="ONEFILEPERCHANNEL";

        // add all properties in initParams that will be readonly
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE"<<"STACKTYPE";
        QStringList types;
        for (int i=0; i<files.size(); i++) {
            types.append("image");
        }

        QFRawDataRecord* e=project->addRawData(getID(), tr("new image stack"), files, initParams, paramsReadonly, types);
        if (e->error()) { // when an error occured: remove record and output an error message
            project->deleteRawData(e->getID());
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing image stack:\n%1").arg(e->errorDescription()));
            services->log_error(tr("Error while importing image stack:\n    %1\n").arg(e->errorDescription()));
        }
    }
}

void QFRDRImageStackPlugin::insertSingleFileHDualViewImageStack()
{
    insertSingleFileDualViewImageStack('h');
}

void QFRDRImageStackPlugin::insertSingleFileVDualViewImageStack()
{
    insertSingleFileDualViewImageStack('v');
}

void QFRDRImageStackPlugin::insertSingleFileDualViewImageStack(char dvMode)
{
    if (project) {
        QString imType="image";
        if (dvMode=='h') imType="image_hdualview";
        if (dvMode=='v') imType="image_vdualview";
        if (dvMode=='l') imType="image_ldualview";
        if (dvMode=='r') imType="image_rdualview";
        if (dvMode=='t') imType="image_tdualview";
        if (dvMode=='b') imType="image_bdualview";
        // file format to import
        QStringList format_names=QFRDRImageStackData::getImageFilterList(services);
        QStringList format_descriptions=QFRDRImageStackData::getImageFormatNameList(services);
        QStringList reader_id=QFRDRImageStackData::getImageReaderIDList(services);
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("image_stack/current_format_filter", format_names.value(0,"")).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Image File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              format_names.join(";;"), &current_format_name);
        // store the format we just used
        settings->getQSettings()->setValue("image_stack/current_format_filter", current_format_name);
        // here we store some initial parameters
        QMap<QString, QVariant> initParams;
        // set whatever you want (FILETYPE is just an example)!
        QString format_id=reader_id.value(format_names.indexOf(current_format_name), "");
        initParams["FILETYPE"]=format_id;
        initParams["STACKTYPE"]="ONEFILEPERSTACK";

        // add all properties in initParams that will be readonly
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE"<<"STACKTYPE";

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
                services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(format_descriptions.value(format_names.indexOf(current_format_name), "")));
                if (!format_id.isEmpty()) {
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly, QStringList(imType));
                    if (e->error()) { // when an error occured: remove record and output an error message
                        project->deleteRawData(e->getID());
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
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



Q_EXPORT_PLUGIN2(image_stack, QFRDRImageStackPlugin)
