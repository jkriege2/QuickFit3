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
        QAction* getHelpAction() const { return actHelp; }
        QAction* getSelectAction() const { return actSelectModel; }

    signals:
        
    public slots:
        void updateFitFunctions(const QString& filter=QString(""));
        void updateFitFunctions(const QStringList& availableFF);
        void setCurrentFitFunction(const QString& id);
        void showHelpCurrent();
        void selectModelDialog();
    protected slots:
        void reloadFitFunctions();
    protected:
        QString m_filter;
        QStringList m_availableFuncs;
        QAction* actHelp;
        QAction* actSelectModel;

};

#endif // QFFITFUNCTIONCOMBOBOX_H
