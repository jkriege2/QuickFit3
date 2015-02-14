/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-02-02 16:43:09 +0100 (Mo, 02 Feb 2015) $  (revision $Rev: 3775 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFEVALCOLOCALIZATIONITEM_H
#define QFEVALCOLOCALIZATIONITEM_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"

/*! \brief evaluation item class 
    \ingroup qf3evalp_eval_colocalization


    
    
*/
class QFEValColocalizationItem : public QFEvaluationItem {
        Q_OBJECT
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFEValColocalizationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFEValColocalizationItem();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("eval_colocalization"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Colocalization Analysis"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/eval_colocalization/eval_colocalization_small.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return getTypeName(); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/eval_colocalization/eval_colocalization.png"); };
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Evaluation"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);

        
        /** \breif return \c true if an evaluation has been performed for the given record \a r1 */
        bool hasEvaluation(QFRawDataRecord* r1, int stack, int ch1, int ch2);


        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        QString getEvaluationResultID(int stack, int ch1, int ch2);

		/** \brief perform the evaluation for the given record. The dlgEvaluationProgress can be used to display the evaluation progress and to check whether the user has pressed Cancel
		 *
		 *  \param record the record to do the evaluation for
		 *  \param dlgEvaluationProgress a QProgressDialog
		 */
        void doEvaluation(QFRawDataRecord* record, int frame=-1, int channel1=0, int channel2=1,  QProgressDialog* dlgEvaluationProgress=NULL);

    protected:
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(const QFRawDataRecord* record) const;
        
        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);

};

#endif // QFEVALCOLOCALIZATIONITEM_H
