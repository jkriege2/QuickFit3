#ifndef QFENHANCEDCOMBOBOX_H
#define QFENHANCEDCOMBOBOX_H

#include <QComboBox>
#include "lib_imexport.h"
#include <QVariant>


/*! \brief enhanced QComboBox
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFEnhancedComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFEnhancedComboBox(QWidget *parent = 0);
        QVariant currentData(int role=Qt::UserRole) const;

    public slots:
        void setCurrentData(const QVariant& data, int role=Qt::UserRole);
    signals:

    public slots:

};

#endif // QFENHANCEDCOMBOBOX_H
