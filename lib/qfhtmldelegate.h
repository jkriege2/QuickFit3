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

#ifndef QFHTMLDELEGATE_H
#define QFHTMLDELEGATE_H

#include "lib_imexport.h"
#include <QStyledItemDelegate>

/*! \brief subclass of QStyledItemDelegate which is able to display HTML markupped texts.
    \ingroup qf3lib_widgets

    If displayRichTextEditor is \c true then also a richtext-editing component is used as editor. Otherwise the
    default QStyledItemDelegate methods are used.

    \see <a href="http://www.qtwiki.de/wiki/%27HTML%27-Delegate">http://www.qtwiki.de/wiki/%27HTML%27-Delegate</a>
*/
class QFLIB_EXPORT QFHTMLDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        QFHTMLDelegate(QObject* parent=NULL);
        virtual ~QFHTMLDelegate();

        void setPrintMode(bool enabled);
        bool getPrintMode() const;

        bool displayRichTextEditor() const;
        void setDisplayRichTextEditor(bool d);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    private:
        bool m_displayRichTextEditor;
        QRegExp rxHTML;
        bool m_printMode;
    protected:
        void drawCheck(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QRect &rect, Qt::CheckState state) const;

        QString calcDisplayedText(const QModelIndex& index, const QStyleOptionViewItem &option, QPoint &offset, bool &isHTML, QPixmap *img=NULL) const;
};

#endif // QFHTMLDELEGATE_H
