/*
Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfpevalcurvefit.h"
#include "optionswidget.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfcurvefitevaluation.h"

QFPEvalCurveFit::QFPEvalCurveFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFPEvalCurveFit::~QFPEvalCurveFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalCurveFit::createRecord(QFProject* parent) {
    return new QFCurveFitEvaluation(parent);
}


void QFPEvalCurveFit::registerToMenu(QMenu* menu_main) {
    QMenu* menu=menu_main->addMenu(QIcon(":/curve_fit.png"), tr("General Curve Fitting"));
    QAction* actCURVE=new QFActionWithNoMenuRole(QIcon(":/curve_fit.png"), tr("Curve Fitting"), parentWidget);
    actCURVE->setStatusTip(tr("Insert a least-squares fit evaluation"));
    connect(actCURVE, SIGNAL(triggered()), this, SLOT(insertCurveFit()));
    menu->addAction(actCURVE);
    menu->addSeparator();
    QAction* actHIST=new QFActionWithNoMenuRole(QIcon(":/curve_fit/hist.png"), tr("Histogram Fitting"), parentWidget);
    actHIST->setStatusTip(tr("Insert a least-squares fit evaluation with presets for fitting histograms"));
    connect(actHIST, SIGNAL(triggered()), this, SLOT(insertHistogramFit()));
    menu->addAction(actHIST);
    QAction* actMEXP=new QFActionWithNoMenuRole(QIcon(":/curve_fit/mexp.png"), tr("Multi-Exponential Decay Fitting"), parentWidget);
    actMEXP->setStatusTip(tr("Insert a least-squares fit evaluation with presets for fitting multi-exponential decays"));
    connect(actMEXP, SIGNAL(triggered()), this, SLOT(insertHistogramFit()));
    menu->addAction(actMEXP);
 }



QString QFPEvalCurveFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalCurveFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalCurveFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

void QFPEvalCurveFit::insertCurveFit() {
    if (project) {
        project->addEvaluation(getID(), "Curve Fit");
    }
}

void QFPEvalCurveFit::insertHistogramFit()
{
    if (project) {
        QFEvaluationItem* e=project->addEvaluation(getID(), "Histogram Fit");
        if (e) {
            e->setPresetProperty("PRESET_FIT_MODEL", "gen_multigaussian_sqrte", false, false);
            e->setPresetProperty("PRESET_components", 2, false, false);
            e->setPresetProperty("PRESET_amplitude_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude1_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude2_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude3_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude4_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude5_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude6_MIN", 0.0, false, false);
        }
    }
}

void QFPEvalCurveFit::insertMultiExpFit()
{
    if (project) {
        QFEvaluationItem* e=project->addEvaluation(getID(), "Multi-Exp Decay Fit");
        if (e) {
            e->setPresetProperty("PRESET_FIT_MODEL", "gen_multiexp", false, false);
            e->setPresetProperty("PRESET_components", 2, false, false);
            e->setPresetProperty("PRESET_amplitude_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude1_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude2_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude3_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude4_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude5_MIN", 0.0, false, false);
            e->setPresetProperty("PRESET_amplitude6_MIN", 0.0, false, false);
        }
    }
}

Q_EXPORT_PLUGIN2(curve_fit, QFPEvalCurveFit)
