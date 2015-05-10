#ifndef QFETCSPCIMPORTERLIFETIMEVIEW_H
#define QFETCSPCIMPORTERLIFETIMEVIEW_H

#include <QDialog>
#include "qfplotter.h"
#include <QVector>
#include <QList>
#include "stdint.h"


namespace Ui {
    class QFETCSPCImporterLifetimeView;
}

class QFETCSPCImporterLifetimeView : public QDialog
{
        Q_OBJECT

    public:
        explicit QFETCSPCImporterLifetimeView(QWidget *parent = 0);
        ~QFETCSPCImporterLifetimeView();

        void initArrialtimes(const QList<QVector<int16_t> >& arrivaltimes, double mtOffset=0, double mtFactor=1);

    signals:
        void setLifetimeFilter(int ch, double min_ns, double max_ns);
    public slots:
        void setDescription(const QString& text);
        void updatePlot();
        void showHelp();
        void on_btnSelectRange_toggled(bool enabled);
        void userRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers);
        void sendRange();
    private:
        Ui::QFETCSPCImporterLifetimeView *ui;
        QList<QVector<int16_t> > arrivaltimes;
        double mtOffset;
        double mtFactor;
        QList<JKQTPxyLineGraph*> graphs;
        QMap<QAction*, int> actsCopyRange;
        JKQTPverticalRange* plteRange;
};

#endif // QFETCSPCIMPORTERLIFETIMEVIEW_H
