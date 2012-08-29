#ifndef QFFITALGORITHMCOMBOBOX_H
#define QFFITALGORITHMCOMBOBOX_H

#include <QComboBox>
#include "qffitalgorithm.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFFitAlgorithmComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFFitAlgorithmComboBox(QWidget *parent = 0);
        QString currentFitAlgorithmID() const;
        QFFitAlgorithm* createCurrentInstance(QObject *parent=NULL) const;
    signals:

    public slots:
        void updateFitAlgorithms();
        void setCurrentAlgorithm(const QString& id);

};

#endif // QFFITALGORITHMCOMBOBOX_H
