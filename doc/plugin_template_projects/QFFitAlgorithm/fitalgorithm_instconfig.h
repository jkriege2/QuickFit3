#ifndef HEADER_CONFIG_H
#define HEADER_CONFIG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "jkdoubleedit.h"

/*! \brief This class represents a configuration dialog for ...
    \ingroup doxygen_GROUPNAME
 */
class QFFitAlgorithmInstConfigDialog : public QDialog {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFFitAlgorithmInstConfigDialog(QWidget *parent = 0);

        /** \brief set the gradDelta value before displaying the dialog and also (optionally) its increment */
        void setParamName(double value=10, double inc=1);
        /** \brief returns the current gradDelta value */
        inline double getParamName() { return spinParam->value(); };

    protected:
        QDoubleSpinBox* spinParam;
        QDialogButtonBox* buttons;
};

#endif //HEADER_CONFIG_H
