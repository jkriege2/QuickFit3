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


#include "qfeval_item.h"
#include "qfeval_editor.h"
//#include "qmoretextobject.h"


QFEVALItem::QFEVALItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFEVALItem::~QFEVALItem() {

}


void QFEVALItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFEVALItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFEVALItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFEVALEditor(services, propEditor, parent);
};

bool QFEVALItem::isApplicable(QFRawDataRecord* record) const {
    /* CHECK WHETHER A GIVEN RECORD MAY BE USED TOGETHER WITH THE EVALUATION */
    return true;
}

bool QFEVALItem::hasEvaluation(QFRawDataRecord* r1) {
    // checks, whether any results exist for the given eval ID
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFEVALItem::getEvaluationResultID() {
    // in the simplest case, the evaluation ID only contains the type-name and the record ID, this way, different
	// evaluation objects may write results into the same RDR and keep them separate. You may want to extend this ID,
	// e.g. by the used fit function ...
	return QString("%1_%2").arg(getType()).arg(getID());
}


void QFEVALItem::doEvaluation(QFRawDataRecord* record,QProgressDialog* dlgEvaluationProgress) {
    QApplication::processEvents();
    if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?
    
    /*
        DO YOUR EVALUATION HERE
    */

    QFPluginServices::getInstance()->log_text(tr("evaluation complete\n"));
    
    // write back fit results to record!
    record->disableEmitResultsChanged();
    record->resultsSetBoolean(getEvaluationResultID(), "evaluation_completed", true);
    record->enableEmitResultsChanged();
    
}
