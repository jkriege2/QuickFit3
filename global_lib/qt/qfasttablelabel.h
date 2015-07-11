/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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





/**
 * \defgroup jfasttablelabel Class for fast table rendering
 * \ingroup tools_qt
 */

/** \file jfasttablelabel.h
  * \ingroup jfasttablelabel
  */


#ifndef QFASTTABLELABEL_H
#define QFASTTABLELABEL_H

#include <QWidget>
#include <QPair>
#include <QHash>
#include <QStringList>

/*! \brief class for fast rendering of tables
    \ingroup jfasttablelabel

    The table is specified with a simple syntax: each column is separated by a column_separator, which is set, when calling setData() and
    each new line in the string is a new row in the table. The method setData() immediately interprets the given string and stores the table
    values in an internal hash table. You can edit the table using setData(), clear() and table(). Each table entry is a string. The first
    m_headerrows row and m_headercolumns columns are drawn in bold face as headers (setHeaderCells(), default is 1 for both). Using setBorder()
    you can set a free-space border around the table (in pixels).

    while the width of the table is wider than the widget, the table is drawn as small as possible. When the width gets larger though,
    the table columns are strechted to the full width, if no column_sizes have been set (default, or call clearColumnSizes()). If column
    sizes are set, the table column will be drawn according to them (relative to the widget width). The column widthes are interpreted
    as number between 0 and 100. So they all have to sum up to 100. If columns are not set, they are set to equal widthes over the remaining space.



 */
class QFastTableLabel : public QWidget {
    Q_OBJECT
public:
    explicit QFastTableLabel(QWidget *parent = 0);

    void clear();
    void setData(quint16 row, quint16 column, QString text);
    QString table(quint16 row, quint16 column) const;
    /*! \brief set contents as special string with markup

        Each line in \a text is a row in the table, columns are separated by the given column separator. Here's an example:
\code
setData("|col1|col 2\nrow1|1|2\nrow2|3|4\nrow 3|5|6|7");
\endcode
       will draw a 4-row, 3-column table with some dummy values.

       You can use special line that start with the character \a specialline to set formating:
         - a line \code >w50|25|25 \endcode will set the column widthes to the specified values. So
           \c label->setData(QString(">b25|25|25|25\n|col1|col 2\nrow1|1|2\nrow2|3|4\nrow 3|5|6|7"));
           will create a table with 4 columns, that are 25% of the total width.
       .

      */
    void setData(QString text, QChar column_separator=QChar('|'), QChar specialline=QChar('>'));
    quint16 rows() const;
    quint16 columns() const;

    void clearColumnSizes();
    void setColumnSize(quint16 column, char size);

    void setHeaderCells(quint16 m_headerrows, quint16 m_headercolumns);
    void setGrid(bool enabled);
    void setBorder(int border);

    virtual QSize sizeHint () const;

signals:

public slots:

protected:
    void clear_noupdate();

    void paintEvent(QPaintEvent *event);

    int columnWidth(quint16 column) const;
    int rowHeight(quint16 row) const;


    QHash<QPair<quint16, quint16>, QString> data;
    QHash<quint16, char> column_sizes;

    quint16 m_rows;
    quint16 m_columns;
    bool m_grid;
    quint16 m_headerrows;
    quint16 m_headercolumns;
    int m_border;
    int m_cellSpacing;

};

#endif // QFASTTABLELABEL_H
