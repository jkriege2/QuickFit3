#ifndef QFWIZARD_H
#define QFWIZARD_H

#include <QWizard>
#include <QWizardPage>
#include "lib_imexport.h"
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include "qfenhancedcombobox.h"
#include <QCheckBox>


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


        virtual void initializePage();
        virtual bool validatePage();

        void setUserPreviousPage(QWizardPage* page);
        void setUserOnValidateArgument(QWizardPage* page);
    signals:
        void onInitialize(QWizardPage* page);
        void onInitialize(QWizardPage* page, QWizardPage* userPreviousPage);
        void onValidate(QWizardPage* page);
        void onValidate(QWizardPage* page, QWizardPage* userPage);



    public slots:
    protected:
        QWizardPage* m_userLast;
        QWizardPage* m_userValidateArgument;

};

class QFLIB_EXPORT QFFormWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFFormWizardPage(QWidget *parent = 0);
        explicit QFFormWizardPage(const QString& title, QWidget *parent = 0);

        QFormLayout* getFormLayout() const {
            return m_layout;
        }

        void addRow(QWidget *label, QWidget *field);
        void addRow(QWidget *label, QLayout *field);
        void addRow(const QString &labelText, QWidget *field);
        void addRow(const QString &labelText, QLayout *field);
        void addRow(QWidget *widget);
        void addRow(QLayout *layout);

    signals:

    public slots:
    protected:
        QFormLayout* m_layout;

};


class QFLIB_EXPORT QFEnableableFormWizardPage : public QFFormWizardPage
{
        Q_OBJECT
    public:
        explicit QFEnableableFormWizardPage(QWidget *parent = 0);
        explicit QFEnableableFormWizardPage(const QString& title, QWidget *parent = 0);

        void setEnableCheckbox(const QString& name, bool enabled);
        void setEnableable(bool enableable);
        void setNextPageIfDisabled(QWizardPage* nextIfDisabled);

        virtual int nextId() const;
    signals:

    public slots:
    protected:
        QCheckBox* chkMain;
        QWidget* widMain;
        QWizardPage* nextIfDisabled;

    private:
        void createWidgets();

};


class QFLIB_EXPORT QFTextWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFTextWizardPage(const QString &title, const QString& text, QWidget *parent = 0);

    signals:

    public slots:

};

class QFLIB_EXPORT QFComboBoxWizardPage : public QFEnableableFormWizardPage
{
        Q_OBJECT
    public:
        explicit QFComboBoxWizardPage(const QString &title, QWidget *parent = 0);
        void setLabel(const QString& label);
        void setItems(const QStringList& items, const QList<QIcon>& icons=QList<QIcon>());
        void setData(const QVariantList& data, int role=Qt::UserRole);

        int currentItem() const;
        void setCurrentItem(int idx);

        QVariant currentData(int role=Qt::UserRole) const;
        void setCurrentData(const QVariant& data, int role=Qt::UserRole);

    signals:

    public slots:
    protected:
        QLabel* label;
        QFEnhancedComboBox* combo;

};


class QFLIB_EXPORT QFCheckboxListWizardPage : public QFEnableableFormWizardPage
{
        Q_OBJECT
    public:
        explicit QFCheckboxListWizardPage(const QString &title, QWidget *parent = 0);
        void setItems(const QStringList& items);
        void clear();
        void addItem(const QString& item);
        void setChecked(const QList<bool>& checked);
        void setChecked(int id, bool checked);

        bool getChecked(int id) const;
        QList<bool> getChecked() const;
        int count() const;
        void setNextPageIfAllDisabled(QWizardPage* nextIfDisabled);

        virtual int nextId() const;

    signals:

    public slots:
    protected:
        QList<QCheckBox*> boxes;
        QWizardPage* nextIfAllDisabled;

};

#endif // QFWIZARD_H
