#ifndef QFRDRRUNSELECTIONINTERFACE_H
#define QFRDRRUNSELECTIONINTERFACE_H

#include <QtPlugin>

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


};

Q_DECLARE_INTERFACE( QFRDRRunSelectionsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRRunSelectionsInterface/1.1")

#endif // QFRDRRUNSELECTIONINTERFACE_H
