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
#include <QModelIndexList>
#include <QXmlStreamWriter>
#include <QtXml>
#include "qftools.h"
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QModelIndex>
#include "libwid_imexport.h"

/*! \brief this class is used to manage a table of values (QVariant)
    \ingroup qf3lib_widgets

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
class QFWIDLIB_EXPORT QFTableModel : public QAbstractTableModel {
        Q_OBJECT
    protected:
        /** \brief converts a (row, column) adress into a QHash index: (row<<16) | column */
        inline quint32 xyAdressToUInt32(quint16 row, quint16 column) const {
            quint32 r=row;
            quint32 c=column;
            return c| (r<<16);
        }

        /** \brief extracts the row from a UInt32 adress */
        inline quint16 UInt32ToRow(quint32 a) const {
            quint32 as=a>>16;
            return as & 0xFFFF;
        }

        /** \brief extracts the column from a UInt32 adress */
        inline quint16 UInt32ToColumn(quint32 a) const  {
            return a & 0xFFFF;
        }

        bool doEmitSignals;

        QVariant defaultEditValue;

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
        /** \brief this map is used to store additional data for roles >=Qt::UserRole */
        QHash<quint32, QHash<int, QVariant> > moreDataMap;

        QHash<quint32, QHash<int, QVariant> > headerDataMap;

        /** \brief string list that contains the column names */
        QStringList columnNames;
        /** \brief indicates whether the model is readonly (via the QAbstractTableModel interface!!!) or not */
        bool readonly;
        /** \brief indicates whether the model is still checkable when readonly */
        bool readonlyButStillCheckable;
        /** \brief indicates whether the data has been changed since the last call of resetChanged(); */
        bool hasDataChanged;

        /** \brief indicates whether the vertical header should show row numbers */
        bool verticalHeaderShowRowNumbers;
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

        inline void setReadonlyButStillCheckable(bool r) { readonlyButStillCheckable=r; }
        inline bool isReadonly() const { return readonly; }
        inline void setReadonly(bool r) { readonly=r; emit readonlyChanged(r); emit notReadonlyChanged(!r);}
        inline void resetChanged() { hasDataChanged=false; }
        inline bool hasChanged() const { return hasDataChanged; }

        inline void setVerticalHeaderShowRowNumbers(bool enabled) { verticalHeaderShowRowNumbers=enabled; reset(); }
        inline bool getVerticalHeaderShowRowNumbers() const { return verticalHeaderShowRowNumbers; }

        QVariantList getColumnData(int column, int role = Qt::EditRole) const;
        QVector<double> getColumnDataAsNumbers(int column, int role = Qt::EditRole) const;

        /** \brief swap the contents of two cells */
        void swapCells(quint16 row1, quint16 column1, quint16 row2, quint16 column2);

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
        /** \brief sets a whole column */
        void setColumn(quint16 column, const QList<QVariant> &value);
        /** \brief sets a whole column */
        void setColumn(quint16 column, const QVector<double> &value);
        /** \brief sets a whole column */
        void setColumnCreate(quint16 column, const QList<QVariant> &value);
        /** \brief sets a whole column */
        void setColumnCreate(quint16 column, const QVector<double> &value);
        /** \brief copy the contents of cell (row_old, column_old) to cell (row, column) */
        void copyCell(quint16 row, quint16 column, quint16 row_old, quint16 column_old);
        /** \brief set the given cell to the supplied value, if the cell is outside the table size, the table is resized accordingly */
        void setCellCreate(quint16 row, quint16 column, QVariant value);
        /** \brief returns the value in the supplied cell */
        QVariant cell(quint16 row, quint16 column) const;
        /** \brief set the given cell to the supplied value in a role>=Qt::UserRole */
        void setCellUserRole(int role, quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in a role>=Qt::UserRole */
        void setCellUserRoleCreate(int role, quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in EditRole */
        void setCellEditRole(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in EditRole */
        void setCellEditRoleCreate(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Background */
        void setCellBackgroundRole(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Background */
        void setCellBackgroundRoleCreate(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Checked */
        void setCellCheckedRole(quint16 row, quint16 column, QVariant value);
        /** \brief set the given cell to the supplied value in Checked */
        void setCellCheckedRoleCreate(quint16 row, quint16 column, QVariant value);
        /** \brief returns the value in the supplied cell in EditRole */
        QVariant cellEditRole(quint16 row, quint16 column) const;
        /** \brief returns the value in the supplied cell in a given role>=Qt::UserRole */
        QVariant cellUserRole(int role, quint16 row, quint16 column) const;
        /** \brief set the column title */
        void setColumnTitle(quint16 column, QString name);
        /** \brief set the column title */
        void setColumnTitleCreate(quint16 column, QString name);
        /** \brief return the column title */
        QString columnTitle(quint16 column) const;
        /** \brief search for a row that contains the given value in the given column. Adds a row if it was not found and returns row number */
        quint16 getAddRow(quint16 column, QVariant data);
        /** \brief set data stored in the column header column title */
        void setColumnHeaderData(quint16 column, int role, const QVariant &data);
        QVariant getColumnHeaderData(quint16 column, int role) const;
        bool hasColumnHeaderData(quint16 column, int role) const;
        QList<quint32> getColumnHeaderDataRoles() const;

        /** \brief set the default QVariant value displayed in an editor, if the value does not yet exist */
        void setDefaultEditValue(QVariant defaultEditValue);

        /** \brief return a list of all column titles */
        QStringList getColumnTitles() const;


        /** \brief save the contents in a <a href="http://en.wikipedia.org/wiki/SYmbolic_LinK_(SYLK)">SYLK file (SYmbolic LinK)</a>
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         */
        bool saveSYLK(const QString& filename, char format = 'g', int precision = 6);
        /** \brief save the contents in a Comma-Separated-Values file
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         */
        bool saveCSV(const QString& filename, QString column_separator=QString(", "), char decimal_separator='.', QString header_start=QString("#!"), char format = 'g', int precision = 6, QModelIndexList selection=QModelIndexList());
        /** \brief save the contents as Comma-Separated-Values into a QTextStream
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         */
        bool saveCSV(QTextStream& out, QString column_separator=QString(", "), char decimal_separator='.', QString header_start=QString("#!"), char format = 'g', int precision = 6, bool setCodecAndEncoding=true, QModelIndexList selection=QModelIndexList());
        /** \brief read the contents in a Comma-Separated-Values file into the current model (if it is not readonly!)
         *
         * \return \c true on success and \c false on error (e.g. couldn't open file ...)
         */
        bool readCSV(const QString& filename, char column_separator=',', char decimal_separator='.', QString header_start=QString("#!"), char comment_start='#');
        /** \brief read CSV from a QTextStream and write it into the table (may be resized) starting from ( \a start_row, \a start_col ) */
        bool readCSV(QTextStream& in, char column_separator=',', char decimal_separator='.', QString header_start=QString("#!"), char comment_start='#', int start_row=0, int start_col=0, bool clearTable=false);

        /*! \brief saves the given selection (or all cells, if the selection is empty) as XML

            \param selection copy only these cells, or all if this is empty (default)
            \param createXMLFragment if this is \c true, the output will not be a complete XML document (incl. header etc.), but only a raw fragment
            \returns the selection as XML

            The format in the clipboard is XML and the MIME type is called \c quickfit3/qfrdrtable, the encoding is UTF-8:
\verbatim
<qfrdrtable>
  <columns> <!-- names for all columns used in the data section -->
    <col col="0" name="0"/>
    <col col="1" name="1"/>
    <col col="2" name="2"/>
  </columns>
  <data><!-- the actual data, each cell separately, the first cell is always (0,0)
             no matter where it was before -->
    <cell row="0" col="0" type="string">a</cell>
    <cell row="0" col="1" type="double">1.26</cell>
    <cell row="0" col="2" type="string">So Jan 1 2012</cell>
    <cell row="1" col="0" type="string">b</cell>
    <cell row="1" col="1" type="double">1.2586</cell>
    <cell row="1" col="2" type="bool">true</cell>
    <cell row="2" col="0" type="string">c</cell>
    <cell row="2" col="1" type="double">1.587e-05</cell>
    <cell row="2" col="2" type="int">1234</cell>
  </data>
</qfrdrtable>
\endverbatim
          */
        QString saveXML(QModelIndexList selection=QModelIndexList(), bool createXMLFragment=false);
        /** \brief saves the given \a selection as XML into a file \a filename */
        bool saveXML(const QString& filename, QModelIndexList selection=QModelIndexList());
        /** \brief reads an XML-encoded table (see saveXML() ) from the string \a data and inserts it ito the table, starting at \a start_row, \a start_col */
        bool readXML(const QString& data, int start_row=0, int start_col=0, bool clearTable=false);
        /** \brief reads an XML-encoded table (see saveXML() ) from the file \a filename */
        bool readXML(const QString& filename);
        /** \brief copies the given selection (or all cells, if the selection is empty) to the clipboard in an XML format (see saveXML() ) */
        void copy(QModelIndexList selection=QModelIndexList(), bool createXMLFragment=false);
        /*! \brief pastes data from the cklipboard into the table, starting from the given position ... the table is resized if needed */
        void paste(int row_start=0, int column_start=0);

        bool getDoEmitSignals() const;
        void enableSignals(bool emitReset=true);
        void disableSignals();
        /*void deleteRows(QList<quint16> rs);
        void deleteColumns(QList<quint16> rs);*/
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
        /** \brief remove all data from given column, but do not remove the column (also does not delete it's header) */
        void emptyColumn(quint16 c);
        /** \brief delete all contents from the given cell */
        void deleteCell(quint16 r, quint16 column);
        /** \brief delete all contents from the given cells */
        void deleteCells(QModelIndexList selection);

    signals:
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void readonlyChanged(bool readonly);
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void notReadonlyChanged(bool notReadonly);


        void columnRemoved(int i);
        void columnAdded(int i);
        void columnTitleChanged(int i);
    private:
};

class QFWIDLIB_EXPORT QFTableModelColumnHeaderModel : public QAbstractListModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFTableModelColumnHeaderModel(QFTableModel * table, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFTableModelColumnHeaderModel();

        void setModel(QFTableModel * model);
        void setHasNone(bool hasNone);

        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    public slots:
        void rebuildModel();
        void nameChanged(int i);
        void nameDeleted(int i);
        void nameAdded(int i);
    protected:
        QFTableModel * model;
        bool hasNone;
};

#endif // QFTABLEMODEL_H
