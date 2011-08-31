#include "fitalgorithm_simanneal.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "simannealconfig.h"
#include <QApplication>

QFFitAlgorithmSimAnneal::QFFitAlgorithmSimAnneal() {
    setParameter("max_iterations", 50000);
    setParameter("fmax", 1);
    setParameter("ns", 20);
    setParameter("rt", 0.5);
    setParameter("c", 2);
    setParameter("nt", 50);
    setParameter("nepsilon", 4);
    setParameter("t0", 0.001);
    rg=new MTRand;
}

QFFitAlgorithmSimAnneal::~QFFitAlgorithmSimAnneal() {
    delete rg;
}

double QFFitAlgorithmSimAnneal::chi2(QFFitAlgorithm::Functor* model, double* p) {
    chi2EvalCount++;
    register double c2=0;
    register int imax=model->get_evalout();
    double* data=(double*)calloc(imax, sizeof(double));

    model->evaluate(data, p);

    for (register int i=0; i<imax; i++) {
        register double d=data[i];
        c2=c2+d*d;
    }

    free(data);
    return c2;
}


QFFitAlgorithm::FitResult QFFitAlgorithmSimAnneal::intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, QFFitAlgorithm::Functor* model, const double* paramsMin, const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    // parameter count
    int fitParamCount=model->get_paramcount();


    // the maximum number of iterations allowed to the SA algorithm */
    int iterationsMax=getParameter("max_iterations").toInt();

    // counter for the algorithm iterations */
    unsigned int iterations;


    // threshold \f$ \overline{f}_{\mathrm{max}} \f$ to detect the convergence of the algorithm */
    double fmax=getParameter("fmax").toDouble();

    // contains the initial \f$ \chi^2 \f$ (calculated with chi2() ) */
    double initialChi2;

    // initial temperature multiplicator \f$ t_= \f$: \f$ T_{start}=T_0\cdot10^{\lfloor\log_{10}(\chi^2(\vec{p}_0))\rfloor} \f$ */
    double T0=getParameter("t0").toDouble();

    // number \f$ N_s \f$ of step length variations bevore convergence check */
    double NS=getParameter("ns").toDouble();

    // reduction factor for the temperature \f$ T_{i+1}=T_i\cdot r_t \f$ */
    double rT=getParameter("rt").toDouble();

    // change constant for step length update (equal for all parameters) */
    double c=getParameter("c").toDouble();

    // number \f$ N_T \f$ of possible step variations */
    double NT=getParameter("nt").toDouble();

    // for convergence: in the last \f$ N_\epsilon \f$ iterations' \f$ \chi^2_i(\cdot) \f$ values, two
    //         subsequent values \f$ \chi^2_{i-1}(\cdot) \f$ and \f$ \chi^2_i(\cdot) \f$ may not differ by more than \f$ f_{max} \f$
    int Nepsilon=getParameter("nepsilon").toDouble();

    // step vector, this vector holds the current maximúm step sizes */
    double* v=(double*)calloc(fitParamCount,sizeof(double));
    // init step vector v
    for (int i=0; i<fitParamCount; i++) {
        v[i]=fabs(paramsMax[i]-paramsMin[i])/1000.0;
    };

    // current parameters */
    double* x0=(double*)calloc(fitParamCount,sizeof(double));;
    memcpy(x0, initialParams, fitParamCount*sizeof(double));
    memcpy(paramsOut, initialParams, fitParamCount*sizeof(double));

    // next parameters */
    //double* x1;

    // current \f$ \chi^2 \f$ */
    double currentChi2;

    // best fit \f$ \chi^2 \f$ of parameterset param (best fit parameters)*/
    double bestChi2;


    // reset function evaluation and iteration counters
    chi2EvalCount=0;
    iterations=0;

    // init random number generator
    randSeed();

    currentChi2=chi2(model, x0);
    bestChi2=currentChi2;
    initialChi2=currentChi2;

    // increment the progress every iterations_progress_step iterations
    int iterations_progress_step=iterationsMax/50;

    setProgressMax(50);


    int error=0;
    bool finished=false;
    double T=T0*pow(10.0,floor(log10(currentChi2)));
    int* dirVariations=(int*)calloc(fitParamCount, sizeof(int));
    double* f_asterisk=(double*)calloc(Nepsilon, sizeof(double));
    for (int i=0; i<Nepsilon; i++) {
        f_asterisk[i]=currentChi2;
    }
    int k=0;

    do {
        for (unsigned int tempSteps=0; tempSteps<NT; tempSteps++) {

            for (register unsigned int stepVariations=0; stepVariations<NS; stepVariations++) {
                iterations++;
                for (register int h=0; h<fitParamCount; h++) {

                    // x' = x + r*v[h] * eh
                    //     where eh is the unit vector in direction h
                    // for this ensure that a_h <= x'_h <= b_h
                    register double new_xh=0;
                    register double old_xh=x0[h];
                    do {
                        // r=random number from flat distribution between -1..1
                        new_xh=x0[h]+(ran(2.0)-1.0)*v[h];
                    } while (new_xh<paramsMin[h] || new_xh>paramsMax[h]);
                    x0[h]=new_xh;
                    register double testChi2=chi2(model, x0);
                    if (testChi2<currentChi2) {
                        // accept trial step
                        currentChi2=testChi2;
                        dirVariations[h]++;

                        // save best fits
                        if (currentChi2<bestChi2) {
                            memcpy(paramsOut, x0, fitParamCount*sizeof(double));
                            bestChi2=testChi2;
                        }
                        //std::cout<<std::endl;
                    } else {
                        // Metropolis-Monte-Carlo step
                        if (ran()<exp((currentChi2-testChi2)/T)) {
                            // accept trial step
                            currentChi2=testChi2;
                            dirVariations[h]++;
                        } else {
                            // reset to former value (old_xh)
                            x0[h]=old_xh;
                        }
                    }
                }
            }

            // update the step vector
            for (register int h=0; h<fitParamCount; h++) {
                if (dirVariations[h]>0.6*(double)NS) {
                    v[h]=v[h]*(1.0+c*(dirVariations[h]/(double)NS-0.6)/0.4);
                    if (v[h]>fabs(paramsMax[h]-paramsMin[h])/2.0) v[h]=fabs(paramsMax[h]-paramsMin[h])/2.0;
                } else if (dirVariations[h]<0.4*(double)NS) {
                    v[h]=v[h]/(1.0+c*(0.4-dirVariations[h]/(double)NS)/0.4);
                    if (v[h]>fabs(paramsMax[h]-paramsMin[h])/2.0) v[h]=fabs(paramsMax[h]-paramsMin[h])/2.0;
                }
                dirVariations[h]=0;

            }

            if (isCanceled()) {
                error=-3;
            }
        }
        // temperature update
        T=T*rT;
        //std::cout<<"  new temperature: "<<T<<std::endl;

        // stop criterion
        f_asterisk[k]=currentChi2;
        finished=true;
        //std::cout<<"  fmax="<<fmax<<"     ";
        for (register int u=0; (u<Nepsilon) && (finished); u++) {
            finished=finished && (fabs(currentChi2-f_asterisk[u])<=fmax);
            //std::cout<<"   |"<<currentChi2<<"-"<<f_asterisk[u]<<"|="<<fabs(currentChi2-f_asterisk[u]);
        }
        //std::cout<<std::endl;
        k++; if (k>=Nepsilon) k=0;

        memcpy(x0, paramsOut, fitParamCount*sizeof(double));
        currentChi2=bestChi2;

        //std::cout<<"  bestChi2="<<bestChi2<<"   bestParam="<<doublevectortostr(param, fitParamCount)<<"   iterations="<<iterations<<std::endl;

        if ((iterations%iterations_progress_step)==0) {
            incProgress();
        }
    } while ((!finished) && (error==0) && ((long)iterations<iterationsMax));



    QString reason="", reason_simple="";
    if ((long)iterations>=iterationsMax) {
        reason+=QObject::tr(" stopped by maximum iterations.");
        reason_simple+=QObject::tr(" stopped by maximum iterations.");
    }
    if (finished) {
        reason+=QObject::tr(" stopped by abortion criterion f<sub>max</sub>.");
        reason_simple+=QObject::tr(" stopped by abortion criterion f_max.");
    }
    if (error!=0) {
        QString errorDescription="";
        if (error==-1) errorDescription=QObject::tr("Not a Number (NaN) occured during chi2() function evaluation");
        if (error==-2) errorDescription=QObject::tr("Infinity occured (Inf) during chi2() function evaluation");
        if (isCanceled() || (error==-3)) errorDescription=QObject::tr("canceled by user");
        reason+=QObject::tr(" stopped by error: %1").arg(errorDescription);
        reason_simple+=QObject::tr(" stopped by error: %1").arg(errorDescription);
    }
    result.addString("stop_reason", reason_simple);






    result.addNumber("initial_error_sum", initialChi2);
    result.addNumber("error_sum", currentChi2);
    result.addNumber("iterations", iterations);
    result.addNumber("function_evals", chi2EvalCount);
    result.addNumber("fit_paramcount", fitParamCount);



    for (int i=0; i<fitParamCount; i++) {
        paramErrorsOut[i]=0;
    }

    result.message=QObject::tr("<b>SimAneal</b> returned after %1 iterations and %2 &chi;<sup>2</sup>-function evaluations.<br>reason: %3\nSES = %4  [old_SES = %5]").arg(iterations).arg(chi2EvalCount).arg(reason).arg(currentChi2).arg(initialChi2);
    result.messageSimple=QObject::tr("SimAneal returned after %1 iterations and %2 chi2-function evaluations. reason: %3, SES = %4  [old_SES = %5]").arg(iterations).arg(chi2EvalCount).arg(reason_simple).arg(currentChi2).arg(initialChi2);

    std::cout<<result.messageSimple.toStdString()<<std::endl;

    result.fitOK=(error==0);


    free(v);
    free(x0);
    free(dirVariations);
    free(f_asterisk);


    return result;
}

bool QFFitAlgorithmSimAnneal::displayConfig() {

    SimannealConfigDialog* dlg=new SimannealConfigDialog(0);
    dlg->setMaxIterations(getParameter("max_iterations").toInt());
    dlg->setT0(getParameter("t0").toDouble());
    dlg->setNS(getParameter("ns").toDouble());
    dlg->setNT(getParameter("nt").toDouble());
    dlg->setFMax(getParameter("fmax").toDouble());
    dlg->setC(getParameter("c").toDouble());
    dlg->setRT(getParameter("rt").toDouble());
    dlg->setNEpsilon(getParameter("nepsilon").toDouble());

    if (dlg->exec()==QDialog::Accepted) {
        setParameter("max_iterations", dlg->getMaxIterations());
        setParameter("t0", dlg->getT0());
        setParameter("ns", dlg->getNS());
        setParameter("nt", dlg->getNT());
        setParameter("fmax", dlg->getFMax());
        setParameter("c", dlg->getC());
        setParameter("rt", dlg->getRT());
        setParameter("nepsilon", dlg->getNEpsilon());
        delete dlg;
        return true;
    } else {
        delete dlg;
        return false;
    }
}


Q_EXPORT_PLUGIN2(fitalgorithm_simanneal, QFPFitAlgorithmSimAnneal)
