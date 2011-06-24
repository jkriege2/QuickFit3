#include "qfrdrimagingfcs_dataeditor.h"
#include "qfrdrimagingfcs_data.h"

QFRDRImagingFCSDataEditor::QFRDRImagingFCSDataEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
}

QFRDRImagingFCSDataEditor::~QFRDRImagingFCSDataEditor()
{
    //dtor
}

void QFRDRImagingFCSDataEditor::createWidgets() {
    /* create you widgets here */
	QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);

    labTest=new QLabel(this);
	l->addWidget(labTest);

};

void QFRDRImagingFCSDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:

    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));

        rawDataChanged();
    } else {
		labTest->setText(tr("error: not the correct record class!!!"));
    }

};

void QFRDRImagingFCSDataEditor::rawDataChanged() {
	// emitted when the raw data changes
	QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
	labTest->setText(tr("loaded record with files<br><i>%1</i>").arg(m->getFiles().join("<br>")));//.arg(m->getData()));
};

void QFRDRImagingFCSDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    QString data=settings->getQSettings()->value(QString("imaging_fcs/editor/data"), "blabla").toString();
};


void QFRDRImagingFCSDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imaging_fcs/editor/data"), "blabla");
};



