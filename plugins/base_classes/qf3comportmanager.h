#ifndef QF3COMPORTMANAGER_H
#define QF3COMPORTMANAGER_H
#include <QVector>
#include <QList>
#include <QString>
#include <QSettings>
#include <QMutex>
#include "../../../../../LIB/trunk/jkserialconnection.h"

/*! \brief you may use this class to manage a list of COM ports.
    \ingroup qf3extensionplugins

    The function addCOMPort() reads a port configuration from QSettings. If the given port is already in the list,
    its ID is returned, otherwise the port is added.
 */

class QF3ComPortManager {
    public:
        QF3ComPortManager();
        ~QF3ComPortManager();

        void clear();

        int addCOMPort(QSettings& settings, QString prefix=QString(""));
        void storeCOMPort(int port, QSettings& settings, QString prefix=QString(""));

        JKSerialConnection* getCOMPort(int port) const;
        QMutex* getMutex(int port) const;
    protected:
        struct COMPORTS {
            /** \brief serial connection object */
            JKSerialConnection* com;
            QMutex* mutex;
        };
        QList<COMPORTS> coms;
};

#endif // QF3COMPORTMANAGER_H
