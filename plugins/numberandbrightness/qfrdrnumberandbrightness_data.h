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

#ifndef QFRDRNUMBERANDBRIGHTNESSDATA_H
#define QFRDRNUMBERANDBRIGHTNESSDATA_H

#include <QList>
#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "qfrawdatarecord.h"
#include "qfrdrnumberandbrightness_dataeditor.h"
#include "qfrawdatarecordfactory.h"
#include "qfrdroverviewimageinterface.h"
#include "qfrdrsimplecountrateinterface.h"
#include "csvtools.h"
#include "qtriple.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimageselectioninterface.h"
#include "qfimporter.h"
#include "qfimporterimageseries.h"
#include "qfimportermanager.h"
#include "tools.h"
#include "qfrdrimagemasktools.h"
#include "imagetools.h"

/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_number_and_brightness

*/
class QFRDRNumberAndBrightnessData : public QFRawDataRecord, public QFRDRAdditionalImagesInterface, public QFRDRImageSelectionInterface, public QFRDRImageMaskTools, public QFRDROverviewImagesInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRAdditionalImagesInterface  QFRDRImageSelectionInterface QFRDRImageMaskInterface QFRDROverviewImagesInterface)
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "number_and_brightness"; };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Number & Brightness"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/number_and_brightness/qfrdrnumberandbrightness_small.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("Number & Brightness analysis of image data"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/number_and_brightness/qfrdrnumberandbrightness.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount();
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) ;
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL);
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr(""); };
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr(""); };


        double* getImage() const;
        double* getImageVariance() const;
        double* getBackground() const;
        double* getBackgroundVariance() const;
        double* getAppNumberImage() const;
        double* getAppBrightnessImage() const;
        double* getNumberImage() const;
        double* getBrightnessImage() const;

        double getImage(int idx) const;
        double getImageVariance(int idx) const;
        double getBackground(int idx) const;
        double getBackgroundVariance(int idx) const;
        double getAppNumberImage(int idx) const;
        double getAppBrightnessImage(int idx) const;
        double getNumberImage(int idx) const;
        double getBrightnessImage(int idx) const;

        int getWidth() const;
        int getHeight() const;

        void recalcNumberAndBrightness();



        virtual int getOverviewImageCount() const;
        virtual uint32_t getOverviewImageWidth() const;
        virtual uint32_t getOverviewImageHeight() const;
        virtual QString getOverviewImageName(int image) const;
        virtual double* getOverviewImage(int image) const;


        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionCount() */
        virtual int getImageSelectionCount() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionName() */
        virtual QString getImageSelectionName(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::loadImageSelection() */
        virtual bool* loadImageSelection(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::addImageSelection() */
        virtual void addImageSelection(const bool *selection, const QString& name=QString(""), bool overwriteIfSameNameExists=true);
        /** \copydoc QFRDRImageSelectionInterface::deleteImageSelection() */
        virtual void deleteImageSelection(int selection);
        /** \copydoc QFRDRImageSelectionInterface::setImageSelectionName() */
        virtual void setImageSelectionName(int selection, const QString& name);
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionWidth() */
        virtual int getImageSelectionWidth() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionHeight() */
        virtual int getImageSelectionHeight() const;


        /** \copydoc QFRDROverviewImageInterface::getPreviewImageCount() */
        virtual int getAdditionalImagesCount() const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageWidth() */
        virtual int getAdditionalImagesWidth(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageHeight() */
        virtual int getAdditionalImagesHeight(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageName() */
        virtual QString getAdditionalImagesName(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImage() */
        virtual double* getAdditionalImage(int img) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageGeoElements() */
        virtual QList<QFRDRAdditionalImagesInterface::AdditionalImagesGeoElement> getAdditionalImagesAnnotations(int image) const;





        /** \copydoc QFRDRImageToRunInterface::xyToRun() */
        virtual int xyToIndex(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::runToX() */
        virtual int indexToX(int run) const;
        /** \copydoc QFRDRImageToRunInterface::runToY() */
        virtual int indexToY(int run) const;

        QFImageHalf getSelectedImageHalf() const;

    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w) const;
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);


        enum FileOperations {
            TakeSqrt,
            Square,
            NoOperation
        };

        /** \brief load overview image file */
        bool loadFile(double* target, const QString& filename, FileOperations op=NoOperation);

        void loadQFPropertiesFromB040SPIMSettingsFile(QSettings& settings);

        /** \brief load an image file into the given arrays */
        bool loadImage(const QString &filename, double **data, int *width, int *height);

        /** \brief load a TIFF video file into the given arrays */
        bool loadVideo(const QString &filename, double **data, int *width, int *height, uint32_t *frames);

        /** \brief allocate memory to store a \a x by \a y set of correlation curves (+ additional data, like average and sigmas) with \a N datapoints each */
        virtual void allocateContents(int x, int y);


    private:
        /** \brief width of the image */
        int width;
        /** \brief height of the image */
        int height;

        double* background;
        double* backgroundVariance;
        double* image;
        double* imageVariance;
        double* appNumberImage;
        double* appBrightnessImage;
        double* numberImage;
        double* brightnessImage;


        struct ovrImageData {
            double* image;
            QString name;
            int width;
            int height;
            QList<QFRDRAdditionalImagesInterface::AdditionalImagesGeoElement> geoElements;

            ovrImageData() {
                image=NULL;
                name=QString("");
                width=0;
                height=0;
            }
        };

        QList<ovrImageData> ovrImages;

        struct ImageSelection {
            ImageSelection() {
                name="";
                selection=NULL;
            }

            bool* selection;
            QString name;
        };

        QList<ImageSelection> selections;

        void clearSelections();

        void clearOvrImages();

        /** \brief the leaveout list */
        bool* leaveout;

};


#endif // QFRDRNUMBERANDBRIGHTNESSDATA_H
