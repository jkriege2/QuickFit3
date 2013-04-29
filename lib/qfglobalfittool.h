#ifndef QFGLOBALFITTOOL_H
#define QFGLOBALFITTOOL_H
#include "qffitalgorithm.h"



/*! \brief this is a special functor which is used for data fitting of a multi-dataterm fit problem with objective functions declared by QFFitFunction
    \ingroup qf3lib_fitting

    The QFFitFunction declares a set of \f$ k=1..K\f$ 1D function \f$ f_k(x; \vec{p}_k), \vec{p}_k\in\mathbb{R}^{N_k} \f$ .
    This functor is provided with a set of data \f$ (x_{k,m}, y_{k,m}, \sigma_{k,m}), m=1..M_k \f$
    where \f$ \sigma_{k,m} \f$ are weights for the data.

    Then This Functor implementation maps the 1D Function \f$ f_k(x; \vec{p}_k) \f$ to a \f$ \sum\limits_kM_k \f$ dimensional function
    \f$ \vec{g}(\vec{p}_1,...,\vec{p}_K) \f$ with:
        \f[ \vec{g}(\vec{p}_1,...,\vec{p}_K) = \begin{pmatrix}
            (y_{1,1}-f_1(x_{1,1}; m(\vec{p}_1)))/\sigma_{1,1} \\
            \ldots \\
            (y_{1,M_1}-f_1(x_{1,M_1}; m(\vec{p}_1)))/\sigma_{1,M_1} \\
            (y_{2,1}-f_2(x_{2,1}; m(\vec{p}_2)))/\sigma_{2,1} \\
            \ldots \\
            (y_{2,M_2}-f_2(x_{2,M_2}; m(\vec{p}_2)))/\sigma_{2,M_2} \\
            \ldots \\
            \ldots \\
            (y_{K,M_K}-f_K(x_{K,M_K}; m(\vec{p}_K)))/\sigma_{K,M_K} \\
            \end{pmatrix}
        \f]
    This allows to solve global optimization problems where the objective function contains several dataterms:
      \f[ \xhi^2(\vec{p}_1,...,\vec{p}_K)=\vec{g}(\vec{p}_1,...,\vec{p}_K)\cdot\vec{g}(\vec{p}_1,...,\vec{p}_K) = \sum\limits_{k=1}^K \sum\limits_{m=^}^{M_k}\left|\frac{y_{k,m}-f_k(x_{k,m}; \vec{p}_k)}{\sigma_{k,m}}\right|^2 \f]
    Internally this class uses a set of FitQFFitFunctionFunctor objects to evaluate each separate fit function.

    In addition this class allows to link some of the parameters together: A set of global parameter \f$ P_i \f$ may be used
    in several of the single parameter vectors \f$ \vec{p}_k \f$. So for each \f$ P_i \f$ a list of pairs \f$ \langle k,\iota_p\rangle_j \f$
    has to be stored that says which parameter \f$ \iota_p \f$ (as index in \f$ \vec{p}_k \f$ ) in the k-th data-term is set to the global
    parameter \f$ P_i \f$. The parameter of evaluate() will then have this format:
\beginverbatim
     N(globals)       N(1)-N(1globals)             N(2)-N(2globals)
+-----------------+----------------------+-------------------------------------+ . . . . . . .
|    global       |    local parameter   |           local parameter           |
| parameters P(.) | of dataterm 1: p1(.) |        of dataterm 2: p2(.)         |
+-----------------+----------------------+-------------------------------------+ . . . . . . .
              ++++                     ++                                     ++
          ++++                        +  +                                      ++
      ++++                           +    +                                       ++
  ++++                              +      +                                        ++
  +---------------------------------+      +------------------------------------------+ . . . . . . .
  |     PARAMETER VECTOR TO         |      |           PARAMETER VECTOR TO            |
  |     dataterm1.evaluate()        |      |           dataterm2.evaluate()           |
  +---------------------------------+      +------------------------------------------+ . . . . . . .
  | p1(0), p1(1), P(0), p1(2), ...  |      | p2(0), p2(1), P(0), p2(2), P(1), p2(3)...|
  +---------------------------------+      +------------------------------------------+ . . . . . . .
          N(1) elements                                   N(2) elements
\endverbatim
    Here \c N(1), \c N(2),... are the number of parameters in each data term and \c N(1globals), \c N(2globals),... are
    the number of these parameters, replaced by global parameters. The data of each data term is stored in an element
    of subFunctors. The functor subFunctorData::f is used to evaluate \f$ f_k(x; \vec{p}_k) \f$, the property
    subFunctorData::mapToLocal contains the index in the main parameter array (top in image above) of the
    according parameter.

*/
class QFLIB_EXPORT QFFitMultiQFFitFunctionFunctor: public QFFitAlgorithm::Functor {
    public:
        /*! \brief constructor, initializes the functor
        */
        QFFitMultiQFFitFunctionFunctor() ;

        virtual ~QFFitMultiQFFitFunctionFunctor();

        /** \brief add data term */
        void addTerm(QFFitFunction* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataWeight, uint64_t M);

        /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
        virtual void evaluate(double* evalout, const double* params);

        /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
        virtual void evaluateJacobian(double* evalout, const double* params);

        /** \brief returns \c true if all models implement theor jacobians analytically and therefore evaluateJacobian() may be used */
        virtual bool get_implementsJacobian() const;

        /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
        virtual int get_paramcount() const;

        /** \brief init linking information */
        void setAllLinks(const QList<QList<QPair<int, int> > >& linkedParams);
        /** \brief add global parameter */
        void addGlobalParam(const QList<QPair<int, int> > & links);
        /** \brief add linking information to global parameter */
        void addToGlobalParam(int param, QPair<int, int> link);
        /** \brief clear object */
        void clear();

        bool isLinkedSingle(int idx) const;
        /** \brief returns the IDX of the global parameter (i.e. of the outer list in linkedParams) to which the pait <dataTermID, parameterID> is linked */
        int getLinkID(int dataTermID, int parameterID);
    protected:
        struct subFunctorData {
            /** \brief functor to evaluate this data term */
            QFFitAlgorithm::FitQFFitFunctionFunctor* f;
            /** \brief number of local parameters to this data term */
            int localParams;
            /** \brief maps from the global parameter vector (as supplied to evaluate() ) to the local vector, as supplied to f->evaluate() */
            int* mapToLocal;
        };

        /** \brief functions to evaluate the single data terms */
        QList<subFunctorData> subFunctors;

        /*! \brief encodes the linked parameters.

            This data structure contains a list of global parameters. For each such parameter it contains a list
            of <dataTermID, parameterID> pairs that specify which properties are linked together.
         */
        QList<QList<QPair<int, int> > > linkedParams;

        int m_paramCount;
        int m_linkedParamsCount;

        void recalculateInternals();

};





/*! \brief tool class to perform global fitting with linked parameters, using QFFitMultiQFFitFunctionFunctor
    \ingroup qf3lib_fitting
 */
class QFGlobalFitTool {
    public:
        QFGlobalFitTool(QFFitAlgorithm* algorithm);
        ~QFGlobalFitTool();
    protected:

        QFFitAlgorithm* m_algorithm;
        QFFitMultiQFFitFunctionFunctor* functor;
};

#endif // QFGLOBALFITTOOL_H
