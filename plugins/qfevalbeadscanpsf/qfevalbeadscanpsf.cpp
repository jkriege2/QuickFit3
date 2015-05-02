/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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


#include "qfevalbeadscanpsf.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "qfevalbeadscanpsf_item.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "fitfuncpsfgaussian.h"
#include "fitfuncpsfgaussianbeamwidth.h"
#include "fitfuncpsfgaussianbeamzintensity.h"
QFEvalBeadScanPSF::QFEvalBeadScanPSF(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFEvalBeadScanPSF::~QFEvalBeadScanPSF()
{
    //dtor
}

QFEvaluationItem* QFEvalBeadScanPSF::createRecord(QFProject* parent) {
    return new QFEvalBeadScanPSFItem(parent);
}


void QFEvalBeadScanPSF::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFEvalBeadScanPSF::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}



void QFEvalBeadScanPSF::init()
{
    QFPluginServices::getInstance()->getFitFunctionManager()->registerFitFactory(this);
}

QStringList QFEvalBeadScanPSF::fitFunctionFactoryGetIDs() const
{
    QStringList s;
    s<<"beadscan_psf_gaussian";
    s<<"beadscan_psf_gaussianbeam_width";
    s<<"beadscan_psf_gaussianbeam_zintensity";
    return s;
}

QFFitFunction *QFEvalBeadScanPSF::fitFunctionFactoryGet(const QString &id) const
{
    if (id=="beadscan_psf_gaussian") {
        return new QFFitFunctionPSFGaussian();
    } else if (id=="beadscan_psf_gaussianbeam_width") {
        return new QFFitFunctionPSFGaussianBeamWidth();
    } else if (id=="beadscan_psf_gaussianbeam_zintensity") {
        return new QFFitFunctionPSFGaussianBeamZIntensity();
    }
    return NULL;
}

QString QFEvalBeadScanPSF::fitFunctionFactoryGetHelpFile(const QString &id) const
{
    return QFPluginServices::getInstance()->getPluginHelpDirectory(getID())+QString("/%1.html").arg(id);
}

Q_EXPORT_PLUGIN2(eval_beadscanpsf, QFEvalBeadScanPSF)
