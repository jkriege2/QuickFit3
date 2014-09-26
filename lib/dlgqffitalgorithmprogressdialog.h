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

#ifndef DLGQFFITALGORITHMPROGRESSDIALOG_H
#define DLGQFFITALGORITHMPROGRESSDIALOG_H

#include <QDialog>
#include "lib_imexport.h"
#include "qffitalgorithm.h"


namespace Ui {
    class QFFitAlgorithmProgressDlg; // forward
};


/*! \brief a progress dialog, which also supports a super-progress
    \ingroup qf3lib_fitting

    This dialog implements the methods, as in the interface QFFitAlgorithmReporter. In addition this dialog allows to display
    a super message and progress which will be displayed above the default status message (see images below) using
      - reportSuperStatus()
      - setSuperProgressMax()
      - setSuperProgress()
      - incSuperProgress()
    .

    \note you can NOT directly use this dialog as a QFFitAlgorithmReporter as this won't work over thread boundries. Instead
          use dlgQFFitAlgorithmProgressDialogReporter, by first instaciating dlgQFFitAlgorithmProgressDialog and then
          dlgQFFitAlgorithmProgressDialogReporter. Finally you may use dlgQFFitAlgorithmProgressDialogReporter as a QFFitAlgorithmReporter
          savely even over thread boundaries!

    \image html dlg_fitalgorithmprogress.png
    \image latex dlg_fitalgorithmprogress.png

    After initializing the dialog, you may display it using display() and then call done() to close it again.
    Note that this dialog implements a delay for displaying. So if you set this delay (in milliseconds) using setDisplayDelay()
    to a positive value, display() will return immediately and the dialog will be displayed with the given delay.
    Initially the delay is 500 milliseconds. If done() is called before the delay is over the dialog won't be displayed at all.

    Using setAllowCancel() you may en-/disable the cancel Button.


*/
class QFLIB_EXPORT dlgQFFitAlgorithmProgressDialog : public QDialog
{
        Q_OBJECT
    public:
        dlgQFFitAlgorithmProgressDialog(QWidget* parent=NULL);
        virtual ~dlgQFFitAlgorithmProgressDialog();


        /** \brief return \c true, if the user has canceled the fit procedure */
        virtual bool isCanceled() { return m_canceled; }

        /** \brief set the display delay in milliseconds */
        void setDisplayDelay(double delay_in_msecs) { m_displayDelay=delay_in_msecs; }
        /** \brief enable or disable the cancel button. */
        void setAllowCancel(bool enable);

    public slots:
        /** \brief use this to display the widget */
        void display();
        /** \brief call when the dialog should be hidden */
        void done();


        /** \brief report a status message */
        virtual void reportStatus(const QString& message);
        /** \brief set the progress maximum to \a max */
        virtual void setProgressMax(int max=100);
        /** \brief set the current progress to the given value */
        virtual void setProgress(int value);
        /** \brief set the current progress to 100% */
        virtual void setProgressFull();
        /** \brief increment the current progress */
        virtual void incProgress(int increment=1);

        /** \brief report a super status message */
        void reportSuperStatus(const QString& message);
        /** \brief set the super progress maximum to \a max */
        void setSuperProgressMax(int max=100);
        /** \brief set the current super progress to the given value */
        void setSuperProgress(int value);
        /** \brief increment the current super progress */
        void incSuperProgress(int increment=1);


    private:
        bool m_canceled;
        bool m_display;
        double m_displayDelay;
        Ui::QFFitAlgorithmProgressDlg* ui;
    private slots:
        void on_btnCancel_clicked();
        void doDisplay();
};




/*! \brief this class translates QFFitAlgorithmReporter calls over thread borders to a dlgQFFitAlgorithmProgressDialog
    \ingroup qf3lib_fitting
*/
class QFLIB_EXPORT dlgQFFitAlgorithmProgressDialogReporter : public QObject, public QFFitAlgorithmReporter
{
        Q_OBJECT
    public:
        dlgQFFitAlgorithmProgressDialogReporter(dlgQFFitAlgorithmProgressDialog* parent);

        /** \brief report a status message */
        virtual void reportStatus(const QString& message);
        /** \brief set the progress maximum to \a max */
        virtual void setProgressMax(int max=100);
        /** \brief set the current progress to the given value */
        virtual void setProgress(int value);
        /** \brief set the current progress to 100% */
        virtual void setProgressFull();
        /** \brief increment the current progress */
        virtual void incProgress(int increment=1);
        /** \brief return \c true, if the user has canceled the fit procedure */
        virtual bool isCanceled();
    signals:
        void s_reportStatus(const QString& message);
        void s_setProgressMax(int max);
        void s_setProgress(int value);
        void s_setProgressFull();
        void s_incProgress(int increment);
    private:
        dlgQFFitAlgorithmProgressDialog* m_parent;
};

#endif // DLGQFFITALGORITHMPROGRESSDIALOG_H
