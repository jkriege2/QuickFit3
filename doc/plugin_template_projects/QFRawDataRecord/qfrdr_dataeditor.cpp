#include "qfrdr_dataeditor.h"
#include "qfrdr_data.h"

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



