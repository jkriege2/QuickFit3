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
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QVector<double> > &data, int format, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void copyCSV(const QList<QList<double> >& data, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
        /*! \brief save the given \a data in the given \A format (index into the list, as returned by dataGetFormats() )
         */
        static void save(const QList<QList<QVariant> > &data, int format, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
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
};

#endif // DATATOOLS_H
