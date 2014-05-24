#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtGui>
#include "ui_optionsdialog.h"
#include "../lib/programoptions.h"
#include "qfpluginoptionsdialog.h"

/*! \brief options dialog wrapper class for QuickFit
    \ingroup qf3app
*/
class OptionsDialog : public QDialog, public Ui::OptionsDlg {
        Q_OBJECT
    public:
        /** Default constructor */
        OptionsDialog(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~OptionsDialog();
        /** \brief display options dialog and store to the given ProgramOptions object */
        void open(ProgramOptions* options);

        void setPlugins(const QList<QFPluginOptionsDialogInterface*>& plugins);
    protected slots:
        void on_cmbStylesheet_currentIndexChanged( const QString & text );
        void on_cmbStylesheet_highlighted( const QString & text );
        void on_cmbStyle_currentIndexChanged( const QString & text );
        void on_cmbStyle_highlighted( const QString & text );
        void on_btnHelp_clicked();
        void on_listWidget_currentRowChanged(int currentRow );
        void on_spinMath_valueChanged(int value);

    private:
        ProgramOptions* m_options;
        QList<QFPluginOptionsWidget*> m_plugins;
};

#endif // OPTIONSDIALOG_H
