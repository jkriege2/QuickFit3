#ifndef QFFITALGORITHM_H
#define QFFITALGORITHM_H

#include "qffitfunction.h"

/*! \brief describes a virtual base class fitting algorithms that are applied to QFFitFunction objects.
    \ingroup qf3lib_fitting

*/
class QFFitAlgorithm
{
    public:
        QFFitAlgorithm() {}
        virtual ~QFFitAlgorithm() {}
    protected:
    private:
};

#endif // QFFITALGORITHM_H
