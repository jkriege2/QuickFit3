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

#ifndef QFRDRIMAGESTACK_H
#define QFRDRIMAGESTACK_H

#include "qfpluginrawdata.h"


class QFImagePlotWizardPage; // forward
class QFSelectFilesListWizardPage;

/*!
    \defgroup qf3rdrdp_image_stack Raw Data Record Plugin that manages a set of 4D image stacks
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class that manages a set of 4D image stacks
    \ingroup qf3rdrdp_image_stack
    */

class QFRDRImageStackPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFRDRImageStackPlugin")
#endif

    public:
        /** Default constructor */
        QFRDRImageStackPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRImageStackPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menuMain);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("image_stack"); }

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Image Stack"); }

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("This plugin manages stacks of images (4D)"); }

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012-2014 by Jan W. Krieger"); }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        }

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/image_stack/qfrdrimagestack.png"); }

        virtual void init();
    protected slots:
        /** \brief insertdata from file*/
        void insertSingleFileImageStack();
        void insertMultiFileImageStack();
        QFRawDataRecord *addMultiFileImageStack(const QStringList& files, const QString& format_id);
        QFRawDataRecord* addSingleFileDualViewImageStack(char dvMode, const QString &filename, const QString& format_id);

        void insertSingleFileHDualViewImageStack();
        void insertSingleFileVDualViewImageStack();
        void addSingleFileDualViewImageStack(char dvMode='h');
        void startProjectWizardLightsheetAnalysis(bool insertEval=true);
        void startProjectWizardLightsheetAnalysisData();
        void startProjectWizardPSFAnalysis(bool insertEval=true);
        void startProjectWizardPSFAnalysisData();
        void startImagestackWizard();

    private:
        QPointer<QDoubleSpinBox> wizLSAnalysisedtPixelSize;
        QPointer<QSpinBox> wizLSAnalysisspinMaskSize;
        QPointer<QDoubleSpinBox> wizLSAnalysisedtStepSize;
        QPointer<QComboBox> wizLSAnalysiscmbFitDir;
        QPointer<QComboBox> wizLSAnalysiscmbStackMode;
        QPointer<QFImagePlotWizardPage> wizLSAnalysisImgPreview;
        QPointer<QFSelectFilesListWizardPage> wizSelfiles;
    private slots:
        void wizLSAnalysisImgPreviewOnValidate(QWizardPage* page, QWizardPage* userPage);
        void wizLSAnalysisImgPreviewMaskChanged(int masksize);
};

#endif // QFRDRIMAGESTACK_H
