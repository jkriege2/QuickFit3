#ifndef JKMATH_H_INCLUDED
#define JKMATH_H_INCLUDED


/*! \brief the sign function
    \ingroup tools_math_func
    \return -1 for \c x<0, 1 for \c x==0 and 1 else
*/
inline double sgn(double x) {
    return (x<0)?-1:((x==0)?0:1);
}

/*! \brief the square of the arguemnt \f$ x^2 \f$
    \ingroup tools_math_func
*/
inline double sqr(double x) {
    return x*x;
}

#endif // JKMATH_H_INCLUDED
