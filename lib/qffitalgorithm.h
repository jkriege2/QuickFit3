#ifndef QFFITALGORITHM_H
#define QFFITALGORITHM_H

#include "qffitfunction.h"

/*! \brief describes a virtual base class fitting algorithms that are applied to QFFitFunction objects.
    \ingroup qf3lib_fitting

*/
class QFFitAlgorithm
{
    public:
        /** \brief class destructor */
        virtual ~QFFitAlgorithm() {}

        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;
        /** \brief return a HTML file to be displayed as algorithm help */
        virtual QString helpFile() const { return QString(""); };

    protected:
    private:
};

#endif // QFFITALGORITHM_H
