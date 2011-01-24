#include "qffitalgorithm.h"
#include <QMessageBox>
#include <QObject>
#include <cfloat>
#include <cstdio>

class privateQFFitAlgorithmFitFunctor: public QFFitAlgorithm::Functor {
    public:
        privateQFFitAlgorithmFitFunctor(QFFitFunction* model, double* currentParams, bool* fixParams, double* dataX, double* dataY, double* dataWeight, uint64_t N):
            QFFitAlgorithm::Functor(N)
        {
            m_model=model;
            m_dataX=dataX;
            m_dataY=dataY;
            m_dataWeight=dataWeight;
            m_N=N;
            functorFromModel=NULL;
            modelFromFunctor=NULL;

            // now we calculate the mapping of the data
            m_paramCount=0;
            for (int i=0; i<model->paramCount(); i++) {
                if (!fixParams[i]) {
                    QFFitFunction::ParameterDescription d=model->getDescription(i);
                    if (d.fit) {
                        m_paramCount++;
                    }
                }
            }
            functorFromModel=(int*)calloc(model->paramCount(), sizeof(int));
            modelFromFunctor=(int*)calloc(m_paramCount, sizeof(int));
            int pid=0;
            for (int i=0; i<model->paramCount(); i++) {
                functorFromModel[i]=-1;
                if (!fixParams[i]) {
                    QFFitFunction::ParameterDescription d=model->getDescription(i);
                    if (d.fit && model->isParameterVisible(i, currentParams)) {
                        functorFromModel[i]=pid;
                        modelFromFunctor[pid]=i;
                        //printf("   mapping m=%2d -> f=%2d [%s]\n", i, pid, d.id.toStdString().c_str());
                        pid++;
                    }
                }
            }
            modelParams=(double*)calloc(model->paramCount(), sizeof(double));
            for (int i=0; i<model->paramCount(); i++) {
                modelParams[i]=currentParams[i];
            }
        };

        ~privateQFFitAlgorithmFitFunctor() {
            if (functorFromModel) free(functorFromModel);
            if (modelFromFunctor) free(modelFromFunctor);
            free(modelParams);
        }

        inline double* createMappedArrayForFunctor(double* modelData) {
            double* result=(double*)calloc(m_paramCount, sizeof(double));

            for (register int i=0; i<m_paramCount; i++) {
                result[i]=modelData[modelFromFunctor[i]];
            }

            return result;
        }

        inline void mapArrayFromFunctorToModel(double* modelData, double* functorData) {
            for (register int i=0; i<m_paramCount; i++) {
                modelData[modelFromFunctor[i]]=functorData[i];
            }
        }


        inline virtual void evaluate(double* evalout, double* params, int Nparams) {
            mapArrayFromFunctorToModel(modelParams, params);
            for (int i=0; i<get_evalout(); i++) {
                evalout[i]=(m_dataY[i]-m_model->evaluate(m_dataX[i], modelParams))/m_dataWeight[i];
            }
        }

        virtual int get_paramcount() const { return m_paramCount; };
    protected:

        QFFitFunction* m_model;
        double* m_dataX;
        double* m_dataY;
        double* m_dataWeight;
        uint64_t m_N;
        int m_paramCount;
        int* functorFromModel;
        int* modelFromFunctor;
        double* modelParams;

};


QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double* paramsOut, double* paramErrorsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams, double* paramsMin, double* paramsMax) {
    QFFitAlgorithm::FitResult result;
    double* pparamsMin=paramsMin;
    double* pparamsMax=paramsMax;
    bool* pparamsFix=fixParams;
    if (paramsMin==NULL) {
        pparamsMin=(double*)calloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            pparamsMin[i]=-DBL_MAX;
        }
    }
    if (paramsMax==NULL) {
        pparamsMax=(double*)calloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            pparamsMax[i]=DBL_MAX;
        }
    }
    if (fixParams==NULL) {
        pparamsFix=(bool*)calloc(model->paramCount(), sizeof(bool));
        for (int i=0; i<model->paramCount(); i++) {
            pparamsFix[i]=false;
        }
    }

    privateQFFitAlgorithmFitFunctor fm(model, initialParams, pparamsFix, dataX, dataY, dataWeight, N);
    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    result=intMinimize(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);

    for (int i=0; i<model->paramCount(); i++) {
        paramsMax[i]=initialParams[i];
        paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    free(tparamsMax);
    free(tparamsMin);
    free(tparamErrorsOut);
    free(tparamsOut);
    free(tinitialParams);

    if (paramsMin==NULL) free(pparamsMin);
    if (paramsMax==NULL) free(pparamsMax);
    if (fixParams==NULL) free(pparamsFix);
    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double* paramsOut, double* paramErrorsOut, Functor* model, double* initialParams, double* paramsMin, double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    double* pparamsMin=paramsMin;
    double* pparamsMax=paramsMax;
    if (paramsMin==NULL) {
        pparamsMin=(double*)calloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            pparamsMin[i]=-DBL_MAX;
        }
    }
    if (paramsMax==NULL) {
        pparamsMax=(double*)calloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            pparamsMax[i]=DBL_MAX;
        }
    }

    result=intMinimize(paramsOut, paramErrorsOut, initialParams, model, pparamsMin, pparamsMax);

    if (paramsMin==NULL) free(pparamsMin);
    if (paramsMax==NULL) free(pparamsMax);
    return result;
}

bool QFFitAlgorithm::displayConfig() {
    QMessageBox::information(NULL, name(), QObject::tr("No configuration dialog is supplied for this optimization algorithm!"));
    return false;
}
