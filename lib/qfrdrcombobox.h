#ifndef QFRDRCOMBOBOX_H
#define QFRDRCOMBOBOX_H

#include <QComboBox>
#include "lib_imexport.h"
#include "qfrawdatarecord.h"
#include "qfproject.h"
#include "qfmatchrdrfunctor.h"


/*! \brief combobox that displays a list of all raw data records matching a given QFMatchRDRFunctor in a given project
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFRDRComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRComboBox(QWidget *parent = 0);
        
        void init(QFProject* project, QFMatchRDRFunctor* matchFunctor=NULL);
        QFRawDataRecord* currentRDR() const;
    signals:
        void currentRDRChanged(QFRawDataRecord* record);
    public slots:
        void setCurrentRDR(const QFRawDataRecord* record);
    protected slots:
        void myCurrentIndexChanged(int i);
    protected:
        QFProject* project;
        QFMatchRDRFunctor* matchFunctor;
        
};

#endif // QFRDRCOMBOBOX_H
