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

/*! \brief description of a filter
    \ingroup qf3lib_widgets


 */
struct QFWIDLIB_EXPORT FilterDescription {
    QString name;
    QString manufacturer;
    QString type;
    bool isValid;

    FilterDescription();
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
    FilterDescription filter();

    /** brief return the i-th objective description */
    FilterDescription getFilterDescription(int i);

    bool filterExists(QString name);

    void setFilterINI(QString globalfilters, QString localfilters=QString(""));

    void loadSettings(QSettings& settings, QString property);
    void saveSettings(QSettings& settings, QString property);

    static QList<FilterDescription> getFilterList(QString globalfilters, QString localfilters=QString(""));
    static QIcon getFilterIcon(const FilterDescription& description);
public slots:
    void loadFilters();
    void storeFilters();
    void addFilter();
    void deleteFilter();
    void editFilter();
    void setReadOnly(bool readonly);

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

#endif // FILTERS_H
