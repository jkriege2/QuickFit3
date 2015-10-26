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

#ifndef PHOTONSWIDGET_H
#define PHOTONSWIDGET_H

#include <QWidget>
#include "jkstyledbutton.h"
#include"parameters.h"
#include "jkqtplotter.h"
#include "alexdata.h"
#include "DialogReconstructMarkerRecords.h"

namespace Ui {
class PhotonsWidget;
}

class PhotonsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PhotonsWidget(QWidget *parent = 0, AlexData* data=NULL);
    ~PhotonsWidget();

    /// store the settings of this widget in \a settings with prefix \a prefix (e.g. group)
    void storeSettings(QSettings& settings, const QString& prefix=QString("")) const;
    /// load the widget's settings from \a settings. the \a prefix must be the same as for storeSettings
    void loadSettings(QSettings& settings, const QString& prefix=QString(""));

    void automatedRun(const QString &filename);

private:
    Ui::PhotonsWidget *ui;
    JKStyledButton* edt;

    AlexData* ad;

    ReconstructionMethod method;

    JKQtPlotter* plot;
    enum ChannelMappings {
        Donor=0,
        Acceptor=1
    } Route0, Pin4;

    double binwidth;

    double periodReconstruct;
    double offsetReconstruct;

private slots:
    void readT3Rfile(const QString file=QString(""));
    void plotIntensity();
    void getRangeStart(double startTime);
    void getRangeStop(double endTime);
    void backgroundAnalysis();
    void updateInfo(bool hasMarker);

    void sliderMoved(int value);

signals:
    void backgroundCalculated();

};

#endif // PHOTONSWIDGET_H
