#ifndef LEVMARCONFIG_H
#define LEVMARCONFIG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include "qfdoubleedit.h"

/*! \brief This class represents a configuration dialog for the Levmar Levenberg-Marquardt Library
    \ingroup qf3fitalgp_levmar
 */
class LevmarConfigDialog : public QDialog {
        Q_OBJECT
    public:
        /** \brief class constructor */
        LevmarConfigDialog(QWidget *parent = 0);

        /** \brief set the gradDelta value before displaying the dialog and also (optionally) its increment */
        void setGradDelta(double value=1e-6, double inc=1e-6);
        /** \brief returns the current gradDelta value */
        inline double getGradDelta() { return neditGradDelta->value(); };

        /** \brief set the mu value before displaying the dialog and also (optionally) its increment */
        void setMu(double value=1e-3, double inc=1e-3);
        /** \brief returns the current mu value */
        inline double getMu() { return neditMu->value(); };

        /** \brief set the epsilon1 value before displaying the dialog and also (optionally) its increment */
        void setEpsilon1(double value=1e-15, double inc=1e-15);
        /** \brief returns the current epsilon1 value */
        inline double getEpsilon1() { return neditEpsilon1->value(); };

        /** \brief set the epsilon2 value before displaying the dialog and also (optionally) its increment */
        void setEpsilon2(double value=1e-15, double inc=1e-15);
        /** \brief returns the current epsilon2 value */
        inline double getEpsilon2() { return neditEpsilon2->value(); };

        /** \brief set the epsilon3 value before displaying the dialog and also (optionally) its increment */
        void setEpsilon3(double value=1e-15, double inc=1e-15);
        /** \brief returns the current epsilon3 value */
        inline double getEpsilon3() { return neditEpsilon3->value(); };

        /** \brief set the MaxIterations value before displaying the dialog and also (optionally) its increment */
        void setMaxIterations(int value=3000, int inc=1000);
        /** \brief returns the current MaxIterations value */
        inline int getMaxIterations() { return spinMaxIterations->value(); };


        /** \brief set the MaxIterations value before displaying the dialog and also (optionally) its increment */
        void setNumGrad(bool value=false);
        /** \brief returns the current MaxIterations value */
        inline int getNumGrad() { return chkNumGrad->isChecked(); };

    protected:
        QSpinBox* spinMaxIterations;
        QFDoubleEdit* neditGradDelta;
        QFDoubleEdit* neditMu;
        QFDoubleEdit* neditEpsilon1;
        QFDoubleEdit* neditEpsilon2;
        QFDoubleEdit* neditEpsilon3;
        QCheckBox* chkNumGrad;
        QDialogButtonBox* buttons;
};

#endif //LEVMARCONFIG_H
