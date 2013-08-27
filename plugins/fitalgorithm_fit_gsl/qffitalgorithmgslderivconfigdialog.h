#ifndef QFFitAlgorithmGSLDerivativeBaseCONFIGDIALOG_H
#define QFFitAlgorithmGSLDerivativeBaseCONFIGDIALOG_H

#include <QDialog>
#include "qffitalgorithm.h"

namespace Ui {
    class QFFitAlgorithmGSLDerivativeBaseConfigDialog;
}

class QFFitAlgorithmGSLDerivativeBaseConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFitAlgorithmGSLDerivativeBaseConfigDialog(QWidget *parent = 0);
        ~QFFitAlgorithmGSLDerivativeBaseConfigDialog();
        
        /** \brief initialize the dialog from the parameters in the given algorithm */
        void init(const QFFitAlgorithm* algorithm);
        /** \brief store the parameters to the given algorithm */
        void storeParams(QFFitAlgorithm* algorithm) const;
    private:
        Ui::QFFitAlgorithmGSLDerivativeBaseConfigDialog *ui;
};

#endif // QFFitAlgorithmGSLDerivativeBaseCONFIGDIALOG_H
