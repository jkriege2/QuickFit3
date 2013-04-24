#ifndef QFRDRIMFCSCORRELATORREMOTE_H
#define QFRDRIMFCSCORRELATORREMOTE_H

#include <QtPlugin>
#include <stdint.h>

/*! \brief allows to remotely control an imFCS correltor plugin
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRIMFCSCorrelatorRemote {
    public:
        virtual ~QFRDRIMFCSCorrelatorRemote() {}

        virtual void imfcsCorrRemoteUserSelectFile()=0;
        virtual void imfcsCorrRemoteSelectFile(const QString& filename)=0;
        virtual void imfcsCorrRemoteAddJob()=0;
        virtual void imfcsCorrRemoteAddJobSeries(const QString& parameter, double start, double end, double inc=1)=0;

};

Q_DECLARE_INTERFACE( QFRDRIMFCSCorrelatorRemote,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRIMFCSCorrelatorRemote/1.0")

#endif // QFRDRIMFCSCORRELATORREMOTE_H
