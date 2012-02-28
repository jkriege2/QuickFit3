#ifndef QFFCSMAXENTEVALUATIONITEM_H
#define QFFCSMAXENTEVALUATIONITEM_H

#include <QMap>
#include <QSettings>
#include "qftools.h"
#include "qfmathtools.h"

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "../base_classes/qfusesresultsbyindexevaluation.h"

/*! \brief evaluation item class 
    \ingroup qf3rdrdp_GROUPNAME


    
    
*/
class QFFCSMaxEntEvaluationItem : public QFUsesResultsByIndexEvaluation {
        Q_OBJECT
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFFCSMaxEntEvaluationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFFCSMaxEntEvaluationItem();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("fcs_maxent"); }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS MaxEnt Evaluation"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/fcs_maxent/qffcsmaxentevaluation_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("maximum entropy data evaluation plugin for FCS data"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/fcs_maxent/qffcsmaxentevaluation.png"); }
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Evaluation"); }
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);

        /** \brief do the evaluation */
        virtual void doEvaluation(QFRawDataRecord* record, int index);


        void setCurrentWeights(int index);
        int getCurrentWeights() const;
        void setCurrentIndex(int index);
        int getCurrentIndex() const;
        void setCurrentModel(int index);
        int getCurrentModel() const;
        void setAlpha(double alpha);
        double getAlpha() const;


        QString getParameterName(int model, int id) const;
        int getParameterCount(int model) const;
        QString getParameterID(int model, int param);

        QString getCurrentModelName() const;
        QString getModelName(int model) const;
        int getModelCount() const;

        /*! \brief evaluate the model for a given record and index

            this function evaluates the model \f$ f(\tau) \f$ at the given positions \a taus. The results are written into \a output.
            Both arrays \a taus and \a output have to have the size \a N
          */
        void evaluateModel(QFRawDataRecord* record, int index, int model, double* taus, double* output, uint32_t N);

        /*! \brief fills the arrays \A taus and \a dist with the distribution for the given record, index and model

            Afterwards \a N contains the number of elements in taus and dist .
          */
        void getDistributionCopy(QFRawDataRecord* record, int index, int model, double* taus, double* dist);

        uint32_t getDistributionN(QFRawDataRecord *record, int index, int model) const;

        /** \brief calculates fit statistics for the given fit function and dataset. */
        QFFitStatistics calcFitStatistics(QFRawDataRecord* record, int index, int model, double* taus, uint32_t N, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins);

        /** \brief allocate an array for the weights (using calloc(), so use free() to delete the array) and fill
         *         it with the appropriate values, according to the current settings */
        virtual double* allocWeights(bool* weightsOK=NULL, QFRawDataRecord* record=NULL, int run=-100, int data_start=-1, int data_end=-1);

        virtual QString getEvaluationResultID(int currentIndex);
        using QFUsesResultsByIndexEvaluation::getEvaluationResultID;
    protected:
        
        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);


        int currentIndex;
        int currentModel;
        int currentWeights;

        /** \brief returns default values for a parameter */
        virtual bool getParameterDefault(QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QFUsesResultsEvaluation::FitParameterDefault& defaultValue);


};

#endif // QFFCSMAXENTEVALUATIONITEM_H
