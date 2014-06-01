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
