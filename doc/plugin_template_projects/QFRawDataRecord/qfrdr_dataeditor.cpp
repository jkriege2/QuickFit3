/*
Copyright (c) 2014
	
	last modification: $LastChangedDate$  (revision $Rev$)

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


#include "qfrdr_dataeditor.h"
#include "qfrdr_data.h"

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QFRDRDataEditor::QFRDRDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRDataEditor::~QFRDRDataEditor()
{
    //dtor
}

void QFRDRDataEditor::createWidgets() {
    /* create you widgets here */
	QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);

    labTest=new QLabel(this);
	l->addWidget(labTest);

};

void QFRDRDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRData* m=qobject_cast<QFRDRData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
		
        rawDataChanged();
    } else {
		labTest->setText(tr("error: not the correct record class!!!"));
    }

};

void QFRDRDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRData* m=qobject_cast<QFRDRData*>(current);
	labTest->setText(tr("loaded record with files<br><i>%1</i><br><br><b>data: %2</b>").arg(m->getFiles().join("<br>")).arg(m->getData()));
};

void QFRDRDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    QString data=settings->getQSettings()->value(QString("target_id/editor/data"), "blabla").toString();
};


void QFRDRDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("target_id/editor/data"), "blabla");
};



