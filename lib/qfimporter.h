#ifndef QFIMPORTER_H
#define QFIMPORTER_H

#include <QString>
#include "lib_imexport.h"

/*! \brief interface (file) reader reader classes (importer)
    \ingroup qf3importerplugins

*/
class QFLIB_EXPORT QFImporter {
    public:
        virtual ~QFImporter() {}

        /** \brief return a description of the last error that occured */
        QString lastError() const {
            return err;
        }
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;



    protected:

        void setLastError(QString err) {
            this->err=err;
        }

    private:
        QString err;
};


#endif // QFIMPORTER_H
