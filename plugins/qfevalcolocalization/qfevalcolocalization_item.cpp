/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-02-01 22:46:57 +0100 (So, 01 Feb 2015) $  (revision $Rev: 3774 $)

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


#include "qfevalcolocalization_item.h"
#include "qfevalcolocalization_editor.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include <typeinfo>

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>


QFEValColocalizationItem::QFEValColocalizationItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFEValColocalizationItem::~QFEValColocalizationItem() {

}


void QFEValColocalizationItem::intWriteData(QXmlStreamWriter& /*w*/) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFEValColocalizationItem::intReadData(QDomElement* /*e*/) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFEValColocalizationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFEValColocalizationEditor(services, propEditor, parent);
};

bool QFEValColocalizationItem::isApplicable(const QFRawDataRecord* record) const {
    //qDebug()<<"QFEValColocalizationItem::isApplicable("<<record<<") = "<<record->inherits("QFRDRImageStackInterface");
    return record->inherits("QFRDRImageStackInterface"); //dynamic_cast<const QFRDRImageStackInterface*>(record)!=NULL);
}

bool QFEValColocalizationItem::hasEvaluation(QFRawDataRecord* r1, int stack, int ch1, int ch2) {
    // checks, whether any results exist for the given eval ID
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID(stack, ch1, ch2);
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFEValColocalizationItem::getEvaluationResultID(int stack, int ch1, int ch2) {
    // in the simplest case, the evaluation ID only contains the type-name and the record ID, this way, different
	// evaluation objects may write results into the same RDR and keep them separate. You may want to extend this ID,
	// e.g. by the used fit function ...
    if (stack<0 && ch1<0 && ch2<0) return QString("%1_%2").arg(getType()).arg(getID());
    else return QString("%1_%2_stack%3_ch%4_ch%5").arg(getType()).arg(getID()).arg(stack).arg(ch1).arg(ch2);
}

void QFEValColocalizationItem::doEvaluation(QFRawDataRecord *record, int /*frame*/, int /*channel1*/, int /*channel2*/, QProgressDialog *dlgEvaluationProgress) {
    QApplication::processEvents();
    if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?
    
    /*
        DO YOUR EVALUATION HERE
    */

    QFPluginServices::getInstance()->log_text(tr("evaluation complete\n"));
    
    // write back fit results to record!
    record->disableEmitResultsChanged();
    //record->resultsSetBoolean(getEvaluationResultID(-1,-1,-1), "evaluation_completed", true);
    record->enableEmitResultsChanged();
    
}
