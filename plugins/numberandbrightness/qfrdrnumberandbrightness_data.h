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

/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_number_and_brightness

*/
class QFRDRNumberAndBrightnessData : public QFRawDataRecord, public QFRDROverviewImageInterface, public QFRDRImageSelectionInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDROverviewImageInterface  QFRDRImageSelectionInterface)
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
        double* getNumberImage() const;
        double* getBrightnessImage() const;
        int getWidth() const;
        int getHeight() const;

        void recalcNumberAndBrightness();



        /** \brief returns whether to leave out a run */
        virtual bool leaveoutRun(int run) const ;
        /** \brief add a run to the leaveouts */
        virtual void leaveoutAddRun(int run);
        /** \brief remove a run from the leaveouts */
        virtual void leaveoutRemoveRun(int run);
        /** \brief clear all leaveouts */
        virtual void leaveoutClear();

        void maskLoad(const QString& filename);
        void maskSave(const QString& filename) const ;
        void maskClear();
        void maskSetAll();
        void maskSet(uint16_t x, uint16_t y);
        void maskUnset(uint16_t x, uint16_t y, bool value=true);
        void maskToggle(uint16_t x, uint16_t y);
        void maskInvert();
        bool maskGet(uint16_t x, uint16_t y) const ;
        bool* maskGet() const;



        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionCount() */
        virtual int getImageSelectionCount() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionName() */
        virtual QString getImageSelectionName(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::loadImageSelection() */
        virtual bool* loadImageSelection(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::addImageSelection() */
        virtual void addImageSelection(bool* selection, const QString& name=QString(""));
        /** \copydoc QFRDRImageSelectionInterface::deleteImageSelection() */
        virtual void deleteImageSelection(int selection);
        /** \copydoc QFRDRImageSelectionInterface::setImageSelectionName() */
        virtual void setImageSelectionName(int selection, const QString& name);
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionWidth() */
        virtual int getImageSelectionWidth() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionHeight() */
        virtual int getImageSelectionHeight() const;


        /** \copydoc QFRDROverviewImageInterface::getPreviewImageCount() */
        virtual int getOverviewImageCount() const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageWidth() */
        virtual int getOverviewImageWidth(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageHeight() */
        virtual int getOverviewImageHeight(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageName() */
        virtual QString getOverviewImageName(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImage() */
        virtual double* getOverviewImage(int img) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageGeoElements() */
        virtual QList<QFRDROverviewImageInterface::OverviewImageGeoElement> getOverviewImageAnnotations(int image) const;





        /** \copydoc QFRDRImageToRunInterface::xyToRun() */
        virtual int xyToIndex(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::runToX() */
        virtual int indexToX(int run) const;
        /** \copydoc QFRDRImageToRunInterface::runToY() */
        virtual int indexToY(int run) const;

    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
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
        virtual void allocateContents(int x, int y, int N=0);


    private:
        /** \brief width of the image */
        int width;
        /** \brief height of the image */
        int height;

        double* background;
        double* backgroundVariance;
        double* image;
        double* imageVariance;
        double* numberImage;
        double* brightnessImage;


        struct ovrImageData {
            double* image;
            QString name;
            int width;
            int height;
            QList<QFRDROverviewImageInterface::OverviewImageGeoElement> geoElements;

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
