/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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
#include "qfrdrfcsdatainterface.h"
#include "qfrdroverviewimageinterface.h"
#include "qfrdrsimplecountrateinterface.h"
#include "csvtools.h"
#include "qtriple.h"
#include "qfrdrimagingfcsoverviewrateeditor.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimageselectioninterface.h"
#include "qfrdrrunselection.h"
#include "qfrdrimagemask.h"
#include "qfrdrrunselectiontools.h"
#include "qfrdrimagemasktools.h"
#include "qfrdrimagetoruninterface.h"
#include "qfrdrannotationtools.h"


/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRImagingFCSData : public QFRawDataRecord, public QFRDRFCSDataInterface, public QFRDRImageToRunInterface, public QFRDRAdditionalImagesInterface, public QFRDRImageStackInterface, public QFRDRSimpleCountRatesInterface, public QFRDRImageSelectionInterface, public QFRDRRunSelectionTools, public QFRDRImageMaskTools, public QFRDRAnnotationTools {
        Q_OBJECT
        Q_INTERFACES(QFRDRFCSDataInterface QFRDRImageToRunInterface QFRDRAdditionalImagesInterface QFRDRImageStackInterface QFRDRSimpleCountRatesInterface QFRDRImageSelectionInterface QFRDRRunSelectionsInterface QFRDRImageMaskInterface QFRDRAnnotationInterface)
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
        QStringList getExportFiletypes() const;
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() const;
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() const;

        /** \brief returns the number of segments contained in the correlation data */
        int getSegmentCount() const;
        /** \brief returns the length of a segment in seconds */
        double getSegmentDuration() const;
        /** \brief returns the length of the whole measurement in seconds */
        double getMeasurementDuration() const;
        /** \brief returns the frametime in seconds */
        double getFrameTime() const;
        /** \brief returns \c true, if the specified segment is used in the per-pixel-avg. */
        bool segmentUsed(int segment) const;
        /** \brief returns \c true if all segments are used (non excluded) */
        bool allSegmentsUsed() const;
        /** \brief sets whether the given segment should be used, call recalcSegmentedAverages() so these changes take effect! */
        void setSegmentUsed(int segment, bool used);
        /** \brief reread correlation curves and recalculate the pixel averages caring for excluded segments */
        void recalcSegmentedAverages();



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


        bool dualViewModeUserEditable() const;
        DualViewMode dualViewMode() const;
        void setDualViewMode(DualViewMode mode);
        bool isFCCS() const;
        bool isDCCF() const;
        int getDCCFDeltaX() const;
        int getDCCFDeltaY() const;
        bool isACF() const;
        bool overviewImagesSwapped() const;



        /** \brief returns the number of runs in the record */
        virtual int leaveoutGetRunCount() const;
        /** \brief returns whether to leave out a run */
        virtual bool leaveoutRun(int run) const ;
        /** \brief add a run to the leaveouts */
        virtual void leaveoutAddRun(int run);
        /** \brief remove a run from the leaveouts */
        virtual void leaveoutRemoveRun(int run);
        /** \brief clear all leaveouts */
        virtual void leaveoutClear();


        /** \brief recalculate the averages/std. deviations */
        void recalcCorrelations();


        /** \brief return the number of datapoints in the statistics dataset */
        uint32_t getStatisticsN(int channel=0) const;
        /** \brief return the array of average values of the statsitics */
        double* getStatisticsMean(int channel=0) const;
        /** \brief return the array of standard deviation values of the statsitics */
        double* getStatisticsStdDev(int channel=0) const;
        double* getStatisticsMin(int channel=0) const;
        double* getStatisticsMax(int channel=0) const;
        /** \brief return the array of time points [seconds] for the statsitics */
        double *getStatisticsT(int channel=0) const;

        /** \brief return the number of datapoints in the background statistics dataset */
        uint32_t getBackgroundStatisticsN(int channel=0) const;
        /** \brief return the array of average values of the background statsitics */
        double* getBackgroundStatisticsMean(int channel=0) const;
        /** \brief return the array of standard deviation values of the background statsitics */
        double* getBackgroundStatisticsStdDev(int channel=0) const;
        double* getBackgroundStatisticsMin(int channel=0) const;
        double* getBackgroundStatisticsMax(int channel=0) const;
        /** \brief return the array of time points [seconds] for the background  statsitics */
        double *getBackgroundStatisticsT(int channel=0) const;


        /** \brief return the number of datapoints in the Uncorrected statistics dataset */
        uint32_t getUncorrectedStatisticsN(int channel=0) const;
        /** \brief return the array of average values of the Uncorrected statsitics */
        double* getUncorrectedStatisticsMean(int channel=0) const;
        /** \brief return the array of standard deviation values of the Uncorrected statsitics */
        double* getUncorrectedStatisticsStdDev(int channel=0) const;
        double* getUncorrectedStatisticsMin(int channel=0) const;
        double* getUncorrectedStatisticsMax(int channel=0) const;
        /** \brief return the array of time points [seconds] for the Uncorrected  statsitics */
        double *getUncorrectedStatisticsT(int channel=0) const;





        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionCount() */
        virtual int getImageSelectionCount() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionName() */
        virtual QString getImageSelectionName(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::loadImageSelection() */
        virtual bool* loadImageSelection(int selection) const;
        /** \copydoc QFRDRImageSelectionInterface::addImageSelection() */
        virtual void addImageSelection(const bool* selection, const QString& name=QString(""), bool overwriteIfSameNameExists=true);
        /** \copydoc QFRDRImageSelectionInterface::deleteImageSelection() */
        virtual void deleteImageSelection(int selection);
        /** \copydoc QFRDRImageSelectionInterface::setImageSelectionName() */
        virtual void setImageSelectionName(int selection, const QString& name);
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionWidth() */
        virtual int getImageSelectionWidth() const;
        /** \copydoc QFRDRImageSelectionInterface::getImageSelectionHeight() */
        virtual int getImageSelectionHeight() const;



        /** \brief returns a list of the allowed RDR roles, default: an emplty list */
        virtual QStringList getAvailableRoles() const;
        /** \brief if this returns \c true, the user may change the RDRs role in the QFRawDataPropertyEditor, default: \c false */
        virtual bool isRoleUserEditable() const;

        QFRDRImagingFCSData* getRoleFromThisGroup(const QString& role);


    protected:

        virtual bool doCopyFileForExport(const QString& filename, const QString& fileType, QString& newFilename, const QList<QFProject::FileCopyList> *filecopylist=NULL, const QString &subfoldername=QString("rdr_data_files/")) const;

        void loadPostProcess();



        /** \brief indicates whether the internal data representation is "dual view", this is independent of dualViewMode() and setDualViewMode() which only influences the data display! */
        DualViewMode internalDualViewMode() const;
        /** \brief returns which channel of the full dataset to use */
        int internalDualViewModeChannel() const;
        void setInternalDualViewMode(DualViewMode mode);

        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w) const;
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
        bool loadRadhard2File(const QString& filename, bool loadOverview=false);

        /** \brief load overview image file */
        bool loadOverview(double *overviewF, double *overviewF2, const QString &filename);

        /** \brief load an image file into the given arrays */
        bool loadImage(const QString &filename, double **data, int *width, int *height);

        /*! \brief load a TIFF video file into the given arrays

            If the image contains a 16-bit uint image, the video will be scales using the given factor: Iout=scaleOffset+scaleFactor*Iin
         */
        bool loadVideo(const QString &filename, double **data, int *width, int *height, uint32_t *frames, double scaleFactor=1, double scaleOffset=0);

        void loadQFPropertiesFromB040SPIMSettingsFile(QSettings& settings);

        double getTauMin() const;

        /*! \brief selects a subpart of a full DV image for the correlation functions.

            This function performs these tasks:
              # creates a temporary copy of the correlation data.
              # reallocates the internal data fields for the correlations with width/2 or height/2 according to DV setting
              # copies only the data of the selected image half.
            .
         */
        void splitCFsForDualView();
        /*! \brief selects a subpart of a full DV image for the videos.
            This function performs these tasks if in DV-mode:
              # resort video, so left part is followed by right part if this is FCCS otherwise only the left or right part o the video is contained in the video array.
              # the width/height is changed accordingly
              # if in FCCS mode, a pointer to the second channel is stored (video2/videoUncorrected2), never free this pointer, as the complete memory block is
                alloced in one call !
              # no data is reallocated!
              # one temporary internal copy of the data is created
            .
         */
        void splitVideosForDualView();
        /** \brief splits a single video */
        void splitVideo(double* video, double*& video2, int& width, int& height, uint32_t frames);

        virtual void maskMaskChangedEvent();
    protected:
        /** \brief width of the image */
        int width;
        /** \brief height of the image */
        int height;
        /** \brief width of the overview image */
        int widthOvr;
        /** \brief height of the overview image */
        int heightOvr;
        /** \brief number of points in correlation curve */
        int N;
        /** \brief points to the correlation curves */
        double* correlations;
        /** \brief points to the correlation curve erorrs */
        double* sigmas;
        /** \brief average over all correlation curves */
        double* correlationMean;
        /** \brief average over all correlation curves */
        double* correlationStdDev;
        /** \brief time axis [seconds] */
        double* tau;

        /** \brief temporary storage for overview image */
        QVector<double> overviewTemp;


        struct statisticsData {
            /** \brief number of data points in the statistics  */
            uint32_t N;
            /** \brief statistics: average vector */
            double* Avg;
            /** \brief statistics: standard deviation vector */
            double* StdDev;
            /** \brief statistics: time [seconds] vector */
            double* T;
            double* Min;
            double* Max;

            double avgCnt;
            double sigmaCnt;
        };

        /** \brief load the statistics file */
        bool loadStatistics(statisticsData& stat, const QString& filename);

        /** \brief statistics */
        statisticsData stat;
        /** \brief background statistics */
        statisticsData backStat;
        /** \brief uncorrected statistics */
        statisticsData ucStat;
        bool hasStatistics;

        /** \brief statistics 2 */
        statisticsData stat2;
        /** \brief background statistics 2 */
        statisticsData backStat2;
        /** \brief uncorrected statistics 2 */
        statisticsData ucStat2;
        bool hasStatistics2;



        /** \brief overview image */
        double* overviewF;
        double* overviewFSTD;
        double* overviewF2;
        double* overviewF2STD;

        struct ovrImageData {
            double* image;
            QString name;
            QString id;
            int width;
            int height;
            QList<QFRDRAdditionalImagesInterface::AdditionalImagesGeoElement> geoElements;

            ovrImageData() {
                image=NULL;
                name=QString("");
                id="";
                width=0;
                height=0;
            }
        };

        double* video;
        double* video2;
        int video_width;
        int video_height;
        uint32_t video_frames;

        double* videoUncorrected;
        double* videoUncorrected2;
        int videoUncorrected_width;
        int videoUncorrected_height;
        uint32_t videoUncorrected_frames;

        QString maskTemp;

        QList<ovrImageData> ovrImages;

        struct ImageSelection {
            ImageSelection() {
                name="";
                selection=NULL;
            }

            bool* selection;
            QString name;
            QString tempSel;
        };

        QList<ImageSelection> selections;

        void clearSelections();

        void clearOvrImages();

        int getExpectedFileWidth() const;
        int getExpectedFileHeight() const;

        void splitImage(double *overviewF, double *overviewF2, const double *inputImage, uint32_t nx, uint32_t ny);
    public:

        /*! \brief if splitImage() will split it's image and the given inputImage has the correct (double) size, this will read the oldIndex-th pixel from the left or right subimage! */
        double readValueFromPossiblySplitImage(const double *inputImage, uint32_t nx, uint32_t ny, int oldIndex, bool readOtherChannel=false);
        /*! \brief if splitImage() will split it's image and the given inputImage has the correct (double) size, this will read the oldIndex-th pixel from the left or right subimage! */
        float readValueFromPossiblySplitImage(const float *inputImage, uint32_t nx, uint32_t ny, int oldIndex, bool readOtherChannel=false);

    protected:
        /** \brief allocate memory to store a \a x by \a y set of correlation curves (+ additional data, like average and sigmas) with \a N datapoints each */
        virtual void allocateContents(int x, int y, int N);
        /** \brief allocate memory for overview images */
        virtual void allocateOverviews(int x, int y);
        void allocateStatistics(statisticsData& stat, uint32_t N);
        void addImageAsResult(const QString &metadata_eid, const QString &metadata_egrou, const QString &metadata_elabel, const QString &param, QRegExp rx, const QString &plabel);
        void addCFAmplitudeAsResult(const QString &metadata_eid, const QString &metadata_egrou, const QString &metadata_elabel, const QString &param, const QString &plabel, int avg);
    public:
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsWidth() */
        virtual int getImageFromRunsWidth() const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsHeight() */
        virtual int getImageFromRunsHeight() const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsChannels() */
        virtual int getImageFromRunsChannels() const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsChannelsAdvised() */
        virtual int getImageFromRunsChannelsAdvised() const;
        /** \copydoc QFRDRImageToRunInterface::xyToRun() */
        virtual int xyToRun(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::runToX() */
        virtual int runToX(int run) const;
        /** \copydoc QFRDRImageToRunInterface::runToY() */
        virtual int runToY(int run) const;
        /** \copydoc QFRDRImageToRunInterface::xyToIndex() */
        virtual int xyToIndex(int x, int y) const;
        /** \copydoc QFRDRImageToRunInterface::getImageFromRunsPreview() */
        virtual double* getImageFromRunsPreview(int channel=0) const;


        /** \brief returns true, if a given run/pixel is inside the second DualView channel */
        bool indexIsDualView2(int32_t sel);

        /** \copydoc QFRDROverviewImageInterface::getPreviewImageCount() */
        virtual int getAdditionalImagesCount() const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageWidth() */
        virtual int getAdditionalImagesWidth(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageHeight() */
        virtual int getAdditionalImagesHeight(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageName() */
        virtual QString getAdditionalImagesName(int image) const;
        virtual QString getOverviewImageID(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImage() */
        virtual double* getAdditionalImage(int image) const;
        /** \copydoc QFRDROverviewImageInterface::getPreviewImageGeoElements() */
        virtual QList<QFRDRAdditionalImagesInterface::AdditionalImagesGeoElement> getAdditionalImagesAnnotations(int image) const;




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

        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateAverage() */
        virtual double getSimpleCountrateAverage(int run=-1, int channel=0, bool swapChannels=false) const ;
        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateVariance() */
        virtual double getSimpleCountrateStdDev(int run=-1, int channel=0, bool swapChannels=false) const ;
        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateChannels() */
        virtual int getSimpleCountrateChannels() const ;
        /** \copydoc QFRDRSimpleCountRatesInterface::getSimpleCountrateRuns() */
        virtual int getSimpleCountrateRuns() const ;

};


#endif // QFRDRIMAGINGFCSDATA_H
