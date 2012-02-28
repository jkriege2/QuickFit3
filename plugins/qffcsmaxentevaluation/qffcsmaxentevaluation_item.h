#ifndef QFFCSMAXENTEVALUATIONITEM_H
#define QFFCSMAXENTEVALUATIONITEM_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"

/*! \brief evaluation item class 
    \ingroup qf3rdrdp_GROUPNAME


    
    
*/
class QFFCSMaxEntEvaluationItem : public QFEvaluationItem {
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

        
        /** \breif return \c true if an evaluation has been performed for the given record \a r1 */
        bool hasEvaluation(QFRawDataRecord* r1);


        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        inline QString getEvaluationResultID() {
            return QString("%1_%2").arg(getType()).arg(getID());
        }


    protected:
        
        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);

};

#endif // QFFCSMAXENTEVALUATIONITEM_H
