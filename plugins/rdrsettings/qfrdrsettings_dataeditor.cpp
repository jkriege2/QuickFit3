#include "qfrdrsettings_dataeditor.h"
#include "qfrdrsettings_data.h"

QFRDRSettingsDataEditor::QFRDRSettingsDataEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
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
    filterModel=new QSortFilterProxyModel(this);
    filterModel->setSourceModel(model);
    filterModel->setDynamicSortFilter(true);
    tree->setModel(filterModel);

    edtFilterName=new QFEnhancedLineEdit(this);
    edtFilterName->setToolTip(tr("enter a phrase and the settings tree will be <b>filtered</b>, so it only contains entries that contain the filter string in either their path or value.<br><br>You may use <b>wildcards</b> <tt>*</tt> to match >=0 and <tt>?</tt> to match exactly one unknown character."));
    connect(edtFilterName, SIGNAL(textChanged(QString)), this, SLOT(nameFilterChanged(QString)));

    edtFilterName->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterName, edtFilterName));
    QFCompleterFromFile* cc=new QFCompleterFromFile(this);
    cc->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"plugins/rdr_settings/completers/filter.txt");
    edtFilterName->setCompleter(cc);


    l->addWidget(new QLabel("<b>filename:</b>"), 0,0);
    l->addWidget(labFilename,0,1,1,3);
    l->addWidget(new QLabel("<b>filter:</b>"), 1,0);
    l->addWidget(edtFilterName,1,1);
    QLabel* lab=new QLabel("<b>contents:</b>");
    lab->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->addWidget(lab, 2,0);
    l->addWidget(tree, 2,1,1,3);
    l->setRowStretch(2,1);
    l->setColumnStretch(1,1);
    l->setColumnStretch(3,1);

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
        edtFilterName->setText("");


    } else {
        edtFilterName->setText("");
        labFilename->setText("");
        model->setSettings(NULL);
    }

}

void QFRDRSettingsDataEditor::nameFilterChanged(const QString &filter) {
    QString f=filter;
    /*if (!f.startsWith('*')) f="*"+f;
    if (!f.endsWith('*')) f=f+"*";*/
    model->setFilter(f, QRegExp::Wildcard);
    tree->expandAll();
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



