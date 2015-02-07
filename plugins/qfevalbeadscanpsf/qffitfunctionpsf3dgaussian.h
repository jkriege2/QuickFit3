#ifndef QFFITFUNCTIONPSF3DGAUSSIAN_H
#define QFFITFUNCTIONPSF3DGAUSSIAN_H

#include "qffitfunctionnd.h"
#include "qffitfunctionbase.h"

/*! \brief QFFitFunction3D class for fit function: 3D PSF: gaussian, 1/e^2-halfwidth
    \ingroup qf3rdrdp_eval_beadscanpsf

*/
class QFFitFunctionPSF3DGaussian : public QFFitFunction3D
{
    public:
        QFFitFunctionPSF3DGaussian();

        /** \brief return a name for the model */
        virtual QString name() const { return QObject::tr("3D PSF: gaussian, 1/e^2-halfwidth"); }
        /** \brief return a short unique model ID string */
        virtual QString id() const { return "beadscan_psf3d_gaussian"; }
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QString("Point Spread Function (PSF)"); };

        virtual double evaluate(double x, double y, double z, const double* parameters) const ;
};

#endif // QFFITFUNCTIONPSF3DGAUSSIAN_H
