#ifndef QFMATCHRDRFUNCTOR_H
#define QFMATCHRDRFUNCTOR_H

#include "lib_imexport.h"
#include "qfrawdatarecord.h"
#include "qfproject.h"
#include <QList>
#include <QPointer>

/*! \brief derive a functor from this class which specifies which QFRawDataRecords to be selected for a list
    \ingroup qf3lib_tools

    You can either just use the method matches() to check whether a record matches the defined conditions or call getFilteredList()
    which returns a list of all records in the given project that match the conditions.

    When deriving a new function you will only have to implement matches().
  */
class QFLIB_EXPORT QFMatchRDRFunctor {
    public:
        explicit QFMatchRDRFunctor();
        virtual ~QFMatchRDRFunctor();
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const =0;
        /** \brief returns a list of all QFRawDataRecords that match this functor in a project */
        QList<QPointer<QFRawDataRecord> > getFilteredList(QFProject* project);
};

/*! \brief  QFMatchRDRFunctor that select all records
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFMatchRDRFunctorSelectAll: public QFMatchRDRFunctor {
    public:
        explicit QFMatchRDRFunctorSelectAll();
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const ;
};


/*! \brief QFMatchRDRFunctor that select all records with a given type
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFMatchRDRFunctorSelectType: public QFMatchRDRFunctor {
    public:
        explicit QFMatchRDRFunctorSelectType(const QString& type);
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const ;

        QString getType() const;
        void setType(const QString& type);
    protected:
        QString type;
};

#endif // QFMATCHRDRFUNCTOR_H
