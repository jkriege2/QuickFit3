#ifndef QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
#define QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H

#include <QStringList>
#include <QString>
#include <QtPlugin>

/*! \brief interface that allows to automatically config for a global fit
    \ingroup qf3rdrdp_fcs

    The data is returned as a GlobalFitConfig struct, which defines which models to use and which parameters to link in them:
    The member globalParams lists all global parameters and contains a list of a stringlist.
    The first inner list enumerates the files/models
    and the second gives the parameters that should be linked within that file:
      <center>\c [globalParam, File] -> {parameters to link in the given file}</center>

    So, if the first global parameter should link the parameter "D" and the second parameter "C" in every of three models,
    the contents of globalParams should be:
    \code
      globalParams[0] = { {D}, {D}, {D} };
      globalParams[1] = { {C}, {C}, {C} };
    \endcode
    If in addition, the first parameter should also link D1 and D2 in every model, the we have:
    \code
      globalParams[0] = { {D1,D2}, {D1,D2}, {D1,D2} };
      globalParams[1] = { {C}, {C}, {C} };
    \endcode

*/
class QFFitFunctionConfigForGlobalFitInterface
{
    public:
        virtual ~QFFitFunctionConfigForGlobalFitInterface() {}

        struct GlobalFitConfig {
            QString groupLabel;
            QString menuEntryLabel;
            QStringList models;
            QStringList roles;
            QList<QStringList> fixes;
            QList<QList<QStringList> > globalParams;
        };

        virtual int getGlobalFitConfigCount() const=0;
        virtual GlobalFitConfig getGlobalFitConfig(int i) const=0;

};

Q_DECLARE_INTERFACE( QFFitFunctionConfigForGlobalFitInterface,
                     "www.dkfz.de.b040.quickfit3.plugin.QFFitFunctionConfigForGlobalFitInterface/1.0")



#endif // QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
