#ifndef QCONFIGCOMBOBOXWIDGET_H
#define QCONFIGCOMBOBOXWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QAction>
#include <QToolButton>
#include <QSettings>
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QConfigComboboxWidget : public QWidget
{
        Q_OBJECT
    public:
        typedef bool (*SaveWidgetFunction)(const QWidget*, QSettings&, const QString& prefix);
        typedef bool (*LoadWidgetFunction)(QWidget*, const QSettings&, const QString& prefix);
        typedef bool (*EqualsWidgetFunction)(const QWidget*, const QSettings&, const QString& prefix);
        typedef bool (*RegisterWidgetFunction)(QWidget*, QConfigComboboxWidget* config);
        typedef bool (*UnregisterWidgetFunction)(QWidget*, QConfigComboboxWidget* config);

        struct WidgetFunctions {
            SaveWidgetFunction* saveWidget;
            LoadWidgetFunction* loadWidget;
            EqualsWidgetFunction* equalsWidget;
            RegisterWidgetFunction* registerWidget;
            UnregisterWidgetFunction* unregisterWidget;
        };

        explicit QConfigComboboxWidget(QString filename=QString("configcombobox.ini"), QWidget *parent = 0);
        explicit QConfigComboboxWidget(QWidget *parent = 0);
        virtual ~QConfigComboboxWidget();

        void registerWidget(const QString& id, QWidget* widget);
        void unregisterWidget(const QString& id);
        void unregisterWidget(QWidget* widget);
        void unregisterWidgets();
        int currentConfig() const;
        QString currentConfigName() const;
        QString currentFilename() const;
    signals:
        void configChanged();
    public slots:
        void setCurrentConfig(const QString& name);
        void setCurrentConfig(int index);
        void setFilename(const QString& filename);

        void widgetContentsChanged();
    protected slots:
        void saveCurrentConfig();
        void deleteCurrentConfig();
        void currentConfigChanged(int i);

        void saveWidgets(const QString& id);
        void loadWidgets(const QString& id);
        void deleteConfig(const QString& id);

        void createWidgets();
        void loadAllConfig();
    protected:
        QComboBox* combobox;
        QAction* actSave;
        QAction* actDelete;

        QString filename;
        QSettings* settings;
        QMap<QString, QWidget*> widgets;
        QList<WidgetFunctions> widgetFunctions;
        
};

#endif // QCONFIGCOMBOBOXWIDGET_H
