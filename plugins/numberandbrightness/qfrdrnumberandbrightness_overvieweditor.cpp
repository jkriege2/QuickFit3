/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfrdrnumberandbrightness_overvieweditor.h"
#include "qfrdrnumberandbrightness_data.h"


QFRDRNumberAndBrightnessOverviewEditor::QFRDRNumberAndBrightnessOverviewEditor(QFPluginServices* services,  QFRawDataPropertyEditor*  propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRNumberAndBrightnessOverviewEditor::~QFRDRNumberAndBrightnessOverviewEditor()
{
    //dtor
}

void QFRDRNumberAndBrightnessOverviewEditor::createWidgets() {
    display=new QFRDROverviewImageDisplay(this);
    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(display);
}

void QFRDRNumberAndBrightnessOverviewEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:

    display->connectWidgets(current, old);
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		
        rawDataChanged();
    } else {
    }

}


void QFRDRNumberAndBrightnessOverviewEditor::rawDataChanged() {
	// emitted when the raw data changes 
}

void QFRDRNumberAndBrightnessOverviewEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    display->readSettings(*(settings->getQSettings()), QString("QFRDRNumberAndBrightnessOverviewEditor/"));
}


void QFRDRNumberAndBrightnessOverviewEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    display->writeSettings(*(settings->getQSettings()), QString("QFRDRNumberAndBrightnessOverviewEditor/"));
}



