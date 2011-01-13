#ifndef QFFCSFITEVALUATION_H
#define QFFCSFITEVALUATION_H

#include "qfevaluationitem.h"
#include "../fcs/qfrdrfcsdata.h"
#include "qfevaluationitemfactory.h"

/*! \brief evaluation item class for FCS least square fits
    \ingroup qf3evalp_fcsfit
*/
class QFFCSFitEvaluation : public QFEvaluationItem {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSFitEvaluation(QFProject* parent);
        /** Default destructor */
        virtual ~QFFCSFitEvaluation();
    public:
        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("fcs_fit"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS Fit"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/projecttree_emptyevaluation.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("Least squares fitting for fluorescence correlation spectroscopy data"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/projecttree_emptyevaluatione.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; };
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) { return QString("Fit"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, int i=0, QWidget* parent=NULL);
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);

    protected:
        /** \brief write object contents into XML file */
        virtual void intWriteXML(QXmlStreamWriter& w);
        /** \brief read in data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * This method may be used to read additional data (like algorithm configuration ...) from the project file
         * \note You will have to explicitly call the intReadData() metod of the parent class of your newly implemented
         * evaluation class in order to ensure that all data is correctly read from the project file!  Do so by calling
         * \code QFEvaluationItem::intReadData(e); \endcode at the start of your method !!!
         */
        virtual void intReadData(QDomElement* e);
    private:
};

#endif // QFFCSFITEVALUATION_H
