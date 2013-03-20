#ifndef QFAUTOOUTPUTTIMER_H
#define QFAUTOOUTPUTTIMER_H

#include <QElapsedTimer>
#include <QString>
#include "lib_imexport.h"

class QFLIB_EXPORT QFAutoOutputTimer : public QElapsedTimer
{
    public:
        explicit QFAutoOutputTimer(const QString& message);
        ~QFAutoOutputTimer();
    protected:
        QString message;
        
};

#endif // QFAUTOOUTPUTTIMER_H
