#ifndef DATATOOLS_H
#define DATATOOLS_H
#include "csvtools.h"
#include "matlabtools.h"


#include <QString>
#include <QList>
#include <QVector>
#include <QTextStream>
#include <QStringList>
#include "lib_imexport.h"



/*! \brief exchange rows and columns
    \ingroup qf3lib_tools    
*/
QFLIB_EXPORT QList<QVector<double> > dataRotate(const QList<QVector<double> >& data);
/*! \brief exchange rows and columns
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QList<QList<double> > dataRotate(const QList<QList<double> >& data);

/*! \brief exchange rows and columns
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QList<QList<QVariant> > dataRotate(const QList<QList<QVariant> >& data);

/*! \brief if the given table of values contains lists of values (or QSize, QRect, QPoint, QVector ...) these are expanded to additional columns, if columnnames is given, blank items will be inserted as needed
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void dataExpand(QList<QList<QVariant> >& data, QStringList* columnsNames=NULL);


/*! \brief if the given table of values contains lists of values (or QSize, QRect, QPoint, QVector ...) these are reduced to a single value
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void dataReduce(QList<QList<QVariant> >& data, QStringList* columnsNames=NULL);

/*! \brief reduce a variant table and convert to double
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QVector<double> > dataReduceToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames=NULL);

/*! \brief expand a variant table and convert to double
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QVector<double> > dataExpandToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames=NULL);

/*! \brief convert a variant table to double
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QVector<double> > dataToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames=NULL);


/*! \brief convert a double table to a QVariant table
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QList<QVariant> > dataToVariant(const QList<QList<double> >& data);

/*! \brief convert a double table to a QVariant table
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QList<QVariant> > dataToVariant(const QList<QVector<double> >& data);


/*! \brief convert the given data to SYLK
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toSYLK(const QList<QVector<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList());

/*! \brief convert the given data to SYLK
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toSYLK(const QList<QList<QVariant> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList());




/*! \brief this class groups static methods to export to different data formats
    \ingroup qf3lib_tools
*/
class QFLIB_EXPORT QFDataExportHandler {
    public:
        /** \brief you may register an object derived from this class to add new export formats.
         *
         * \note You have to implement at least one of the two save() methods. otherwise, a call to any will lead to an endles loop.
         **/
        class QFLIB_EXPORT DataWriter {
            public:
                virtual ~DataWriter() {}
                virtual QString getFilter() const=0;
                virtual void save(const QList<QVector<double> > &data, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
                virtual void save(const QList<QList<QVariant> > &data, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());

        };
        /** \brief used to register a new data format */
        static void registerDataWriter(DataWriter* writer);

        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QVector<double> > &data, int format, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QVector<double> > &data,const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void copyCSV(const QList<QList<double> >& data, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QList<QVariant> > &data, int format, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QList<QVariant> > &data, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void copyCSV(const QList<QList<QVariant> >& data, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data as a Matlab script
         */
        static void copyMatlab(const QList<QList<QVariant> >& data);
        /*! \brief save the given \a data as a Matlab script
         */
        static void copyMatlab(const QList<QList<double> >& data);
        /*! \brief returns a list of filename filters for the formats to which data may be saves
        */
        static QStringList getFormats();
    protected:
        static QList<DataWriter*> writers;
};

#endif // DATATOOLS_H
