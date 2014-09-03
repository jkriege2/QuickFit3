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
#include <QSet>
#include <QPair>

// TODO: reimplement this properly, so ACF0 and ACF1 in FCCS evals see both count rates ... possibly use an internal binary dataformat?
// TODO: add bleach correction

class QFETCSPCImporterThreadProgress; // forward
class QFETCSPCImporterJobThread; // forward

#define CORRELATOR_MTAUALLMON 0
#define CORRELATOR_MTAUONEMON 1


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
    /** \brief parameter for TCSPC importer */
    QString importerParameter;
    /** \brief fileformat of the input file */
    int fileFormat;
    /** \brief correlator to use */
    int fcs_correlator;
    /** \brief correlator: number of linear correlators */
    int fcs_S;
    /** \brief correlator: channels per lin. correlator */
    int fcs_P;
    /** \brief correlator: m factor */
    int fcs_m;
    /** \brief minimum integration time for FCS in seconds */
    double fcs_taumin;
    /** \brief integration time for FCS countrate */
    double fcs_crbinning;
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

    bool doFCS;
    bool doCountrate;

    QSet<QPair<int, int> > fcs_correlate;
    QSet<int> countrate_channels;

    QMap<QString, QVariant> props;
    QString comment;

};

struct QFETCSPCImporterJobThreadAddFileProps {
    QStringList files;
    QString type;
    QMap<QString, QVariant> props;
    QString comment;
    QString group;
    QString role;
    QFETCSPCImporterJobThreadAddFileProps(const QStringList& files, const QString& type, const QMap<QString, QVariant>& props) {
        this->files=files;
        this->type=type;
        this->props=props;
    }
};

/*! \brief this thread does all the evaluation work
    \ingroup qf3ext_tcspcimporter



    This class might save the countrate to a binary file with this format:
\verbatim
data                                                   size [bytes]
+---------------------------------------------------+
| "QF3.0CNTRT"                    file id           |  10
+---------------------------------------------------+
| channels                     channels in this file|  2 (uint16)
| itemsPerChannel      items in each channel's array|  8 (uint64)
| deltaT             duration of one item in seconds|  8 (double)
+---------------------------------------------------+
| input channel 0                                   |  2 (uint16)
| input channel 1                                   |  2 (uint16)
| ...                                               |
| input channel channels-1                          |  2 (uint16)
+---------------------------------------------------+
| cr[0] of channel 0                                |  2 (uint16)
| cr[0] of channel 1                                |  2 (uint16)
| ...                                               |
| cr[0] of channel channels-1                       |  2 (uint16)
+---------------------------------------------------+
.                                                   .
.                                                   .
.                                                   .
+---------------------------------------------------+
| cr[itemsPerChannel-1] of channel 0                |  2 (uint16)
| cr[itemsPerChannel-1] of channel 1                |  2 (uint16)
| ...                                               |
| cr[itemsPerChannel-1] of channel channels-1       |  2 (uint16)
+---------------------------------------------------+
\endverbatim
        \note All numbers are stored in little-endian form!!!
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
    QList<QFETCSPCImporterJobThreadAddFileProps > getAddFiles() const;
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

    void runEval(QFTCSPCReader* reader, QFile *countrateFile);

    /** \brief converts a (row, column) adress into a QHash index: (row<<16) | column */
    inline quint32 xyAdressToUInt32(quint16 row, quint16 column) const {
        quint32 r=row;
        quint32 c=column;
        return c| (r<<16);
    }

    /** \brief extracts the row from a UInt32 adress */
    inline quint16 UInt32ToRow(quint32 a) const {
        quint32 as=a>>16;
        return as & 0xFFFF;
    }

    /** \brief extracts the column from a UInt32 adress */
    inline quint16 UInt32ToColumn(quint32 a) const  {
        return a & 0xFFFF;
    }

    /** \brief converts a (x,y,z) adress into a QHash index*/
    inline quint64 xyzAdressToUInt64(quint16 x, quint16 y, quint16 z) const {
        quint64 r=x;
        quint64 c=y;
        quint64 zz=z;
        return zz|(c<<16)| (r<<32);
    }

    /** \brief extracts the x from a UInt64 adress */
    inline quint16 UInt32ToX(quint64 a) const {
        quint64 as=a>>32;
        return as & 0xFFFF;
    }

    /** \brief extracts the y from a UInt64 adress */
    inline quint16 UInt32ToY(quint64 a) const {
        quint64 as=a>>16;
        return as & 0xFFFF;
    }

    /** \brief extracts the Z from a UInt64 adress */
    inline quint16 UInt32ToZ(quint64 a) const {
        return a & 0xFFFF;
    }

    int m_status;
    bool was_canceled;

    /** \brief the Job object to work on */
    TCSPCImporterJob job;
    double duration;

    /** \brief this reader object is used to read the frames from the file */
    QFTCSPCReader* reader;

    QLocale outLocale;
    QList<QFETCSPCImporterJobThreadAddFileProps > addFiles;
    QString outputFilenameBase;


    static QMutex* mutexFilename;

    QFPluginServices* pluginServices;

    typedef correlatorjb<double, double> corrjb_type;
    typedef MultiTauCorrelator<uint16_t, uint64_t> corrjk_type;

    QMap<uint32_t, corrjb_type*> corrjb;
    QMap<uint32_t, corrjk_type*> corrjk;
    QMap<uint64_t, QVector<double> > fcs_ccfs;
    QMap<uint32_t, QVector<double> > fcs_crs;
    QVector<double> fcs_tau;

    void clearCorrelators();
    void createCorrelators();
    void copyCorrelatorIntermediateResults(uint16_t fcs_segment);
    void shiftIntoCorrelators(uint16_t *fcs_countrate, uint32_t count);

    double starttime;
    double range_duration;
    double file_duration;
    double real_duration;
    uint16_t channels;
    uint64_t countrate_items;
    uint64_t real_countrate_items;

    struct ccfFileConfig {
        QString filename;
        int channel1;
        int channel2;

        QStringList filenameCR;

        bool cr_swapped;

        ccfFileConfig() {
            channel1=0;
            channel2=0;
            filename="";
            filenameCR.clear();
            cr_swapped=false;
        }
    };
};

#endif // QFETCSPCImporterJOBTHREAD_H
