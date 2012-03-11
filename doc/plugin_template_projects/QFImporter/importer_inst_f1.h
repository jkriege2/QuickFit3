#ifndef HEADER_F1_H
#define HEADER_F1_H
#include "qfpluginimporters.h"
#include "qfimporter.h"




/*! \brief QFImporter class for fit function
    \ingroup doxygen_GROUPNAME

*/
class QFFitAlgorithmInst_F1: public QFImporter {
    public:
        QFFitAlgorithmInst_F1();
        virtual ~QFFitAlgorithmInst_F1() {}
        /*! \copydoc QFFitFunction::filter()   */
        virtual QString filter() const ;
        /*! \copydoc QFFitFunction::formatName()   */
        virtual QString formatName() const ;
};

#endif // HEADER_F1_H
