#ifndef QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
#define QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H



#include <QtPlugin>

/*! \brief interface that allows to automatically config for a global fit
    \ingroup qf3rdrdp_fcs

    The data is returned as a GlobalFitConfig struct, which defines which models to use and which parameters to link in them:
    The member globalParams lists all global parameters. and contains a list of a stringlist. The first inner list enumerates the files
    and the second gives the parameters that should be linked within that file:
      <center>\c [globalParam, File] -> {parameters to link in the given file}</center>


*/
class QFFitFunctionConfigForGlobalFitInterface
{
    public:
        virtual ~QFFitFunctionConfigForGlobalFitInterface() {}

        struct GlobalFitConfig {
            QString groupLabel;
            QString menuEntryLabel;
            QStringList models;
            QList<QList<QStringList> > globalParams;
        };

        virtual int getGlobalFitConfigCount() const=0;
        virtual GlobalFitConfig getGlobalFitConfig(int i) const=0;

};

Q_DECLARE_INTERFACE( QFFitFunctionConfigForGlobalFitInterface,
                     "www.dkfz.de.b040.quickfit3.plugin.QFFitFunctionConfigForGlobalFitInterface/1.0")



#endif // QFFITFUNCTIONCONFIGFORGLOBALFITINTERFACE_H
