#ifndef HEADER_A1_CONFIG_H
#define HEADER_A1_CONFIG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "qffitalgorithm.h"

namespace Ui {
    class QFFitAlgorithmInst_A1ConfigDialog;
}


/*! \brief This class represents a configuration dialog for ...
    \ingroup doxygen_GROUPNAME
 */
class QFFitAlgorithmInst_A1ConfigDialog : public QDialog {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFFitAlgorithmInst_A1ConfigDialog(QWidget *parent = 0);
		~QFFitAlgorithmInst_A1ConfigDialog();

        /** \brief initialize the dialog from the parameters in the given algorithm */
        void init(const QFFitAlgorithm* algorithm);
        /** \brief store the parameters to the given algorithm */
        void storeParams(QFFitAlgorithm* algorithm) const;
    private:
        Ui::QFFitAlgorithmInst_A1ConfigDialog *ui;
};

#endif //HEADER_A1_CONFIG_H
