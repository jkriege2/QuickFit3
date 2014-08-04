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



QFComboBoxWizardPage::QFComboBoxWizardPage(const QString &title, QWidget *parent):
    QFFormWizardPage(title, parent)
{
    label = new QLabel(this);
    combo=new QFEnhancedComboBox(this);
    addRow(label, combo);
}

void QFComboBoxWizardPage::setLabel(const QString &label)
{
    this->label->setText(label);
}

void QFComboBoxWizardPage::setItems(const QStringList &items, const QList<QIcon> &icons)
{
    combo->clear();
    for (int i=0; i<items.size(); i++) {
        combo->addItem(icons.value(i, QIcon()), items.value(i));
    }
}

void QFComboBoxWizardPage::setData(const QVariantList &data, int role)
{
    for (int i=0; i<data.size(); i++) {
        if (i<combo->count()) combo->setItemData(i, data.value(i, QVariant()), role);
    }
}

int QFComboBoxWizardPage::currentItem() const
{
    return combo->currentIndex();
}

void QFComboBoxWizardPage::setCurrentItem(int idx)
{
    combo->setCurrentIndex(idx);
}

QVariant QFComboBoxWizardPage::currentData(int role) const
{
    return combo->currentData(role);
}

void QFComboBoxWizardPage::setCurrentData(const QVariant &data, int role)
{
    combo->setCurrentData(data, role);
}


QFFormWizardPage::QFFormWizardPage(QWidget *parent):
    QFWizardPage(parent)
{
    m_layout = new QFormLayout;
    setLayout(m_layout);
}

QFFormWizardPage::QFFormWizardPage(const QString &title, QWidget *parent):
    QFWizardPage(title, parent)
{
    m_layout = new QFormLayout;
    setLayout(m_layout);
}

void QFFormWizardPage::addRow(QWidget *label, QWidget *field)
{
    m_layout->addRow(label, field);
}

void QFFormWizardPage::addRow(QWidget *label, QLayout *field)
{
    m_layout->addRow(label, field);
}

void QFFormWizardPage::addRow(const QString &labelText, QWidget *field)
{
    m_layout->addRow(labelText, field);
}

void QFFormWizardPage::addRow(const QString &labelText, QLayout *field)
{
    m_layout->addRow(labelText, field);
}

void QFFormWizardPage::addRow(QWidget *widget)
{
    m_layout->addRow(widget);
}

void QFFormWizardPage::addRow(QLayout *layout)
{
    m_layout->addRow(layout);
}
