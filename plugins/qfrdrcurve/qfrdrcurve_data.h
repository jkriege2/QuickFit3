/*
Copyright (c) 2015
	
	last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFRDRCURVEDATA_H
#define QFRDRCURVEDATA_H

#include <QList>
#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "qfrawdatarecord.h"
#include "qfrdrcurve_dataeditor.h"
#include "qfrawdatarecordfactory.h"
#include "qfrdrcurvesinterface.h"
#include "qftablemodel.h"


/*! \brief QFRawDataRecord implementation
    \ingroup qf3rdrdp_curves

*/
class QFRDRCurveData : public QFRawDataRecord , public QFRDRUserCurvesInterface, public QFRDRUserCurvesPropertiesEditInterface {
        Q_OBJECT
        Q_INTERFACES(QFRDRUserCurvesInterface QFRDRUserCurvesPropertiesEditInterface)
    public:
        /** Default constructor */
        QFRDRCurveData(QFProject* parent);
        /** Default destructor */
        virtual ~QFRDRCurveData();

        /** \brief return type (short type string) */
        virtual QString getType() const { return "curves"; }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("XY-Curves"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/curves/qfrdrcurve_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return getTypeName(); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/curves/qfrdrcurve.png"); }
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; }
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) {
            if (i==0) return tr("XY-Curves editor");
            return QString("");
        }
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i=0, QWidget* parent=NULL) {
            if (i==0) return new QFRDRCurveDataEditor(services, propEditor, parent);
            return NULL;
        }
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() const {
            QStringList sl;
            return sl;
        }
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() const { return tr(""); }
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() const { return tr(""); }


        virtual int userCurvesGetCount() const;
        virtual QString userCurvesGetName(int index) const;
        virtual QVector<double> userCurvesGetX(int index) const;
        virtual QVector<double> userCurvesGetXError(int index) const;
        virtual QVector<double> userCurvesGetY(int index) const;
        virtual QVector<double> userCurvesGetYError(int index) const;
        virtual bool userCurvesGetLogX(int index) const;
        virtual bool userCurvesGetLogY(int index) const;
        virtual QFRDRCurvesInterface::CurveType userCurvesGetType(int index) const;
        virtual QString userCurvesGetXLabel(int index) const;
        virtual QString userCurvesGetYLabel(int index) const;


        virtual void userCurvesSetName(int index, const QString& name) ;
        virtual void userCurvesSetLogX(int index, bool l) ;
        virtual void userCurvesSetLogY(int index, bool l) ;
        virtual void userCurvesSetType(int index, QFRDRCurvesInterface::CurveType type) ;
        virtual void userCurvesSetXLabel(int index, const QString& label) ;
        virtual void userCurvesSetYLabel(int index, const QString& label) ;

    protected:
        /** \brief write the contents of the object to a XML file */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <b>and</b> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);


        /** \brief this struct represents one curve. We use a QFTableModel (a mighty data-table) to repüresent the raw data */
        struct CurveData {
            CurveData() {
                logX=logY=false;
                table=NULL;
                type= QFRDRCurvesInterface::ctPoints;
                xcolumn=xerrorcolumn=ycolumn=yerrorcolumn=-1;
            }

            QPointer<QFTableModel> table;
            bool logX;
            bool logY;
            QString name;
            QString xlabel;
            QString ylabel;
            CurveType type;
            int xcolumn;
            int xerrorcolumn;
            int ycolumn;
            int yerrorcolumn;
        };
        QList<CurveData> curves;
        void clearCurves();

    private:

};


#endif // QFRDRCURVEDATA_H
