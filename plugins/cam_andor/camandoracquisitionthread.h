#ifndef CAMANDORACQUISITIONTHREAD_H
#define CAMANDORACQUISITIONTHREAD_H


#include <QThread>
#include <QString>
#include <QStringList>
#include <inttypes.h>
#include <QFile>
#include "tinytiffwriter.h"

/*! \brief Thread used for image series acquisitions by QFExtensionCameraAndor
    \ingroup qf3ext_andor

 */
class CamAndorAcquisitionThread : public QThread {
        Q_OBJECT
    public:
        /** Default constructor */
        CamAndorAcquisitionThread(QObject* parent=NULL);
        /** Default destructor */
        virtual ~CamAndorAcquisitionThread();
        /** \brief initialize this thread */
        bool init(int camera, QString filenamePrefix, int fileformat, int64_t numKinetics, int width, int height, double exposureTime, QString log_prefix);
        /** \brief progress of acquisition 0..100 */
        double getProgress() { return progress; };

        QStringList getOutputFilenames() { return outputFilenames; }
        QStringList getOutputFilenameTypes() { return outputFilenameTypes; }
    public slots:
        /** \brief cancel the acquisition */
        void cancelAcquisition() { canceled=true; }

    signals:
        void log_error(QString message);
        void log_text(QString message);
        void log_warning(QString message);
    protected:
        int m_camera;
        QString m_filenamePrefix;
        int m_fileformat;
        double m_exposureTime;
        int64_t m_numKinetics;
        int m_width;
        int m_height;
        bool spooling;
        bool canceled;
        double progress;
        QString m_log_prefix;
        QStringList outputFilenames;
        QStringList outputFilenameTypes;

        TinyTIFFFile* tiff;
        QFile* raw;

        virtual void run();
    private:
};

#endif // CAMANDORACQUISITIONTHREAD_H
