/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagingfcsoverviewrateeditor.h"
#include "qfrdrimagingfcsedittools.h"
#include "qfrawdatapropertyeditor.h"

QFRDRImagingFCSOverviewRateEditor::QFRDRImagingFCSOverviewRateEditor(QFPluginServices *services, QFRawDataPropertyEditor *propEditor, QWidget *parent) :
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRImagingFCSOverviewRateEditor::~QFRDRImagingFCSOverviewRateEditor() {
}

void QFRDRImagingFCSOverviewRateEditor::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old) {
    crEditor->connectWidgets(current, old);
    imEditor1->connectWidgets(current, old);
}

void QFRDRImagingFCSOverviewRateEditor::rawDataChanged() {
}

void QFRDRImagingFCSOverviewRateEditor::readSettings() {
    if (!settings) return;
    crEditor->readSettings(*(settings->getQSettings()), QString("imfcsimageeditor/overview_rate/rate_editor/"));
    imEditor1->readSettings(*(settings->getQSettings()), QString("imfcsimageeditor/overview_rate/image_editor1/"));
    jkloadSplitter(*(settings->getQSettings()), mainSplitter, QString("imfcsimageeditor/overview_rate/splitter/"));
}

void QFRDRImagingFCSOverviewRateEditor::writeSettings() {
    if (!settings) return;
    crEditor->writeSettings(*(settings->getQSettings()), QString("imfcsimageeditor/overview_rate/rate_editor/"));
    imEditor1->writeSettings(*(settings->getQSettings()), QString("imfcsimageeditor/overview_rate/image_editor1/"));
    jksaveSplitter(*(settings->getQSettings()), mainSplitter,QString("imfcsimageeditor/overview_rate/splitter/"));
}

void QFRDRImagingFCSOverviewRateEditor::createWidgets() {


    QHBoxLayout* mainLay=new QHBoxLayout();
    setLayout(mainLay);

    mainSplitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    mainLay->addWidget(mainSplitter);

    imEditor1=new QFRDROverviewImageDisplay(this);
    mainSplitter->addWidget(imEditor1);

    crEditor=new QFRDRImagingFCSCountrateDisplay(this);
    mainSplitter->addWidget(crEditor);

    connect(imEditor1, SIGNAL(displayedFrame(double)), crEditor, SLOT(showFrameIndicator1(double)));
}
