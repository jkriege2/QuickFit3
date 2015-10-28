/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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


#include "qfplayercontrols.h"
#include <QGridLayout>
#include <cmath>

QFPLayerControls::QFPLayerControls(QWidget *parent) :
    QWidget(parent)
{
    timer=new QTimer(this);
    timer->setInterval(1000/20);
    timer->setSingleShot(true);
    timer->stop();

    realFPS=0;
    fpsCounter=0;
    lastPlaying=false;

    createActions();
    createWidgets();


    connect(timer, SIGNAL(timeout()), this, SLOT(timerTriggered()));
}

void QFPLayerControls::storeSettings(QSettings &/*settings*/, QString /*prefix*/) const {
}

void QFPLayerControls::readSettings(QSettings &/*settings*/, QString /*prefix*/) {
}

void QFPLayerControls::setSingleShot(bool singleShot) {
    timer->setSingleShot(singleShot);
}

void QFPLayerControls::setRange(int min, int max) {
    slider->setRange(min, max);
    updateWidgets();
}

void QFPLayerControls::setFPS(double FPS) {
    spinFPS->setValue(FPS);
    updateWidgets();
}

void QFPLayerControls::singleShotTimerStart() {
    timer->setSingleShot(true);
    timer->start();
}

void QFPLayerControls::updateWidgets() {
    if (actPlayPause->isChecked()) {
        actPlayPause->setIcon(QIcon(":/libqf3widgets/player_pause.png"));
        actPlayPause->setText(tr("pause"));
    } else {
        actPlayPause->setIcon(QIcon(":/libqf3widgets/player_play.png"));
        actPlayPause->setText(tr("play"));
    }

    actRewind->setEnabled(slider->value()>slider->minimum());
    actNextFrame->setEnabled(slider->value()<slider->maximum());
    actNextMoreFrame->setEnabled(slider->value()<=slider->maximum()-slider->pageStep());
    actPrevFrame->setEnabled(slider->value()>slider->minimum());
    actPrevMoreFrame->setEnabled(slider->value()>=slider->minimum()+slider->pageStep());

    timer->setInterval(qMax(5,(int)round(1000.0/spinFPS->value())));
}

void QFPLayerControls::timerTriggered() {
    if (actPlayPause->isChecked()) {
        fpsCounter++;
        if (fpsCounter>2) realFPS=double(fpsCounter)/(double(fpsTimer.elapsed())/1000.0);
        if (fpsCounter>30) {
            fpsCounter=0;
            fpsTimer.start();
        }
        if (slider->value()<slider->maximum()) {
            slider->setValue(slider->value()+1);
        } else {
            if (chkReplay->isChecked()) {
                slider->setValue(slider->minimum());
            } else {
                timer->stop();
            }
        }
    }
    updateWidgets();
}

void QFPLayerControls::playPauseTriggered(bool checked) {
    timer->setInterval(qMax(5,(int)round(1000.0/spinFPS->value())));
    if (checked) {
        timer->start();
        realFPS=0;
        fpsCounter=0;
        fpsTimer.start();
    } else {
        timer->stop();
    }

    updateWidgets();
}

void QFPLayerControls::createWidgets() {
    QGridLayout* lay=new QGridLayout();
    setLayout(lay);
    QToolButton* btn=new QToolButton(this);
    btn->setDefaultAction(actRewind);
    lay->addWidget(btn, 0, 0);
    btn=new QToolButton(this);
    btn->setDefaultAction(actPlayPause);
    lay->addWidget(btn, 0, 1);

    QWidget* w=new QWidget(this);
    w->setMinimumWidth(10);
    lay->addWidget(w, 0, 2);

    btn=new QToolButton(this);
    btn->setDefaultAction(actPrevMoreFrame);
    lay->addWidget(btn, 0, 3);

    btn=new QToolButton(this);
    btn->setDefaultAction(actPrevFrame);
    lay->addWidget(btn, 0, 4);

    btn=new QToolButton(this);
    btn->setDefaultAction(actNextFrame);
    lay->addWidget(btn, 0, 5);

    btn=new QToolButton(this);
    btn->setDefaultAction(actNextMoreFrame);
    lay->addWidget(btn, 0, 6);

    lay->addItem(new QSpacerItem(20,5), 0, 7);
    chkReplay=new QCheckBox(tr("replay"), this);
    chkReplay->setChecked(true);
    lay->addWidget(chkReplay, 0, 8);
    lay->addItem(new QSpacerItem(5,5, QSizePolicy::Expanding), 0, 9);
    label=new QLabel("", this);
    label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    lay->addWidget(label, 0, 10, 1, 3);

    slider=new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksBelow);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    lay->addWidget(slider, 1, 0, 1, 10);

    spinFPS=new QDoubleSpinBox(this);
    spinFPS->setRange(0.1, 100);
    spinFPS->setValue(10);
    connect(spinFPS, SIGNAL(valueChanged(double)), this, SLOT(updateWidgets()));
    lay->addWidget(new QLabel(tr("FPS:"), this), 1, 10);
    lay->addWidget(spinFPS, 1, 11);
    lay->addItem(new QSpacerItem(10,5,QSizePolicy::MinimumExpanding,QSizePolicy::Preferred), 1,12);
    updateWidgets();
}

void QFPLayerControls::createActions() {
    actPlayPause=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_play.png"), tr("play"), this);
    actPlayPause->setCheckable(true);
    actPlayPause->setChecked(false);
    lastPlaying=false;
    connect(actPlayPause, SIGNAL(toggled(bool)), this, SLOT(playPauseTriggered(bool)));
    actRewind=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_start.png"), tr("back to first frame"), this);
    connect(actRewind, SIGNAL(triggered()), this, SLOT(rewind()));
    actNextFrame=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_fwd.png"), tr("one frame forward"), this);
    connect(actNextFrame, SIGNAL(triggered()), this, SLOT(nextTriggered()));
    actNextMoreFrame=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_ffwd.png"), tr("some frames forward"), this);
    connect(actNextMoreFrame, SIGNAL(triggered()), this, SLOT(nextMoreTriggered()));
    actPrevFrame=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_rew.png"), tr("one frame backward"), this);
    connect(actPrevFrame, SIGNAL(triggered()), this, SLOT(prevTriggered()));
    actPrevMoreFrame=new QFActionWithNoMenuRole(QIcon(":/libqf3widgets/player_rrew.png"), tr("some frames backward"), this);
    connect(actPrevMoreFrame, SIGNAL(triggered()), this, SLOT(prevMoreTriggered()));
}

void QFPLayerControls::hideEvent(QHideEvent *event) {
    lastPlaying=actPlayPause->isChecked();
    pause();
    QWidget::hideEvent(event);
}

void QFPLayerControls::showEvent(QShowEvent *event) {
    if (lastPlaying) play();
    QWidget::showEvent(event);
}

void QFPLayerControls::play() {
    actPlayPause->setChecked(true);
}

void QFPLayerControls::pause() {
    actPlayPause->setChecked(false);
}

void QFPLayerControls::rewind() {
    slider->setValue(slider->minimum());
}

void QFPLayerControls::setPosition(int frame) {
    slider->setValue(frame);
}

void QFPLayerControls::setReplay(bool replay) {
    chkReplay->setChecked(replay);
}

void QFPLayerControls::sliderMoved(int value) {
    emit showFrame(value);
    label->setText(tr("frame %1/%2 (%3fps)").arg(value-slider->minimum()+1).arg(slider->maximum()-slider->minimum()+1).arg(realFPS, 0, 'f', 1));
}

void QFPLayerControls::prevMoreTriggered() {
    slider->triggerAction(QAbstractSlider::SliderPageStepSub);
}

void QFPLayerControls::nextMoreTriggered() {
    slider->triggerAction(QAbstractSlider::SliderPageStepAdd);
}

void QFPLayerControls::prevTriggered() {
    slider->triggerAction(QAbstractSlider::SliderSingleStepSub);
}

void QFPLayerControls::nextTriggered() {
    slider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}

double QFPLayerControls::getRelativeValue() const {
    return double(slider->value()-slider->minimum())/double(slider->maximum()-slider->minimum());
}

int QFPLayerControls::getPosition() const {
    return slider->value();
}

bool QFPLayerControls::getReplay() const {
    return chkReplay->isChecked();
}

double QFPLayerControls::getFPS() const {
    return spinFPS->value();
}

bool QFPLayerControls::isPlaying() const
{
    return actPlayPause->isChecked();
}

