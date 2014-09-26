/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFSELECTRDRDIALOG_H
#define QFSELECTRDRDIALOG_H

#include <QDialog>
#include "libwid_imexport.h"
#include "qfrawdatarecord.h"
#include "qfproject.h"
#include <QList>
#include <QPointer>
#include "qfmatchrdrfunctor.h"


namespace Ui {
    class QFSelectRDRDialog;
}


/*! \brief a dialog that allows to select a raw data record which matches given criteria
    \ingroup qf3lib_widgets

    This class displays a dialog with a list of all RDRs in a given project that match a given filter (defined as
    QFSelectRDRDialogMatchFunctor derived functor). The functor object is given as a pointer (to allow subclassing)
    and may thus either be private or not. A private functor is destroyed by the destructor, a non-private one is not
    and the class user will have to take care of that.

    After the dialog closes the currently selected RDRs may be returned using getSelectedRDRs() and getSelectedRDR(). The dialog can
    be configure to allow multi- or single-selection using setAllowMultiSelect(). In it's default form the dialog also allows
    to select to create a new record. To deactivate this use setAllowCreateNew(false). It is also possible to display a description
    text (setDescription() ) above the widgets and to add additional widgets below the sleection list using addWidget().

    if no project is given in the contructor (i.e. \c project=NULL ) the current project is used.
  */
class QFWIDLIB_EXPORT QFSelectRDRDialog : public QDialog
{
        Q_OBJECT
        
    public:
        /** \brief class constructor
         *  \note the given functor pointer is NOT private, i.e. the functor is not destroyed together with this object */
        explicit QFSelectRDRDialog(QFMatchRDRFunctor* matchFunctor, QWidget *parent = 0);
        /** \brief class constructor. the user may select whether the functor is private or not*/
        explicit QFSelectRDRDialog(QFMatchRDRFunctor* matchFunctor, bool functorPrivate, QWidget *parent = 0);
        /** \brief class constructor which displays a dialog that shows all records (i.e. default functor) */
        explicit QFSelectRDRDialog(QWidget *parent = 0);
        virtual ~QFSelectRDRDialog();

        void addWidget(const QString& label, QWidget* widget);
        void addWidget(QLabel* label, QWidget* widget);
        void setAllowCreateNew(bool allowNew);
        void setAllowMultiSelect(bool allowMulti);
        void selectRDR(QFRawDataRecord* rdr);
        void setDescription(const QString& text);
        void setProject(QFProject* project);

        QList<QPointer<QFRawDataRecord> > getAllRDRs() const;
        QList<QPointer<QFRawDataRecord> > getSelectedRDRs() const;
        QPointer<QFRawDataRecord> getSelectedRDR() const;
        bool doCreateNew() const;

        void setOnlineHelp(const QString& helpfile);

        QList<QFRawDataRecord *> getSelectedRDRsp() const;
    public slots:
        /** \brief re-read the RDRs from the given project */
        void updateRdrList();

    protected slots:
        void on_btnAll_clicked();
        void on_btnNone_clicked();
        void on_lineEditFilter_textChanged(const QString & text );
        void on_btnHelp_clicked();
    protected:
        Ui::QFSelectRDRDialog *ui;
        QFMatchRDRFunctor* matchFunctor;
        bool functorPrivate;
        QPointer<QFProject> project;
        QList<QPointer<QFRawDataRecord> > rdrList;
        QString helpfile;

};



#endif // QFSELECTRDRDIALOG_H
