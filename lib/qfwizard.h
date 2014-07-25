#ifndef QFWIZARD_H
#define QFWIZARD_H

#include <QWizard>
#include "lib_imexport.h"

class QFLIB_EXPORT QFWizard : public QWizard
{
        Q_OBJECT
    public:
        explicit QFWizard(QWidget *parent = 0);

    signals:

    public slots:

};

#endif // QFWIZARD_H
