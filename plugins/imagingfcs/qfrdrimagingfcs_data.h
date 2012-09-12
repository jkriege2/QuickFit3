#ifndef QFRDRIMAGINGFCSDATA_H
#define QFRDRIMAGINGFCSDATA_H

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
#include "qfrdrimagingfcs_dataeditor.h"
#include "qfrdrimagingfcs_dataeditor_image.h"
#include "qfrdrimagingfcs_dataeditor_countrate.h"
#include "qfrawdatarecordfactory.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "../interfaces/qfrdrimagetoruninterface.h"
#include "../interfaces/qfrdroverviewimageinterface.h"
#include "../interfaces/qfrdrsimplecountrateinterface.h"
#include "csvtools.h"
#include "qtriple.h"
#include "qfrdrimagingfcsoverviewrateeditor.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimageselectioninterface.h"
#include "qfrdrrunselection.h"


/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRImagingFCSData : public QFRawDataRecord, public QFRDRFCSDataInterface, public QFRDRImageToRunInterface, public QFRDROverviewImageInterface, public QFRDRImageStackInterface, public QFRDRSimpleCountRatesInterface, public QFRDRImageSelectionInterface, public QFRDRRunSelectionsInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRFCSDataInterface QFRDRImageToRunInterface QFRDROverviewImageInterface QFRDRImageStackInterface QFRDRSimpleCountRatesInterface QFRDRImageSelectionInterface QFRDRRunSelectionsInterface)
    public:
        enum DualViewMode {
            dvNone=0,
            dvHorizontal=1,
            dvVertical=2
        };


        /** Default constructor */
        QFRDRImagingFCSData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRImagingFCSData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "imaging_fcs"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Imaging FCS Data"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/imaging_fcs/qfrdrimagingfcs_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("manages FCS correlation curves from an imaging FCS experiment (SPIM-FCS, TIR-FCS ...)"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/imaging_fcs/qfrdrimagingfcs.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount();
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i);
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, int i=0, QWidget *parent=NULL);
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes();
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle();
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes();



        /** \copydoc QFRDRFCSDataInterface::getCorrelationRuns() */
        virtual int getCorrelationRuns() const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationN() */
        virtual long long getCorrelationN() const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationT() */
        virtual double* getCorrelationT() const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelation() */
        virtual double* getCorrelation() const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationRun() */
        virtual double* getCorrelationRun(int run) const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationRunError() */
        virtual double* getCorrelationRunError(int run) const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationRunName() */
        virtual QString getCorrelationRunName(int run) const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationMean() */
        virtual double* getCorrelationMean() const;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationStdDev() */
        virtual double* getCorrelationStdDev() const;
        /** \copydoc QFRDRFCSDataInterface::isCorrelationRunVisible() */
        virtual bool isCorrelationRunVisible(int run) const ;
        /** \copydoc QFRDRFCSDataInterface::getCorrelationRunErrors() */
        virtual double* getCorrelationRunErrors() const ;


        DualViewMode dualViewMode() const;
        void setDualViewMode(DualViewMode mode);



        /** \brief returns whether to leave out a run */
        virtual bool leaveoutRun(int run) const ;
        /** \brief add a run to the leaveouts */
        virtual void leaveoutAddRun(int run);
        /** \brief remove a run from the leaveouts */
        virtual void leaveoutRemoveRun(int run);
        /** \brief clear all leaveouts */
        virtual void leaveoutClear();

        void maskLoad(const QString& filename);
        void maskLoadFromString(QString maskstring);
        void maskSave(const QString& filename) const ;
        QString maskToString() const;
        void maskClear();
        void maskSetAll();
        void maskSet(uint16_t x, uint16_t y);
        void maskUnset(uint16_t x, uint16_t y, bool value=true);
        void maskToggle(uint16_t x, uint16_t y);
        void maskInvert();
        bool maskGet(uint16_t x, uint16_t y) const ;
        bool* maskGet() const;

        /** \brief recalculate the averages/std. deviations */
        void recalcCorrelations();


        /** \brief return the number of datapoints in the statistics dataset */
        uint32_t getStatisticsN() const;
        /** \brief return the array of average values of the statsitics */
        double* getStatisticsMean() const;
        /** \brief return the array of standard deviation values of the statsitics */
        double* getStatisticsStdDev() const;
        double* getStatisticsMin() const;
        double* getStatisticsMax() const;
        /** \brief return the array of time points [seconds] for the statsitics */
        double *getStatisticsT() const;

        /** \brief return the number of datapoints in the background statistics dataset */
        uint32_t getBackgroundStatisticsN() const;
        /** \brief return the array of average values of the background statsitics */
        double* getBackgroundStatisticsMean() const;
        /** \brief return the array of standard deviation values of the background statsitics */
        double* getBackgroundStatisticsStdDev() const;
        double* getBackgroundStatisticsMin() const;
        double* getBackgroundStatisticsMax() const;
        /** \brief return the array of time points [seconds] for the background  statsitics */
        double *getBackgroundStatisticsT() const;


        /** \brief return the number of datapoints in the Uncorrected statistics dataset */
        uint32_t getUncorrectedStatisticsN() const;
        /** \brief return the array of average values of the Uncorrected statsitics */
        double* getUncorrectedStatisticsMean() const;
        /** \brief return the array of standard deviation values of the Uncorrected statsitics */
        double* getUncorrectedStatisticsStdDev() const;
        double* getUncorrectedStatisticsMin() const;
        double* getUncorrectedStatisticsMax() const;
        /** \brief return the array of time points [seconds] for the Uncorrected  statsitics */
        double *getUncorrectedStatisticsT() const;





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




    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

        /** \brief load data file from videocorrelator CSV file */
        bool loadVideoCorrelatorFile(const QString& filename);

        /** \brief load data file in binary format*/
        bool loadVideoCorrelatorFileBin(const QString &filename);

        /** \brief load data file from Radhard2 array */
        bool loadRadhard2File(const QString& filename);

        /** \brief load overview image file */
        bool loadOverview(const QString& filename);

        /** \brief load standard deviation overview image file */
        bool loadOverviewSTD(const QString& filename);

        /** \brief load an image file into the given arrays */
        bool loadImage(const QString &filename, double **data, int *width, int *height);

        /*! \brief load a TIFF video file into the given arrays

            If the image contains a 16-bit uint image, the video will be scales using the given factor: Iout=scaleOffset+scaleFactor*Iin
         */
        bool loadVideo(const QString &filename, double **data, int *width, int *height, uint32_t *frames, double scaleFactor=1, double scaleOffset=0);

        /** \brief load the statistics file */
        bool loadStatistics(const QString& filename);

        /** \brief load the background statistics file */
        bool loadBackgroundStatistics(const QString& filename);

        /** \brief load the uncorrected statistics file */
        bool loadUncorrectedStatistics(const QString& filename);

        void loadQFPropertiesFromB040SPIMSettingsFile(QSettings& settings);

        double getTauMin() const;

    private:
        //DualViewMode m_dualview;
        /** \brief width of the image */
        int width;
        /** \brief height of the image */
        int height;
        /** \brief number of points in correlation curve */
        int N;
        /** \brief points to the correlation curves */
        double* correlations;
        /** \brief average over all correlation curves */
        double* correlationMean;
        /** \brief average over all correlation curves */
        double* correlationStdDev;
        /** \brief points to the correlation curve erorrs */
        double* sigmas;
        /** \brief time axis [seconds] */
        double* tau;


        /** \brief number of data points in the statistics  */
        uint32_t statN;
        /** \brief statistics: average vector */
        double* statAvg;
        /** \brief statistics: standard deviation vector */
        double* statStdDev;
        /** \brief statistics: time [seconds] vector */
        double* statT;
        double* statMin;
        double* statMax;

        /** \brief background number of data points in the statistics  */
        uint32_t backStatN;
        /** \brief background statistics: average vector */
        double* backStatAvg;
        /** \brief background statistics: standard deviation vector */
        double* backStatStdDev;
        /** \brief background statistics: time [seconds] vector */
        double* backStatT;
        double* backStatMin;
        double* backStatMax;

        /** \brief uncorrected number of data points in the statistics  */
        uint32_t ucStatN;
        /** \brief uncorrected statistics: average vector */
        double* ucStatAvg;
        /** \brief uncorrected statistics: standard deviation vector */
        double* ucStatStdDev;
        /** \brief uncorrected statistics: time [seconds] vector */
        double* ucStatT;
        double* ucStatMin;
        double* ucStatMax;

        bool hasStatistics;
        double statAvgCnt;
        double statSigmaCnt;
        double backStatAvgCnt;
        double backStatSigmaCnt;

        /** \brief overview image */
        double* overviewF;
        double* overviewFSTD;

        struct ovrImageData {
            double* image;
            QString name;
            QString id;
            int width;
            int height;
            QList<QFRDROverviewImageInterface::OverviewImageGeoElement> geoElements;

            ovrImageData() {
                image=NULL;
                name=QString("");
                id="";
                width=0;
                height=0;
            }
        };

        double* video;
        int video_width;
        int video_height;
        uint32_t video_frames;

        double* videoUncorrected;
        int videoUncorrected_width;
        int videoUncorrected_height;
        uint32_t videoUncorrected_frames;

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
        //QList<int> leaveout;
    protected:
        /** \brief allocate memory to store a \a x by \a y set of correlation curves (+ additional data, like average and sigmas) with \a N datapoints each */
        virtual void allocateContents(int x, int y, int N);
        void allocateStatistics(uint32_t N);
        void allocateBackgroundStatistics(uint32_t N);
        void allocateUncorrectedStatistics(uint32_t N);


    public:
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsWidth() */
        virtual int getImageFromRunsWidth() const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsHeight() */
        virtual int getImageFromRunsHeight() const;
        /** \copydoc QFRDRImageToRunInterface::xyToRun() */
        virtual int xyToRun(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::runToX() */
        virtual int runToX(int run) const;
        /** \copydoc QFRDRImageToRunInterface::runToY() */
        virtual int runToY(int run) const;
        /** \copydoc QFRDRImageToRunInterface::xyToIndex() */
        virtual int xyToIndex(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsPreview() */
        virtual double* getImageFromRunsPreview() const;

        /** \brief returns true, if a given run/pixel is inside the second DualView channel */
        bool indexIsDualView2(int32_t sel);

        /** \copydoc QFRDROverviewImageInterface::getPreviewImageCount() */
        virtual int getOverviewImageCount() const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageWidth() */
        virtual int getOverviewImageWidth(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageHeight() */
        virtual int getOverviewImageHeight(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageName() */
        virtual QString getOverviewImageName(int image) const;
        virtual QString getOverviewImageID(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImage() */
        virtual double* getOverviewImage(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageGeoElements() */
        virtual QList<QFRDROverviewImageInterface::OverviewImageGeoElement> getOverviewImageAnnotations(int image) const;




        /** \copydoc QFRDRImageStackInterface::getImageStackCount() */
        virtual int getImageStackCount() const;
        /** \copydoc QFRDRImageStackInterface::getImageStackFrames() */
        virtual uint32_t getImageStackFrames(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackWidth() */
        virtual int getImageStackWidth(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackHeight() */
        virtual int getImageStackHeight(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackChannels() */
        virtual int getImageStackChannels(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStack() */
        virtual double* getImageStack(int stack, uint32_t frame=0, uint32_t channel=0) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXUnitFactor() */
        virtual double getImageStackXUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXUnitName() */
        virtual QString getImageStackXUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackXName() */
        virtual QString getImageStackXName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYUnitFactor() */
        virtual double getImageStackYUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYUnitName() */
        virtual QString getImageStackYUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackYName() */
        virtual QString getImageStackYName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTUnitFactor() */
        virtual double getImageStackTUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTUnitName() */
        virtual QString getImageStackTUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTName() */
        virtual QString getImageStackTName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCUnitFactor() */
        virtual double getImageStackCUnitFactor(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCUnitName() */
        virtual QString getImageStackCUnitName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackCName() */
        virtual QString getImageStackCName(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackDescription() */
        virtual QString getImageStackDescription(int stack) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackChannelName() */
        virtual QString getImageStackChannelName(int stack, int channel) const;
        /** \copydoc QFRDRImageStackInterface::getImageStackTimepointName() */
        virtual QString getImageStackTimepointName(int stack, int t) const;

        /** \copydoc QFRDRCountRatesInterface::getSimpleCountrateAverage() */
        virtual double getSimpleCountrateAverage(int run=-1) const ;
        /** \copydoc QFRDRCountRatesInterface::getSimpleCountrateVariance() */
        virtual double getSimpleCountrateVariance(int run=-1) const ;

};


#endif // QFRDRIMAGINGFCSDATA_H
