#ifndef QFFITALGORITHMPARAMETERSTORAGE_H
#define QFFITALGORITHMPARAMETERSTORAGE_H
#include "lib_imexport.h"
#include "qfproject.h"
#include <QHash>
#include <QString>
#include <QPointer>
#include "qffitalgorithm.h"
#include "qffitalgorithmmanager.h"
#include <QXmlStreamWriter>
#include <QDomElement>

class QFLIB_EXPORT QFFitAlgorithmParameterStorage
{
    public:
        explicit QFFitAlgorithmParameterStorage(QFProject* project=NULL);
        virtual ~QFFitAlgorithmParameterStorage();

        /*! \brief save all parameters of a given QFFitAlgorithm to the internal fit algorithm parameter store algorithm_parameterstore
        */
        virtual void storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm);
        /*! \brief load all parameters of a given QFFitAlgorithm from the internal fit algorithm parameter store algorithm_parameterstore
        */
        virtual void restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm) const;
        virtual void clearQFFitAlgorithmParameters();

        void configFitAlgorithm(QFFitAlgorithm* algorithm);
        static void configFitAlgorithm(QFFitAlgorithm* algorithm, QFFitAlgorithmParameterStorage* data);


    protected:

        /*! \brief parameter store for fit algorithm parameters

        This map effectively has two strings as key. the first is the fit algorithm ID and the second the parameter name
        The value stored for each key is a QVariant.
        */
        QHash<QString, QHash<QString, QVariant> > algorithm_parameterstore;


        QPointer<QFProject> algparam_project;

        virtual void writeQFFitAlgorithmParameters(QXmlStreamWriter& w, const QString &tagName=QString("algorithm")) const;
        virtual void readQFFitAlgorithmParameters(QDomElement elt, const QString &tagName=QString("algorithm"));
        virtual void setQFFitAlgorithmParametersProject(QFProject* project);
};

#endif // QFFITALGORITHMPARAMETERSTORAGE_H
