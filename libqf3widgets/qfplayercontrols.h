/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFPLAYERCONTROLS_H
#define QFPLAYERCONTROLS_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QDoubleSpinBox>
#include <QTimer>
#include <QSlider>
#include <QSettings>
#include <QCheckBox>
#include <QElapsedTimer>
#include "libwid_imexport.h"
#include <QResizeEvent>



/*! \brief a This widget may be used as a media player control panel
    \ingroup qf3lib_widgets

    There are two modes:
      - if \c setSingleShot(true) you will have to manually call singleShotTimerStart() at the end of your
        handler for showFrame(int frame).
      - if \c setSingleShot(false) the signal showFrame(int frame) will be emited regulary and you are responsible for
        handling it fast enough.
    .

*/

class QFWIDLIB_EXPORT QFPLayerControls : public QWidget {
        Q_OBJECT
    public:
        explicit QFPLayerControls(QWidget *parent = 0);

        void storeSettings(QSettings& settings, QString prefix=QString("")) const;
        void readSettings(QSettings& settings, QString prefix=QString(""));

        void setSingleShot(bool singleShot=true);

        double getRelativeValue() const;

        int getPosition() const;
        bool getReplay() const;
        double getFPS() const;
        bool isPlaying() const;

    signals:
        void showFrame(int frame);
    public slots:
        void play();
        void pause();
        void rewind();
        void setPosition(int frame);
        void setReplay(bool replay);
        void setRange(int min, int max);
        void setFPS(double FPS);
        void singleShotTimerStart();


    protected slots:
        void updateWidgets();
        void timerTriggered();

        void playPauseTriggered(bool checked);
        void nextTriggered();
        void prevTriggered();
        void nextMoreTriggered();
        void prevMoreTriggered();

        void sliderMoved(int value);


    protected:
        void createWidgets();
        void createActions();

        void hideEvent(QHideEvent* event);
        void showEvent(QShowEvent* event);

        QTimer* timer;

        QSlider* slider;
        QDoubleSpinBox* spinFPS;
        QCheckBox* chkReplay;
        QLabel* label;
        QAction* actPlayPause;
        QAction* actNextFrame;
        QAction* actNextMoreFrame;
        QAction* actPrevFrame;
        QAction* actPrevMoreFrame;
        QAction* actRewind;
        QElapsedTimer fpsTimer;
        int fpsCounter;
        double realFPS;
        bool lastPlaying;
};

#endif // QFPLAYERCONTROLS_H

