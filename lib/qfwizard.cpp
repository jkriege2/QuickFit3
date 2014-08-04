#include "qfwizard.h"
#include <QtGui>

QFWizard::QFWizard(QWidget *parent) :
    QWizard(parent)
{
}


QFWizardPage::QFWizardPage(QWidget *parent):
    QWizardPage(parent)
{

}

QFWizardPage::QFWizardPage(const QString &title, QWidget *parent):
    QWizardPage(parent)
{
    setTitle(title);
}


QFTextWizardPage::QFTextWizardPage(const QString& title, const QString &text, QWidget *parent):
    QFWizardPage(title, parent)
{
    QLabel *label = new QLabel(text, this);
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

