#ifndef SIMANNEALCONFIG_H
#define SIMANNEALCONFIG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include "jkdoubleedit.h"

/*! \brief This class represents a configuration dialog for the Levmar Levenberg-Marquardt Library
    \ingroup qf3fitalgp_sa

 */
class SimannealConfigDialog : public QDialog {
        Q_OBJECT
    public:
        /** \brief class constructor */
        SimannealConfigDialog(QWidget *parent = 0);

        /** \brief set the fmax value before displaying the dialog and also (optionally) its increment */
        void setFMax(double value=1, double inc=0.1);
        /** \brief returns the current fmax value */
        inline double getFMax() { return neditFMax->value(); };

        /** \brief set the T0 value before displaying the dialog and also (optionally) its increment */
        void setT0(double value=0.001, double inc=0.0001);
        /** \brief returns the current T0 value */
        inline double getT0() { return neditT0->value(); };

        /** \brief set the T0 value before displaying the dialog and also (optionally) its increment */
        void setRT(double value=0.5, double inc=0.05);
        /** \brief returns the current T0 value */
        inline double getRT() { return neditRT->value(); };

        /** \brief set the c value before displaying the dialog and also (optionally) its increment */
        void setC(double value=2, double inc=0.5);
        /** \brief returns the current c value */
        inline double getC() { return neditC->value(); };

        /** \brief set the NS value before displaying the dialog and also (optionally) its increment */
        void setNS(int value=20, int inc=1);
        /** \brief returns the current NS value */
        inline int getNS() { return spinNS->value(); };

        /** \brief set the NT value before displaying the dialog and also (optionally) its increment */
        void setNT(int value=50, int inc=1);
        /** \brief returns the current NT value */
        inline int getNT() { return spinNT->value(); };

        /** \brief set the Nepsilon value before displaying the dialog and also (optionally) its increment */
        void setNEpsilon(int value=4, int inc=1);
        /** \brief returns the current Nepsilon value */
        inline int getNEpsilon() { return spinNEpsilon->value(); };

        /** \brief set the MaxIterations value before displaying the dialog and also (optionally) its increment */
        void setMaxIterations(int value=50000, int inc=1000);
        /** \brief returns the current MaxIterations value */
        inline int getMaxIterations() { return spinMaxIterations->value(); };


    protected:
        QSpinBox* spinMaxIterations;
        JKDoubleEdit* neditFMax;
        JKDoubleEdit* neditT0;
        JKDoubleEdit* neditRT;
        JKDoubleEdit* neditC;
        QSpinBox*  spinNS;
        QSpinBox*  spinNT;
        QSpinBox*  spinNEpsilon;

        QDialogButtonBox* buttons;
};

#endif //SIMANNEALCONFIG_H
