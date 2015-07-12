/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef OBJECTIVES_H
#define OBJECTIVES_H

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
#include <QPointer>
#include "qfmanyfilessettings.h"
/*! \brief description of an objective
    \ingroup qf3lib_widgets


 */
struct QFWIDLIB_EXPORT ObjectiveDescription {
    QString name;
    QString manufacturer;
    double magnification;
    double NA;
    ObjectiveDescription();
    bool operator<(const ObjectiveDescription& other) const {
        return name<other.name;
    }
};



namespace Ui {
    class QF3ObjectiveEditor;
}

/*! \brief editor for objective data
    \ingroup qf3lib_widgets


 */
class QFWIDLIB_EXPORT QF3ObjectiveEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QF3ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent = 0);
    ~QF3ObjectiveEditor();
    void init(const ObjectiveDescription& objective);
    ObjectiveDescription getData();
private:
    Ui::QF3ObjectiveEditor *ui;
};

/*! \brief This class is used to notify all QF3ObjectiveCombobox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup qf3lib_widgets
*/
class QF3ObjectiveComboboxNotifier: public QObject {
        Q_OBJECT
    public:
    QF3ObjectiveComboboxNotifier(QObject* parent=NULL): QObject(parent) {};
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
class QFWIDLIB_EXPORT QF3ObjectiveCombobox : public QWidget {
    Q_OBJECT
public:
    QF3ObjectiveCombobox(QWidget *parent = 0);
    ~QF3ObjectiveCombobox();

    /** \brief return the current objective */
    ObjectiveDescription objective();


    /** brief return the i-th objective description */
    ObjectiveDescription getObjectiveDescription(int i);

    bool objectiveExists(QString name);

    void setObjectivesINI(QString globalobjectives, QString localobjectives=QString(""));
    void loadSettings(QSettings& settings, QString property);
    void loadSettings(QFManyFilesSettings& settings, QString property);
    void saveSettings(QSettings& settings, QString property);
    void saveSettings(QFManyFilesSettings& settings, QString property);
public slots:
    void loadObjectives();
    void storeObjectives();
    void addObjective();
    void deleteObjective();
    void editObjective();
    void setReadOnly(bool readonly);
signals:
    void objectiveChanged();
protected slots:
    void currentObjectiveChanged(int idx);

protected:
    QList<ObjectiveDescription> objectives;
    QEnhancedComboBox* cmbObjective;
    QToolButton* btnAddObjective;
    QToolButton* btnEditObjective;
    QToolButton* btnDeleteObjective;
    QLabel* labObjectiveDescription;
    static QF3ObjectiveComboboxNotifier* m_notifier;
    QString globalobjectives;
    QString localobjectives;
    QHBoxLayout* hbl;

};

#endif // OBJECTIVES_H
