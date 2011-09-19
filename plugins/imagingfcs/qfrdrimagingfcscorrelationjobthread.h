#ifndef QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
#define QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H

#include <QThread>
#include "qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagereader.h"

/*! \brief this thread does all the correlation work
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSCorrelationJobThread : public QThread
{
    Q_OBJECT
public:
    explicit QFRDRImagingFCSCorrelationJobThread(QObject *parent = 0);
    int status() const;
    void init(Job job);
    static QStringList getImageFilterList();
    static QStringList getImageFormatNameList();
    QStringList getAddFiles();
signals:
    void messageChanged(QString message);
    void progressChanged(int progress);
    void progressIncrement(int progress_inc);
    void rangeChanged(int min, int max);
    void statusChanged(int status);
public slots:
    void cancel();
protected:
    static QList<QFRDRImageReader*> getImageReaderList();
    void run();
    void correlate_loadall();
    void correlate_loadsingle();
    void calcBackgroundCorrection();
    int m_status;
    bool was_canceled;
    Job job;

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
    QLocale outLocale;
    QStringList addFiles;

};

#endif // QFRDRIMAGINGFCSCORRELATIONJOBTHREAD_H
