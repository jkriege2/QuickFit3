/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

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

#include "jktetrismainwindow.h"
#include "ui_jktetrismainwindow.h"
#include "programoptions.h"

JKTetrisMainWindow::JKTetrisMainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JKTetrisMainWindow)
{
    ui->setupUi(this);

    ui->Tetris->setLevelLabel(ui->labLevel);
    ui->Tetris->setScoreLabel(ui->labPoints);
    ui->Tetris->setLinesRemovedLabel(ui->labLinesRemoved);
    ui->Tetris->setNextPieceLabel(ui->labNext);
    connect(ui->btnStart, SIGNAL(clicked()), ui->Tetris, SLOT(start()));
    connect(ui->btnPause, SIGNAL(toggled(bool)), ui->Tetris, SLOT(pause(bool)));
    connect(ui->Tetris, SIGNAL(pauseEnabled(bool)), ui->btnPause, SLOT(setEnabled(bool)));
    connect(ui->Tetris, SIGNAL(gameOver(int,int)), this, SLOT(gameOver(int,int)));
    connect(ui->Tetris, SIGNAL(setPauseState(bool)), ui->btnPause, SLOT(setChecked(bool)));
    ui->Tetris->start();
    ProgramOptions::getConfigWindowGeometry(this, "quickfit/entertain/window");
    ui->labHighscore->setText(tr("HIGHSCORE: %1 [level: %3] (%2)")
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_date", QDate::currentDate()).toDate().toString())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_level", 0).toInt()));
    setAttribute(Qt::WA_DeleteOnClose);
}

JKTetrisMainWindow::~JKTetrisMainWindow()
{
    delete ui;
}

void JKTetrisMainWindow::gameOver(int score, int level)
{
    QMessageBox::information(this, tr("Game Over!"), tr("Your game is over in level %2!\n\nSCORE = %1\n").arg(score).arg(level));
    if (score>ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt()) {
        ProgramOptions::setConfigValue("quickfit/entertain/highscore", score);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_level", level);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_date", QDate::currentDate());
    }
    ui->labHighscore->setText(tr("HIGHSCORE: %1 [level: %3] (%2)")
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_date", QDate::currentDate()).toDate().toString())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_level", 0).toInt()));
}

void JKTetrisMainWindow::on_btnClose_clicked()
{
    close();
}

void JKTetrisMainWindow::closeEvent(QCloseEvent *event)
{
    ProgramOptions::setConfigWindowGeometry(this, "quickfit/entertain/window");

    int score=ui->Tetris->getScore();
    int level=ui->Tetris->getLevel();

    if (score>ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt()) {
        ProgramOptions::setConfigValue("quickfit/entertain/highscore", score);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_level", level);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_date", QDate::currentDate());
    }

    QWidget::closeEvent(event);
}



void JKTetrisMainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {
            if (ui->btnPause->isEnabled()) {
                ui->btnPause->setChecked(true);
            }
        }
    }
}

void JKTetrisMainWindow::resizeEvent(QResizeEvent *event)
{
    int w=event->size().width()/3;
    int h=event->size().height();
    int bw=ui->Tetris->getBoardWidth();
    int bh=ui->Tetris->getBoardHeight();
    QSize ts=ui->Tetris->getTileSize();
    ts.setWidth(qMin(w/bw, h/bh));
    ts.setHeight(ts.width());

    ts=QSize(qBound(16,ts.width(), 32), qBound(16,ts.height(), 32));
    //qDebug()<<ts<<w<<h<<bw<<bh;
    ui->Tetris->setTileSize(ts);
    QWidget::resizeEvent(event);
}
