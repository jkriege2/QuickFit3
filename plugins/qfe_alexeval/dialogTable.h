/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#ifndef DIALOGTABLE_H
#define DIALOGTABLE_H

#include <QDialog>
#include<QTableWidget>

namespace Ui {
class DialogTable;
}

class DialogTable : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogTable(QWidget *parent = 0);
    ~DialogTable();
    void setTableHeader(QList<QString> tableHeader);
    QList<QString> getTableHeader();
    QTableWidget * getTableWidget();
    void populate();
    void setItem(int row, int col, const QString &text);
    QString getItem(int row, int col);
    QVariant getItemValue(int row, int col);
    int getRowCount();
    int getColumnCount();
    bool getAuto() {return isAuto();}

    
private:
    Ui::DialogTable *ui;
    QList<QString> m_TableHeader;
private slots:
    void insertItem();
    void deleteItem();
    void on_pushButtonAdd_clicked();
    void on_pushButtonRemove_clicked();
    bool isAuto();
};

#endif // DIALOGTABLE_H
