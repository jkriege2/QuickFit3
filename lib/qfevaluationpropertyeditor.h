/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFEVALUATIONPROPERTYEDITOR_H
#define QFEVALUATIONPROPERTYEDITOR_H

#include "lib_imexport.h"
#include <QtGui>
#include <QSortFilterProxyModel>
#include "qfproject.h"
#include "qfevaluationitem.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"
#include "qfhtmldelegate.h"
#include "qfevaluationresultsmodel.h"
#include "qenhancedtableview.h"
#include "qvisiblehandlesplitter.h"
#include "qfenhancedlineedit.h"
#include "qfstyledbutton.h"
#include "qfcompleterfromfile.h"
#include "qfproject.h"
#include "qfhistogramservice.h"

class QFEvaluationPropertyEditor; // forward


/*! \brief This QSortFilterProxyModel implements a filter proxy model which  to filter out records
          that are not usable together with a given QFMultiListEvaluation.
    \ingroup qf3lib_project

    This class depends on the first Qt::UserRole to supply the ID of the record to be filtered. It calls
    QFEvaluation::isApplicable() to find out whether a raw data record is to be displayed.

    If  the given evaluation also supports item selection this implments the feature using the Qt::CheckStateRole role.


 */
class QFLIB_EXPORT QFEvaluationRawDataModelProxy : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        QFEvaluationRawDataModelProxy(QObject *parent = 0);

        virtual ~QFEvaluationRawDataModelProxy() {};

        /** \brief set the evaluation against which the records shall be checked */
        void setEvaluation(QFEvaluationItem* eval);
        void setEditor(QFEvaluationPropertyEditor* editor);

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;



    protected slots:
        /** \brief called when the selection in the currently set evaluation changed */
        void selectionChanged(QList<QPointer<QFRawDataRecord> > selectedRecords);
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        /** \brief QFMultiListEvaluation used to check whether a raw data record is applicable */
        QPointer<QFEvaluationItem> eval;
        QPointer<QFEvaluationPropertyEditor> editor;

};


class QFEvaluationPropertyEditorPrivate;
/*! \brief this class implements the parent widget for an evaluation window

    \note This class does not implement a lot of functionality, so it is mainly used, when your QFEvaluationEditor(s)
          do all the work (including which data to work on ...), i.e. if you derived you evaluation item directly from
          QFEvaluationItem. If you derived from a specialised child of QFEvaluationItem, other property editor classes
          may better suit the features of your evaluation (e.g. QFMultiListEvaluationPropertyEditor). Which property
          editor object is created may be set by the class, as it is created in the QFEvaluationItem::createPropertyEditor()
          method of you evaluation item.

 */
class QFLIB_EXPORT QFEvaluationPropertyEditor : public QWidget {
        Q_OBJECT
    protected:

    public:
        /** Default constructor */
        QFEvaluationPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFEvaluationItem* current=NULL, int id=0, QWidget* parent=NULL, Qt::WindowFlags f = Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        /** Default destructor */
        virtual ~QFEvaluationPropertyEditor();
        /** \brief set a ProgramOptions object to use for storing application settings */
        void setSettings(ProgramOptions* settings);
        /** \brief switches the filtering of the files list on/off */
        void setFilesListFilteres(bool filtered);
        bool isFilesListFiltered() const;

        /** \brief get the current record */
        QFEvaluationItem* getCurrent() const;


        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief return widget ID */
        int getID();
        /** \brief set the current record */
        void setCurrent(QFEvaluationItem* c);
        /** \brief deselect the currently highlighted record and choose another one from the available records! */
        void deselectCurrent();
        /** \brief returns the menubar of this widget */
        QMenuBar* getMenuBar() const;
        /** \brief chages the visiblity of the menubar */
        void setMenuBarVisible(bool visible);
        /** \brief add a new menu to te menubar that is visible for the specified editor only, if editor>=0 or always, if editor<0 */
        QMenu* addMenu(const QString &title, int editor=-1);
        /** \brief add a new menu (if it does not yet exist) to te menubar that is visible for the specified editor only, if editor>=0 or always, if editor<0 */
        QMenu* addOrFindMenu(const QString &title, int editor=-1);

        /** \brief tells the editor to switch the visibility of the specified menu according to the set editor (see addMenu() ), but does not add the menu to the main menu bar */
        void registerMenu(QMenu* menu, int editor=-1);
        /** \brief returns a pointer to the help menu of this editor */
        QMenu* getHelpMenu() const;

        QPointer<QFEvaluationEditor> getEditor() const;
        /** \brief send the given command string to all edtor tabs */
        void sendEditorCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant());
    private:
        QPointer<QFEvaluationPropertyEditorPrivate> p;
    protected:
        /** \brief points to the record currently displayed */
        QPointer<QFEvaluationItem> current;
        /** \brief model showing all available raw data records */
        QFProjectRawDataModel* rdrModel;
        /** \brief proxy model to filter rdrModel */
        QFEvaluationRawDataModelProxy* rdrProxy;
        /** \brief datamodel used for display of evaluation results */
        QFEvaluationResultsModel* resultsModel;
        /** \brief read the settings from ProgramOptions set by setSettings() */
        virtual void readSettings();
        void closeEvent( QCloseEvent* event);
        void resizeEvent(QResizeEvent* event);
        virtual bool event(QEvent * ev);

        friend class QFEvaluationPropertyEditorPrivate;

};

#endif // QFEVALUATIONPROPERTYEDITOR_H
