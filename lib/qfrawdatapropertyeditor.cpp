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

#include "qfrawdatapropertyeditor.h"
#include "dlgnewproperty.h"
#include "qfrawdatarecordfactory.h"
#include "qfversion.h"
#include "qfhtmldelegate.h"
#include "qfstyleditemdelegate.h"
#include <QModelIndex>
#include <QModelIndexList>
#include "qfhistogramservice.h"
#include <QItemSelectionModel>
#include <QItemSelection>

#include "qfrawdatapropertyeditor_private.h"

// TODO: add some more options to the fit results display: store column under different name


QFRawDataPropertyEditor::QFRawDataPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFRawDataRecord* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    p=new QFRawDataPropertyEditor_private(this);

    p->timerResizeProps=new QTimer(this);
    p->timerResizeProps->setSingleShot(true);
    p->timerResizeProps->setInterval(500);
    p->services=services;
    p->settings=set;
    p->id=id;
    connect(p->timerResizeProps, SIGNAL(timeout()), p, SLOT(resizePropertiesTable()));

    setSettings(set);
    //std::cout<<"creating QFRawDataPropertyEditor ... creating widgets ...\n";
    p->createWidgets();
    //std::cout<<"creating QFRawDataPropertyEditor ... setting current ...\n";
    setCurrent(current);
    //std::cout<<"creating QFRawDataPropertyEditor ... DONE!\n";
    readSettings();
}

QFRawDataPropertyEditor::~QFRawDataPropertyEditor()
{
    writeSettings();
}



void QFRawDataPropertyEditor::closeEvent ( QCloseEvent * event ) {
    writeSettings();
    event->accept();
}




void QFRawDataPropertyEditor::resizeEvent ( QResizeEvent * event ) {
    p->labAveragedresults->setMaximumWidth(event->size().width());
    p->cmbGroup->setMaximumWidth(event->size().width()/3);
    QWidget::resizeEvent(event);
    p->labTop->setMaximumWidth(event->size().width()/2);
}



QMenuBar *QFRawDataPropertyEditor::getMenuBar() const
{
    return p->menuBar;
}

void QFRawDataPropertyEditor::setMenuBarVisible(bool visible)
{
    p->menuBar->setVisible(visible);
}

QMenu *QFRawDataPropertyEditor::addMenu(const QString& title, int editor)
{
    p->menuBar->removeAction(p->menuHelp->menuAction());
    QMenu* m=p->menuBar->addMenu(title);
    p->menuBar->addAction(p->menuHelp->menuAction());
    p->menus.append(qMakePair(editor, m));
    p->currentTabChanged(p->tabMain->currentIndex());
    return m;
}

QMenu *QFRawDataPropertyEditor::addOrFindMenu(const QString &title, int editor)
{
    for (int i=0; i<p->menus.size(); i++)  {
        if ((p->menus[i].first==editor || p->menus[i].first<0) && p->menus[i].second && p->menus[i].second->title()==title) return p->menus[i].second;
    }
    return addMenu(title, editor);
}

void QFRawDataPropertyEditor::registerMenu(QMenu *menu, int editor) {
    p->menus.append(qMakePair(editor, menu));
    p->currentTabChanged(p->tabMain->currentIndex());
}



void QFRawDataPropertyEditor::setCurrent(QFRawDataRecord* c) {
    p->setCurrent(c);
}

QFRawDataRecord *QFRawDataPropertyEditor::getCurrent() const
{
    return p->current;
}


void QFRawDataPropertyEditor::setSettings(ProgramOptions* settings) {
    //std::cout<<"QFRawDataPropertyEditor::setSettings("<<settings<<")\n";
    p->settings=settings;
    if (p->current && p->tabMain) {
        for (int i=0; i<p->current->getEditorCount(); i++) {
            if (qobject_cast<QFRawDataEditor *>(p->tabMain->widget(i))) {
                qobject_cast<QFRawDataEditor *>(p->tabMain->widget(i))->setSettings(settings, p->id);
            }
        }
    }
    readSettings();
}

void QFRawDataPropertyEditor::readSettings() {
    if (!p->settings) return;
    loadWidgetGeometry(*(p->settings->getQSettings()), this, QPoint(20, 20), QSize(800, 600), "rawdatapropeditor/");
    if (p->tabMain) {
        //helpWidget->readSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
        //int idx=settings->getQSettings()->value("rawdatapropeditor/currentTab", 0).toInt();
        //if ((idx>=0) && (idx<tabMain->count())) tabMain->setCurrentIndex(idx);
    }
    for (int i=0; i<p->editorList.size(); i++) {
        if (p->editorList[i]) {
            p->editorList[i]->readSettings();
        }
    }
    p->currentSaveDir=p->settings->getQSettings()->value("rawdatapropeditor/lastSaveDir", p->currentSaveDir).toString();
}

void QFRawDataPropertyEditor::writeSettings() {
    if (!p->settings) return;
    saveWidgetGeometry(*(p->settings->getQSettings()), this, "rawdatapropeditor/");
    if (p->tabMain) {
        p->settings->getQSettings()->setValue("rawdatapropeditor/currentTab", p->tabMain->currentIndex());
        //helpWidget->writeSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
    }
    for (int i=0; i<p->editorList.size(); i++) {
        if (p->editorList[i]) {
            p->editorList[i]->writeSettings();
        }
    }
    p->settings->getQSettings()->setValue("rawdatapropeditor/lastSaveDir", p->currentSaveDir);
}


QMenu *QFRawDataPropertyEditor::getHelpMenu() const
{
    return p->menuHelp;
}

void QFRawDataPropertyEditor::showTab(int tab)
{
    p->tabMain->setCurrentIndex(tab);
}

int QFRawDataPropertyEditor::getTabs() const
{
    return p->tabMain->count();
}

void QFRawDataPropertyEditor::sendEditorCommand(const QString &command, const QVariant &param1, const QVariant &param2, const QVariant &param3, const QVariant &param4, const QVariant &param5)
{
    for (int i=0; i<p->editorList.size(); i++) {
        if (p->editorList[i]) p->editorList[i]->sendEditorCommand(command, param1, param2, param3, param4, param5);
    }
}

QList<QPointer<QFRawDataEditor> > QFRawDataPropertyEditor::getEditorList() const
{
    return p->editorList;
}


