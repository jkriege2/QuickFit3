#include "qffcsmaxentevaluation_item.h"
#include "qffcsmaxentevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfmathtools.h"
#include "libb040mem.h"


#define sqr(x) ((x)*(x))

QFFCSMaxEntEvaluationItem::QFFCSMaxEntEvaluationItem(QFProject* parent):
    QFUsesResultsByIndexAndModelEvaluation(parent, true, false)
{
    currentIndex=-1;
    currentModel=0;
}

QFFCSMaxEntEvaluationItem::~QFFCSMaxEntEvaluationItem() {

}

QString QFFCSMaxEntEvaluationItem::getEvaluationResultID(int currentIndex, int model) const {
    if (currentIndex<0) return QString("%1_%2_m%3_runavg").arg(getType()).arg(getID()).arg(model);
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(model).arg(currentIndex);
}



void QFFCSMaxEntEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    QFUsesResultsByIndexAndModelEvaluation::intWriteData(w);
    w.writeStartElement("maxent_config");
    w.writeAttribute("current_weights", QString::number(m_weighting));
    w.writeEndElement();

}

void QFFCSMaxEntEvaluationItem::intReadData(QDomElement* e) {
    QFUsesResultsByIndexAndModelEvaluation::intReadData(e);

    QDomElement e1=e->firstChildElement("maxent_config");
    m_weighting=indexToWeight(e1.attribute("current_weights", "0").toInt());


}



QFEvaluationEditor* QFFCSMaxEntEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFFCSMaxEntEvaluationEditor(services, propEditor, parent);
}

bool QFFCSMaxEntEvaluationItem::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}


int QFFCSMaxEntEvaluationItem::getIndexMin(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return -1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}

int QFFCSMaxEntEvaluationItem::getIndexMax(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return data->getCorrelationRuns()-1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}







QString QFFCSMaxEntEvaluationItem::getCurrentModelName() const
{
    return getModelName(getCurrentModel());
}





void QFFCSMaxEntEvaluationItem::setAlpha(double alpha) {
    setFitValue("maxent_alpha", alpha);
}

double QFFCSMaxEntEvaluationItem::getAlpha() const {
    return getFitValue("maxent_alpha");
}




///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void QFFCSMaxEntEvaluationItem::setNdist(uint32_t Ndist) {
    setFitValue("maxent_Ndist", Ndist);
}

uint32_t QFFCSMaxEntEvaluationItem::getNdist() const {
    return round(getFitValue("maxent_Ndist"));
}

void QFFCSMaxEntEvaluationItem::setNumIter(uint32_t NumIter) {
    setFitValue("maxent_numiter", NumIter);
}

uint32_t QFFCSMaxEntEvaluationItem::getNumIter() const {
    return round(getFitValue("maxent_numiter"));
}

void QFFCSMaxEntEvaluationItem::setWXY(double wxy) {
    setFitValue("maxent_wxy", wxy);
}

double QFFCSMaxEntEvaluationItem::getWXY() const {
    return getFitValue("maxent_wxy");
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////



QVector<double> QFFCSMaxEntEvaluationItem::getDistribution(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "maxent_distribution");
    }
    return res;
}

QVector<double> QFFCSMaxEntEvaluationItem::getDistributionTaus(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "maxent_tau");
    }
    return res;
}

QVector<double> QFFCSMaxEntEvaluationItem::getDistributionDs(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "maxent_D");
    }
    return res;
}




QFFitStatistics QFFCSMaxEntEvaluationItem::calcFitStatistics(QFRawDataRecord *record, int index, int model, double *taus, double* modelValsIn, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {

        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);

        double* modelVals=modelValsIn;
        bool freeModelVals=false;
        if (!modelVals) {
            modelVals=(double*)malloc(N*sizeof(double));
            QVector<double> dist=getDistribution(record, index, model);
            QVector<double> distTau=getDistributionTaus(record, index, model);
            evaluateModel(record, index, model, taus, modelVals, N, distTau.data(), dist.data(), dist.size());
            freeModelVals=true;
        }
        //double* modelVals=(double*)malloc(N*sizeof(double));
        //
        //(record, index, model, taus, modelVals, N);
        double* weights=allocWeights(NULL, record, index);

        QFFitStatistics result= calculateFitStatistics(N, taus, modelVals, corrdata, weights, datacut_min, datacut_max, MaxEntParams, runAvgWidth, residualHistogramBins);

        if (record) {
            if (hasResults(record, index)) {
                QString param="";
                setFitResultValue(record, index, model, param="fitstat_chisquared", result.residSqrSum);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                setFitResultValue(record, index,model,  param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_residavg", result.residAverage);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("residual average"), QString("&lang;E&rang;"));

                setFitResultValue(record, index, model, param="fitstat_residavg_weighted", result.residWeightAverage);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_residstddev", result.residStdDev);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                setFitResultValue(record, index, model, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_fitparams", result.fitparamN);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("fit params"));

                setFitResultValue(record, index, model, param="fitstat_datapoints", result.dataSize);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("datapoints"));

                setFitResultValue(record, index, model, param="fitstat_dof", result.degFreedom);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("degrees of freedom"));

                setFitResultValue(record, index, model, param="fitstat_r2", result.Rsquared);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("R squared"), tr("R<sup>2</sup>"));

                setFitResultValue(record, index, model, param="fitstat_tss", result.TSS);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("total sum of squares"));

            }
        }

        if (freeModelVals) free(modelVals);

        return result;
    }
    return QFFitStatistics();

}

















////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENT THE FIT MODEL, THE FIT AND ALL THE PARAMETERS REQUIRED FOR THOSE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QFFCSMaxEntEvaluationItem::evaluateModel(QFRawDataRecord *record, int index, int model, double *taus, double *modelEval, uint32_t N, double* distTaus, double* dist, uint32_t Ndist) const {
    if (!taus || !modelEval) return;
    //qDebug()<<"evalModel(N="<<N<<"  distributionN="<<distributionN<<")";

    QVector<double> param_vector(getParameterCount(model));
    switch(model)
        {
            case 0:
                    param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                    param_vector[1]=getFitValue(record,index,model,"trip_theta");
                    param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                    break;
            case 1: param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                    param_vector[1]=getFitValue(record,index,model,"trip_theta");
                    param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                    param_vector[3]=getFitValue(record,index,model,"dark_tau")*1e-6;
                    param_vector[4]=getFitValue(record,index,model,"dark_theta");
                    param_vector[5]=getFitValue(record,index,model,"offset");
                    break;
            case 2: param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                    param_vector[1]=getFitValue(record,index,model,"trip_theta");
                    param_vector[2]=getFitValue(record,index,model,"dark_tau")*1e-6;
                    param_vector[3]=getFitValue(record,index,model,"dark_theta");
                    param_vector[4]=getFitValue(record,index,model,"offset");
                    break;
            case 3: param_vector[0]=getFitValue(record,index,model,"A");
                    break;
            case 4: param_vector[0]=getFitValue(record,index,model,"tau_1")*1e-6;
                    param_vector[1]=getFitValue(record,index,model,"tau_2")*1e-6;
                    param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                    param_vector[3]=getFitValue(record,index,model,"fraction");
                    param_vector[4]=getFitValue(record,index,model,"particle_number");
                    param_vector[5]=getFitValue(record,index,model,"offset");
                    break;

            case 5: //param_vector[0]=getFitValue(record,index,model,"maxent_wxy")*1e-3;
                    param_vector[0]=getFitValue(record,index,model,"focus_height")*1e-3;
                    param_vector[1]=getFitValue(record,index,model,"pixel_size")*1e-3;
                    param_vector[2]=getFitValue(record,index,model,"offset");
                    break;
            case 6: param_vector[0]=getFitValue(record,index,model,"pixel_size")*1e-3;
                    param_vector[1]=getFitValue(record,index,model,"offset");
                    break;

         }
    double* params=param_vector.data();

    MaxEntB040 mem_eval;
    mem_eval.evaluateModel(taus,modelEval,N,distTaus,dist,Ndist,params,model,getFitValue(record,index,model,"maxent_wxy")*1e-3);


}





QString QFFCSMaxEntEvaluationItem::getModelName(int model) const {
    switch(model)
    {
        case 0: return tr("FCS: 3D diffusion with Triplet");
        case 1: return tr("FCS: 3D diffusion with 2 blinking components");
        case 2: return tr("FCS: 2D diffusion with 2 blinking components");
        case 3: return tr("Dynamic Light Scattering");
        case 4: return tr("FCS Blinking with 3D diffusion");
        case 5: return tr("SPIM-FCS 3D");
        case 6: return tr("camera TIR-FCS 3D");
    }
    /*
    if (model==0) return tr("FCS: 3D diffusion with Triplet");
    */

    return "";
}

bool QFFCSMaxEntEvaluationItem::getParameterDefault(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {
    switch (currentModel)
    {
    case 0:
            if (parameterID=="trip_tau") {
                defaultValue.value=3;
                return true;
                }
            if (parameterID=="trip_theta") {
                defaultValue.value=0.2;
                return true;
                }
            if (parameterID=="focus_struct_fac") {
                defaultValue.value=6;
                return true;
            }
        break;
            /* if (parameterID=="n_particle") {
            defaultValue.value=8;
            return true;
        }*/
   case 1:
            if (parameterID=="trip_tau") {
                defaultValue.value=3.0;
                return true;
                }
            if (parameterID=="trip_theta") {
                defaultValue.value=0.2;
                return true;
                }
            if (parameterID=="focus_struct_fac") {
                defaultValue.value=6.0;
                return true;
                    }
            if (parameterID=="dark_tau") {
                defaultValue.value=100.0;
                return true;
                    }
            if (parameterID=="dark_theta") {
                defaultValue.value=0.0;
                return true;
                    }
            if (parameterID=="offset") {
                defaultValue.value=0.0;
                return true;
                    }
            break;
    case 2:
             if (parameterID=="trip_tau") {
                 defaultValue.value=3.0;
                 return true;
                 }
             if (parameterID=="trip_theta") {
                 defaultValue.value=0.2;
                 return true;
                 }
             if (parameterID=="dark_tau") {
                 defaultValue.value=100.0;
                 return true;
                     }
             if (parameterID=="dark_theta") {
                 defaultValue.value=0.0;
                 return true;
                     }
             if (parameterID=="offset") {
                 defaultValue.value=0.0;
                 return true;
                     }
            break;
    case 3:
             if (parameterID=="A") {
                 defaultValue.value=1.0;
                 return true;
                 }

            break;
    case 4:
             if (parameterID=="tau_1") {
                 defaultValue.value=3.0;
                 return true;
                 }
             if (parameterID=="tau_2") {
                 defaultValue.value=3.0;
                 return true;
                 }
             if (parameterID=="focus_struct_fac") {
                 defaultValue.value=6.0;
                 return true;
                     }
             if (parameterID=="fraction") {
                 defaultValue.value=1.0;
                 return true;
                     }
             if (parameterID=="particle_number") {
                 defaultValue.value=1;
                 return true;
                     }
             if (parameterID=="offset") {
                 defaultValue.value=0.0;
                 return true;
                     }


            break;
        case 5:
                 if (parameterID=="focus_height") {
                     defaultValue.value=1200;
                     return true;
                     }
                 if (parameterID=="pixel_size") {
                     defaultValue.value=400;
                     return true;
                         }
                 if (parameterID=="offset") {
                     defaultValue.value=0.0;
                     return true;
                         }
            break;
        case 6:
                 if (parameterID=="pixel_size") {
                     defaultValue.value=400;
                     return true;
                         }
                 if (parameterID=="offset") {
                     defaultValue.value=0.0;
                     return true;
                         }

            break;

    }

    if (parameterID=="maxent_alpha") {
        defaultValue.value=0.001;
        return true;
    } else if (parameterID=="maxent_Ndist") {
        defaultValue.value=300;
        return true;
    } else if (parameterID=="maxent_wxy") {
        defaultValue.value=250;
        return true;
    } else if (parameterID=="maxent_numiter") {
        defaultValue.value=20;
        return true;
    }

    return false;
}


void QFFCSMaxEntEvaluationItem::doFit(QFRawDataRecord* record, int index, int model, int defaultMinDatarange, int defaultMaxDatarange, int runAvgWidth, int residualHistogramBins) {
    bool doEmit=record->isEmitResultsChangedEnabled();
    bool thisDoEmitResults=get_doEmitResultsChanged();
    bool thisDoEmitProps=get_doEmitResultsChanged();
    set_doEmitResultsChanged(false);
    set_doEmitPropertiesChanged(false);
    record->disableEmitResultsChanged();

    //qDebug() << "START DEBUGGING 0: We enter the do fit Method with MODEL = " << model;



    /* IMPLEMENT THIS

      Ergebnisse können einfach mit einer der setFitResult... Methoden gespeichert werden:

        //                                          PARAMETERNAME           WERT
        setFitResultValueBool(record, index, model, "evaluation_completed", true);
        //                                      PARAMETERNAME   WERT   FEHLER und EINHEIT (z.B. "ms") dazu
        setFitResultValue(record, index, model, "my_parameter", value, error, unit);

    */

    // HERE IS A DUMMY IMPLEMENTATION THAT OUTPUTS A SIMPLE GAUSSIAN
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        getProject()->getServices()->log_text(tr("running MaxEnt fit with model '%1' on raw data record '%2', run %3 ... \n").arg(getModelName(model)).arg(record->getName()).arg(index));
        // which datapoints should we actually use?
        int rangeMinDatarange=0;
        int rangeMaxDatarange=data->getCorrelationN();
        if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
        if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;
        getProject()->getServices()->log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(defaultMinDatarange).arg(defaultMaxDatarange).arg(defaultMaxDatarange-defaultMinDatarange));




        uint32_t N=data->getCorrelationN();
        double* taus=data->getCorrelationT();
        double* distTaus=NULL;//(double*)calloc(Ndist,sizeof(double));
        double* distDs=NULL;//(double*)calloc(Ndist,sizeof(double));
        double* dist=NULL;//(double*)calloc(Ndist,sizeof(double));
        double* modelEval=(double*)malloc(N*sizeof(double));
        bool weightsOK=false;
        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);
        double* weights=allocWeights(&weightsOK, record, index, rangeMinDatarange, rangeMaxDatarange);
        if (!weightsOK) getProject()->getServices()->log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        //////////Load Algorithm Parameters ////////////////////////////////////////////////
        double alpha=getAlpha();
        int NumIter=getNumIter();
        uint32_t Ndist=getNdist();
        ////////////////////////////////////////////////////////////////////////////////////
        bool fitSuccess=false;


        //qDebug() << "DEBUG #1: alpha = " << alpha << " NumIter = " << NumIter << "Ndist =" << Ndist ;

        //////////Load Model Parameters//////////////////////////////////////////////////////
        //int parameter_count=getParameterCount(model);
        QVector<double> param_vector(getParameterCount(model));
        switch(model)
            {
                // FCS 3D diffusion with triplet
                case 0: param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                        param_vector[1]=getFitValue(record,index,model,"trip_theta");
                        param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                        break;
                // FCS 3D diffusion with 2 blinking components
                case 1:
                        param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                        param_vector[1]=getFitValue(record,index,model,"trip_theta");
                        param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                        param_vector[3]=getFitValue(record,index,model,"dark_tau")*1e-6;
                        param_vector[4]=getFitValue(record,index,model,"dark_theta");
                        param_vector[5]=getFitValue(record,index,model,"offset");
                        break;
                // FCS 2D diffusion with 2 blinking components
                case 2:
                        param_vector[0]=getFitValue(record,index,model,"trip_tau")*1e-6;
                        param_vector[1]=getFitValue(record,index,model,"trip_theta");
                        param_vector[2]=getFitValue(record,index,model,"dark_tau")*1e-6;
                        param_vector[3]=getFitValue(record,index,model,"dark_theta");
                        param_vector[4]=getFitValue(record,index,model,"offset");
                        break;
                case 3: param_vector[0]=getFitValue(record,index,model,"A");
                        break;
                case 4: param_vector[0]=getFitValue(record,index,model,"tau_1")*1e-6;
                        param_vector[1]=getFitValue(record,index,model,"tau_2")*1e-6;
                        param_vector[2]=getFitValue(record,index,model,"focus_struct_fac");
                        param_vector[3]=getFitValue(record,index,model,"fraction");
                        param_vector[4]=getFitValue(record,index,model,"particle_number");
                        param_vector[5]=getFitValue(record,index,model,"offset");
                        break;
                case 5: //param_vector[0]=getFitValue(record,index,model,"maxent_wxy")*1e-3;
                        param_vector[0]=getFitValue(record,index,model,"focus_height")*1e-3;
                        param_vector[1]=getFitValue(record,index,model,"pixel_size")*1e-3;
                        param_vector[2]=getFitValue(record,index,model,"offset");
                        break;
                case 6: param_vector[0]=getFitValue(record,index,model,"pixel_size")*1e-3;
                        param_vector[1]=getFitValue(record,index,model,"offset");
                        break;



             }

        double *param_list=param_vector.data();
        //////////////////////////////////////////////////////////////////////////////////////////////////

        /*
        double kappa=getFitValue(record,index,model,"focus_struct_fac");
        double tripTau=getFitValue(record,index,model,"trip_tau")*1e-6;
        double tripTheta=getFitValue(record,index,model,"trip_theta");
        */

        QVector<double> init_tau=getFitValueNumberArray(record, index, model, "maxent_tau");
        QVector<double> init_dist=getFitValueNumberArray(record, index, model, "maxent_distribution");

        //qDebug()<<init_tau;

        bool old_distribution=false; // default value
        if (init_tau.size()>0 && init_dist.size()>0)
            {
                Ndist=qMin(init_tau.size(), init_dist.size());
                distTaus=(double*)calloc(Ndist,sizeof(double));
                dist=(double*)calloc(Ndist,sizeof(double));
                for (uint32_t i=0; i<Ndist; i++)
                    {
                        distTaus[i]=init_tau[i];
                        dist[i]=init_dist[i];
                    }
                old_distribution=true;
                //qDebug()<< "OLDDIST TRUE";
            }
        else
            {
                distTaus=NULL;
                dist=NULL;
                old_distribution=false;
                //qDebug()<< "OLDDIST FALSE";
                if (int64_t(Ndist)>(rangeMaxDatarange-rangeMinDatarange))
                    {
                        Ndist=(rangeMaxDatarange-rangeMinDatarange);
                    }

            }



        QElapsedTimer time;
        time.start();

        /////////////////////////////////////////////////////////
        /// MaxEnt Implementation ///////////////////////////////
        /////////////////////////////////////////////////////////
        MaxEntB040 mem;
        mem.setData(taus,corrdata,weights,N,rangeMinDatarange,rangeMaxDatarange,Ndist,dist,distTaus, model,getParameterCount(model),param_list, getFitValue(record,index,model,"maxent_wxy")*1e-3);
        mem.run(alpha,NumIter,param_list,model,getParameterCount(model));
        fitSuccess=true;
        if (old_distribution==false)
            {
                distTaus=(double*)calloc(Ndist,sizeof(double));
                dist=(double*)calloc(Ndist,sizeof(double));
            }
        mem.writeDistTaus(distTaus);
        mem.writeDistribution(dist);
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////


        // reset all NAN to 0
        for (unsigned int i=0; i<Ndist; i++) {
            double d=dist[i];
            if (!QFFloatIsOK(dist[i])) dist[i]=0;
            //qDebug()<<distTaus[i]<<" ,\t"<<d<<" ,\t"<<dist[i];
        }

        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
/*
        // REPLACE THIS WITH THE ACTUAL MAXENT FIT!!!
        double T0=1e-4;
        double sigma=2e-5;
        double dSum=0;
        for (int i=rangeMinDatarange; i<rangeMaxDatarange; i++) {
            const double t=taus[i];
            dist[i]=exp(-0.5*sqr(t-T0)/sqr(sigma));
            dSum=dSum+dist[i];
        }
        for (int i=rangeMinDatarange; i<rangeMaxDatarange; i++) {
            dist[i]=dist[i]/dSum;
        }
        fitSuccess=true;
        // FIT CODE COMPLETE
*/

        // duration measurement
        double duration=double(time.elapsed())/1000.0;

        getProject()->getServices()->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(duration).arg(fitSuccess?tr("success"):tr("no convergence")));

        distDs=(double*)calloc(Ndist, sizeof(double));
        double wxy=getWXY();
        for (uint32_t i=0; i<Ndist; i++) {
            distDs[i]=wxy*wxy/1000000.0/(4.0*distTaus[i]);
        }

        // now store the results:
        QString param;
        setFitResultValueNumberArray(record, index, model, param="maxent_tau", distTaus, Ndist, QString("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt distribution: lag times"), QString("MaxEnt distribution: lag times <i>&tau;</i>"));
        setFitResultSortPriority(record, index, model, param, true);

        setFitResultValueNumberArray(record, index, model, param="maxent_D", distDs, Ndist, QString("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt distribution: diffusion coefficients"), QString("MaxEnt distribution: diffusion coefficients <i>D</i>"));
        setFitResultSortPriority(record, index, model, param, true);

        setFitResultValueNumberArray(record, index, model, param="maxent_distribution", dist, Ndist);
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt distribution"), QString("MaxEnt distribution: <i>p(&tau;)</i>"));
        setFitResultSortPriority(record, index, model, param, true);



        // save all the default values for all fit parameters as results.
        for (int i=0; i<getParameterCount(model); i++) {
            param=getParameterID(model, i);
            double value=getFitValue(record, index, model, param);
            //qDebug()<<"model param "<<i<<": "<<param<<" = "<<value;
            setFitResultValue(record, index, model, param, value, getParameterUnit(model, i, false));
            setFitResultGroup(record, index, model, param, tr("fit results"));
            setFitResultLabel(record, index, model, param, getParameterName(model, i, false), getParameterName(model, i, true));
            setFitResultSortPriority(record, index, model, param, true);
            //qDebug()<<"model param "<<i<<": "<<param<<" = "<< getFitValue(record, index, model, param)<<getParameterUnit(model, i, false);
        }

        // you can overwrite certain of these parameters using code like this:
        /*setFitResultValue(record, index, model, param="focus_struct_fac", getFitValue(record, index, model, param));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("focus structure factor"), QString("focus structure factor <i>&gamma;</i>"));
        setFitResultSortPriority(record, index, model, param, true);*/




        // store number of iterations ... you may also store more fit algorithm properties like this
        setFitResultValue(record, index, model, param="maxent_alpha", alpha);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt scaling parameter alpha"), tr("scaling parameter &alpha;"));

        setFitResultValueInt(record, index, model, param="maxent_iterations", ceil(getFitValue(record, index, model, param)+NumIter));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("number of MaxEnt iterations"), tr("number of MaxEnt iterations"));

        setFitResultValueInt(record, index, model, param="maxent_numiter", NumIter);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("number of MaxEnt iterations in last run of algorithm"), tr("last MaxEnt iterations"));

        setFitResultValueString(record, index, model, param="used_model", getModelName(model));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model name"), tr("used model name"));

        setFitResultValueInt(record, index, model, param="maxent_Ndist",Ndist);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt number of distribution points"), tr("MaxEnt number of distribution points"));

        setFitResultValueInt(record, index, model, param="used_model_id", model);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model id"), tr("used model id"));

        setFitResultValueInt(record, index, model, param="used_run", index);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used run"), tr("used run"));

        setFitResultValue(record, index, model, param="runtime", duration, tr("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("fit runtime"), tr("fit runtime"));

        setFitResultValueString(record, index, model, param="fitalg_message", tr("MaxEnt finished successfully after %1 iterations with alpha=%2").arg(NumIter).arg(alpha));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt message"), tr("MaxEnt message"));

        setFitResultValueString(record, index, model, param="fitalg_messageHTML", tr("<b>MaxEnt finished successfully</b><br>after %1 iterations with &alpha;=%2").arg(NumIter).arg(alpha));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt message"), tr("MaxEnt message"));

        // CALCULATE FIT STATISTICS
        //   now we evaluate the model for the given distribution
        /////////////////////////
        /////////////////////////
        // I changed the 7th argument in the evaluateModel call from &(taus[rangeMindatarange]) to &(distTaus[rangeMinDatarange])
        // this is what the 7th and 8th argument used to be like: &(distTaus[rangeMinDatarange]), &(dist[rangeMinDatarange])
        // this was the last argument:  rangeMaxDatarange-rangeMinDatarange, now changed to Ndist
        ////////////////////////

        evaluateModel(record, index, model, taus, modelEval, N, distTaus,dist,Ndist);
        //   then we can call calcFitStatistics()
        QFFitStatistics fit_stat=calcFitStatistics(record, index, model, taus, corrdata, N,Ndist, rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);
        //   finally we have to free the memory allocated in the calcFitStatistics() result.
        fit_stat.free();

        free(dist);
        free(weights);
        free(modelEval);
        free(distTaus);
        free(distDs);


    }

    if (doEmit) record->enableEmitResultsChanged(true);
    set_doEmitResultsChanged(thisDoEmitResults);
    set_doEmitPropertiesChanged(thisDoEmitProps);
}


QString QFFCSMaxEntEvaluationItem::getParameterName(int model, int id, bool html) const {
    switch (model)
    {
        case 0: //3D diffusion with triplet
            if (id==0) return (html)?tr("triplet decay time &tau;<sub>T</sub> [&mu;s]"):tr("triplet decay time [microseconds]");
            if (id==1) return (html)?tr("triplet fraction &theta;<sub>T</sub> [0..1]"):tr("triplet fraction [0..1]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            break;
        case 1: //3D diffusion with 2 blinking components
            if (id==0) return (html)?tr("triplet decay time &tau;<sub>T</sub> [&mu;s]"):tr("triplet decay time [microseconds]");
            if (id==1) return (html)?tr("triplet fraction &theta;<sub>T</sub> [0..1]"):tr("triplet fraction [0..1]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            if (id==3) return (html)?tr("dark state decay time &tau;<sub>D</sub> [&mu;s]"):tr("dark state decay time [microseconds]");
            if (id==4) return (html)?tr("dark state fraction &theta;<sub>D</sub> [0..1]"):tr("dark state fraction [0..1]");
            if (id==5) return (html)?tr("Offset G<sub>&#x221E;</sub>"):tr("Offset G_infinity");
                break;
        case 2:
            if (id==0) return (html)?tr("triplet decay time &tau;<sub>T</sub> [&mu;s]"):tr("triplet decay time [microseconds]");
            if (id==1) return (html)?tr("triplet fraction &theta;<sub>T</sub> [0..1]"):tr("triplet fraction [0..1]");
            if (id==2) return (html)?tr("dark state decay time &tau;<sub>D</sub> [&mu;s]"):tr("dark state decay time [microseconds]");
            if (id==3) return (html)?tr("dark state fraction &theta;<sub>D</sub> [0..1]"):tr("dark state fraction [0..1]");
            if (id==4) return (html)?tr("Offset G<sub>&#x221E;</sub>"):tr("Offset G_infinity");
                break;
        case 3: //3D diffusion with 2 blinking components
            if (id==0) return (html)?tr("A"):tr("A");
                break;
        case 4: //3D diffusion with 2 blinking components
            if (id==0) return (html)?tr("decay time &tau;<sub>1</sub> [&mu;s]"):tr(" decay time 1 [microseconds]");
            if (id==1) return (html)?tr("decay time &tau;<sub>2</sub> [&mu;s]"):tr(" decay time 2 [microseconds]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            if (id==3) return (html)?tr("fraction &rho;"):tr("fraction");
            if (id==4) return (html)?tr("particle number"):tr("particle number");
            if (id==5) return (html)?tr("Offset G<sub>&#x221E;</sub>"):tr("Offset G<sub>&#x221E;</sub>");
            break;
        case 5: //3D SPIM-FCS
            //if (id==0) return (html)?tr("focus 1/e<sup>2</sup>-width w<sub>xy</sub> [nm]"):tr("focus 1/e^2-width [nm]");
            if (id==0) return (html)?tr("focus 1/e<sup>2</sup>-height w<sub>z</sub> [nm]"):tr("focus 1/e^2-height [nm]");
            if (id==1) return (html)?tr("pixel size a [nm]"):tr("pixel size [nm]");
            if (id==2) return (html)?tr("Offset G<sub>&#x221E;</sub>"):tr("Offset G_infinity");
            break;
        case 6: //2D SPIM-FCS
            if (id==0) return (html)?tr("pixel size a [nm]"):tr("pixel size [nm]");
            if (id==1) return (html)?tr("Offset G<sub>&#x221E;</sub>"):tr("Offset G_infinity");
            break;
        }
        return QString();
}


QString QFFCSMaxEntEvaluationItem::getParameterUnit(int model, int id, bool html) const {
    switch (model)
    {
    case 0:
        if (id==0) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==1) return QString("");
        if (id==2) return QString("");
            break;
    case 1:
        if (id==0) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==1) return QString("");
        if (id==2) return QString("");
        if (id==3) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==4) return QString("");
        if (id==5) return QString("");
            break;
    case 2:
        if (id==0) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==1) return QString("");
        if (id==2) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==3) return QString("");
        if (id==4) return QString("");
            break;
    case 3:
        if (id==0) return QString("");
            break;
    case 4:
        if (id==0) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==1) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==2) return QString("");
        if (id==3) return QString("");
        if (id==4) return QString("");
        if (id==5) return QString("");
            break;
    case 5:
        if (id==0) return (html)?tr("nm"):tr("nm");
        if (id==1) return (html)?tr("nm"):tr("nm");
        if (id==2) return QString("");
            break;
    case 6:
        if (id==0) return (html)?tr("nm"):tr("nm");
        if (id==1) return QString("");
            break;
    }
    return QString();
}
int QFFCSMaxEntEvaluationItem::getParameterCount(int model) const {
    switch (model)
    {
        case 0: return 3;
        case 1: return 6;
        case 2: return 5;
        case 3: return 1;
        case 4: return 6;
        case 5: return 3;
        case 6: return 2;


    }
    return 0;
}

QString QFFCSMaxEntEvaluationItem::getParameterID(int model, int id) const {
    switch (model)
    {
    case 0:
        if (id==0) return tr("trip_tau");
        if (id==1) return tr("trip_theta");
        if (id==2) return tr("focus_struct_fac");
        break;
    case 1:
        if (id==0) return tr("trip_tau");
        if (id==1) return tr("trip_theta");
        if (id==2) return tr("focus_struct_fac");
        if (id==3) return tr("dark_tau");
        if (id==4) return tr("dark_theta");
        if (id==5) return tr("offset");
            break;
    case 2:
        if (id==0) return tr("trip_tau");
        if (id==1) return tr("trip_theta");
        if (id==2) return tr("dark_tau");
        if (id==3) return tr("dark_theta");
        if (id==4) return tr("offset");
            break;
    case 3:
        if (id==0) return tr("A");
            break;
    case 4:
        if (id==0) return tr("tau_1");
        if (id==1) return tr("tau_2");
        if (id==2) return tr("focus_struct_fac");
        if (id==3) return tr("fraction");
        if (id==4) return tr("particle_number");
        if (id==5) return tr("offset");
            break;
    case 5:
        if (id==0) return tr("focus_height");
        if (id==1) return tr("pixel_size");
        if (id==2) return tr("offset");
            break;
    case 6:
        if (id==0) return tr("pixel_size");
        if (id==1) return tr("offset");
            break;

    }
    return QString("m%1_p%2").arg(model).arg(id);
}

int QFFCSMaxEntEvaluationItem::getModelCount(QFRawDataRecord *r, int index) const {
    return 7;
}
