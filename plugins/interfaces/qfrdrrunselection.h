#ifndef QFRDRRUNSELECTIONINTERFACE_H
#define QFRDRRUNSELECTIONINTERFACE_H

#include <QtPlugin>
#include <QString>
#include <QList>
#include <QChar>

/*! \brief interface for run selection/leaveout
    \ingroup qf3rdrdp_fcs

*/
class QFRDRRunSelectionsInterface {
    public:
        virtual ~QFRDRRunSelectionsInterface() {};


        /** \brief returns the number of runs in the record */
        virtual int leaveoutGetRunCount() const=0;
        /** \brief returns whether to leave out a run */
        virtual bool leaveoutRun(int run) const=0 ;
        /** \brief add a run to the leaveouts */
        virtual void leaveoutAddRun(int run)=0;
        /** \brief remove a run from the leaveouts */
        virtual void leaveoutRemoveRun(int run)=0;
        /** \brief clear all leaveouts */
        virtual void leaveoutClear()=0;




        virtual void leaveoutLoad(const QString& filename)=0;
        virtual void leaveoutLoadFromString(const QString &leaveoutstring)=0;
        virtual void leaveoutSave(const QString& filename) const =0;
        virtual QString leaveoutToString() const=0;
        virtual QString leaveoutToIndexString(QChar separator=QChar(',')) const=0;
        virtual void leaveoutLoadFromIndexString(const QString& data, QChar separator=QChar(','))=0;
        virtual void leaveoutSetAll()=0;
        virtual void leaveoutInvert()=0;
        virtual QList<int> leaveoutToIndexList()=0;
        virtual void leaveoutChangedEvent()=0;


};

Q_DECLARE_INTERFACE( QFRDRRunSelectionsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRRunSelectionsInterface/1.1")

#endif // QFRDRRUNSELECTIONINTERFACE_H
