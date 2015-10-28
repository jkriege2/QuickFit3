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

#include "dlgwelcomescreen.h"
#include "ui_dlgwelcomescreen.h"
#include "programoptions.h"
#include "qfversion.h"
#include "qfenhancedlabel.h"
#include "qfelidedlabel.h"
#include <QDesktopWidget>

DlgWelcomeScreen::DlgWelcomeScreen(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::DlgWelcomeScreen)
{
    ui->setupUi(this);
    ui->chkShow->setChecked(ProgramOptions::getConfigValue("quickfit/welcomescreen", true).toBool());
    ui->labQF->setText(tr("QuickFit %1 <small>SVN: %2 - compiledate: %3</small>").arg(qfInfoVersion()).arg(qfInfoGITVersion()).arg(qfInfoCompileDate()));
    ui->labThesis->setOpenUrl(QUrl("http://www.ub.uni-heidelberg.de/archiv/17187"));
    ui->labExamples->setOpenUrl(QUrl("http://www.dkfz.de/Macromol/quickfit/examples.html"));
    ui->labWebsite->setOpenUrl(QUrl(qfInfoWeblink()));
    ui->labWebsite->setText(qfInfoWeblink());
    ui->labSourcecode->setOpenUrl(QUrl(qfInfoSourceWeblink()));
    ui->labSourcecode->setText(qfInfoSourceWeblink());
    ui->toolBox->setCurrentIndex(ProgramOptions::getConfigValue("welcomescreen/currentIndex", 0).toInt());
    resize(QSize(650,650));
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QApplication::desktop()->availableGeometry()
        ));
    ProgramOptions::getConfigWindowGeometry(this, "welcomescreen/size");
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgWelcomeScreen::~DlgWelcomeScreen()
{
    ProgramOptions::setConfigValue("quickfit/welcomescreen", ui->chkShow->isChecked());
    ProgramOptions::setConfigValue("welcomescreen/currentIndex", ui->toolBox->currentIndex());
    ProgramOptions::setConfigWindowGeometry(this, "welcomescreen/size");
    delete ui;
}

void DlgWelcomeScreen::addAction(DlgWelcomeScreen::Screens s, QAction *act, int pixmapSize, bool closeOnClick)
{
    QGridLayout* lay=NULL;
    if (s==scFirst) {
        lay=ui->layFirst;
    } else if (s==scRecent) {
        lay=ui->layRecent;
    } else if (s==scTutorials) {
        lay=ui->layTutorials;
    }
    if (lay) {
        QFEnhancedLabel* li;
        QFElidedLabel* lt;
        int c=lay->rowCount();
        lay->addWidget(li=new QFEnhancedLabel(this), c,0);
        lay->addWidget(lt=new QFElidedLabel(this), c,1);
        li->setPixmap(act->icon().pixmap(pixmapSize));
        li->setUnderlineOnHover(true);
        li->setCursor(Qt::PointingHandCursor);
        QString s=act->text();
        s=s.replace("&&", "****++****");
        s=s.remove("&");
        s=s.replace("****++****", "&");
        lt->setText(s);
        lt->setUnderlineOnHover(true);
        lt->setCursor(Qt::PointingHandCursor);
        lt->setAlignment(Qt::AlignBottom|Qt::AlignLeft);
        connect(li, SIGNAL(clicked()), act, SLOT(trigger()));
        connect(lt, SIGNAL(clicked()), act, SLOT(trigger()));
        if (closeOnClick) {
            connect(li, SIGNAL(clicked()), this, SLOT(close()));
            connect(lt, SIGNAL(clicked()), this, SLOT(close()));
        }

    }
}

void DlgWelcomeScreen::addSeparator(DlgWelcomeScreen::Screens s)
{
    QGridLayout* lay=NULL;
    if (s==scFirst) {
        lay=ui->layFirst;
    } else if (s==scRecent) {
        lay=ui->layRecent;
    } else if (s==scTutorials) {
        lay=ui->layTutorials;
    }
    if (lay) {
        int c=lay->rowCount();
        QFrame* f;
        lay->addWidget(f=new QFrame(this), c,0, 1, 2);
        f->setLineWidth(1);
        f->setFrameStyle(QFrame::HLine);
        f->setFrameShadow(QFrame::Plain);
    }
}

void DlgWelcomeScreen::finalizeLayout()
{
    ui->layFirst->addItem(new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Expanding), ui->layFirst->count(),0);
    ui->layFirst->setColumnStretch(0,0);
    ui->layFirst->setColumnStretch(1,1);
    ui->layRecent->addItem(new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Expanding), ui->layRecent->count(),0);
    ui->layRecent->setColumnStretch(0,0);
    ui->layRecent->setColumnStretch(1,1);
    ui->layTutorials->addItem(new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Expanding), ui->layTutorials->count(),0);
    ui->layTutorials->setColumnStretch(0,0);
    ui->layTutorials->setColumnStretch(1,1);
}

void DlgWelcomeScreen::on_chkShow_toggled(bool /*enabled*/)
{
    ProgramOptions::setConfigValue("quickfit/welcomescreen", ui->chkShow->isChecked());
}

void DlgWelcomeScreen::on_btnClose_clicked()
{
    close();
}
