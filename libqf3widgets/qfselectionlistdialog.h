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


#ifndef QFSELECTIONLISTDIALOG_H
#define QFSELECTIONLISTDIALOG_H

#include <QDialog>
#include <QList>
#include <QSettings>
#include <QMap>
#include <QListWidgetItem>
#include "libwid_imexport.h"

namespace Ui {
    class QFSelectionListDialog;
}
/*! \brief a dialog that allows to select a subset of items from a list.
    \ingroup qf3lib_widgets

    Selections may be saved under a user-specified name to a QSettings object (call init() before and writeSettings() after exec()).
    Also it is possible to set the text of the top-label using setLabel() and to add widgets before the button box using addWidget().

    To initialize the list of choices, call init() before exec(), which will load the user-preset lists from the specified
    QSettings object and init the list with the items in the \a itemNames list. Each item is stored and recognized by it's ID
    which is initialized  with the \a itemData argument to init(). If this list is empty, the name is used. To get the list
    of items, the suer selected, call getSelect() which will return a list of the IDs or getSelectedIndexes() which will return
    the indexes of the selected items.
  */
class QFWIDLIB_EXPORT QFSelectionListDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFSelectionListDialog(QWidget *parent = 0, bool selection_saveable=true);
        ~QFSelectionListDialog();

        void init(QStringList itemNames, QList<QVariant> itemData, QList<QColor> colors);
        void init(QStringList itemNames, QList<QVariant> itemData=QList<QVariant>());
        void init(QStringList itemNames, QList<QVariant> itemData, QSettings &settings, const QString &prefix=QString());
        void init(QStringList itemNames, QList<QVariant> itemData, QList<QColor> colors, QSettings &settings, const QString &prefix=QString());

        void init(QStringList itemNames, QStringList itemData, QList<QColor> colors);
        void init(QStringList itemNames, QStringList itemData);
        void init(QStringList itemNames, QStringList itemData, QSettings &settings, const QString &prefix=QString());
        void init(QStringList itemNames, QStringList itemData, QList<QColor> colors, QSettings &settings, const QString &prefix=QString());

        void setLabel(const QString& text);
        void writeList(QSettings& settings, const QString& prefix=QString());
        QStringList getSelectedDataStrings() const;
        QList<QVariant> getSelected() const;
        QList<bool> getSelectedBoolList() const;
        QList<int> getSelectedIndexes() const;
        void addWidget(const QString &label, QWidget *widget);
        void selectItems(QList<bool> items);
        
    protected slots:
        void on_btnSave_clicked();
        void on_btnSelectNone_clicked();
        void on_btnSelectAll_clicked();
        void getDataColumnsByUserComboBoxSelected(int index);
        void getDataColumnsByUserItemChanged(QListWidgetItem *widgetitem);
    protected:
        QMap<QString, QStringList> getDataColumnsByUserSaved;
    private:
        Ui::QFSelectionListDialog *ui;
        bool selection_saveable;
};

#endif // QFSELECTIONLISTDIALOG_H
