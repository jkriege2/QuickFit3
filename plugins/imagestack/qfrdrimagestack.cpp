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
#include "qfwizard.h"
#include "qfselectfileslistwidget.h"
#include "qfpluginservices.h"
#include "qfevaluationitemfactory.h"
#include "qfimageplot.h"
#include "imagetools.h"


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

void QFRDRImageStackPlugin::init()
{
    if (QFPluginServices::getInstance()->getEvaluationItemFactory()->contains("spim_lightsheet_eval")) {
        services->registerWizard("project_wizards", tr("SPIM Lightsheet Analysis"), QIcon(QFPluginServices::getInstance()->getEvaluationItemFactory()->getIconFilename("spim_lightsheet_eval")), this, SLOT(startProjectWizardLightsheetAnalysis()));
        services->registerWizard("rdr_wizards", tr("Image Stacks for SPIM Lightsheet Analysis"), QIcon(QFPluginServices::getInstance()->getEvaluationItemFactory()->getIconFilename("spim_lightsheet_eval")), this, SLOT(startProjectWizardLightsheetAnalysisData()));
    }
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

        addMultiFileImageStack(files, reader_id.value(format_names.indexOf(current_format_name), ""));
    }
}

QFRawDataRecord* QFRDRImageStackPlugin::addMultiFileImageStack(const QStringList &files, const QString& format_id)
{


    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    // set whatever you want (FILETYPE is just an example)!
    initParams["FILETYPE"]=format_id;
    initParams["STACKTYPE"]="ONEFILEPERCHANNEL";

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"STACKTYPE";
    QStringList types;
    for (int i=0; i<files.size(); i++) {
        types.append("image");
    }

    QStringList filenames;
    for (int i=0; i<files.size(); i++) {
        filenames<<QFileInfo(files[i]).fileName();
    }
    QFRawDataRecord* e=project->addRawData(getID(), qfGetLargestCommonStart(filenames, tr("new image stack")), files, initParams, paramsReadonly, types);
    if (e&&e->error()) { // when an error occured: remove record and output an error message
        project->deleteRawData(e->getID());
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing image stack:\n%1").arg(e->errorDescription()));
        services->log_error(tr("Error while importing image stack:\n    %1\n").arg(e->errorDescription()));
        return NULL;
    }
    return e;
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

void QFRDRImageStackPlugin::startProjectWizardLightsheetAnalysisData()
{
    startProjectWizardLightsheetAnalysis(false);
}
















void QFRDRImageStackPlugin::startProjectWizardLightsheetAnalysis(bool insertEval)
{
    QFWizard* wiz=new QFWizard(parentWidget);
    wiz->setWindowTitle(tr("SPIM Lightsheet Analysis Project Wizard"));
    wiz->addPage(new QFTextWizardPage(tr("Introduction"),
                                      tr("This wizard will help you to perform a SPIM lightsheet analysis, i.e. it will help you to load an image stack, which contains a lightsheet scan taken with a 45° mirror and then set all properties accordingly.<br><center><img src=\":/image_stack/spim_lightsheet_scan.png\"></center>"),
                                      wiz));





    QFSelectFilesWizardPage* selfiles;
    wiz->addPage(selfiles=new QFSelectFilesWizardPage(tr("Image stack files ...")));
    selfiles->setSubTitle(tr("Select one or more image stack files, that contain your lightsheet scan. Each file will be treated as one color channel in the final image stack.<br>You can click on '+' to add files, '-' to remove files and use the arrow buttons to change the order of the files in the list."));
    selfiles->setFilters(QFRDRImageStackData::getImageFilterList(services), QFRDRImageStackData::getImageReaderIDList(services));
    selfiles->setSettingsIDs("image_stack/last_lightsheetwizard_dir", "image_stack/last_lightsheetwizard_filter");
    selfiles->setAddOnStartup(false);
    selfiles->setOnlyOneFormatAllowed(true);

    wiz->addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image stack preview ...")));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!"));
    selfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(selfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();

    if (insertEval) {
        wizLSAnalysiscmbFitDir=new QComboBox(wizLSAnalysisImgPreview);
        wizLSAnalysiscmbFitDir->addItem(tr("fit rows (lightsheet vertical)"));
        wizLSAnalysiscmbFitDir->addItem(tr("fit column (lightsheet horizontal)"));
        wizLSAnalysiscmbFitDir->setCurrentIndex(1);
    }

    wizLSAnalysisedtPixelSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizLSAnalysisedtPixelSize->setRange(0,100000);
    wizLSAnalysisedtPixelSize->setSuffix(" nm");
    wizLSAnalysisedtPixelSize->setValue(400);
    wizLSAnalysisedtPixelSize->setDecimals(2);

    wizLSAnalysisedtStepSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizLSAnalysisedtStepSize->setRange(0,100000);
    wizLSAnalysisedtStepSize->setSuffix(" nm");
    wizLSAnalysisedtStepSize->setValue(1000);
    wizLSAnalysisedtStepSize->setDecimals(2);

    wizLSAnalysisspinMaskSize=new QSpinBox(wizLSAnalysisImgPreview);
    wizLSAnalysisspinMaskSize->setRange(0,100000);
    wizLSAnalysisspinMaskSize->setSuffix(" pixels");
    wizLSAnalysisspinMaskSize->setValue(0);
    connect(wizLSAnalysisspinMaskSize, SIGNAL(valueChanged(int)), this, SLOT(wizLSAnalysisImgPreviewMaskChanged(int)));

    if (wizLSAnalysiscmbFitDir) {
        ProgramOptions::getConfigQComboBox(wizLSAnalysiscmbFitDir, "image_stack/startProjectWizardLightsheetAnalysis/cmbFitDir");
        wizLSAnalysisImgPreview->addRow(tr("fit direction"), wizLSAnalysiscmbFitDir);
    }
    if (wizLSAnalysisedtPixelSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtPixelSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtPixelSize");
        wizLSAnalysisImgPreview->addRow(tr("pixel size"), wizLSAnalysisedtPixelSize);
    }
    if (wizLSAnalysisedtStepSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizLSAnalysisedtStepSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtStepSize");
        wizLSAnalysisImgPreview->addRow(tr("step size"), wizLSAnalysisedtStepSize);
    }
    if (wizLSAnalysisspinMaskSize) {
        ProgramOptions::getConfigQSpinBox(wizLSAnalysisspinMaskSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisspinMaskSize");
        wizLSAnalysisImgPreview->addRow(tr("mask size"), wizLSAnalysisspinMaskSize);
    }

    QFTextWizardPage* last;
    if (insertEval) wiz->addPage(last=new QFTextWizardPage(tr("Finalize"),tr("You completed this wizard. The selected files will now be inserted as an image stack raw data record (RDR) into the project. Also a SPIM Lightsheet Analysis evaluation will be added to the project and preconfigured, as selected on the last page.<br><br>After the project is set up, you can perform the lightsheet analysis by double-clicking the according evaluation item and running the evaluation there."), wiz));
    else wiz->addPage(last=new QFTextWizardPage(tr("Finalize"), tr("You completed this wizard. The selected files will now be inserted as an image stack raw data record (RDR) into the project."), wiz));
    last->setFinalPage(true);


    if (wiz->exec()) {
        if (wizLSAnalysiscmbFitDir) {
            ProgramOptions::setConfigQComboBox(wizLSAnalysiscmbFitDir, "image_stack/startProjectWizardLightsheetAnalysis/cmbFitDir");
        }
        if (wizLSAnalysisedtPixelSize) {
            ProgramOptions::setConfigQDoubleSpinBox(wizLSAnalysisedtPixelSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtPixelSize");
        }
        if (wizLSAnalysisedtStepSize) {
            ProgramOptions::setConfigQDoubleSpinBox(wizLSAnalysisedtStepSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisedtStepSize");
        }
        if (wizLSAnalysisspinMaskSize) {
            ProgramOptions::setConfigQSpinBox(wizLSAnalysisspinMaskSize, "image_stack/startProjectWizardLightsheetAnalysis/wizLSAnalysisspinMaskSize");
        }
        QStringList files=selfiles->files();
        QString filterid=selfiles->fileFilterIDs().value(0);
        //qDebug()<<"OK"<<files<<filters;
        if (files.size()>0 && !filterid.isEmpty()) {

            qDebug()<<filterid<<files;
            QFRawDataRecord* e=addMultiFileImageStack(files,filterid);
            QFRDRImageStackData* is=qobject_cast<QFRDRImageStackData*>(e);
            if (is) {
                is->maskClear();
                int image_width=is->getImageStackWidth(0);
                int image_height=is->getImageStackHeight(0);
                int size=wizLSAnalysisspinMaskSize->value();
                for (int y=0; y<image_height; y++) {
                    for (int x=0; x<image_width; x++) {
                        is->maskSet(x,y, x<size || x>image_width-size || y<size || y>image_height-size);
                    }
                }
                is->maskMaskChangedEvent();
            }
            if (e)  {
                if (wizLSAnalysisedtPixelSize) e->setQFProperty("PIXEL_WIDTH", wizLSAnalysisedtPixelSize->value(), false, true);
                if (wizLSAnalysisedtPixelSize) e->setQFProperty("PIXEL_HEIGHT", wizLSAnalysisedtPixelSize->value(), false, true);
                if (wizLSAnalysisedtStepSize) e->setQFProperty("DELTAZ", wizLSAnalysisedtStepSize->value(), false, true);
                e->setQFProperty("USEMASK", true, false, false);
                if (wizLSAnalysiscmbFitDir) e->setQFProperty("ORIENTATION", wizLSAnalysiscmbFitDir->currentIndex(), false, false);
            }



            QFEvaluationItem* ev=NULL;
            if (insertEval) {
                ev=project->addEvaluation("spim_lightsheet_eval", "SPIM Lightsheet Analysis");
                if (ev) {
                    //ev->setQFProperty()
                }
            }
        }
    }
    delete wiz;
}

void QFRDRImageStackPlugin::wizLSAnalysisImgPreviewOnValidate(QWizardPage *page, QWizardPage *userPage)
{
    QFImagePlotWizardPage* plot=qobject_cast<QFImagePlotWizardPage*>(userPage);
    QFSelectFilesWizardPage* files=qobject_cast<QFSelectFilesWizardPage*>(page);
    if (plot && files) {
        QFImporter::FileInfo info;
        double* image=NULL;
        int width=0, height=0;
        plot->setImage(files->files().value(0, ""), files->fileFilterIDs().value(0, ""), -1, image, width, height, &info);
        if (wizLSAnalysisedtPixelSize) {
            if (info.properties.contains("PIXEL_WIDTH")) {
                wizLSAnalysisedtPixelSize->setValue(info.properties["PIXEL_WIDTH"].toDouble());
            } else if (info.properties.contains("PIXEL_HEIGHT")) {
                wizLSAnalysisedtPixelSize->setValue(info.properties["PIXEL_HEIGHT"].toDouble());
            }
        }

        if (wizLSAnalysisedtStepSize) {
            if (info.properties.contains("DELTAZ")) {
                wizLSAnalysisedtStepSize->setValue(info.properties["DELTAZ"].toDouble());
            } else if (info.properties.contains("STEPSIZE")) {
                wizLSAnalysisedtStepSize->setValue(info.properties["STEPSIZE"].toDouble());
            }
        }
        if (wizLSAnalysiscmbFitDir && image && width>0 && height>0)  {
            QFImageSymmetry s=qfGetImageSymetry(image, width, height);
            if (s==qfisVertical) wizLSAnalysiscmbFitDir->setCurrentIndex(0);
            if (s==qfisHorizonal) wizLSAnalysiscmbFitDir->setCurrentIndex(1);
            qfFree(image);
        }
        if (wizLSAnalysisspinMaskSize) wizLSAnalysisImgPreviewMaskChanged(wizLSAnalysisspinMaskSize->value());
    }
}

void QFRDRImageStackPlugin::wizLSAnalysisImgPreviewMaskChanged(int masksize)
{
    if (wizLSAnalysisImgPreview) {
        wizLSAnalysisImgPreview->getImagePlot()->setMaskAround(masksize);
    }
}


Q_EXPORT_PLUGIN2(image_stack, QFRDRImageStackPlugin)


