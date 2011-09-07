#ifndef QFTABLEMODEL_H
#define QFTABLEMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QVariant>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QTextCodec>
#include <cmath>
#include <QDateTime>
#include <iostream>

/*! \brief this class is used to manage a table of values (QVariant)
    \ingroup qf3rdrdp_table

    This class provides import/export routines for Comma-Separated-Value files (and others):
    \verbatim
# Comment
#! "title1", "title2"
"string", 0.0, 4.56e-6 #comment
"string2", 1.0, 23
'string 3', , 6
    \endverbatim

    data is internally stored as a 2D array of QVariant objects. The 2D array is implemented as follows:
     - The row and column number are given as quint16, i.e. as 16-bit unsigned integers
     - row and columns are combined to a quint32 by calculating: (row<<16) | column
     - This quint32 is used as a QHash index. If a cell is empty either an invalid QVariant is stored, or
       the QHash simply does not contain a QVariant for this cell.
     - row and column count are stored externally.
     - data is stored in a QFTableModel object which is also externally accessible for data access.
    .
 */
class QFTableModel : public QAbstractTableModel {
        Q_OBJECT
    protected:
        /** \brief converts a (row, column) adress into a QHash index: (row<<16) | column */
        inline quint32 xyAdressToUInt32(quint16 row, quint16 column) const {
            quint32 r=row;
            quint32 c=column;
            return c| (r<<16);
        };

        /** \brief extracts the row from a UInt32 adress */
        inline quint16 UInt32ToRow(quint32 a) const {
            quint32 as=a>>16;
            return as & 0xFFFF;
        };

        /** \brief extracts the column from a UInt32 adress */
        inline quint16 UInt32ToColumn(quint32 a) const  {
            return a & 0xFFFF;
        };

        /** \brief the number of rows */
        quint16 rows;
        /** \brief the number of columns */
        quint16 columns;
        /** \brief this map is used to store tha data */
        QHash<quint32, QVariant> dataMap;
        /** \brief this map is used to store tha data for the edit role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
        QHash<quint32, QVariant> dataEditMap;
        /** \brief this map is used to store tha data for the background role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
        QHash<quint32, QVariant> dataBackgroundMap;
        /** \brief this map is used to store tha data for the background role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
        QHash<quint32, QVariant> dataCheckedMap;
        /** \brief string list that contains the column names */
        QStringList columnNames;
        /** \brief indicates whether the model is readonly (via the QAbstractTableModel interface!!!) or not */
        bool readonly;
        /** \brief indicates whether the data has been changed since the last call of resetChanged(); */
        bool hasDataChanged;
    public:
        /** Default constructor */
        QFTableModel(QObject * parent=NULL);
        /** Default destructor */
        virtual ~QFTableModel();

        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const ;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        inline bool isReadonly() const { return readonly; };
        inline void setReadonly(bool r) { readonly=r; emit readonlyChanged(r); emit notReadonlyChanged(!r);};
        inline void resetChanged() { hasDataChanged=false; };
        inline bool hasChanged() const { return hasDataChanged; };

        /** \brief resize the internal table (cells that are out of range will be deleted) */
        void resize(quint16 rows, quint16 columns);
        /** \brief change datatype in cell
         *
         * The type conversion is done in any case. If the current value may not be converted, the cell
         * is set with a default value for the new datatype and \c false is returned, otherweise \c true.
         */
        bool changeDatatype(quint16 row, quint16 column, QVariant::Type newType);
        /** \brief set the given cell to the supplied value */
        void setCell(quint16 row, quint16 column, QVariant value);
        /** \brief returns the value in the supplied cell */
        QVariant cell(quint16 row, quint16 column) const;
        /** \brief set the given cell to the supplied value in EditRole */
        void setCellEditRole(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Background */
        void setCellBackgroundRole(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Checked */
        void setCellCheckedRole(quint16 row, quint16 column, QVariant value);
        /** \brief returns the value in the supplied cell in EditRole */
        QVariant cellEditRole(quint16 row, quint16 column) const;
        /** \brief set the column title */
        void setColumnTitle(quint16 column, QString name);
        /** \brief return the column title */
        QString columnTitle(quint16 column) const;
        /** \brief search for a row that contains the given value in the given column. Adds a row if it was not found and returns row number */
        quint16 getAddRow(quint16 column, QVariant data);

        /** \brief save the contents in a <a href="http://en.wikipedia.org/wiki/SYmbolic_LinK_(SYLK)">SYLK file (SYmbolic LinK)</a>
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         */
        bool saveSYLK(const QString& filename, char format = 'g', int precision = 6);
        /** \brief save the contents in a Comma-Separated-Values file
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         *
         *
         */
        bool saveCSV(const QString& filename, QString column_separator=QString(", "), char decimal_separator='.', QString header_start=QString("#!"), char format = 'g', int precision = 6);
        /** \brief read the contents in a Comma-Separated-Values file into the current model (if it is not readonly!)
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         *
         *
         */
        bool readCSV(const QString& filename, char column_separator=',', char decimal_separator='.', QString header_start=QString("#!"), char comment_start='#');
        //bool readCSV(const QString& filename, const QString& column_separator, const QString& decimal_separator, const QString& header_start, const QString& comment_start);
    public slots:
        /** \brief append a new row */
        inline void appendRow() { resize(rows+1, columns); }
        /** \brief append a new column */
        inline void appendColumn() { resize(rows, columns+1); }
        /** \brief empty the table and reset its size to (0,0) */
        void clear();
        /** \brief insert a new row before current row */
        void insertRow(quint16 r=0);
        /** \brief insert a new column before current column */
        void insertColumn(quint16 c=0);
        /** \brief remove the given row */
        void deleteRow(quint16 r);
        /** \brief remove the given column */
        void deleteColumn(quint16 c);


    signals:
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void readonlyChanged(bool readonly);
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void notReadonlyChanged(bool notReadonly);


    private:
};

#endif // QFTABLEMODEL_H
