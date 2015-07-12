/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qprogresslistwidget.h"
#include <QApplication>

QProgressListWidget::QProgressListWidget(QWidget *parent) :
    QWidget(parent)
{
    m_icons[QProgressListWidget::statusNone]=QIcon(":/qprogresslistwidget/statusNone.png");
    m_icons[QProgressListWidget::statusDone]=QIcon(":/qprogresslistwidget/statusDone.png");
    m_icons[QProgressListWidget::statusFailed]=QIcon(":/qprogresslistwidget/statusFailed.png");
    m_icons[QProgressListWidget::statusNotStarted]=QIcon(":/qprogresslistwidget/statusNotStarted.png");
    m_icons[QProgressListWidget::statusInProgress]=QIcon(":/qprogresslistwidget/statusInProgress.png");
    m_iconSize=QSize(16,16);

    m_centered=true;
    layout=new QGridLayout();
    setLayout(layout);
}

void QProgressListWidget::defineIcon(int status, QIcon icon) {
    m_icons[status]=icon;
}

QIcon QProgressListWidget::getIcon(int status) const {
    return m_icons.value(status, QIcon());
}

int QProgressListWidget::getStatus(int item) const {
    return m_status.value(item, QProgressListWidget::statusNone);
}

QString QProgressListWidget::getText(int item) const {
    return m_items.value(item, "");
}

void QProgressListWidget::deleteItem(int item) {
    m_items.removeAt(item);
    m_status.removeAt(item);
    updateWidgets();
}

int QProgressListWidget::count() const {
    return qMin(m_items.size(), m_status.size());
}

void QProgressListWidget::addItem(const QString &text, int status) {
    m_items.append(text);
    m_status.append(status);
    updateWidgets();
}

QSize QProgressListWidget::getIconSize() const {
    return m_iconSize;
}

void QProgressListWidget::setIconSize(QSize size) {
    m_iconSize=size;
    updateWidgets();
}

bool QProgressListWidget::isCentered() const {
    return m_centered;
}

void QProgressListWidget::setCentered(bool centered) {
    m_centered=centered;
    updateWidgets();
}

void QProgressListWidget::start() {
    for (int i=0; i<count(); i++) {
        if (m_status[i]==QProgressListWidget::statusNotStarted) {
            m_status[i]=QProgressListWidget::statusInProgress;
            break;
        }
    }
    updateWidgets();
}

void QProgressListWidget::nextItem(int newStatusLast) {
    bool findNext=false;
    for (int i=0; i<count(); i++) {
        if (findNext) {
            if (m_status[i]==QProgressListWidget::statusNotStarted) {
                m_status[i]=QProgressListWidget::statusInProgress;
                break;
            }
        } else if (m_status[i]==QProgressListWidget::statusInProgress) {
            m_status[i]=newStatusLast;
            findNext=true;
        }
    }
    updateWidgets();
}

void QProgressListWidget::updateWidgets() {
    int newCount=count();
    int oldCount=m_widgets.size();

    if (newCount<oldCount) {
        for (int i=oldCount-1; i>=newCount; i--) {
            layout->removeWidget(m_widgets[i].labIcon);
            layout->removeWidget(m_widgets[i].labText);
            delete m_widgets[i].labIcon;
            delete m_widgets[i].labText;
            m_widgets.removeAt(i);
        }
    } else if (newCount>oldCount) {
        for (int i=oldCount; i<newCount; i++) {
            QProgressListWidget::widgetstruct w;
            w.labIcon=new QLabel(this);
            w.labIcon->setAlignment(Qt::AlignTop|Qt::AlignRight);
            w.labText=new QLabel(this);
            w.labText->setAlignment(Qt::AlignTop|Qt::AlignLeft);
            int row=layout->rowCount();
            layout->addWidget(w.labIcon, row, 0);
            layout->addWidget(w.labText, row, 1);
            m_widgets.append(w);
        }
    }
    for (int i=0; i<newCount; i++) {
        m_widgets[i].labIcon->setPixmap(m_icons.value(m_status.value(i, QProgressListWidget::statusNone), QIcon()).pixmap(m_iconSize));
        m_widgets[i].labText->setText(m_items.value(i, ""));
    }
    if (layout->columnCount()>1) {
        layout->setColumnStretch(0,0);
        layout->setColumnStretch(1,1);
    }
}

void QProgressListWidget::setItemStatus(int item, int status)
{
    m_status[item]=status;
    updateWidgets();
}

void QProgressListWidget::setItemText(int item, const QString& text) {
    m_items[item]=text;
    updateWidgets();
}

QProgressListDialog::QProgressListDialog(QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f)
{
    m_cancelRejects=false;
    m_wasCanceled=false;

    createWidgets();
}

QProgressListDialog::QProgressListDialog(const QString &labelText, const QString &cancelButtonText, int minimum, int maximum, QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f)
{
    m_cancelRejects=false;
    m_wasCanceled=false;
    m_hasProgressBar=true;

    createWidgets();
    setHasProgressBar(true);
    setRange(minimum, maximum);
    btnCancel->setText(cancelButtonText);
    setProgressText(labelText);

}

void QProgressListDialog::cancel() {
    m_wasCanceled=true;
    reject();
}

void QProgressListDialog::showEvent(QShowEvent *event) {
    m_wasCanceled=false;
    QDialog::showEvent(event);
}

void QProgressListDialog::createWidgets() {

    QGridLayout* layout=new QGridLayout();
    setLayout(layout);
    list=new QProgressListWidget(this);
    layout->addWidget(list, 0,0,1,2);
    progressLine=new QFrame(this);
    progressLine->setVisible(false);
    progressLine->setFrameStyle(QFrame::HLine);
    layout->addWidget(progressLine, 1,0,1,2);
    progress=new QProgressBar(this);
    progress->setVisible(false);
    layout->addWidget(progress, 1,0,1,2);
    labProgress=new QLabel(this);
    labProgress->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
    labProgress->setVisible(false);
    layout->addWidget(labProgress, 2,0,1,2);

    btnCancel=new QPushButton(tr("&Cancel"), this);
    layout->addWidget(btnCancel, 3,1);
    layout->addItem(new QSpacerItem(5,5,QSizePolicy::Expanding),1,0);

    connect(btnCancel, SIGNAL(pressed()), this, SLOT(cancel()));
}

void QProgressListDialog::defineIcon(int status, QIcon icon)
{
    list->defineIcon(status, icon);
}

void QProgressListDialog::nextItem(int newStatusLast)
{
    list->nextItem(newStatusLast);
    QApplication::processEvents();
    QApplication::processEvents();

}

void QProgressListDialog::start()
{
    list->start();
    QApplication::processEvents();
    QApplication::processEvents();

}

void QProgressListDialog::setItemStatus(int item, int status)
{
    list->setItemStatus(item, status);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setItemText(int item, const QString &text)
{
    list->setItemText(item, text);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setRange(int min, int max)
{
    progress->setRange(min, max);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setMinimum(int min)
{
    progress->setMinimum(min);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setMaximum(int max)
{
    progress->setMaximum(max);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setValue(int value)
{
    progress->setValue(value);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setProgressText(const QString &text)
{
    labProgress->setText(text);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setLabelText(const QString &text)
{
    setProgressText(text);
}

void QProgressListDialog::addItem(const QString &text, int status)
{
    list->addItem(text, status);
    repaint();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListDialog::setHasCancelButton(bool hasCancel, bool cancelRejects)
{
    btnCancel->setVisible(hasCancel);
    m_cancelRejects=cancelRejects;
}

void QProgressListDialog::setHasProgressBar(bool hasProg) {
    m_hasProgressBar=hasProg;
    labProgress->setVisible(m_hasProgressBar);
    progress->setVisible(m_hasProgressBar);
    progressLine->setVisible(m_hasProgressBar);
}

bool QProgressListDialog::wasCanceled() const
{
    return m_wasCanceled;
}

QProgressListWidget * QProgressListDialog::getProgressList() const
{
    return list;
}

void QProgressListDialog::reset()
{
    list->reset();
    QApplication::processEvents();
    QApplication::processEvents();
}

void QProgressListWidget::reset()
{
    for (int i=0; i<count(); i++) {
        if (m_status[i]>QProgressListWidget::statusNone && m_status[i]<QProgressListWidget::statusUser) {
            m_status[i]=QProgressListWidget::statusNotStarted;
        }
    }
    updateWidgets();
}

