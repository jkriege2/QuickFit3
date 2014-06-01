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



