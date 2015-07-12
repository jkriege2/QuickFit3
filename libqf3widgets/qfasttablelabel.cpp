/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#include "qfasttablelabel.h"
#include <QPainter>
#include <QDebug>
QFastTableLabel::QFastTableLabel(QWidget *parent) :
    QWidget(parent)
{
    m_grid=true;
    m_headerrows=1;
    m_headercolumns=1;
    m_border=3;
    m_cellSpacing=7;
    clear_noupdate();
}

void QFastTableLabel::clear() {
    clear_noupdate();
    update();
}

void QFastTableLabel::clear_noupdate() {
    data.clear();
    m_rows=m_columns=0;
}

void QFastTableLabel::setData(quint16 y, quint16 x, QString text) {
    data[qMakePair(y, x)]=text;
    if (x+1>m_columns) m_columns=x+1;
    if (y+1>m_rows) m_rows=y+1;
    update();
}

QString QFastTableLabel::table(quint16 y, quint16 x) const {
    return data.value(qMakePair(y,x), QString(""));
}

quint16 QFastTableLabel::rows() const {
    return m_rows;
}

quint16 QFastTableLabel::columns() const {
    return m_columns;
}

void QFastTableLabel::setData(QString text, QChar column_separator, QChar specialline) {
    clear_noupdate();
    QStringList rows=text.split('\n');
    m_rows=0;
    m_columns=0;
    for (quint16 r=0; r<rows.size(); r++) {
        QString trim=rows[r].trimmed();
        if (trim.startsWith(QString(specialline)+"b")) {
            column_sizes.clear();
            QStringList line=trim.right(trim.size()-2).split(column_separator);
            for (quint16 c=0; c<line.size(); c++) {
                bool ok=false;
                int val=line[c].toInt(&ok);
                if (ok) {
                    column_sizes[c]=val;
                }
            }
        } else {
            QStringList line=trim.split(column_separator);
            for (quint16 c=0; c<line.size(); c++) {
                if (c+1>m_columns) m_columns=c+1;
                data[qMakePair(m_rows, c)]=line[c].trimmed();
            }
            m_rows++;
        }
    }

    update();
}

int QFastTableLabel::columnWidth(quint16 column) const {
    if (column>m_columns) return 0;
    int w=0;
    for (quint16 r=0; r<m_rows; r++) {
        QFont f=font();
        if (r<m_headerrows) f.setBold(true);
        int ww=QFontMetricsF(f).width(table(r, column));
        if (ww>w) w=ww;
    }
    return w;
}

int QFastTableLabel::rowHeight(quint16 row) const {
    if (row>m_rows) return 0;
    QFont f=font();
    f.setBold(true);
    return QFontMetricsF(f).height();
}

void QFastTableLabel::clearColumnSizes() {
    column_sizes.clear();
    update();
}

void QFastTableLabel::setColumnSize(quint16 column, char size) {
    column_sizes[column]=size;
    update();
}

void QFastTableLabel::setHeaderCells(quint16 headerrows, quint16 headercolumns) {
    this->m_headerrows=headerrows;
    this->m_headercolumns=headercolumns;
    update();
}

void QFastTableLabel::setGrid(bool enabled) {
    m_grid=enabled;
    update();
}
void QFastTableLabel::setBorder(int border) {
    m_border=border;
    update();
}

void QFastTableLabel::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    QFont f=font();
    QFont fh=f;
    fh.setBold(true);

    int twidth=0;
    int theight=0;
    twidth=twidth+(m_columns-1)*m_cellSpacing;
    QList<int> cw, rh;
    for (quint16 c=0; c<m_columns; c++) {
        int w=columnWidth(c);
        cw.append(w);
        twidth=twidth+w+m_cellSpacing;
    }
    for (quint16 c=0; c<m_rows; c++) {
        int w=rowHeight(c);
        rh.append(w);
        theight=theight+w+m_cellSpacing;
    }

    int extraWidth=0;
    if (column_sizes.isEmpty()) {
        if ((twidth<width())  && (m_columns>1)) {
            extraWidth=(width()-twidth)/(m_columns-1)-m_cellSpacing;
            twidth=twidth+(m_columns-1)*extraWidth;

        }
    } else {
        int sum=0;
        int count=0;
        QHashIterator<quint16, char> it(column_sizes);
        while (it.hasNext()) {
            it.next();
            sum=sum+it.value();
            count++;
        }
        int rest=100-sum;
        if (rest<0) rest=0;
        int restwidth=0;
        if (m_columns-count>0) restwidth=rest/(m_columns-count);
        //qDebug()<<"restwidth="<<restwidth;
        twidth=0;
        for (quint16 c=0; c<m_columns; c++) {
            int rw=width()*restwidth/100;
            if (rw<=0) rw=cw[c];
            int w=width()*column_sizes.value(c, 0)/100;
            if (w==0) w=rw;
            cw[c]=w;
            twidth=twidth+w+m_cellSpacing;
        }
    }


    int y=m_border;
    int yTextOffset=QFontMetricsF(fh).ascent()+1;
    for (quint16 r=0; r<m_rows; r++) {
        int x=m_border;
        for (quint16 c=0; c<m_columns; c++) {
            if (c<m_headercolumns || r<m_headerrows) painter.setFont(fh);
            else painter.setFont(f);
            painter.setClipRect(QRect(x, y, cw[c], rh[r]+m_cellSpacing));
            painter.setClipping(true);
            painter.drawText(x, y+yTextOffset, table(r, c));
            painter.setClipping(false);
            x=x+cw[c]+m_cellSpacing+extraWidth;
            if (m_grid) {
                if (c<m_columns-1) {
                    painter.drawLine(x-m_cellSpacing/2, m_border, x-m_cellSpacing/2, m_border+theight);
                }
            }
        }


        y=y+rh[r]+m_cellSpacing;
        if (m_grid) {
            if (r<m_rows-1) {
                painter.drawLine(m_border, y-m_cellSpacing/2, m_border+twidth, y-m_cellSpacing/2);
            }
        }
    }
}

QSize QFastTableLabel::sizeHint () const {
    QFont f=font();
    QFont fh=f;
    fh.setBold(true);

    int twidth=0;
    int theight=0;
    twidth=twidth+(m_columns-1)*m_cellSpacing;
    QList<int> cw, rh;
    for (quint16 c=0; c<m_columns; c++) {
        int w=columnWidth(c);
        cw.append(w);
        twidth=twidth+w+m_cellSpacing;
    }
    for (quint16 c=0; c<m_rows; c++) {
        int w=rowHeight(c);
        rh.append(w);
        theight=theight+w+m_cellSpacing;
    }
    return QSize(twidth, theight);
}
