/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include <QProgressBar>
#include <QTextEdit>
#include <QRadioButton>

class QFWizardPage; // forward

class QFLIB_EXPORT QFWizardValidateFunctor {
    public:
        inline virtual ~QFWizardValidateFunctor() {}
        virtual bool isValid(QFWizardPage* page)=0;
};

class QFLIB_EXPORT QFWizardIsCompleteFunctor {
    public:
        inline virtual ~QFWizardIsCompleteFunctor() {}
        virtual bool isComplete(const QFWizardPage* page) const=0;
};

class QFLIB_EXPORT QFWizardNextPageFunctor {
    public:
        inline virtual ~QFWizardNextPageFunctor() {}
        virtual int nextID(const QFWizardPage* page) const=0;
};

class QFLIB_EXPORT QFWizardFixedNextPageFunctor: public QFWizardNextPageFunctor {
    public:
        explicit inline QFWizardFixedNextPageFunctor(int id) { this->id=id; }
        inline virtual int nextID(const QFWizardPage* page) const {
            Q_UNUSED(page)
            return id;
        }
    protected:
        int id;
};

class QFLIB_EXPORT QFWizard : public QWizard
{
        Q_OBJECT
    public:
        explicit QFWizard(QWidget *parent = 0, const QString& config_prefix=QString());
        explicit QFWizard(QSize windowSize, QWidget *parent = 0, const QString& config_prefix=QString());

    signals:

    public slots:

    protected:
        virtual void closeEvent(QCloseEvent * e);
        virtual void done(int result);
        QString configPrefix;

};


class QFLIB_EXPORT QFWizardPage : public QWizardPage
{
        Q_OBJECT
    public:
        explicit QFWizardPage(QWidget *parent = 0);
        explicit QFWizardPage(const QString& title, QWidget *parent = 0);

        virtual ~QFWizardPage();


        virtual void initializePage();
        virtual bool validatePage();
        virtual bool isComplete() const;
        virtual int nextId() const;

        void setUserPreviousPage(QWizardPage* page);
        void setUserOnValidatePage(QWizardPage* page);
        void setUserPreviousArgument(void* page);
        void setUserOnValidateArgument(void* page);

        void setExternalValidate(bool enabled=true);
        void setExternalIsValid(bool valid=true);
        /** \brief sets a QFWizardValidateFunctor that is called on validatePage().
         *
         * \note you have to free the functor by hand, if you don't set setFreeFunctors(true) */
        void setValidateFunctor(QFWizardValidateFunctor* validateFunctor);
        /** \brief sets a QFWizardIsCompleteFunctor that is called on isComplete().
         *
         * \note you have to free the functor by hand, if you don't set setFreeFunctors(true) */
        void setIsCompleteFunctor(QFWizardIsCompleteFunctor* isCompleteFunctor);
        /** \brief sets a QFWizardNextPageFunctor that is called on nextId().
         *
         * \note you have to free the functor by hand, if you don't set setFreeFunctors(true) */
        void setNextIDFunctor(QFWizardNextPageFunctor* nextIDFunctor);
        void setFreeFunctors(bool enabled=true);
        void setNextID(int nextid);
    signals:
        void onInitialize(QWizardPage* page);
        void onInitialize(QWizardPage* page, QWizardPage* userPreviousPage);
        void onInitializeA(QWizardPage* page, void* userArg);
        void onValidate(QWizardPage* page);
        void onValidate(QWizardPage* page, QWizardPage* userPage);
        void onValidateA(QWizardPage* page, void* userArg);
    public slots:
    protected:
        QWizardPage* m_userLast;
        QWizardPage* m_userValidatePage;
        void* m_userLastArg;
        void* m_userValidateArg;
        bool m_externalvalidate;
        bool m_isvalid;
        bool m_freeFunctors;
        QFWizardValidateFunctor* m_validator;
        QFWizardIsCompleteFunctor* m_iscomplete;
        QFWizardNextPageFunctor* m_nextID;

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
        QVBoxLayout* getMainLayout() const {
            return m_mainlay;
        }
        QWidget* getFormLayoutWidget() const {
            return widMain;
        }

        void addRow(QWidget *label, QWidget *field);
        void addRow(QWidget *label, QLayout *field);
        void addRow(const QString &labelText, QWidget *field);
        void addRow(const QString &labelText, QLayout *field);
        void addRow(QWidget *widget);
        void addRow(QLayout *layout);
        QLabel* addRow(const QString& text);
        QLabel* addRow(const QString &labelText, const QString& text);
        QLabel* addRow(QWidget *label, const QString& text);
        void setRowEnabled(int row, bool enabled=true);
        void setWidgetEnabled(QWidget* wid, bool enabled=true);
        void setRowVisible(int row, bool enabled=true);
        void setWidgetVisible(QWidget* wid, bool enabled=true);

    signals:

    public slots:
    protected:
        QFormLayout* m_layout;
        QVBoxLayout* m_mainlay;
        QWidget* widMain;
    private:
        void createWidgets();

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
        void addItem(const QString& text, const QVariant &data=QVariant());
        void addItem(const QIcon& icon, const QString& text, const QVariant &data=QVariant());

        int currentItem() const;
        void setCurrentItem(int idx);

        QVariant currentData(int role=Qt::UserRole) const;
        void setCurrentData(const QVariant& data, int role=Qt::UserRole);

    signals:
        void currentIndexChanged(int index);
    protected slots:
        void indexChanged(int index);
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
        void addItem(const QString& item, bool checked=false);
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

class QFLIB_EXPORT QFRadioButtonListWizardPage : public QFEnableableFormWizardPage
{
        Q_OBJECT
    public:
        explicit QFRadioButtonListWizardPage(const QString &title, QWidget *parent = 0);
        void setItems(const QStringList& items);
        void clear();
        void addItem(const QString& item, bool checked=false);
        void setChecked(int id);
        void setChecked(int id, bool checked);
        void setEnabled(int id, bool enabled);

        bool isChecked(int id) const;
        int isChecked() const;
        int count() const;

    signals:

    public slots:
    protected:
        QList<QRadioButton*> boxes;

};


class QFLIB_EXPORT QFProcessingWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFProcessingWizardPage(const QString &title, const QString& text, QWidget *parent = 0);

        bool wasCanceled() const;
        void setAllowCancel(bool enabled);

        virtual bool isComplete() const;
    signals:
        void startProcessing();
        void cancelProcessing();
    public slots:
        void setProcessingFinished(bool status=true);
        void setProgress(int value);
        void setSubProgress(int value);
        void setSubProgress(int id, int value);
        void setRange(int minimum, int maximum);
        void setSubRange(int minimum, int maximum);
        void setSubRange(int id, int minimum, int maximum);
        void setMessage(const QString& text);
        void setSubMessage(const QString& text);
        void setSubMessage(int id, const QString& text);
        void cancelClicked();
        void startClicked();
        void incProgress(int inc=1);
        void incSubProgress(int inc=1);
        void addErrorMessage(const QString& message);
    protected:

        void addSubProgressWidgets();
        bool m_canceled;
        bool m_done;
        bool m_allowCancel;
        QVBoxLayout *mainLay;
        QProgressBar* progress;
        QList<QProgressBar*> progressSub;
        QLabel* labMessage;
        QList<QLabel*> labSubMessage;
        QPushButton* btnStart;
        QTextEdit* edtErrors;

};

#endif // QFWIZARD_H
