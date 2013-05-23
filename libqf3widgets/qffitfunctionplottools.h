#ifndef QFFITFUNCTIONPLOTTOOLS_H
#define QFFITFUNCTIONPLOTTOOLS_H

#include "libwid_imexport.h"
#include <QVector>
#include <QString>
#include "jkqtpelements.h"
#include "qffitfunction.h"

struct JKQTPxQFFitFunctionLineGraphFunctionParams; // forward

class QFWIDLIB_EXPORT JKQTPxQFFitFunctionLineGraph: public JKQTPxFunctionLineGraph {
        Q_OBJECT
    public:

        /** \brief class constructor */
        JKQTPxQFFitFunctionLineGraph(JKQtBasePlotter* parent=NULL);
        ~JKQTPxQFFitFunctionLineGraph();



        GetMacro(QFFitFunction*, fitFunction);
        GetSetMacro(bool, ownsFunction);
        GetSetMacro(double, scaleX);
        GetSetMacro(double, offsetX);

        void set_fitFunction(QFFitFunction* fitFunction, bool ownsFunction=true);
    protected:
        QFFitFunction* fitFunction;
        bool ownsFunction;

        double scaleX;
        double offsetX;

        JKQTPxQFFitFunctionLineGraphFunctionParams* intParam;

        virtual void collectParameters();
        //virtual void createPlotData( bool collectParams=true);


};

#endif // QFFITFUNCTIONPLOTTOOLS_H
