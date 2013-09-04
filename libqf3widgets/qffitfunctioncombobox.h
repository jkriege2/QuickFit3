#ifndef QFFITFUNCTIONCOMBOBOX_H
#define QFFITFUNCTIONCOMBOBOX_H

#include <QComboBox>
#include "qffitfunction.h"
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QFFitFunctionComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFFitFunctionComboBox(QWidget *parent = 0);
        QString currentFitFunctionID() const;
        QFFitFunction* createCurrentInstance(QObject *parent=NULL) const;
    signals:
        
    public slots:
        void updateFitFunctions(const QString& filter=QString(""));
        void setCurrentFitFunction(const QString& id);
    protected slots:
        void reloadFitFunctions();
    protected:
        QString m_filter;

};

#endif // QFFITFUNCTIONCOMBOBOX_H
