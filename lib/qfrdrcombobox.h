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
        ~QFRDRComboBox();
        
        /** \brief initialises/fills the combobox from the given project and using the given functor.
          *
          * If \a functorPrivate is \c true the functor belongs to this object and will be deleted if no longer needed
          */
        virtual void init(QFProject* project, QFMatchRDRFunctor* matchFunctor, bool functorPrivate=false);
        QFRawDataRecord* currentRDR() const;
        int currentRDRID() const;
    signals:
        void currentRDRChanged(QFRawDataRecord* record);
        void refilledEmpty();
        void refilled(bool full);
    public slots:
        void setCurrentRDR(const QFRawDataRecord* record);
        void setCurrentRDRID(int record);
        void refill();

    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFProject* project;
        QFMatchRDRFunctor* matchFunctor;
        bool functorPrivate;
        
};

#endif // QFRDRCOMBOBOX_H
