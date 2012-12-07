#ifndef QFHISTOGRAMSERVICE_H
#define QFHISTOGRAMSERVICE_H





#include "lib_imexport.h"
#include <QString>
#include <QSettings>
#include <QToolBar>
#include <QMenu>



/*! \brief wrapper class that allows plugins to access basic logging services
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFHistogramService {
    public:
        QFHistogramService();
        virtual ~QFHistogramService();

        struct Histogram {
            QVector<double> data;
            QString name;
        };


        virtual QWidget* getCreateView(const QString& name, const QString& title)=0;
        virtual void clearView(const QString& name)=0;
        virtual void addHistogramToView(const QString& name, const Histogram& histogram)=0;

        static QFHistogramService* getInstance() {
            return instance;
        }

    private:
        static QFHistogramService* instance;
};



#endif // QFHISTOGRAMSERVICE_H
