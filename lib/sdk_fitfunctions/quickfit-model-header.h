
/*!
    \file quickfit-model-header.h
    \ingroup quickfit3_models

 */

#ifndef QUICKFITMODELHEADER_H
#define QUICKFITMODELHEADER_H

#include <windows.h>
#include "quickfit-model-tools.h"



#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/** \brief writes the name of the model into buffer
 *  \ingroup quickfit3_model3dadiffusion
 *
 *    \param buffer the buffer the result will be stored in. Quickfit will allocate at least 1 kByte for this
 *                  buffer, when the function is called.
 *    \param type   this tells the function what information to return (see below).
 *
 * These values are applicable to \c type:
 *   - <b>type=0</b> (default): getModelName() returns the long name of the model
 *   - <b>type=1</b>: getModelName() returns a short name (ID) of the model, without whitespaces, which may
 *                    be used as part of filenames, INI sections ...
 *   - <b>type=3</b>: getModelName() will return the filename of the description HTML file reltive to the model's library location!
 * .
 *
 */
void DLL_EXPORT getModelName(char* buffer, int type);



/** \brief returns the number of fit parameters in this model
 *  \ingroup quickfit3_model3dadiffusion
 */
unsigned int DLL_EXPORT getParameterCount();



/** \brief return the evaluated model function at the given position \a t and with the paramaters \a parameters.
 *  \ingroup quickfit3_model3dadiffusion
 *
 * \param t the time step where to evaluate the model
 * \param parameters an array with the model parameters to use during evaluation
 *
 * \note If a parameter transformation is implemented, this function works on the <b>transformed parameters</b>!
 */
double DLL_EXPORT evaluate(double t, double* parameters);


#ifdef __cplusplus
}
#endif

#endif // QUICKFITMODELHEADER_H


