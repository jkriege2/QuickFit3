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

#ifndef QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
#define QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
#include <stdint.h>
#include <QThread>
#include <QPointer>
#include "qfimporterimageseries.h"
#include <QLocale>
#include <QList>
#include <QVector>
#include <QStringList>
#include <QMutex>
#include "qftools.h"
#include "multitau-correlator.h"
#include "correlator_multitau.h"
#include <QtGlobal>
#include <QtEndian>
#include "qfrdrimagingfcstools.h"
#include "qfpluginservices.h"

class QFRDRImagingFCSThreadProgress; // forward
class QFRDRImagingFCSCorrelationJobThread; // forward

#define CORRELATOR_DIRECT 0
#define CORRELATOR_DIRECT_INT 1
#define CORRELATOR_DIRECTAVG 2
#define CORRELATOR_DIRECTAVG_INT 3
#define CORRELATOR_MTAUALLMON 4
#define CORRELATOR_MTAUONEMON 5
#define CORRELATOR_CORRELATORFROMSHAREDLIB 6

#define BLEACH_NONE 0
#define BLEACH_REMOVEAVG 1
#define BLEACH_EXP 2
#define BLEACH_EXPREG 3
#define BLEACH_EXP_POLY2 4
#define BLEACH_EXP_POLY3 5
#define BLEACH_DBL_EXP 6
#define BLEACH_EXP_POLY4 7
#define BLEACH_EXP_POLY5 8


/*! \brief job description for correlation
    \ingroup qf3rdrdp_imaging_fcs

    The jobs are processed by QFRDRImagingFCSCorrelationJobThread objects (in the run() method) that are
    usually called from a QFRDRImagingFCSCorrelationDialog dialog instance.

    A job completely describes the correlations to be done for an image series.

    For a detailed description of how a job is executed, please refer to the
    documentation of QFRDRImagingFCSCorrelationJobThread which actually contains
    the code that executed the job.
*/
struct IMFCSJob {
    /** \brief progress widget for this job */
    QPointer<QFRDRImagingFCSThreadProgress> progress;
    /** \brief thread object for this job */
    QPointer<QFRDRImagingFCSCorrelationJobThread> thread;
    /** \brief name of the input file */
    QString filename;
    /** \brief name of the background file */
    QString filenameBackground;
    /** \brief name of the input file description */
    QString descriptionFilename;
    /** \brief fileformat of the input file */
    int fileFormat;
    /** \brief correlator to use */
    int correlator;
    /** \brief background correction mode */
    int backgroundCorrection;
    /** \brief offset to substract from frames */
    float backgroundOffset;
    /** \brief correlator: number of linear correlators */
    int S;
    /** \brief correlator: channels per lin. correlator */
    int P;
    /** \brief correlator: m factor */
    int m;
    /** \brief first frame to calculate */
    int32_t range_min;
    /** \brief last frame to calculate */
    int32_t range_max;
    /** \brief add job results to project */
    bool addToProject;
    /** \brief duration of a single frame */
    double frameTime;
    /** \brief output prefix */
    QString prefix;
    /** \brief output postfix */
    QString postfix;
    /** \brief calculate ACF of each pixel */
    bool acf;
    /** \brief calculate the CCF to the nearest neighbour pixels */
    bool ccf;
    /** \brief statistics over frames */
    uint32_t statistics_frames;
    /** \brief background statistics over frames */
    uint32_t backstatistics_frames;
    /** \brief calculate statistics */
    bool statistics;
    /** \brief create video */
    bool video;
    /** \brief average over this number of frames, if creating a video */
    uint32_t video_frames;
    /** \brief cut the input sequencein thsi number of segments and calculate and average + stddev for every point in the ACF/CCF */
    int32_t segments;
    /** \brief binning factor */
    int binning;
    /** \brief average instead of sum in binning? */
    bool binAverage;
    /** \brief en-/disables cropping */
    bool use_cropping;
    /** \brief for cropping mode: first x-value */
    int crop_x0;
    /** \brief for cropping mode: last x-value */
    int crop_x1;
    /** \brief for cropping mode: first y-value */
    int crop_y0;
    /** \brief for cropping mode: last y-value */
    int crop_y1;
    /** \brief indicates whether distance-CCF is activated */
    bool distanceCCF;
    /** \brief distanceCCF Delta x */
    QList<int> DCCFDeltaX;
    /** \brief distanceCCF Delta y */
    QList<int> DCCFDeltaY;
    /** \brief role names for DCCFs */
    QStringList DCCFrole;
    /** \brief bleach correction, 0: none, 1: remove frame average, 2: remove mono-exponential */
    int bleach;

    /** \brief number of frames to average over when determining the bleach correction parameters */
    uint32_t bleachAvgFrames;
    /** \brief use interleaved binning */
    bool interleaved_binning;

    /** \brief indicates whether to use the following camera settings */
    bool cameraSettingsGiven;
    /** \brief width of camera+microscope pixels (in image plane) in nm */
    double cameraPixelWidth;
    /** \brief height of camera+microscope pixels (in image plane) in nm */
    double cameraPixelHeight;

    int dualViewMode;
    bool addFCCSSeparately;
    bool addNandB;

};

/*! \brief this thread does all the correlation work
    \ingroup qf3rdrdp_imaging_fcs

    This class actually implements the code that executes the correlation jobs defined in Job.
    These jobs (together with objects of this class QFRDRImagingFCSCorrelationJobThread) are usually
    created by QFRDRImagingFCSCorrelationDialog.

    This class is a QThread decendent and the real data processing is done in the run() method.
    Befor calling it, make sure to call the ini() method that initialises the Job to be executed.
    Errors and progress are reported over these signals (compatible to QFRDRImagingFCSThreadProgress):
      - messageChanged() display a new message
      - progressChanged() update the absolute value of the progress bar
      - progressIncrement() increment the progress bar
      - rangeChanged() change the range of the progress bar
      - statusChanged() processing status changes: -1: error, 0: waiting, 1: processing, 2: successfully done
    .
    the status as reported by statusChanged() may also be read using status().

    The methods getImageFilterList() and getImageFormatNameList() return lists of the available image format
    filters (filter string for file open dialogs and names for the filters). getImageReader() returns an
    instances of QFRDRImageReader, usable to read frames.

    The correlation process does this \em [progress]:
      - \b (1) \em [10] open input file
      - \b (2) \em [10] count the number of frames in the input file
      - \b (3) \em [1000] go through all frames and calculate the image series statistics ( the progress is possibly split, if two or more passes are needed):
        - \b (3.1) one frame with the averaged grey value over eachpixel separately (average frame)
        - \b (3.2) min/max grey value over all frames (used e.g. for background correction and video color range correction)
        - \b (3.3) average/standard deviation/min/max grey value over Job:statistics_frames frames for each datapoint
      .
      - \b (4) \em [100] set background correction value (possibly as calculated in (3)) and also apply the bleaching correction
      - \b (5) \em [1000] calculate ACFs
      - \b (6) \em [1000] calculate the CCFs
      - \b (7) \em [1000] calculate the distance CCF
      - \b (8) save results:
        - \b (8.1) \em [10] average frame \c avgFrame as 16-bit uint TIFF  (\c basename.overview.tif ), with color scaling: smallest \c avgFrame value -> 0, largest \c avgFrame value -> 65535
        - \b (8.2) \em [100] video as multi-frame 16-bit uint TIFF (\c basename.video.tift ), with color scaling over ALL frames in video: smallest value -> 0, largest value -> 65535
        - \b (8.3) \em [10] statistics as comma-separated file (\c basename.statistics.dat ) in the format \verbatim    time[seconds], average, stddev, min, max  \endverbatim
        - \b (8.4) \em [10] ACF as comma-separated file (\c basename.autocorrelation.dat ) in the format \verbatim    lag time[seconds], acf, acf_error  \endverbatim The \c acf_error is optional.
        - \b (8.5) \em [10] CCF as comma-separated file (\c basename.crosscorrelation.dat ) in the format \verbatim    lag time[seconds], ccf_left, ccf_right, ccf_top, ccf_bottom, ccf_left_error, ccf_right_error, ccf_top_error, ccf_bottom_error  \endverbatim The \c ccf_errors are optional.
        - \b (8.6) \em [10] distance CCF as comma-separated file (\c basename.dccf.dat ) in the format \verbatim    lag time[seconds], dccf, dccf_error  \endverbatim The \c dccf_errors are optional.
        - \b (8.7) \em [10] settings (\c basename.evalsettings.txt ) in the format \verbatim    settings_name   : value  \endverbatim
      .
    .

    The \b bleaching \b correction fits a simple exponential model \f[ f(t)=A\cdot\exp\left(-t/\tau\right) \f] to  subset of every pixel's timeseries \f$ I(x,y,t) \f$. Before fitting the parameters are already guessed from the
    first and last datapoint in the timeseries. The timeseries will contain roughly 500 equally spaced frames from the original series. Each of these frames
    can be an average over IMFCSJob::bleachAvgFrames subsequent frames. A second possibility is to normalize all frames to a common intensity.


*/
class QFRDRImagingFCSCorrelationJobThread : public QThread {
        Q_OBJECT
    public:
        enum FileTypes {
            ftCorrelation=0,
            ftNandB=1
        };

        struct Fileinfo{                
            Fileinfo(const QString& filename=QString(""), const QString& role=QString(""), int internalDualViewMode=0, int dualViewID=0, bool isNandB=true, const QString &group=QString(""));
            QString filename;
            QString filenameVar;
            QString filenameBack;
            QString filenameBackVar;
            QString filenameEvalSettings;
            QString role;
            QString group;
            int internalDualViewMode;
            int dualViewID;
            FileTypes filetype;
            QMap<QString, QVariant> props;
            QString comment;
        };

        Fileinfo getFileInfo(const QString& filename, const QString& filenameEvalSettings, const QString& role="", const QString& group="");
        Fileinfo getFileInfo(const QString& filename, const QString& filenameEvalSettings, const QString& role, int dualViewID, const QString& group="");
        Fileinfo getFileInfoNandB(const QString& filename, const QString& filenameVar, const QString& filenameBack, const QString &filenameBackVar, const QString &filenameEvalSettings, const QString &role, int dualViewID, const QString& group=QString(""));
        QString getGroupName() const;

        explicit QFRDRImagingFCSCorrelationJobThread(QFPluginServices* services, QObject *parent = 0);
        ~QFRDRImagingFCSCorrelationJobThread();
        int status() const;
        void init(IMFCSJob job);
        static QStringList getImageFilterList(QFPluginServices* pluginservices);
        static QStringList getImageFormatNameList(QFPluginServices *pluginservices);
        static QFImporterImageSeries* getImageReader(int idx, QFPluginServices* pluginservices);
        static int getImageReaderCount(QFPluginServices* pluginservices);
        QList<Fileinfo> getAddFiles() const;
        IMFCSJob getJob() const;
        double durationMS() const;
        double durationS() const;
    signals:
        void messageChanged(QString message);
        void progressChanged(int progress);
        void progressIncrement(int progress_inc);
        void rangeChanged(int min, int max);
        void statusChanged(int status);
    public slots:
        void cancel();
    protected:
        inline double bleachCorrectExp(const double &intensity, int i, double t);
        inline double bleachCorrectExpPoly2(const double &intensity, int i, double t);
        inline double bleachCorrectExpPoly3(const double &intensity, int i, double t);
        inline double bleachCorrectExpPoly4(const double &intensity, int i, double t);
        inline double bleachCorrectExpPoly5(const double &intensity, int i, double t);
        inline double bleachCorrectDblExp(const double &intensity, int i, double t);

        /*! \brief replace some special substrings in the output filename pre- and postfix

            These sequences are replaced (case-insensitive):
              - \c %counter% by the value of \a counter, if this value is >0, or by an empty string
              - \c %S% S parameter of the correlator
              - \c %P% P parameter of the correlator
              - \c %M% m parameter of the correlator
              - \c %framerate% framerate in Herz
              - \c %frametime% frametime in microseconds
              - \c %first% first used frame
              - \c %last% last used frame
              - \c %backoffset% offset of background correction
              - \c %backcorrection% background correction mode
              - \c %backcorrectionid% background correction mode as number
              - \c %segments% segments the image series is cut into
              - \c %correlator% correlator type used
              - \c %correlatorid% correlator type used as number
              - \c %bleach% bleach correction type
            .
          */
        QString replacePostfixSpecials(const QString& input, int counter=-1) const;


        /** \brief the run method of the thread */
        void run();

        /** \brief calculate correlation functions, statistics, video, ... by loading the whole sequence into memory */
        void correlate_loadall();
        /** \brief calculate correlation functions, statistics, video, ... by loading the whole sequence into memory */
        void correlate_loadsingle();
        /** \brief calculate the background correction from the data gathered from a first run through the data */
        void calcBackgroundCorrection();
        void calcBleachCorrection(float* fit_frames, double *fit_t, int NFitFrames=0);

        /*! \brief calculate the CCF between the given timeseries

            This function calculates the CCF between the two positions in the given image series that are shifted by (shiftX, shiftY).
            A CCF for every pixel is calculated and stored in ccf, ccf_tau and ccf_std, if the pixels and its shifted counterpart are
            both inside the image area.
         */
        void correlate_series(float* image_series, uint32_t frame_width, uint32_t frame_height, uint32_t shiftX, uint32_t shiftY, uint64_t frames, double** ccf_tau, double** ccf, double** ccf_std, uint32_t& ccf_N, const QString& message, uint32_t increment_progress=250, double **ccf_segments_io=NULL);


        /*! \brief contribute the data in the given image to the given correlators

        */
        void contribute_to_correlations(QList<MultiTauCorrelator<double, double> *> &ccfjk, QList<correlatorjb<double, double> *> &ccfjb, float *frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t shiftX, uint32_t shiftY, uint64_t frame, uint64_t segment_frames, double *ccf_tau, double *ccf, double *ccf_std, uint64_t ccf_N);
        void average_ccfs(double **acf, double **acf_std, uint32_t acf_N, uint32_t frame_width, uint32_t frame_height, uint32_t segments);
        void prepare_ccfs(QList<MultiTauCorrelator<double, double> *> &acfjk, QList<correlatorjb<double, double> *> &acfjb, double **acf, double **acf_std, double** acf_t, uint32_t& acf_N, uint32_t frame_width, uint32_t frame_height, uint32_t segments);

        /*! \brief store a set of correlation functions as a CSV file */
        bool saveCorrelationCSV(const QString& filename, double *corrTau, double **corrs, double** correrrs, uint32_t corrN, uint32_t N, uint32_t width, uint32_t height, double input_length, QString& error, int progress_steps=0, double segmentLengthSeconds=-1) ;
        /*! \brief store a set of correlation functions as a binary file

            Format description:
              - This format may store several (corrN) different correlation curves for several pixel (e.g. crosscorrelations to four neighbor pixels),
                but usually this feature is not used and corrN=1
              - all correlation curves have to have the same time axis ("lag times in seconds") and all curves have to have the same number N of values
              - the file will contain data for width*height pixels, ordered in row-major form
              - The file is split into two parts: The first part contains a complete set of correlation curves + (if sets=2) errors.
                The second part may contain the single-segment curves the averages in the first part are calculated from
            .

            The binary file has this data layout:
\verbatim
data                                                   size [bytes]                    datatype
+---------------------------------------------------+
| "QF3.1imFCS" or "QF3.0imFCS"               file id|  10                              string
+---------------------------------------------------+
| width                               width of image|  4                               uint
| height                             height of image|  4                               uint
| corrN                 correlation curves per pixel|  4                               uint
| N                        datapoints/lags per curve|  4                               uint
| sets 1: cfs only  2: cfs + errors                 |  4                               uint
+---------------------------------------------------+
| lag times in seconds                              |  8*N                             double
+---------------------------------------------------+
| cf[0] of pixel[0]                                 |  8*N                             double
| cf[1] of pixel[0]                                 |  8*N                             double
| ...                                               |
| cf[corN-1] of pixel[0]                            |  8*N                             double
+---------------------------------------------------+     --+
| errcf[0]  of pixel[0]                             |  8*N  |                          double
| errcf[1]  of pixel[0]                             |  8*N  | only if sets==2          double
| ...                                               |       | or sets==4               double
| errcf[corN-1]  of pixel[0]                        |  8*N  |                          double
+---------------------------------------------------+     --+
+---------------------------------------------------+
| cf[0] of pixel[1]                                 |  8*N                             double
| cf[1] of pixel[1]                                 |  8*N                             double
| ...                                               |
| cf[corN-1] of pixel[1]                            |  8*N                             double
+---------------------------------------------------+     --+
| errcf[0]  of pixel[1]                             |  8*N  |                          double
| errcf[1]  of pixel[1]                             |  8*N  | only if sets==2          double
| ...                                               |       | or sets==4               double
| errcf[corN-1]  of pixel[1]                        |  8*N  |                          double
+---------------------------------------------------+     --+
.                                                   .
.                                                   .
.                                                   .
+---------------------------------------------------+
| cf[0] of pixel[width*height-1]                    |  8*N                             double
| cf[1] of pixel[width*height-1]                    |  8*N                             double
| ...                                               |
| cf[corN-1] of pixel[width*height-1]               |  8*N                             double
+---------------------------------------------------+     --+
| errcf[0]  of pixel[width*height-1]                |  8*N  |                          double
| errcf[1]  of pixel[width*height-1]                |  8*N  | only if sets==2          double
| ...                                               |       | or sets==4               double
| errcf[corN-1]  of pixel[width*height-1]           |  8*N  |                          double
+---------------------------------------------------+     --+
\endverbatim
            This data format may also store all the intermedate correlation curves that are averaged to yield avg+/-stddev. These are stored after the above
            described data and their presence is indicatedsimply by an ongoing file.
            Here the data is organized pixel-by-pixel (row-major), where each pixel is a record containing the segments one after the other, where each
            segemnt contains the corrN correlation functions of length N.

            The data in the second part of the file is only readable, if the file ID is \c "QF3.1imFCS"!!!

\verbatim
data                                                   size [bytes]                   datatype
.                                                   .
. ...   START OF FILE                               .
.                                                   .
+---------------------------------------------------+
| segments                   segments/runs per pixel|  4
+---------------------------------------------------+
| segment[0] cf[0] of pixel[0]                      |  8*N                             double
| segment[0] cf[1] of pixel[0]                      |  8*N                             double
| ...                                               |
| segment[0] cf[corN-1] of pixel[0]                 |  8*N                             double
+---------------------------------------------------+
| segment[1] cf[0] of pixel[0]                      |  8*N                             double
| segment[1] cf[1] of pixel[0]                      |  8*N                             double
| ...                                               |
| segment[1] cf[corN-1] of pixel[0]                 |  8*N                             double
+---------------------------------------------------+
.                                                   .
.                                                   .
.                                                   .
+---------------------------------------------------+
| segment[segments-1] cf[0] of pixel[0]             |  8*N                             double
| segment[segments-1] cf[1] of pixel[0]             |  8*N                             double
| ...                                               |
| segment[segments-1] cf[corN-1] of pixel[0]        |  8*N                             double
+---------------------------------------------------+
+---------------------------------------------------+
| segment[0] cf[0] of pixel[1]                      |  8*N                             double
| segment[0] cf[1] of pixel[1]                      |  8*N                             double
| ...                                               |
| segment[0] cf[corN-1] of pixel[1]                 |  8*N                             double
+---------------------------------------------------+
| segment[1] cf[0] of pixel[1]                      |  8*N                             double
| segment[1] cf[1] of pixel[1]                      |  8*N                             double
| ...                                               |
| segment[1] cf[corN-1] of pixel[1]                 |  8*N                             double
+---------------------------------------------------+
.                                                   .
.                                                   .
.                                                   .
.                                                   .
.                                                   .
.                                                   .
+---------------------------------------------------+
| segment[segments-1] cf[0] of pixel[width*height-1]|  8*N                             double
| segment[segments-1] cf[1] of pixel[width*height-1]|  8*N                             double
| ...                                               |
| segment[segments-1] cf[corN-1] of pixel[width*height-1] |  8*N                       double
+---------------------------------------------------+
\endverbatim
            \note All numbers are stored in little-endian form!!! and the pixel order is always row major!
            \note There was a bug in older QuickFit 3 versions so the second part of the file is scrambled. This is ONLY the case
                  for file version \c "QF3.0imFCS" files with the version tag \c "QF3.1imFCS" can be read completely!
        */
        bool saveCorrelationBIN(const QString& filename, double *corrTau, double** corrs, double** correrrs, uint32_t corrN, uint32_t N, uint32_t width, uint32_t height, double **corrSegments, QString& error, int progress_steps=0, double segmentLengthSeconds=-1) ;

        struct StatisticsDataset {
                QVector<float> time;
                QVector<float> mean;
                QVector<float> std;
                QVector<float> min;
                QVector<float> max;

                void clear();

        };
        struct contribute_to_statistics_state {
            float sum;
            float sum2;
            bool statFirst;
            float* video_frame;
            float sframe_max;
            float sframe_min;
            uint64_t cnt;

            explicit contribute_to_statistics_state(int size);
            ~contribute_to_statistics_state();

        };

        void contribute_to_statistics(contribute_to_statistics_state& state, float* frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t frame, uint32_t frames, float** average_frame, float** sqrsum_frame, float** video, uint32_t& video_frame_num, float& frames_min, float& frames_max, StatisticsDataset& statistics, bool isBackground=false);
        void contribute_to_dv2_statistics(contribute_to_statistics_state& state1, contribute_to_statistics_state& state2, float* frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t frame, uint32_t frames, StatisticsDataset &statistics1, StatisticsDataset &statistics2, bool isBackground=false);

        bool saveStatistics(const QString& filename, const QString& filename_plot, const StatisticsDataset& statistics, const QString& title=QString("Statistics"), QString* error=NULL);
        bool SaveSDTIFF(const QString &filename, float *average_frame, float *sqrsum_frame, int frame_width, int frame_height, int frames, const QString& title=QString("Standard Deviation File"), QString* error=NULL);
        bool SaveTIFFFloat(const QString &filename, float *average_frame, int frame_width, int frame_height, const QString &title=QString("Overview File"), QString *error=NULL);
        bool SaveTIFFUInt16_scaled(const QString &filename, float *average_frame, int frame_width, int frame_height, const QString &title=QString("Overview File"), QString *error=NULL);

        int m_status;
        bool was_canceled;

        /** \brief the Job object to work on */
        IMFCSJob job;
        double duration;

        /** \brief this reader object is used to read the frames from the file */
        QFImporterImageSeries* reader;
        uint32_t frames;
        uint32_t first_frame;
        uint32_t frame_width;
        uint32_t frame_height;
        float* average_frame;
        float* sqrsum_frame;
        float* average_uncorrected_frame;
        float* sqrsum_uncorrected_frame;
        float* video;
        float* video_uncorrected;
        uint32_t video_count;
        uint32_t real_video_count;
        float frames_min;
        float frames_max;
        float baseline;



        StatisticsDataset statistics;
        StatisticsDataset backstatistics;
        StatisticsDataset statistics_uncorrected;
        StatisticsDataset dv_statistics[2];
        StatisticsDataset dv_backstatistics[2];
        StatisticsDataset dv_statistics_uncorrected[2];


        double* acf_tau;
        double* acf;
        double* acf_segments;
        double* acf_std;
        uint32_t acf_N;
        double* ccf_tau;
        double* ccf1;
        double* ccf2;
        double* ccf3;
        double* ccf4;
        double* ccf1_segments;
        double* ccf2_segments;
        double* ccf3_segments;
        double* ccf4_segments;
        double* ccf1_std;
        double* ccf2_std;
        double* ccf3_std;
        double* ccf4_std;
        uint32_t ccf_N;

        struct DCCFRecord {
            double* dccf_tau;
            double* dccf;
            double* dccf_segments;
            double* dccf_std;
            uint32_t dccf_N;
            uint32_t dccfframe_width;
            uint32_t dccfframe_height;
        };
        QList<DCCFRecord> dccf;

        QLocale outLocale;
        QList<Fileinfo> addFiles;
        QString outputFilenameBase;

        float* backgroundImage;
        float* backgroundImageStd;
        float* bleachPercentage;
        float* bleachAmplitude;
        float* bleachTime;
        float* bleachAmplitude2;
        float* bleachTime2;
        float* bleachPoly2;
        float* bleachPoly3;
        float* bleachPoly4;
        float* bleachPoly5;
        uint8_t* bleachFitOK;
        float* firstFrames;
        float* lastFrames;

        float* fit_frames;
        double* fit_t;
        int NFitFrames;

        static QMutex* mutexFilename;

        QFPluginServices* pluginservices;

        FileTypes filetype;
        QMap<QString, QVariant> props;
        QString comment;

};

#endif // QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
