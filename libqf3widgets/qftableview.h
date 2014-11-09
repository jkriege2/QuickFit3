/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#ifndef QFTABLEVIEW_H
#define QFTABLEVIEW_H

#include <QWidget>
#include "qfhtmldelegate.h"
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QAbstractTableModel>
#include <QTabWidget>
#include <QSet>
#include <QTableView>
#include "qenhancedtableview.h"
#include "qftablemodel.h"
#include <QFormLayout>
#include <QSettings>
#include <QTextDocument>
#include "libwid_imexport.h"
#include <QRadioButton>
#include "qfproperties.h"
#include <QWidget>
/*! \brief This class displays a table of data
    \ingroup qf3lib_widgets



*/
class QFWIDLIB_EXPORT QFTableView : public QWidget
{
        Q_OBJECT
    public:
        explicit QFTableView(QWidget *parent = 0);

        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("tableview/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("tableview/"));

        void writeQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);
        void readQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);

        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);

        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addCopiedColumn(QString name, const double* data, int32_t size);
        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addColumn(QString name, const QList<QVariant>& data);

        void clear();
    signals:

    public slots:
    private:
        void createWidgets();
    protected:
        QFTableModel model;
        QEnhancedTableView* tvMain;

};

#endif // QFTABLEVIEW_H
