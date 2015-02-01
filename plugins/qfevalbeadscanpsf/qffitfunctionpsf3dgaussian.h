#ifndef QFFITFUNCTIONPSF3DGAUSSIAN_H
#define QFFITFUNCTIONPSF3DGAUSSIAN_H

#include "qffitfunctionnd.h"
#include "qffitfunctionbase.h"

class QFFitFunctionPSF3DGaussian : public QFFitFunction3D
{
    public:
        QFFitFunctionPSF3DGaussian();

        /** \brief return a name for the model */
        virtual QString name() const { return QObject::tr("3D PSF: gaussian, 1/e^2-halfwidth"); }
        /** \brief return a short unique model ID string */
        virtual QString id() const { return "psf3d_gaussian"; }

        virtual double evaluate(double x, double y, double z, const double* parameters) const ;
};

#endif // QFFITFUNCTIONPSF3DGAUSSIAN_H
