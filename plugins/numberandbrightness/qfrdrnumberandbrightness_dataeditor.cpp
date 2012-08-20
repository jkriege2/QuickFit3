#include "qfrdrnumberandbrightness_dataeditor.h"
#include "qfrdrnumberandbrightness_data.h"

QFRDRNumberAndBrightnessDataEditor::QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRNumberAndBrightnessDataEditor::~QFRDRNumberAndBrightnessDataEditor()
{
    //dtor
}

void QFRDRNumberAndBrightnessDataEditor::createWidgets() {

}

void QFRDRNumberAndBrightnessDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
		
        rawDataChanged();
    } else {
    }

}

void QFRDRNumberAndBrightnessDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRNumberAndBrightnessData* m=qobject_cast<QFRDRNumberAndBrightnessData*>(current);
}

void QFRDRNumberAndBrightnessDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
}


void QFRDRNumberAndBrightnessDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
}



