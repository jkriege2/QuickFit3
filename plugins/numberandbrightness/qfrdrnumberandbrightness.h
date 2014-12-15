/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFRDRNUMBERANDBRIGHTNESS_H
#define QFRDRNUMBERANDBRIGHTNESS_H

#include "qfpluginrawdata.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"
#include "qftools.h"
#include "qfenhancedcombobox.h"
class QFImagePlotWizardPage; // forward
class QFSelectFilesWizardPage;
class QFSelectFilesListWizardPage;
class QEnhancedLineEdit;
class QFProcessingWizardPage;
class QFComboBoxWizardPage;

/*!
    \defgroup qf3rdrdp_number_and_brightness Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief raw data record plugin class
    \ingroup qf3rdrdp_number_and_brightness
    */

class QFRDRNumberAndBrightnessPlugin : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessPlugin(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessPlugin();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() const  { return tr("number_and_brightness"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Number & Brightness"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("number and brightness data evaluation/record"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2012-2014 by Jan W. Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const  { return QString(":/number_and_brightness/qfrdrnumberandbrightness.png"); };
    protected slots:
        /** \brief insertdata from file*/
        void insertFromImFCSRecord();
        void insertImFCSFile(const QString &filename);
        QFRawDataRecord* insertPreprocessedFiles(const QString &filename_overview, const QString &filename_overviewstd, const QString &filename_background, const QString &filename_backgroundstddev, bool filename_overviewstd_isVar=false, bool filename_backgroundstddev_isVar=false, const QMap<QString, QVariant> &iParams=QMap<QString, QVariant>(), const QStringList &iReadonly=QStringList(), const QString &group=QString());

        /** \brief insert record, if it is not yet contained in the project! */
        QFRawDataRecord *insertProjectRecord(const QString& type, const QString& name, const QString& filename, const QString& description=QString(""), const QString& directory=QString(""), const QMap<QString,QVariant>& init_params=QFStringVariantMap(), const QStringList& init_params_readonly=QStringList());

        bool parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions);
        void startNANDBFromPreprocessedFilesWizard();

        void wizImgPreviewOnValidate(QWizardPage *page, QWizardPage *userPage);
        void wizMaskChanged(int masksize);
        void wizSubimagesChanged(int index);

        void startWizardImageStack();
        void wizImageOverviewFromFilesList(QWizardPage *page, QWizardPage *userPage);
        void wizCameraTypeChanged(int index);
        void wizSelectBackground();
        void wizProcessNAndB();
        void wizAddFinishedFile(const QString& file);
        void wizDVChanged(int index);
    protected:
        QPointer<QDoubleSpinBox> wizPixelSize;
        QPointer<QSpinBox> wizMaskSize;
        QPointer<QDoubleSpinBox> wizStepSize;
        QPointer<QDoubleSpinBox> wizGain;
        QPointer<QDoubleSpinBox> wizExcessNoise;
        QPointer<QFEnhancedComboBox> wizLSAnalysiscmbFitDir;
        QPointer<QFEnhancedComboBox> wizDualViewMode;
        QPointer<QFImagePlotWizardPage> wizLSAnalysisImgPreview;
        QPointer<QFSelectFilesWizardPage> wizSelfiles;
        QPointer<QFSelectFilesListWizardPage> wizSelfilesList;
        QPointer<QEnhancedLineEdit> edtBackgroundFile;
        QPointer<QLineEdit> edtResultsPrefix;
        QPointer<QFComboBoxWizardPage> wizAcqType;
        QPointer<QLabel> wizLabWidth;
        QPointer<QLabel> wizLabHeight;
        QPointer<QLabel> wizFileInfo;
        QPointer<QSpinBox> wizFirstFrame;
        QPointer<QSpinBox> wizLastFrame;
        QPointer<QFProcessingWizardPage> wizProc;
        int wizFileImageWidth;
        int wizFileImageHeight;
        int wizFileFrames;
        int wizRDRImageWidth;
        int wizRDRImageHeight;
        QStringList wizFinishedFiles;


};

#endif // QFRDRNUMBERANDBRIGHTNESS_H

