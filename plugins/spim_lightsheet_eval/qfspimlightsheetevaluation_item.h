#ifndef QFSPIMLIGHTSHEETEVALUATIONITEM_H
#define QFSPIMLIGHTSHEETEVALUATIONITEM_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"

/*! \brief evaluation item class 
    \ingroup qf3rdrdp_GROUPNAME


    
    
*/
class QFSPIMLightsheetEvaluationItem : public QFEvaluationItem {
        Q_OBJECT
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFSPIMLightsheetEvaluationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFSPIMLightsheetEvaluationItem();

        enum Models {
            Gaussian=0
        };

        enum Orientation{
            fitRows,
            fitColumns
        };

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("spim_lightsheet_eval"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("SPIM: Lightsheet Analysis"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/spim_lightsheet_eval/qfspimlightsheetevaluation_small.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("performs the analysis of lightsheet scans for SPIM microscopy"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/spim_lightsheet_eval/qfspimlightsheetevaluation.png"); };
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Lightsheet Analysis"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);

        
        /** \breif return \c true if an evaluation has been performed for the given record \a r1 */
        bool hasEvaluation(QFRawDataRecord* r1, int stack, int channel=-1) const;


        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        QString getEvaluationResultID(int stack, int channel=-1) const;


        void doEvaluation(QFRawDataRecord *record, int stack, int stack_pos, int channel, double deltaX, double deltaY, Orientation orientation=fitRows, Models model=Gaussian) const;
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

#endif // QFSPIMLIGHTSHEETEVALUATIONITEM_H
