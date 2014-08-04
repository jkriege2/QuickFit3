#ifndef QFWIZARD_H
#define QFWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include "lib_imexport.h"



class QFLIB_EXPORT QFWizard : public QWizard
{
        Q_OBJECT
    public:
        explicit QFWizard(QWidget *parent = 0);

    signals:

    public slots:

};



class QFLIB_EXPORT QFWizardPage : public QWizardPage
{
        Q_OBJECT
    public:
        explicit QFWizardPage(QWidget *parent = 0);
        explicit QFWizardPage(const QString& title, QWidget *parent = 0);

    signals:

    public slots:

};

class QFLIB_EXPORT QFTextWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFTextWizardPage(const QString &title, const QString& text, QWidget *parent = 0);

    signals:

    public slots:

};


#endif // QFWIZARD_H
