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


#include "qfeval_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfeval_item.h"
#include "ui_qfeval_editor.h"

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtCore>

QFEVALEditor::QFEVALEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFEVALEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";
    
    // setup widgets
    ui->setupUi(this);
    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(NULL);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    ui->btnSaveReport->setDefaultAction(actSaveReport);

    updatingData=false;
}

QFEVALEditor::~QFEVALEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFEVALEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFEVALItem* item=qobject_cast<QFEVALItem*>(current);
    QFEVALItem* item_old=qobject_cast<QFEVALItem*>(old);

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

void QFEVALEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayEvaluation();    
}

void QFEVALEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("target_id/editor/lastSaveDirectory"), currentSaveDirectory).toString();
};

void QFEVALEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("target_id/editor/lastSaveDirectory"), currentSaveDirectory);
}

void QFEVALEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRawDataRecord* data=currentRecord; // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed

        updatingData=true;
        // assign values to widgets here 
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluation();
    displayData();
}

void QFEVALEditor::displayEvaluation() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (eval->hasEvaluation(record)) {
        if (record->resultsExists(eval->getEvaluationResultID(), "evaluation_completed")) {
            ui->labResults->setText(tr("<b>Results:</b><br>evaluation_completed = %1").arg(record->resultsGetAsString(eval->getEvaluationResultID(), "evaluation_completed")));
        } else {
            ui->labResults->setText(tr("<b>NO RESULTS STORED</b>"));
        }
    } else {
        ui->labResults->setText(tr("<b>NO EVALUATION DONE YET</b>"));    
    }
}

void QFEVALEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;


    if ((!eval)/*||(!data)*/) {
        ui->labRecord->setText(tr("no record selected!"));
        return;
    }

    ui->labRecord->setText(tr("<b>selected record:</b> %1<br>&nbsp;&nbsp;<i>files: </i><br>&nbsp;&nbsp;&nbsp;&nbsp;%2").arg(record->getName()).arg(record->getFiles().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
}






void QFEVALEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doEvaluation(record, dlgEvaluationProgress);

    displayEvaluation();
    displayData();
    dlgEvaluationProgress->setValue(100);
	dlgEvaluationProgress->close();

    QApplication::restoreOverrideCursor();
	resultsChanged();
}


void QFEVALEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
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
        QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
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
	dlgEvaluationProgress->close();
    displayEvaluation();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFEVALEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFEVALItem* eval=qobject_cast<QFEVALItem*>(current);
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
	
	
	
	
	
	/*
	int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

	
	QTextTable* table = cursor.insertTable(2,1, tableFormat);
    {
	    // insert a plot from ui->plotter
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        ui->plotter->get_plotter()->draw(*painter, QRect(0,0,ui->plotter->width(),ui->plotter->height()));
        delete painter;
        double scale=0.9*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("variance vs. average intensity plot:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
        QApplication::processEvents();

		// insert an enhanced table plot from ui->tabResults
        tabCursor=table->cellAt(1,0).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        QPicture picT;
        painter=new JKQTPEnhancedPainter(&picT);
        ui->tabResults->paint(*painter);
        delete painter;
        scale=0.95*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("fit results table:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale);
        QApplication::processEvents();
    }*/

}


