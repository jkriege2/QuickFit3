/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef ANDORGLOBALCAMERASETTINGSWIDGET_H
#define ANDORGLOBALCAMERASETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
    class AndorGlobalCameraSettingsWidget;
}

/*! \brief widget that allows to set the global settings of a camera
    \ingroup qf3ext_andor

 */
class AndorGlobalCameraSettingsWidget : public QWidget {
        Q_OBJECT
    public:
        explicit AndorGlobalCameraSettingsWidget(int camera, QWidget *parent = 0);
        ~AndorGlobalCameraSettingsWidget();

        /** \brief set the widgets to the given values. This does NOT emit settingsChanged() */
        void setSettings(int fan_mode, bool cooling_on, bool cooling_wait, int temperature, int shutterMode);

        /** \brief set the temperature range */
        void setRange(int min, int max);

        /** \brief force emitting settingsChanged() with the currently set settings */
        void forceSettingsChanged();

        /** \brief set the camera information */
        void setInfo(const QString& info);

        /** \brief display current temperature and whether it has reached the set point
         *
         *  \c progress==-2: temperature off
         *  \c progress==-1: temperature error
         *  \c progress==0: temperature stabilized
         *  \c progress==1: temperature not yet reached
         *  \c progress==2: temperature reached but not yet stabilized
         *  \c progress==3: temperature reached, but drifted
         */
        void showCurrentTemperature(int progress, float temperature);
    signals:
        /** \brief emitted whenever the user changes settings */
        void settingsChanged(int camera, int fan_mode, bool cooling_on, bool cooling_wait, int temperature, int shutterMode);

    private:
        Ui::AndorGlobalCameraSettingsWidget *ui;
        /** \brief the camera this is used for */
        int m_camera;
        /** \brief switch emitting settingsChanged() signals on/off */
        bool m_emit;

    protected slots:
        void valuesChanged();
};

#endif // ANDORGLOBALCAMERASETTINGSWIDGET_H
