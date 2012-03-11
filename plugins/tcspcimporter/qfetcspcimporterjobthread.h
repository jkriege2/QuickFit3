#ifndef QFETCSPCImporterJOBTHREAD_H
#define QFETCSPCImporterJOBTHREAD_H

#include <QThread>
#include <QPointer>
#include "qftcspcreader.h"
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
#include "qftcspcreader.h"
#include "qfpluginservices.h"

class QFETCSPCImporterThreadProgress; // forward
class QFETCSPCImporterJobThread; // forward

#define CORRELATOR_DIRECT 0
#define CORRELATOR_DIRECTAVG 1
#define CORRELATOR_MTAUALLMON 2
#define CORRELATOR_MTAUONEMON 3


/*! \brief job description for tcspc evaluation
    \ingroup qf3ext_tcspcimporter

    The jobs are processed by QFETCSPCImporterJobThread objects (in the run() method) that are
    usually called from a QFETCSPCImporterDialog dialog instance.

    A job completely describes the correlations to be done for an image series.

    For a detailed description of how a job is executed, please refer to the
    documentation of QFETCSPCImporterJobThread which actually contains
    the code that executed the job.
*/
struct TCSPCImporterJob {
    /** \brief progress widget for this job */
    QPointer<QFETCSPCImporterThreadProgress> progress;
    /** \brief thread object for this job */
    QPointer<QFETCSPCImporterJobThread> thread;
    /** \brief name of the input file */
    QString filename;
    /** \brief fileformat of the input file */
    int fileFormat;
    /** \brief correlator to use */
    int correlator;
    /** \brief correlator: number of linear correlators */
    int S;
    /** \brief correlator: channels per lin. correlator */
    int P;
    /** \brief correlator: m factor */
    int m;
    /** \brief minimum integration time for FCS in seconds */
    double fcs_taumin;
    /** \brief cut the input sequence in this number of segments and calculate and average + stddev for every point in the ACF/CCF */
    int32_t fcs_segments;
    /** \brief start of range in seconds */
    double range_min;
    /** \brief end of range in seconds */
    double range_max;
    /** \brief add job results to project */
    bool addToProject;
    /** \brief output prefix */
    QString prefix;
    /** \brief output postfix */
    QString postfix;
    /** \brief binning for count rates in seconds */
    double countrate_binning;


};

/*! \brief this thread does all the evaluation work
    \ingroup qf3ext_tcspcimporter


*/
class QFETCSPCImporterJobThread : public QThread
{
    Q_OBJECT
public:
    explicit QFETCSPCImporterJobThread(QFPluginServices* services, QObject *parent = 0);
    ~QFETCSPCImporterJobThread();
    int status() const;
    void init(TCSPCImporterJob job);
    static QStringList getImporterFilterList(QFPluginServices *pluginServices);
    static QStringList getImporterFormatNameList(QFPluginServices *pluginServices);
    static QFTCSPCReader* getImporter(int idx, QFPluginServices *pluginServices);
    static int getImporterCount(QFPluginServices *pluginServices);
    QStringList getAddFiles() const;
    TCSPCImporterJob getJob() const;
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
          - \c %start% first used frame
          - \c %end% last used frame
          - \c %fcs_segments% segments the image series is cut into
          - \c %fcs_taumin% minimum lagtime of FCS
          - \c %cr_binning% countrate binning
          - \c %correlator% correlator type used
          - \c %correlatorid% correlator type used as number
        .
      */
    QString replacePostfixSpecials(const QString& input, int counter=-1) const;


    /** \brief the run method of the thread */
    void run();

    int m_status;
    bool was_canceled;

    /** \brief the Job object to work on */
    TCSPCImporterJob job;
    double duration;

    /** \brief this reader object is used to read the frames from the file */
    QFTCSPCReader* reader;

    QLocale outLocale;
    QStringList addFiles;
    QString outputFilenameBase;


    static QMutex* mutexFilename;

    QFPluginServices* pluginServices;
};

#endif // QFETCSPCImporterJOBTHREAD_H
