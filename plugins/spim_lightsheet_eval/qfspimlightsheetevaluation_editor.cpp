#include "qfspimlightsheetevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfspimlightsheetevaluation_item.h"
#include "ui_qfspimlightsheetevaluation_editor.h"

#include <QtGui>
#include <QtCore>

QFSPIMLightsheetEvaluationEditor::QFSPIMLightsheetEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFSPIMLightsheetEvaluationEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";

    lastMousePreviewX=0;
    lastMousePreviewY=0;

    // setup widgets
    ui->setupUi(this);

    ui->pltFit->get_plotter()->get_xAxis()->set_axisLabel(tr("position [µm]"));
    ui->pltFit->get_plotter()->get_yAxis()->set_axisLabel(tr("intensity [A.U.]"));
    ui->pltFit->get_plotter()->set_plotLabel(tr("image frame cut and fit"));
    ui->pltImage->get_plotter()->set_plotLabel(tr("image frame"));




    prepareImagePlotter(ui->pltImage);

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, JKQTPMathImage::GRAY, ui->pltImage->get_plotter());
    ui->pltImage->addGraph(plteImage);
    c_markDataX=ui->pltImage->getDatastore()->addColumn(markDataX, 2, "mark X");
    c_markDataY=ui->pltImage->getDatastore()->addColumn(markDataY, 2, "mark Y");
    plteMark=new JKQTPxyLineGraph(ui->pltImage->get_plotter());
    QColor c=QColor("red");
    c.setAlphaF(0.5);
    plteMark->set_color(c);
    plteMark->set_lineWidth(3);
    plteMark->set_drawLine(true);
    plteMark->set_symbol(JKQTPnoSymbol);
    plteMark->set_xColumn(c_markDataX);
    plteMark->set_yColumn(c_markDataY);
    ui->pltImage->addGraph(plteMark);







    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(NULL);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    //connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    
    updatingData=false;
}

QFSPIMLightsheetEvaluationEditor::~QFSPIMLightsheetEvaluationEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFSPIMLightsheetEvaluationEditor::prepareImagePlotter(JKQtPlotter* pltImage) {
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltImage->set_zoomByDoubleAndRightMouseClick(false);
    pltImage->set_displayMousePosition(true);
    pltImage->set_displayToolbar(false);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);
    pltImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getXAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getYAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getXAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getYAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getXAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getXAxis()->addAxisTickLabel(0,"0");
    pltImage->get_plotter()->getYAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getYAxis()->addAxisTickLabel(0,"0");

    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltImage->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
}

void QFSPIMLightsheetEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFSPIMLightsheetEvaluationItem* item=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFSPIMLightsheetEvaluationItem* item_old=qobject_cast<QFSPIMLightsheetEvaluationItem*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
    }



    if (item) {
        updatingData=true;

        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        
        updatingData=false;
    }


    displayPreview();
    displayEvaluationResults();
}

void QFSPIMLightsheetEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayPreview();
    displayEvaluationResults();
}

void QFSPIMLightsheetEvaluationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("spim_lightsheet_eval/editor/lastSaveDirectory"), currentSaveDirectory).toString();
};

void QFSPIMLightsheetEvaluationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("spim_lightsheet_eval/editor/lastSaveDirectory"), currentSaveDirectory);
}

void QFSPIMLightsheetEvaluationEditor::updateStack() {
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    int stack=ui->cmbStack->currentIndex();

    if (data) {
        bool oldUpdt=updatingData;
        updatingData=true;


        ui->cmbChannel->clear();
        for (int i=0; i<data->getImageStackChannels(stack); i++) {
            ui->cmbChannel->addItem(data->getImageStackChannelName(stack, i));
        }
        ui->cmbChannel->setCurrentIndex(0);

        ui->spinStackPos->setRange(0, data->getImageStackFrames(stack));
        ui->horizontalScrollBar->setRange(0, data->getImageStackFrames(stack));
        ui->spinStackPos->setValue(0);

        ui->spinDeltaX->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_DELTAX", record->getProperty("PIXEL_WIDTH", ui->spinDeltaX->value()).toDouble()).toDouble());
        ui->spinDeltaZ->setValue(record->getProperty(eval->getEvaluationResultID(stack)+"_DELTAZ", record->getProperty("DELTAZ", ui->spinDeltaZ->value()).toDouble()).toDouble());
        ui->cmbOrientation->setCurrentIndex(record->getProperty(eval->getEvaluationResultID(stack)+"_ORIENTATION", record->getProperty("ORIENTATION", ui->cmbOrientation->currentIndex()).toInt()).toInt());
        ui->cmbModel->setCurrentIndex(record->getProperty(eval->getEvaluationResultID(stack)+"_MODEL", record->getProperty("MODEL", ui->cmbModel->currentIndex()).toInt()).toInt());

        updatingData=oldUpdt;
    }
    // ensure that data of new highlighted record is displayed
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_cmbOrientation_currentIndexChanged(int index)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_ORIENTATION", index, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_cmbModel_currentIndexChanged(int index)
{
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_MODEL", index, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_spinDeltaX_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_DELTAX", value, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_spinDeltaZ_valueChanged(double value) {
    if (updatingData) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if (data && eval) {
        record->setQFProperty(eval->getEvaluationResultID(stack)+"_DELTAZ", value, false, false);
    }
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::on_pltImage_plotMouseClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    lastMousePreviewX=x;
    lastMousePreviewY=y;

    bool updI=ui->pltImage->get_doDrawing();
    bool updF=ui->pltFit->get_doDrawing();
    ui->pltImage->set_doDrawing(false);
    ui->pltFit->set_doDrawing(false);

    double* img=data->getImageStack(stack, ui->spinStackPos->value(), ui->cmbChannel->currentIndex());
    int w=data->getImageStackWidth(stack);
    int h=data->getImageStackHeight(stack);
    JKQTPdatastore* ds=ui->pltFit->getDatastore();
    ui->pltFit->clearGraphs(true);
    ds->clear();


    JKQTPxyLineErrorGraph* plteLineFitData=new JKQTPxyLineErrorGraph(ui->pltFit->get_plotter());
    plteLineFitData->set_title("raw data");
    plteLineFitData->set_drawLine(true);
    plteLineFitData->set_symbol(JKQTPcross);
    plteLineFitData->set_symbolSize(5);
    plteLineFitData->set_yErrorStyle(JKQTPerrorPolygons);
    ui->pltFit->addGraph(plteLineFitData);


    JKQTPxyLineGraph* plteLineFit=new JKQTPxyLineGraph(ui->pltFit->get_plotter());
    plteLineFit->set_title("fit");
    plteLineFit->set_drawLine(true);
    plteLineFit->set_symbol(JKQTPnoSymbol);
    ui->pltFit->addGraph(plteLineFit);

    if(img) {
        if (ui->cmbOrientation->currentIndex()==0) {
            markDataX[0]=0;
            markDataX[1]=w;
            markDataY[0]=markDataY[1]=round(y);
            int c_x=ds->addLinearColumn(w, 0, double(w)*ui->spinDeltaX->value()/1000.0, "dataX");
            int c_y=ds->addCopiedColumn(&(img[(int)round(y)*w]), w, "dataY");
            plteLineFitData->set_xColumn(c_x);
            plteLineFitData->set_yColumn(c_y);
            plteLineFitData->set_xErrorColumn(-1);
            plteLineFitData->set_yErrorColumn(-1);

        } else {
            markDataY[0]=0;
            markDataY[1]=h;
            markDataX[0]=markDataX[1]=round(x);

            int c_x=ds->addLinearColumn(h, 0, double(h)*ui->spinDeltaX->value()/1000.0, "dataX");
            double* data=(double*)malloc(h*sizeof(double));
            for (int yy=0; yy<h; yy++) {
                data[yy]=img[yy*w+(int)round(x)];
            }
            int c_y=ds->addCopiedColumn(data, h, "dataY");
            plteLineFitData->set_xColumn(c_x);
            plteLineFitData->set_yColumn(c_y);
            plteLineFitData->set_xErrorColumn(-1);
            plteLineFitData->set_yErrorColumn(-1);
            free(data);
        }
    }



    ui->pltImage->zoomToFit();
    ui->pltFit->zoomToFit();
    if (updI) {
        ui->pltImage->set_doDrawing(true);
        ui->pltImage->update_plot();
    }
    if (updF) {
        ui->pltFit->set_doDrawing(true);
        ui->pltFit->update_plot();
    }
}

void QFSPIMLightsheetEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayPreview()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayPreview())); // redisplay data, if data changed
        lastMousePreviewX=0;
        lastMousePreviewY=0;

        updatingData=true;
        setUpdatesEnabled(false);
        // assign values to widgets here 
        ui->cmbStack->clear();
        for (int i=0; i<data->getImageStackCount(); i++) {
            ui->cmbStack->addItem(data->getImageStackDescription(i));
        }
        ui->cmbStack->setCurrentIndex(0);
        updateStack();


        setUpdatesEnabled(true);
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluationResults();
    displayPreview();
}

void QFSPIMLightsheetEvaluationEditor::displayEvaluationResults() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    /*if (eval->hasEvaluation(record)) {
        if (record->resultsExists(eval->getEvaluationResultID(), "evaluation_completed")) {
            ui->labResults->setText(tr("<b>Results:</b><br>evaluation_completed = %1").arg(record->resultsGetAsString(eval->getEvaluationResultID(), "evaluation_completed")));
        } else {
            ui->labResults->setText(tr("<b>NO RESULTS STORED</b>"));
        }
    } else {
        ui->labResults->setText(tr("<b>NO EVALUATION DONE YET</b>"));    
    }*/
}

void QFSPIMLightsheetEvaluationEditor::displayPreview() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    int stack=ui->cmbStack->currentIndex();

    if ((!record)||(!eval)||(!data)) return;

    bool updI=ui->pltImage->get_doDrawing();
    bool updF=ui->pltFit->get_doDrawing();
    ui->pltImage->set_doDrawing(false);
    ui->pltFit->set_doDrawing(false);



    double* img=data->getImageStack(stack, ui->spinStackPos->value(), ui->cmbChannel->currentIndex());
    int w=data->getImageStackWidth(stack);
    int h=data->getImageStackHeight(stack);

    plteImage->set_data(img, w,h);
    plteImage->set_width(w);
    plteImage->set_height(h);
    ui->pltImage->setAbsoluteXY(0,w,0,h);

    on_pltImage_plotMouseClicked(lastMousePreviewX, lastMousePreviewY, Qt::NoModifier, Qt::LeftButton);


    ui->pltImage->zoomToFit();
    ui->pltFit->zoomToFit();
    if (updI) {
        ui->pltImage->set_doDrawing(true);
        ui->pltImage->update_plot();
    }
    if (updF) {
        ui->pltFit->set_doDrawing(true);
        ui->pltFit->update_plot();
    }
}


void QFSPIMLightsheetEvaluationEditor::doEvaluation(QFRawDataRecord* record) {
    QApplication::processEvents();
    QApplication::processEvents();

    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record); //if (!data) return;
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);

    if (!eval) return;
    
    if (dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?

    /*
        DO YOUR EVALUATION HERE
    */
    /*

    services->log_text(tr("evaluation complete\n"));
    
    // write back fit results to record!
    record->disableEmitResultsChanged();
    record->resultsSetBoolean(eval->getEvaluationResultID(), "evaluation_completed", true);
    record->enableEmitResultsChanged();
    emit resultsChanged();
    */
}




void QFSPIMLightsheetEvaluationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    doEvaluation(record);

    displayEvaluationResults();
    displayPreview();
    dlgEvaluationProgress->setValue(100);

    QApplication::restoreOverrideCursor();
}


void QFSPIMLightsheetEvaluationEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            doEvaluation(record);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
    displayEvaluationResults();
    displayPreview();
    QApplication::restoreOverrideCursor();
}



void QFSPIMLightsheetEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFSPIMLightsheetEvaluationItem* eval=qobject_cast<QFSPIMLightsheetEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    // we use this QTextCursor to write the document
    QTextCursor cursor(document);
    
    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);

    
    // insert heading
    cursor.insertText(tr("Evaluation Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));
    cursor.movePosition(QTextCursor::End);

}

void QFSPIMLightsheetEvaluationEditor::saveReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                currentSaveDirectory,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        doc->print(printer);
        delete doc;
        delete printer;
        QApplication::restoreOverrideCursor();
    }
}

void QFSPIMLightsheetEvaluationEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}

