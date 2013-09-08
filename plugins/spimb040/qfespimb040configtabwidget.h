#ifndef QFESPIMB040CONFIGTABWIDGET_H
#define QFESPIMB040CONFIGTABWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QPointer>

namespace Ui {
    class QFESPIMB040ConfigTabWidget;
}

class QFESPIMB040ConfigTabWidget : public QWidget{
        Q_OBJECT
    public:
        explicit QFESPIMB040ConfigTabWidget(QWidget *parent = 0);
        ~QFESPIMB040ConfigTabWidget();
        void storeSettings(QSettings &settings, QString prefix) const;
        void loadSettings(QSettings &settings, QString prefix);





        static QFESPIMB040ConfigTabWidget* getInstance() {
            return instance;
        }
        static QString getStylesheet();
        static QString getStyle();

    signals:
        void styleChanged(const QString& style, const QString& stylesheet);
    protected slots:
        void on_cmbStylesheet_currentIndexChanged(int index);
        void on_cmbStyle_currentIndexChanged(const QString &text);
        void on_cmbStyle_highlighted(const QString &text);
        void reloadStylesheets(bool forSure=false);
        void on_btnAutosetConfigs_clicked();
    private:
        Ui::QFESPIMB040ConfigTabWidget *ui;
        static QPointer<QFESPIMB040ConfigTabWidget> instance;
};

#endif // QFESPIMB040CONFIGTABWIDGET_H
