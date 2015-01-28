/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef FILTERS_H
#define FILTERS_H

#include <QDialog>
#include <QString>
#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QHBoxLayout>
#include <QSettings>
#include <QMessageBox>
#include "libwid_imexport.h"
#include "qenhancedcombobox.h"
#include "qfmanyfilessettings.h"
#include <QFormLayout>

/*! \brief description of a filter
    \ingroup qf3lib_widgets


 */
struct QFWIDLIB_EXPORT FilterDescription {
    QString name;
    QString manufacturer;
    QString type;
    bool isValid;

    FilterDescription();
    bool operator<(const FilterDescription& other) const {
        return name<other.name;
    }

};



namespace Ui {
    class QF3FilterEditor;
}

/*! \brief editor for objective data
    \ingroup qf3lib_widgets


 */
class QFWIDLIB_EXPORT QF3FilterEditor : public QDialog {
        Q_OBJECT

    public:
        explicit QF3FilterEditor(const FilterDescription& filter, QWidget *parent = 0);
        ~QF3FilterEditor();
        void init(const FilterDescription& filter);
        FilterDescription getData();
    private:
        Ui::QF3FilterEditor *ui;
};

/*! \brief This class is used to notify all QF3FilterCombobox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup qf3lib_widgets
*/
class QF3FilterComboboxNotifier: public QObject {
        Q_OBJECT
    public:
        QF3FilterComboboxNotifier(QObject* parent=NULL): QObject(parent) {};
    public slots:
        void emitUpdate() { emit doUpdate(); };
    signals:
        void doUpdate();
};
/*! \brief combobox that allows to select an objective
    \ingroup qf3lib_widgets

    This widget uses two settings files (global + local) as a datasource.
    All changes are stored in the local file, which also superseeds settings
    from the global file.

    If only the global objective file is given, we try to write to this one!
 */
class QFWIDLIB_EXPORT QF3FilterCombobox : public QWidget {
    Q_OBJECT
public:
    QF3FilterCombobox(QWidget *parent = 0);
    ~QF3FilterCombobox();

    /** \brief return the current objective */
    FilterDescription filter() const;

    /** brief return the i-th objective description */
    FilterDescription getFilterDescription(int i) const;

    bool filterExists(QString name) const;

    void setFilterINI(QString globalfilters, QString localfilters=QString(""));

    void loadSettings(QSettings& settings, QString property);
    void saveSettings(QSettings& settings, QString property);
    void loadSettings(QFManyFilesSettings& settings, QString property);
    void saveSettings(QFManyFilesSettings& settings, QString property);

    static QList<FilterDescription> getFilterList(QString globalfilters, QString localfilters=QString(""));
    static FilterDescription getFilter(QString filter, QString globalfilters, QString localfilters=QString(""));
    static QIcon getFilterIcon(const FilterDescription& description);
    static QIcon getFilterIcon(QString filter, QString globalfilters, QString localfilters=QString(""));
public slots:
    void loadFilters();
    void storeFilters();
    void addFilter();
    void deleteFilter();
    void editFilter();
    void setReadOnly(bool readonly);
    void setCurrentFilter(const QString& name);

protected slots:
    void currentFilterChanged(int idx);

protected:
    QList<FilterDescription> filters;
    QEnhancedComboBox* cmbFilters;
    QToolButton* btnAddFilter;
    QToolButton* btnEditFilter;
    QToolButton* btnDeleteFilter;
    QLabel* labFilterDescription;
    static QF3FilterComboboxNotifier* m_notifier;
    QString globalfilters;
    QString localfilters;
    QHBoxLayout* hbl;

};



class QFWIDLIB_EXPORT QF3DualViewWidget : public QWidget {
        Q_OBJECT
    public:
        QF3DualViewWidget(QWidget *parent = 0);
        ~QF3DualViewWidget();

        enum Orientation {
            Horizontal =0,
            Vertical=1
        };

        void loadSettings(QSettings& settings, QString property);
        void saveSettings(QSettings& settings, QString property);
        void loadSettings(QFManyFilesSettings& settings, QString property);
        void saveSettings(QFManyFilesSettings& settings, QString property);
        void setFilterINI(QString globalfilters, QString localfilters=QString(""));

        QF3FilterCombobox* getSplitterCombobox() const;
        QF3FilterCombobox* getShortCombobox() const;
        QF3FilterCombobox* getLongCombobox() const;

        FilterDescription filterSplitter() const;
        FilterDescription filterShort() const;
        FilterDescription filterLong() const;
        Orientation orientation() const;
    public slots:
        void setReadOnly(bool ro);
    protected:
        QComboBox* cmbDirection;
        QF3FilterCombobox* fltSplitter;
        QF3FilterCombobox* fltShort;
        QF3FilterCombobox* fltLong;
        QGridLayout* lay;
};


#endif // FILTERS_H
