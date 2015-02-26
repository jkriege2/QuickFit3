/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/




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
#include "lib_imexport.h"
#include <QLinkedList>
#include <QPointer>

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
     - The row and column number are given as quint32, i.e. as 16-bit unsigned integers
     - row and columns are combined to a quint64 by calculating: (row<<16) | column
     - This quint64 is used as a QHash index. If a cell is empty either an invalid QVariant is stored, or
       the QHash simply does not contain a QVariant for this cell.
     - row and column count are stored externally.
     - data is stored in a QFTableModel object which is also externally accessible for data access.
    .


    \section undoreotable Undo & Redo
    This class supports undo/redo while editing. By default this function is disabled.
    If enabled up to undoMaxSteps table states are stored in undoList and the user can go through the list using
    the slots undo() and redo(). The data memeber undoCurrentPosition stores the current position in the undo/redo list:

    There are different possible states. If the list is empty, the undoCurrentPosition has to be -1, no undo/redo is possible, we can only add an item
\verbatim
                     <-- undoCurrentPosition = -1
+------------------+     list size = 0
+------------------+
\endverbatim
    Adding an item will alter the state:
\verbatim

+------------------+     list size = 1
0 newest           | <-- undoCurrentPosition = 0
+------------------+
\endverbatim
    Now we can undo, but not redo.

    If the undo/redo list is non-empty. We can distinguish different cases: If the undoCurrentPosition<=0, we can undo, but not redo
\verbatim

+------------------+     list size = 6
0 newest           | <-- undoCurrentPosition = 0
+------------------+
1                  |
+------------------+
2                  |
+------------------+
3                  |
+------------------+
4                  |
+------------------+
5 oldes            |
+------------------+
\endverbatim
    If the undo/redo list is non-empty. We can distinguish different cases: If the undoCurrentPosition>0, we can undo and redo
    (unless undoCurrentPosition >= list size, then we can only redo, as no further undo steps are available:
\verbatim

+------------------+     list size = 6
0 newest           |
+------------------+
1                  |
+------------------+
2                  |
+------------------+
3                  |
+------------------+
4                  |
+------------------+
5 oldes            | <-- undoCurrentPosition = 5
+------------------+
\endverbatim

   So the functions do as follows:
     - undo() will set the internal state to the state stored at undoCurrentPosition in the list, undoCurrentPosition will be increased by 1, so the
       next call of undo() will undo the next step ... until the end of the list is reached.
     - redo() will set the inertal state to the state stored at undoCurrentPosition-1 and decrease undoCurrentPosition by 1.
   .

   There are some special cases:
     - if undo() is called while undoCurrentPosition=-1, then the internal state is set to the state stored at position 0 and before that the current state is
       prepended to the list. This way we can undo this undo by calling redo().
   .

   If the contents of the table is altered, while undoCurrentPosition>0, all undo steps between 0 and undoCurrentPosition will be deleted!

   New states are always prepended to the list!

   The class will automatically generate undo steps, whenever data is changed. This can be prevented, if operations are grouped between startMultiUndo()
   and endMultiUndo() (several levels are possible!!!). Only the last call to endMultiUndo() will create an undo step! clearMultiUndo() will leave the current
   hierarchy immediately and create an undo-step.

 */
class QFLIB_EXPORT QFTableModel : public QAbstractTableModel {
        Q_OBJECT
    protected:
        /** \brief converts a (row, column) adress into a QHash index: (row<<16) | column */
        inline quint64 xyAdressToUInt64(quint32 row, quint32 column) const {
            quint64 r=row;
            quint64 c=column;
            return c| (r<<32);
        }

        /** \brief extracts the row from a UInt32 adress */
        inline quint32 UInt64ToRow(quint64 a) const {
            quint64 as=a>>32;
            return as & 0xFFFFFFFF;
        }

        /** \brief extracts the column from a UInt32 adress */
        inline quint32 UInt64ToColumn(quint64 a) const  {
            return a & 0xFFFFFFFF;
        }

        bool doEmitSignals;

        QVariant defaultEditValue;

        struct TableState {
                /** \brief the number of rows */
                quint32 rows;
                /** \brief the number of columns */
                quint32 columns;
                /** \brief this map is used to store tha data */
                QHash<quint64, QVariant> dataMap;
                /** \brief this map is used to store tha data for the edit role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
                QHash<quint64, QVariant> dataEditMap;
                /** \brief this map is used to store tha data for the background role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
                QHash<quint64, QVariant> dataBackgroundMap;
                /** \brief this map is used to store tha data for the background role (if this does not contain an entry, the data from dataMap is returned ... writing always occurs in dataMap except with setCellEditRole() ) */
                QHash<quint64, QVariant> dataCheckedMap;
                /** \brief this map is used to store additional data for roles >=Qt::UserRole */
                QHash<quint64, QHash<int, QVariant> > moreDataMap;

                QHash<quint64, QHash<int, QVariant> > headerDataMap;

                /** \brief string list that contains the column names */
                QStringList columnNames;
                /** \brief string list that contains the column names */
                QStringList rowNames;
        };

        TableState state;

        /** \brief indicates whether the model is readonly (via the QAbstractTableModel interface!!!) or not */
        bool readonly;
        /** \brief indicates whether the model is still checkable when readonly */
        bool readonlyButStillCheckable;
        /** \brief indicates whether the data has been changed since the last call of resetChanged(); */
        bool hasDataChanged;

        /** \brief indicates whether the vertical header should show row numbers */
        bool verticalHeaderShowRowNumbers;



        /** \brief if \c true, undo/redo is activated */
        bool undoEnabled;
        /** \brief indicates whether we are currently in an undo multi-step operation */
        int undoIsMultiStep;
        /** \brief number of undo steps */
        int undoMaxSteps;
        /** \brief undo/redo list */
        QList<TableState> undoList;
        int rdlID;
        QList<int> undoListT;
        /** \brief current position in undo/redo list */
        int undoCurrentPosition;

        QAction* actUndo;
        QAction* actRedo;

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

        inline void setVerticalHeaderShowRowNumbers(bool enabled) {
            verticalHeaderShowRowNumbers=enabled;
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            beginResetModel();
            endResetModel();
        #else
            reset();
        #endif
        };
        inline bool getVerticalHeaderShowRowNumbers() const { return verticalHeaderShowRowNumbers; }

        QVariantList getColumnData(int column, int role = Qt::DisplayRole) const;
        QVector<double> getColumnDataAsNumbers(int column, int role = Qt::DisplayRole) const;

        /** \brief swap the contents of two cells */
        void swapCells(quint32 row1, quint32 column1, quint32 row2, quint32 column2);

        /** \brief resize the internal table (cells that are out of range will be deleted) */
        void resize(quint32 rows, quint32 columns);
        /** \brief change datatype in cell
         *
         * The type conversion is done in any case. If the current value may not be converted, the cell
         * is set with a default value for the new datatype and \c false is returned, otherweise \c true.
         */
        bool changeDatatype(quint32 row, quint32 column, QVariant::Type newType);
        /** \brief set the given cell to the supplied value */
        void setCell(quint32 row, quint32 column, QVariant value);
        /** \brief sets a whole column */
        void setColumn(quint32 column, const QList<QVariant> &value);
        /** \brief sets a whole column */
        void setColumn(quint32 column, const QVector<double> &value);
        /** \brief sets a whole column */
        void setColumn(quint32 column, const double* value, int N);
        /** \brief sets a whole column */
        void setColumnCreate(quint32 column, const QList<QVariant> &value);
        /** \brief sets a whole column */
        void setColumnCreate(quint32 column, const QVector<double> &value);
        /** \brief sets a whole column */
        void setColumnCreate(quint32 column, const double* value, int N);
        /** \brief copy the contents of cell (row_old, column_old) to cell (row, column) */
        void copyCell(quint32 row, quint32 column, quint32 row_old, quint32 column_old);
        /** \brief set the given cell to the supplied value, if the cell is outside the table size, the table is resized accordingly */
        void setCellCreate(quint32 row, quint32 column, QVariant value);
        /** \brief returns the value in the supplied cell */
        QVariant cell(quint32 row, quint32 column) const;
        /** \brief returns the value in the supplied cell, as a double */
        double cellDouble(quint32 row, quint32 column) const;
        /** \brief set the given cell to the supplied value in a role>=Qt::UserRole */
        void setCellUserRole(int role, quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in a role>=Qt::UserRole */
        void setCellUserRoleCreate(int role, quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in a role>=Qt::UserRole */
        void setCellsUserRoleCreate(int role, quint32 row0, quint32 row1, quint32 column0, quint32 column1, QVariant value);
        /** \brief set the given cell to the supplied value in EditRole */
        void setCellEditRole(quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in EditRole */
        void setCellEditRoleCreate(quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in Background */
        void setCellBackgroundRole(quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in Background */
        void setCellBackgroundRoleCreate(quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in Checked */
        void setCellCheckedRole(quint32 row, quint32 column, QVariant value);
        /** \brief set the given cell to the supplied value in Checked */
        void setCellCheckedRoleCreate(quint32 row, quint32 column, QVariant value);
        /** \brief returns the value in the supplied cell in EditRole */
        QVariant cellEditRole(quint32 row, quint32 column) const;
        /** \brief returns the value in the supplied cell in a given role>=Qt::UserRole */
        QVariant cellUserRole(int role, quint32 row, quint32 column) const;
        /** \brief set the column title */
        void setColumnTitle(quint32 column, QString name);
        /** \brief set the row title */
        void setRowTitle(quint32 row, QString name);
        /** \brief set the column title */
        void setColumnTitleCreate(quint32 column, QString name);
        /** \brief set the row title */
        void setRowTitleCreate(quint32 row, QString name);
        /** \brief return the column title */
        QString columnTitle(quint32 column) const;
        /** \brief return the column title */
        QString rowTitle(quint32 row) const;
        /** \brief search for a row that contains the given value in the given column. Adds a row if it was not found and returns row number */
        quint32 getAddRow(quint32 column, QVariant data);
        /** \brief set data stored in the column header column title */
        void setColumnHeaderData(quint32 column, int role, const QVariant &data);
        QVariant getColumnHeaderData(quint32 column, int role) const;
        bool hasColumnHeaderData(quint32 column, int role) const;
        QList<quint64> getColumnHeaderDataRoles() const;


        enum copyColumnHeaderMode {
            dontCopyHeader=0,
            copyHeader=1,
            copyHeaderAskUser=2
        };

        /** \brief copies the contents of a column in a table model into the current model
         *
         *  The data ends up, starting at (row_here, column_here) and only the rows between row_model_start and row_model_end from the model are copied (unless these are -1 and -1, then the complete column is copied)
         */
        void copyColumnFromModel(QAbstractTableModel* model, int column, int column_here, int row_here=0, int row_model_start=-1, int row_model_end=-1, copyColumnHeaderMode* copyHeader=NULL, QSet<int> excludedRoles=QSet<int>(), QSet<int> excludedHeaderRoles=QSet<int>());
        void copyColumnFromModel(QAbstractTableModel* model, int column, int column_here, const QList<int>& rows_model, int row_here=0, copyColumnHeaderMode* copyHeader=NULL, QSet<int> excludedRoles=QSet<int>(), QSet<int> excludedHeaderRoles=QSet<int>());
        void copyCellFromModelCreate(QAbstractTableModel* model, int column, int row, int column_here, int row_here, copyColumnHeaderMode* copyHeader=NULL, QSet<int> excludedRoles=QSet<int>(), QSet<int> excludedHeaderRoles=QSet<int>());
        struct QFLIB_EXPORT cellToCopy {
            int r;
            int c;
            int r_here;
            int c_here;
            cellToCopy(int r, int c, int r_here, int c_here) {
                this->r=r;
                this->c=c;
                this->r_here=r_here;
                this->c_here=c_here;
            }
        };

        void copyCellsFromModelCreate(QAbstractTableModel* model, const QList<cellToCopy>& cells, copyColumnHeaderMode* copyHeader=NULL, QSet<int> excludedRoles=QSet<int>(), QSet<int> excludedHeaderRoles=QSet<int>());

        /** \brief set the default QVariant value displayed in an editor, if the value does not yet exist */
        void setDefaultEditValue(QVariant defaultEditValue);

        /** \brief return a list of all column titles */
        QStringList getColumnTitles() const;
        /** \brief return a list of all row titles */
        QStringList getRowTitles() const;


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

        QList<QList<QVariant> > getDataTable(QStringList& colNames, QStringList& rownames, QModelIndexList selection=QModelIndexList());

        /*! \brief saves the given selection (or all cells, if the selection is empty) as XML

            \param selection copy only these cells, or all if this is empty (default)
            \param createXMLFragment if this is \c true, the output will not be a complete XML document (incl. header etc.), but only a raw fragment
            \param template_only if this is \c true, the output will only contain the header and header metadata, but no actual data
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
        QString saveXML(QModelIndexList selection=QModelIndexList(), bool createXMLFragment=false, bool template_only=false);
        /** \brief saves the given \a selection as XML into a file \a filename */
        bool saveXML(const QString& filename, QModelIndexList selection=QModelIndexList(), bool createXMLFragment=false, bool template_only=false);
        enum ReadHeaderMode {
            rhmReadHeader,
            rhmDontReadHeader,
            rhmOverwriteNondefault,
            rhmAskOverwrite
        };

        /** \brief reads an XML-encoded table (see saveXML() ) from the string \a data and inserts it ito the table, starting at \a start_row, \a start_col */
        bool readXML(const QString& data, int start_row=0, int start_col=0, bool clearTable=false, bool read_template_only=false, ReadHeaderMode alsoReadHeaders=rhmOverwriteNondefault);
        /** \brief reads an XML-encoded table (see saveXML() ) from the file \a filename */
        bool readXMLFile(const QString& filename, int start_row=0, int start_col=0, bool clearTable=true, bool read_template_only=false);
        /** \brief copies the given selection (or all cells, if the selection is empty) to the clipboard in an XML format (see saveXML() ) */
        void copy(QModelIndexList selection=QModelIndexList(), bool createXMLFragment=false, bool template_only=false);
        /*! \brief pastes data from the cklipboard into the table, starting from the given position ... the table is resized if needed */
        void paste(int row_start=0, int column_start=0);
        /*! \brief pastes data from the cklipboard into the table, starting from the given position ... the table is resized if needed */
        void pasteHeaderTemplate(int row_start=0, int column_start=0);

        bool getDoEmitSignals() const;
        void enableSignals(bool emitReset=true);
        void disableSignals();
        /*void deleteRows(QList<quint32> rs);
        void deleteColumns(QList<quint32> rs);*/



        void clearUndo();
        void startMultiUndo();
        void endMultiUndo();
        void endMultiUndoAndReset();
        void clearMultiUndo();
        void setUndoEnabled(bool enabled);
        void emitUndoRedoSignals( bool alwaysEmit=false);
        void addUndoStep();

        QAction* getUndoAction() const;
        QAction* getRedoAction() const;

    public slots:
        /** \brief append a new row */
        inline void appendRow() { resize(state.rows+1, state.columns); }
        /** \brief append a new column */
        inline void appendColumn() { resize(state.rows, state.columns+1); }
        /** \brief empty the table and reset its size to (0,0) */
        void clear();
        /** \brief insert a new row before current row */
        void insertRow(quint32 r=0);
        /** \brief insert a new column before current column */
        void insertColumn(quint32 c=0);
        /** \brief remove the given row */
        void deleteRow(quint32 r);
        /** \brief remove the given column */
        void deleteColumn(quint32 c);
        /** \brief remove all data from given column, but do not remove the column (also does not delete it's header) */
        void emptyColumn(quint32 c);
        /** \brief delete all contents from the given cell */
        void deleteCell(quint32 r, quint32 column);
        /** \brief delete all contents from the given cells */
        void deleteCells(QModelIndexList selection);


        /** \brief undo the last change */
        void undo();
        /** \brief redo the next change in the undo/redo list */
        void redo();



    signals:
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void readonlyChanged(bool readonly);
        /** \brief emitted when the readonly status changes (may be used to en-/disable widgets */
        void notReadonlyChanged(bool notReadonly);


        void columnRemoved(int i);
        void columnAdded(int i);
        void columnTitleChanged(int i);
        void rowRemoved(int i);
        void rowAdded(int i);
        void rowTitleChanged(int i);

        void undoAvailable(bool available);
        void redoAvailable(bool available);

    private:

};

/*! \brief helper list-model, which contains a list of the column-titles of a given QFTableModel
    \ingroup qf3lib_widgets



 */
class QFLIB_EXPORT QFTableModelColumnHeaderModel : public QAbstractListModel {
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
        QPointer<QFTableModel> model;
        bool hasNone;
};

/*! \brief helper list-model, which allows to edit the column-titles and other properties of a given QFTableModel
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFTableModelEditColumnHeaderDataModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFTableModelEditColumnHeaderDataModel(QFTableModel * table=NULL, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFTableModelEditColumnHeaderDataModel();

        void setModel(QFTableModel * model);

        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
        void addProperty(int column, const QString& name);
        void clearProperties();
    public slots:
        void rebuildModel();

    protected:
        QPointer<QFTableModel> model;
        QList<QPair<QString, int> > m_properties;
};

#endif // QFTABLEMODEL_H


