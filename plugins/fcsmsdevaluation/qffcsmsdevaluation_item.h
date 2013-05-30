#ifndef QFFCSMSDEvaluationItem_H
#define QFFCSMSDEvaluationItem_H

#include <QMap>
#include <QSettings>
#include "qftools.h"
#include "qfmathtools.h"

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "../base_classes/qfusesresultsbyindexevaluation.h"
#include "../base_classes/qfusesresultsbyindexandmodelevaluation.h"
#include "qffitresultsbyindexevaluationfittools.h"
#include "qffcsweightingtools.h"
#define MSDTHEORYCOUNT 4


/*! \brief evaluation item class 
    \ingroup qf3rdrdp_GROUPNAME


    
    
*/
class QFFCSMSDEvaluationItem : public QFUsesResultsByIndexAndModelEvaluation, public QFSimpleFitParameterEnumeratorInterface, public QFFCSWeightingTools {
        Q_OBJECT
        Q_INTERFACES(QFSimpleFitParameterEnumeratorInterface)
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFFCSMSDEvaluationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFFCSMSDEvaluationItem();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("fcs_msd"); }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS MSD Evaluation"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/fcsmsdevaluation/fcsmsdevaluation_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("extracts the mean squared displacement (MSD) from FCS data"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/fcsmsdevaluation/fcsmsdevaluation.png"); }
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Evaluation"); }
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, QFEvaluationPropertyEditor *propEditor, QWidget *parent=NULL);

        /** \brief do the evaluation */
        virtual void doFit(QFRawDataRecord* record, int index, int model, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, int runAvgWidth=11, int residualHistogramBins=25);

        virtual int getIndexMin(QFRawDataRecord* r) const;
        virtual int getIndexMax(QFRawDataRecord* r) const;
        virtual int getModelCount(QFRawDataRecord* r, int index) const;

        /** \brief returns a list of the possible fit types for calcMSDFits() and calcMSDFit() */
        static QStringList getFitTypes();


        /** \brief return the name for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterName(int model, int id, bool html=false) const;
        /** \brief return the unit for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterUnit(int model, int id, bool html) const;
        /** \brief return the number of parameters in the given model */
        virtual int getParameterCount(int model) const;
        /** \brief return the ID for the given parameter in the given model  */
        virtual QString getParameterID(int model, int param) const;

        /** \brief return the name for current model */
        QString getCurrentModelName() const;
        /** \brief return the name for given model */
        virtual QString getModelName(int model) const;

        /*! \brief evaluate the model for a given record and index

            this function evaluates the model \f$ f(\tau) \f$ at the given positions \a taus. The results are written into \a output.
            Both arrays \a taus and \a output have to have the size \a N . The model is evaluated for a given maxent distribution \f$ p(T) = (T_i, p_{T_i}=p_i)_{i=1..N_D} \f$
            stored in \a distribution_tau and  \a distribution which both habe \a distributionN elements.

            For \c model==0 this function outputs this model:
              \f[ g(\tau)=\frac{1}{N}\cdot\left(\frac{1-\theta+\theta\cdot\exp(-\tau/\tau_T)}{1-\theta}\right)\cdot\sum\limits_{i=1}^{N_D}p_{T_i}\cdot\left(1+\frac{\tau}{T_i}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\cdot T_i}\right)^{-1} \f]
          */
        void evaluateModel(QFRawDataRecord* record, int index, int model, double* taus, double* output, uint32_t N, double* msd_tau, double* msd, uint32_t Nmsd) const;

        /*! \brief returns an array representing the distribution for the given record, index, model combination
          */
        QVector<double> getMSD(QFRawDataRecord* record, int index, int model) const;
        /*! \brief returns an array representing the distribution tau values (lag time axis) for the given record, index, model combination
          */
        QVector<double> getMSDTaus(QFRawDataRecord* record, int index, int model) const;

        /*! \brief fit a power-law to subsections of the specified MSD (from record, index, model) */
        void calcMSDFits(QVector<double>& taus_out, QVector<double>& alpha_out, QVector<double>& D_out, QFRawDataRecord* record, int index, int model, int evalWidth=10, int evalShift=2, int first=0, int fit_type=0, QVector<double>*tau_start=NULL, QVector<double>*tau_end=NULL) const;

        /*! \brief fit a power-law to the specified MSD (from record, index, model) */
        void calcMSDFit(double& alpha_out, bool fixAlpha, double& D_out, bool fixD, QFRawDataRecord* record, int index, int model, double prefactor, int range_min, int range_max, int fit_type=0) const;


        /*! \brief calculates fit statistics for the given fit function and dataset.

            \note If \a modelVals is \c NULL this function will evaluate the model for the given parameters, retrieving the stored
                  distribution with getDistribution() and getDistributionTaus()
         */
        QFFitStatistics calcFitStatistics(QFRawDataRecord* record, int index, int model, double* taus, double* modelVals, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins);

        virtual QString getEvaluationResultID(int currentIndex, int model) const;
        using QFUsesResultsByIndexAndModelEvaluation::getEvaluationResultID;

        double getTheoryAlpha(int i, QFRawDataRecord* record, int run) const;
        double getTheoryD(int i, QFRawDataRecord* record, int run) const;
        double getTheoryPre(int i, QFRawDataRecord* record, int run) const;
        QString getTheoryAlphaName(int i, QFRawDataRecord* record, int run) const;
        QString getTheoryDName(int i, QFRawDataRecord* record, int run) const;
        QString getTheoryPreName(int i, QFRawDataRecord* record, int run) const;
        bool getTheoryEnabled(int i, QFRawDataRecord* record, int run) const;
        void setTheory(int i, bool enabled, double pre, double D, double alpha, QFRawDataRecord* record, int run);
        int getFitWidth(QFRawDataRecord* record, int run) const;
        void setFitWidth(int width, QFRawDataRecord* record, int run);
        bool getFitRangeLimited(QFRawDataRecord* record, int run) const;
        void setFitRangeLimited(bool rangeLimit, QFRawDataRecord* record, int run);
        int getFitType(QFRawDataRecord* record, int run) const;
        void setFitType(int type, QFRawDataRecord* record, int run);
    protected:
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);

        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);


        /** \brief returns default values for a parameter */
        virtual bool getParameterDefault(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QFUsesResultsEvaluation::FitParameterDefault& defaultValue) const;


};

#endif // QFFCSMSDEvaluationItem_H
