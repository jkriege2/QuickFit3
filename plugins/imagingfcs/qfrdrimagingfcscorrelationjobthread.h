#ifndef QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
#define QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H

#include <QThread>
#include <QPointer>
#include "qfrdrimagereader.h"
#include <QLocale>
#include <QList>
#include <QVector>
#include <QStringList>

class QFRDRImagingFCSThreadProgress; // forward
class QFRDRImagingFCSCorrelationJobThread; // forward

/*! \brief job description for correlation
    \ingroup qf3rdrdp_imaging_fcs

    The jobs are processed by QFRDRImagingFCSCorrelationJobThread objects (in the run() method) that are
    usually called from a QFRDRImagingFCSCorrelationDialog dialog instance.

    A job completely describes the correlations to be done for an image series.

    For a detailed description of how a job is executed, please refer to the
    documentation of QFRDRImagingFCSCorrelationJobThread which actually contains
    the code that executed the job.
*/
struct Job {
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
    int DCCFDeltaX;
    /** \brief distanceCCF Delta y */
    int DCCFDeltaY;
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
      - \b (4) \em [100] set background correction value (possibly as calculated in (3))
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

*/
class QFRDRImagingFCSCorrelationJobThread : public QThread
{
    Q_OBJECT
public:
    explicit QFRDRImagingFCSCorrelationJobThread(QObject *parent = 0);
    ~QFRDRImagingFCSCorrelationJobThread();
    int status() const;
    void init(Job job);
    static QStringList getImageFilterList();
    static QStringList getImageFormatNameList();
    static QFRDRImageReader* getImageReader(int idx);
    static int getImageReaderCount();
    QStringList getAddFiles() const;
    Job getJob() const;
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
        .
      */
    QString replacePostfixSpecials(const QString& input, int counter=-1) const;
    void run();
    void correlate_loadall();
    void correlate_loadsingle();
    void calcBackgroundCorrection();
    int m_status;
    bool was_canceled;
    Job job;
    double duration;

    QFRDRImageReader* reader;
    uint32_t frames;
    uint32_t first_frame;
    uint32_t frame_width;
    uint32_t frame_height;
    float* average_frame;
    float* video;
    uint32_t video_count;
    uint32_t real_video_count;
    float frames_min;
    float frames_max;
    float baseline;
    QVector<float> statistics_time;
    QVector<float> statistics_mean;
    QVector<float> statistics_std;
    QVector<float> statistics_min;
    QVector<float> statistics_max;
    double* acf_tau;
    double* acf;
    double* acf_std;
    uint32_t acf_N;
    double* ccf_tau;
    double* ccf1;
    double* ccf2;
    double* ccf3;
    double* ccf4;
    double* ccf1_std;
    double* ccf2_std;
    double* ccf3_std;
    double* ccf4_std;
    uint32_t ccf_N;
    double* dccf_tau;
    double* dccf;
    double* dccf_std;
    uint32_t dccf_N;
    uint32_t dccfframe_width;
    uint32_t dccfframe_height;
    QLocale outLocale;
    QStringList addFiles;
    QString outputFilenameBase;

    float* backgroundImage;

};

#endif // QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
