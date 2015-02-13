/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-02-05 19:29:55 +0100 (Do, 05 Feb 2015) $  (revision $Rev: 3794 $)

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


#include "qfevalcolocalization_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfevalcolocalization_item.h"
#include "ui_qfevalcolocalization_editor.h"
#include "qmoretextobject.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include <typeinfo>
#include "qftabledelegate.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>

QFEValColocalizationEditor::QFEValColocalizationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFEValColocalizationEditor)
{
    updatingData=true;
    

    // setup widgets
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,1);
    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(this);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    ui->btnSaveReport->setDefaultAction(actSaveReport);

    updatingData=false;
}

QFEValColocalizationEditor::~QFEValColocalizationEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFEValColocalizationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFEValColocalizationItem* item=qobject_cast<QFEValColocalizationItem*>(current);
    QFEValColocalizationItem* item_old=qobject_cast<QFEValColocalizationItem*>(old);

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


    displayData();
    displayEvaluation();
}

void QFEValColocalizationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayEvaluation();    
}

void QFEValColocalizationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
};

void QFEValColocalizationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
}

void QFEValColocalizationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    QString resultID=eval->getEvaluationResultID();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));
    if (formerRecord) {
        disconnect(ui->chkAllFrames, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        disconnect(ui->spinBackgroundThreshold, SIGNAL(valueChanged(double)), this, SLOT(resultsChanged()));
        disconnect(ui->spinFrame, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        disconnect(ui->spinChannel1, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        disconnect(ui->spinChannel2, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        disconnect(ui->spinDisplayFrame, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));

    }

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed
        int stack=0;

        updatingData=true;
        // assign values to widgets here 

        ui->spinChannel1->setRange(0, data->getImageStackChannels(stack));
        ui->spinChannel2->setRange(0, data->getImageStackChannels(stack));
        ui->spinDisplayFrame->setRange(0, data->getImageStackFrames(stack));
        ui->spinFrame->setRange(0, data->getImageStackFrames(stack));
        ui->chkAllFrames->setChecked(currentRecord->getQFProperty(resultID+"_allframes", true).toBool());
        ui->spinFrame->setValue(currentRecord->getQFProperty(resultID+"_evalframe", data->getImageStackFrames(stack)/2).toInt());
        ui->spinChannel1->setValue(currentRecord->getQFProperty(resultID+"_channel1", 0).toInt());
        ui->spinChannel2->setValue(currentRecord->getQFProperty(resultID+"_channel2", 1).toInt());
        ui->spinDisplayFrame->setValue(currentRecord->getQFProperty(resultID+"_displayframe", data->getImageStackFrames(stack)/2).toInt());
        ui->spinBackgroundThreshold->setValue(currentRecord->getQFProperty(resultID+"_backthreshold", 0).toDouble()); //statisticsQuantile(data->getImageStack(stack, ui->spinFrame->value(), ui->spinChannel1->value()), data->getImageStackWidth(stack)*data->getImageStackHeight(stack), 0.05)*1.5).toDouble());

        connect(ui->spinBackgroundThreshold, SIGNAL(valueChanged(double)), this, SLOT(resultsChanged()));
        connect(ui->spinFrame, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        connect(ui->spinChannel1, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        connect(ui->spinChannel2, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        connect(ui->spinDisplayFrame, SIGNAL(valueChanged(int)), this, SLOT(resultsChanged()));
        connect(ui->chkAllFrames, SIGNAL(toggled(bool)), this, SLOT(resultsChanged()));
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluation();
    displayData();
}

void QFEValColocalizationEditor::displayEvaluation() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if ((!record)||(!eval)||(!data)) return;

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

void QFEValColocalizationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageStackInterface* data=dynamic_cast<QFRDRImageStackInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    qDebug()<<"displayData "<<record<<data<<eval;
    if ((!record)||(!eval)||(!data)) return;

    ui->plotterCorrelation->clear();
    int stack=0;
    int frame=ui->spinFrame->value();
    int frames=1;
    if (ui->chkAllFrames->isChecked()) {
        frame=0;
        frames=data->getImageStackFrames(stack);
    }
    int ch1=ui->spinChannel1->value();
    int ch2=ui->spinChannel2->value();
    int dframe=ui->spinDisplayFrame->value();
    int width=data->getImageStackWidth(stack);
    int height=data->getImageStackHeight(stack);
    int pixel_count=frames*width*height;
    double thresh=ui->spinBackgroundThreshold->value();
    double* data_ch1=data->getImageStack(stack, frame, ch1);
    double* data_ch2=data->getImageStack(stack, frame, ch2);
    QVector<double> dch1, dch2;
    for (long long int  i=0; i<frames*width*height; i++) {
        if (data_ch1[i]>thresh && data_ch2[i]>thresh) {
            dch1<<data_ch1[i];
            dch2<<data_ch2[i];
        }
    }
    if (dch1.size()>0) {
        data_ch1=dch1.data();
        data_ch2=dch2.data();
    }

    ui->plotterCorrelation->addCopiedCorrelation(QString("channels %1--%2").arg(ch1).arg(ch2), data_ch1, data_ch2, pixel_count);
    ui->plotterCorrelation->updateCorrelation(true);

    ui->plotter->set_doDrawing(false);
    JKQTPdatastore* ds=ui->plotter->getDatastore();
    ui->plotter->clearGraphs();
    ds->clear();
    JKQTPColumnRGBMathImage* img=new JKQTPColumnRGBMathImage(ui->plotter->get_plotter());
    int rch1=ds->addCopiedColumn(data->getImageStack(stack, dframe, ch1), width*height, tr("channel %1").arg(ch1));
    int rch2=ds->addCopiedColumn(data->getImageStack(stack, dframe, ch2), width*height, tr("channel %1").arg(ch2));
    img->set_imageGColumn(rch1);
    img->set_imageRColumn(rch2);
    img->set_autoImageRange(true);
    img->set_width(width);
    img->set_height(height);
    img->set_Nx(width);
    img->set_Ny(height);
    ui->plotter->addGraph(img);
    ui->plotter->getXAxis()->set_axisLabel(tr("x [pixel]"));
    ui->plotter->getYAxis()->set_axisLabel(tr("y [pixel]"));
    ui->plotter->setAbsoluteXY(0,width,0,height);
    ui->plotter->get_plotter()->set_maintainAspectRatio(true);
    ui->plotter->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->plotter->get_plotter()->set_aspectRatio(double(width)/double(height));
    ui->plotter->get_plotter()->set_axisAspectRatio(double(width)/double(height));

    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();
}






void QFEValColocalizationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    //eval->doEvaluation(record, dlgEvaluationProgress);

    displayEvaluation();
    displayData();
    dlgEvaluationProgress->setValue(100);
	dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
	resultsChanged();
}


void QFEValColocalizationEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
	dlgEvaluationProgress->reset();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            //doEvaluation(record);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
	dlgEvaluationProgress->close();
    displayEvaluation();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFEValColocalizationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEValColocalizationItem* eval=qobject_cast<QFEValColocalizationItem*>(current);
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
    cursor.insertText(tr("Colocalization Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(5, 4, tableFormat);
    table->mergeCells(0,1,1,3);
    int row=0;
    int col=0;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(record->getName(), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(record->getID()), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("display frame:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinDisplayFrame->value()), fText);
    row++;

	row=1;
    col=2;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("evaluate all frames:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->chkAllFrames->isChecked()), fText);
    row++;
    if (ui->chkAllFrames->isChecked()) {
        table->cellAt(row, col+0).firstCursorPosition().insertText(tr("evaluate frame:"), fTextBold);
        table->cellAt(row, col+1).firstCursorPosition().insertText(QString::number(ui->spinFrame->value()), fText);
        row++;
    }
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("channel 1:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinChannel1->value()), fText);
    row++;
    table->cellAt(row, col+0).firstCursorPosition().insertText(tr("channel 2:"), fTextBold);
    table->cellAt(row, col+1).firstCursorPosition().insertText(boolToQString(ui->spinChannel2->value()), fText);
    row++;

    cursor.movePosition(QTextCursor::End);
	
	
	
    ui->plotterCorrelation->writeReport(cursor, document);
	

	int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

	
    QTextTable* tablePic = cursor.insertTable(2,1, tableFormat);
    {
	    // insert a plot from ui->plotter
        QTextCursor tabCursor=tablePic->cellAt(1, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        ui->plotter->get_plotter()->draw(*painter, QRect(0,0,ui->plotter->width(),ui->plotter->height()));
        delete painter;
        double scale=0.9*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Overview image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.9);
        QApplication::processEvents();


        QApplication::processEvents();
    }

}


