#ifndef CAMANDORACQUISITIONTHREAD_H
#define CAMANDORACQUISITIONTHREAD_H

#include <QThread>

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
    protected:
        virtual void run();
    private:
};

#endif // CAMANDORACQUISITIONTHREAD_H
