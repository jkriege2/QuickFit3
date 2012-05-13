#include "qfrdrsettings_dataeditor.h"
#include "qfrdrsettings_data.h"

QFRDRSettingsDataEditor::QFRDRSettingsDataEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
}

QFRDRSettingsDataEditor::~QFRDRSettingsDataEditor()
{
    //dtor
}

void QFRDRSettingsDataEditor::createWidgets() {
    /* create you widgets here */
    QGridLayout* l=new QGridLayout(this);
    setLayout(l);

    labFilename=new QLabel(this);
    labFilename->setTextInteractionFlags(Qt::TextSelectableByMouse);

    tree=new QTreeView(this);
    model=new QSettingsModel(this);
    model->setByDepthColoring(true);
    model->setReadOptimizedColoring(true);
    tree->setModel(model);

    l->addWidget(new QLabel("<b>filename:</b>"), 0,0);
    l->addWidget(labFilename,0,1);
    QLabel* lab=new QLabel("<b>contents:</b>");
    lab->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->addWidget(lab, 1,0);
    l->addWidget(tree, 1,1);
    l->setRowStretch(1,1);
    l->setColumnStretch(1,1);

};

void QFRDRSettingsDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    QFRDRSettingsData* m=qobject_cast<QFRDRSettingsData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
		// if so (and if current!=NULL anyways), connect to the new record and read some data
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        model->setSettings(m->getData());
        rawDataChanged();
        tree->expandAll();
        tree->header()->setStretchLastSection(true);
        tree->resizeColumnToContents(0);
        tree->resizeColumnToContents(1);
    } else {
        labFilename->setText("");
        model->setSettings(NULL);
    }

}

void QFRDRSettingsDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRSettingsData* m=qobject_cast<QFRDRSettingsData*>(current);
    labFilename->setText(m->getFiles().value(0,""));
}

void QFRDRSettingsDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
}


void QFRDRSettingsDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
}



