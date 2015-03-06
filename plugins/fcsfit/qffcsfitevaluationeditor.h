/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFFCSFITEVALUATIONEDITOR_H
#define QFFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "tools.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QList>
#include <QPointer>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QTabBar>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <QTextDocument>
#include "datacutslider.h"
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"
#include "dlgqffitalgorithmprogressdialog.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qvisiblehandlesplitter.h"
#include "qffitalgorithmerrorestimatemodewidget.h"
#include "../base_classes/qffitresultsevaluationeditorbase.h"
#include "../base_classes/qffitresultsbyindexevaluationeditorbase.h"
#include "../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "qffcsweightingtools.h"

/*! \brief editor class for FCS least-square fits
    \ingroup qf3evalp_fcsfit

    This class also provides a tool to estimate the focal volume: It reads the parameters from the fit function
    and tries to deduce its meaning by their IDs:
      - the particle number \f$ N \f$ is extracted from \c n_particle or \c 1n_particle (as \f$ 1/N \f$ )
      - the diffusion time \f$ \tau_D \f$ is read from \c diff_tau or \c diff_tau1 (this class recognizes these units for \f$ \tau_D \f$: \c msec, \c ms, \c msecs, \c milliseconds, \c sec, \c s, \c secs, \c seconds
      - the focal size \f$ w_{xy} \f$ is written back into \c focus_struct_fac
    .
    Then the focal size \f$ w_{xy} \f$ is calculated as described in the documentation of dlgEstimateFocalVolume

    \section qf3evalp_fcsfit_fitstatistics Fit Statistics
    This class outputs a variety of fit statistics, that allow to evaluate the goodnes of fit (GoF).
    All of these parameters are calculated for the range of \f$ K \f$ datapoints \f$ (\tilde{\tau}_k, \tilde{g}_k, \tilde{\sigma}_k),\ \  k\in [k_0 .. k_1] \f$ selected by the user.
    First we define from these datapoints the residuals \f$ \tilde{E}_k=\tilde{g}_k-g(\tilde{\tau}_k, \vec{p}) \f$ and weighted residuals \f$ \tilde{E}_{\text{weighted},k}=\frac{\tilde{g}_k-g(\tilde{\tau}_k, \vec{p})}{\tilde{\sigma}_k^2} \f$.
    Then we can further note down these statistics (the weighted variants use \f$\tilde{E}_{\text{weighted},k}\f$ instead of \f$\tilde{E}_k\f$
      - sum of squared resdiuals (sometimes also called the residual sum of squares, RSS): \f[ \chi^2 = \sum\limits_{k=k_0}^{k_1}\tilde{E}_k^2 \f]
      - residual average: \f[ \langle E\rangle= \frac{1}{K}\cdot\sum\limits_{k=k_0}^{k_1}\tilde{E}_{k} \f]
      - residual standard deviation: \f[ \sqrt{\langle E^2\rangle}= \sqrt{ \frac{1}{K}\cdot\sum\limits_{k=k_0}^{k_1}\tilde{E}_{k}^2 -  \frac{1}{K^2}\cdot\left[\sum\limits_{k=k_0}^{k_1}\tilde{E}_{k}\right]^2 }=\sqrt{ \left\langle\left[\tilde{E}_{k}-\langle E\rangle\right]^2\right\rangle} \f]
      - total sum of squares (TSS): \f[ TSS = \langle g^2\rangle= \sum\limits_{k=k_0}^{k_1}\tilde{g}_{k}^2 -  \frac{1}{K}\cdot\left[\sum\limits_{k=k_0}^{k_1}\tilde{g}_{k}\right]^2 \f]
      - number of fit parameters <i>NP</i>: number of free parameters in the parameter vector \f$ \vec{p}\f$ of the model \f$ g(\tilde{\tau}_k, \vec{p}) \f$
      - number of datapoints <i>NR = K</i>
      - degrees of freedom \f$ \mbox{DF}= \mbox{NR}-\mbox{NP}-1\f$
      - coefficient of determination \f[ R^2=1-\frac{\mbox{RSS}}{\mbox{TSS}}=1-\frac{\chi^2}{\langle E^2\rangle} \f]
    .


    Finally also the <b>residual correlation function \f$\hat{\gamma}(k)\f$</b>, i.e. the autocorrelation function of the index-ordered residuals is calculated.
    This function gives hints on whether there are any non-random structures in the fit values, which might not be visible
    in the residuals plots:
      \f[ \hat{\gamma}(k)=\frac{\frac{1}{K}\cdot\sum\limits_{i=k_0}^{k_1-k-1}(\tilde{E}_{i}-\langle E\rangle)\cdot(\tilde{E}_{i+k}-\langle E\rangle)}{\bigl(\langle E^2\rangle\bigr)^2} \f]

    The residuals are displayed together with their running average, where the average over the 10 surrounding values is calculated:
      \f[ \overline{E}^{(10)}(\tilde{\tau}_k)=\frac{1}{11}\sum\limits_{i=k-5}^{k+5}\tilde{E}_i \f]

*/
class QFFCSFitEvaluationEditor : public QFFitResultsByIndexEvaluationEditorWithWidgets {
        Q_OBJECT
    public:

        /** Default constructor */
        QFFCSFitEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSFitEvaluationEditor();

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(,int) with a viable choice for \a defaultMin */
        virtual int getUserMin();
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(int) with a viable choice for \a defaultMax */
        virtual int getUserMax();

        virtual int getUserRangeMax(QFRawDataRecord* rec, int index);
        virtual int getUserRangeMin(QFRawDataRecord* rec, int index);

        virtual void getPlotData(QFRawDataRecord* rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        virtual bool getPlotDataSpecs(QStringList* optionNames=NULL, QList<QFGetPlotdataInterface::GetPlotPlotOptions>* listPlotOptions=NULL);

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


    protected:
        /** \brief combobox to select a model for the weighting */
        QFFCSWeightingCombobox* cmbWeights;
        /** \brief button used to calculate the focal volume */
        QAction* actCalibrateFocalVolume;
        /** \brief button used to calculate the focal volume */
        QToolButton* btnCalibrateFocalVolume;
        QSpinBox* spinRepeats;
        QFFitAlgorithmErrorEstimateModeWidget* widFitErrorEstimate;


        /** \brief create a report in a given QTextDocument object
         *
         *  For correct sizing of the plots: set the textWidth of the document before calling this function!
         */
        virtual void createReportDoc(QTextDocument* document);

        void calibrateFocalVolume_readParam(int run, double &particles, double &tauD, double &particles_error, double &tauD_error, double &wxy, double &wxy_error, double &gamma, double &gamma_error, bool &has_tauD, bool &has_nparticles, bool &has_gamma);
    protected slots:
        /** \brief calibrate the focal volume */
        void calibrateFocalVolume();
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);


        /** \brief replot curves */
        virtual void replotData();
        /** \brief display all data and parameters describing the current record */
        virtual void displayModel(bool newWidget);


        /*! \brief update the fit model function and the residuals in the graphs

            This function only adds the fit function plot objects and does NOT cause a replot of the graphs. It is called by
            replotData().
        */
        virtual void updateFitFunctions();

        /** \brief called when the weights changes */
        void weightsChanged(int weight);
        void repeatsChanged(int r);
        void errorEstimateModeChanged();

    private:
        /** \brief create all widgets on the form */
        void createWidgets();



    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsByIndexEvaluationEditorBase::getUserMin;
        using QFFitResultsByIndexEvaluationEditorBase::getUserMax;



};

#endif // QFFCSFITEVALUATIONEDITOR_H
