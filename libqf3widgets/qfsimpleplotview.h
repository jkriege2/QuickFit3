/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-06-22 12:12:54 +0200 (So, 22 Jun 2014) $  (revision $Rev: 3280 $)

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


#ifndef QFSIMPLEPLOTVIEW_H
#define QFSIMPLEPLOTVIEW_H

#include <QWidget>
#include "qfplotter.h"
#include "qvisiblehandlesplitter.h"
#include <QSet>
#include "qfdoubleedit.h"
#include "qtriple.h"
#include "qfhtmldelegate.h"
#include "qenhancedtableview.h"
#include "qftablemodel.h"
#include <QSettings>
#include <QTextDocument>
#include "libwid_imexport.h"
#include "qfproperties.h"
#include "jkqtptools.h"
#include <stdint.h>

namespace Ui {
    class QFSimplePlotView;
}
/*! \brief This class displays a simple plot, i.e. it plots two datasets against each other
    \ingroup qf3rdrdp_imaging_fcs



*/
class QFWIDLIB_EXPORT QFSimplePlotView : public QWidget
{
        Q_OBJECT

    public:

        struct PlotItem {
                    PlotItem() {
                        data1=NULL;
                        data2=NULL;
                        data3=NULL;
                        external=true;
                        size=0;
                        name="";
                        symbols=true;
                        lines=false;
                    }

                    QString name;
                    double* data1;
                    double* data2;
                    double* data3;//color channel
                    int32_t size;
                    bool external;
                    bool symbols;
                    bool lines;
                };

        explicit QFSimplePlotView(QWidget *parent = 0);
        virtual ~QFSimplePlotView();


        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("simpleplotview/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("simpleplotview/"));

        void writeQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);
        void readQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);

        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);

    signals:

    public slots:

    private:
        Ui::QFSimplePlotView *ui;
};

#endif // QFSIMPLEPLOTVIEW_H
