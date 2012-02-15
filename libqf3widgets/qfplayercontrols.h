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
#include "libwid_imexport.h"



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
};

#endif // QFPLAYERCONTROLS_H

