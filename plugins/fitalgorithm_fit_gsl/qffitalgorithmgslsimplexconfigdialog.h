#ifndef QFFITALGORITHMGSLSIMPLEXCONFIGDIALOG_H
#define QFFITALGORITHMGSLSIMPLEXCONFIGDIALOG_H

#include <QDialog>
#include "qffitalgorithm.h"

namespace Ui {
    class QFFitAlgorithmGSLSimplexConfigDialog;
}

class QFFitAlgorithmGSLSimplexConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFitAlgorithmGSLSimplexConfigDialog(QWidget *parent = 0);
        ~QFFitAlgorithmGSLSimplexConfigDialog();
        
        /** \brief initialize the dialog from the parameters in the given algorithm */
        void init(const QFFitAlgorithm* algorithm);
        /** \brief store the parameters to the given algorithm */
        void storeParams(QFFitAlgorithm* algorithm) const;
    private:
        Ui::QFFitAlgorithmGSLSimplexConfigDialog *ui;
};

#endif // QFFITALGORITHMGSLSIMPLEXCONFIGDIALOG_H
