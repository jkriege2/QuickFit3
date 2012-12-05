#ifndef FITALGORITHM_SIMANNEAL_H
#define FITALGORITHM_SIMANNEAL_H

#include "qfpluginfitalgorithm.h"
#include "MersenneTwister.h"
#include <cmath>
#include <cfloat>

/*!
    \defgroup qf3fitalgp_sa Simulated Annealing Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/



/*! \brief QQFFitAlgorithm class for the Simulated Annealing method
    \ingroup qf3fitalgp_sa


    \section QFFitAlgorithmSA_desc Algorithm Description

    The algorithm implemented in this class is described in:

    <b>[COR87]</b>	Corana, A., Marchesi, M., Martini, C., Ridella, S.: <b>Minimizing Multimodal Functions of Continuous Variables with the "Simulated Annealing" Algorithm.</b>  in: <i>ACM Transactions on Mathematical Software</i> <b>(13)</b> 3: p.262-280, 1987

    This is a relatively simple implementation of the common simmulated annealing method which may be denotes
    in pseudocode like this:
    \code
      p = init_parameter_guess;
      N = 0; // step counter
      NT = init_steps_per_temperature;
      rT = init_temperature_reduction_coefficient;
      T = init_temperature;
      do {
          p1=neighbor(p); // generate new guess for parameters
          if ( chi2(p1) < chi2(p) ) {
              // accept guess p1
              p = p1;
          } else {
              if ( rand() < exp( (chi2(p)-chi2(p1)) / T ) ) {
                  // accept guess p1, although it does not improve chi2
                  p = p1;
              } else {
                  // reject guess p1
              }
          }
          N = N + 1;
          if ( N > NT ) {
              // reduce temperature
              NT = 0;
              T = T * rT;
          }
      } while (not converged);
      return p, chi2(p);
    \endcode

    In this code we do not explain how to check convergence and how to generate a new guess. Here the
    latter is done by a function neighbor(p) which chooses a new vector p1 which is in the vicinity of
    p. The first if ensures that guesses that reduce the \f$ \chi^2 \f$ value are always accepted. This
    implies an ensured downhill movement Guesses that increase \f$ \chi^2 \f$ may be accepted with a propability
    \f$ \exp\left(\frac{\chi^2(p)-\chi^2(p')}{T}\right). \f$ Here \f$ T \f$ is a temperature constant in the
    sense of a metropolis choice.

    The algorithm from [COR87] may be denoted like this:
    -# <b>Preconditions:</b>
        - The model function \f$ m(\tau, \vec{p}) \f$ is described by M parameters \f$ \vec{p}=(p_1,...,p_M)^t \f$. All
          parameters are provided with bounds for their value, i.e. \f$ a_i\leq p_i\leq b_i. \f$
        - the user provides an initial guess \f$ \vec{p}_0 \f$ for the parameters.
        - the algorithm minimizes the objective function \f[ \chi^2(\vec{p})=\sum\limits_{i=1}^N\frac{\bigl(y_i-m(\tau_i, \vec{p})\bigr)^2}{\sigma_i^2} \f]
          i.e. it finds the solution to the minimization problem \f[ \min\limits_{\vec{p}}\chi^2(\vec{p}) \f]
          for a given set of N measurements \f$ (\tau_i, y_i)_{i=1..N} \f$
        - the user provides the parameters \f$ N_{max}, N_S, N_T, f_{max}, T_0 \f$ and \f$ c \f$
    -# <b>Initialization:</b>
        - initial temperature: \f$ T=T_0 \cdot 10^{ \left\lfloor \log_{10}(\chi^2(\vec{p}_0)) \right\rfloor } \f$
        - initial max. step sizes: \f$ v_i=\frac{|b_i-a_i|}{1000},\ \ \ \ \ \ \ \ \ \ (i=1..M) \f$
        - initial best guess: \f$ \vec{p}_{opt}=\vec{p}_0,\ \ \  \chi^2_{opt}=\chi^2(\vec{p}_0) \f$
        - \f$ \vec{p}_{cur}=\vec{p}_0,\ \ \  \chi^2_{cur}=\chi^2(\vec{p}_0) \f$
        - \f$ N=0 \f$ (iteration counter)
    -# <b>Repeat the next steps until convergence or \f$ N>N_{max} \f$ :</b>
        - <b>Repeat \f$ N_T \f$ times:</b>
            - \f$ N\leftarrow N+1 \f$
            - reset all \f$ V_n=0,\ \ \ (n=1,...,M) \f$. The \f$ V_n \f$ count the accepted variation steps
              of the n-th parameter.
            - <b>Repeat \f$ N_S \f$ times:</b>
                - for every parameter \f$ n=1..M \f$ generate a new guess \f[ \vec{p}'=\vec{p} + r\cdot v_n\cdot\vec{e}_n \f]
                  here \f$ r\in[0..1] \f$ is a uniformly distributed random number and \f$ \vec{e}_n \f$ is a vector where all
                  elements are zero, except element n which is one (unit vector in direction n)
                - <b>if</b> \f$ \mathbf{\chi^2(\vec{p}')<\chi^2_{cur}} \f$ then accept the new vector, i.e. \f$ \vec{p}_{cur}=\vec{p}' \f$ and
                  \f$ \chi^2_{cur}=\chi^2(\vec{p}') \f$, if even \f$ \chi^2(\vec{p}')<\chi^2_{opt} \f$ then also accept the
                  new guess as optimal point, i.e. \f$ \vec{p}_{opt}=\vec{p}' \f$ and \f$ \chi^2_{opt}=\chi^2(\vec{p}') \f$
                - <b>otherwise</b> accept the reduction of \f$ \chi^2 \f$ with a metropolis probability \f$ p=\exp\left(\frac{\chi^2_{cur}-\chi^2(\vec{p}')}{T}\right) \f$
                  for \f$ \vec{p}_{cur}, \chi^2_{cur} \f$
                - if a variation in parameter n has been accepted increase \f$ V_n \f$ by 1
            - <b>End Repeat</b>
            - update the step size vector \f$ \vec{v}=(v_1,...,v_M)^t \f$:\vec{p}
              \f[ v_n'=\begin{cases}v_n\cdot\left(1+c\cdot\frac{V_n/N_S-0.6}{0.4}\right) & V_n>0.6\cdot N_S\\ v_n\cdot\frac{1}{1+c\cdot\frac{0.4-V_n/N_S}{0.4}} & V_n<0.4\cdot N_S\\ v_n& \text{otherwise}\end{cases} \f]
        - <b>End Repeat</b>
        - update temperature: \f$ T'=T\cdot r_T \f$
        - check convergence: test whether \f$ |\chi^2_{cur, i}-\chi^2{cur, i-j}|\leq f_{max} \f$ folds for all \f$ j=1..N_\epsilon \f$
          where \f$ \chi^2_{cur, i} \f$ is the \f$ \chi^2_{cur} \f$ value of the i-th convergence check. This means that the change
          between two subsequent iterations may not exceed \f$ f_{max} \f$
    .



*/
class QFFitAlgorithmSimAnneal: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmSimAnneal();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmSimAnneal();
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("Simulated Annealing with box constraints"); };
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_simanneal"); };
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; };
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();

        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        virtual bool isThreadSafe() const ;



    protected:

        /** \brief implements the function to be fit, i.e. \f$ f(\vec{p})=\sum\limits_{i=1}^M\frac{\left[c_i-m(\tau_i, \vec{p})\right]^2}{\sigma_i^2} \f$
          *
          * \param model the model function \f$ m(\tau_i, \vec{p}) \f$ to evaluate
          * \param p parameter vector
          * \return \f$ f(\vec{p})=\sum\limits_{i=1}^M\frac{\left[c_i-m(\tau_i, \vec{p})\right]^2}{\sigma_i^2} \f$
          */
        double chi2(QFFitAlgorithm::Functor* model, double* p);

        /** \brief evaluation count for chi2() */
        unsigned int chi2EvalCount;

        /** \brief Mersenne twister random number generator */
        MTRand* rg;

        /** \brief generate a random number from [0..n] */
        inline double ran(double n=1) {
            return rg->rand(n);
        };

        /** \brief return a gaussian random number with mean \a mean and width \a sigma, i.e. from N(mean, sigma). */
        inline double ranGauss(double mean=0.0, double sigma=1.0) {
            return rg->randNorm(mean, sigma);
        };

        /** \brief seed the random number generator with pseudo-random initial values (system time ...) */
        inline void randSeed() {
            rg->seed();
        };

};



/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_sa
*/
class QFPFitAlgorithmSimAnneal : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
    public:
        QF_PLUGIN

        /** Default constructor */
        QFPFitAlgorithmSimAnneal(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitAlgorithmSimAnneal() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const {
            return QString("fit_simanneal");
        };


        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("Simulated Annealing with box constraints");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("Simulated Annealing Fitting Method, a stochastic optimizer.");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2009-2011 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };


        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            return ":/lib/fitalg_icon.png";
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const {
            QStringList sl;
            sl<<"fit_simanneal";
            return sl;
        }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const {
            if (id=="fit_simanneal") return new QFFitAlgorithmSimAnneal();
            return NULL;
        }
    private:
};

#endif // FITALGORITHM_SIMANNEAL_H
