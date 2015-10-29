#ifndef QFSimplePlotWidget_H
#define QFSimplePlotWidget_H

#include "qfdialog.h"
#include "qfplotter.h"
#include "qffitfunction.h"

namespace Ui {
    class QFSimplePlotWidget;
}

/*! \brief This class is a simple plotting widget with some niceties
    \ingroup qf3rdrdp_imaging_fcs



*/
class QFWIDLIB_EXPORT QFSimplePlotWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit QFSimplePlotWidget(QWidget *parent = 0);
        ~QFSimplePlotWidget();

        QFPlotter *getPlotter() const;
        void startAddingPlots();
        void endAddingPlots();
        void setLog(bool logX, bool logY);
        void setXRange(double rmin, double rmax);
        void setYRange(double rmin, double rmax);
        void setAxisLabel(const QString &labX, const QString &labY);
        void setGrid(bool enabled, const QString &style=QString());
        void setXAxisLabel(const QString& label);
        void setYAxisLabel(const QString& label);
        void addPlot(const QVector<double>& X, const QVector<double>& Y, const QString& spec=QString(), const QString& label=QString());
        void addFunctionPlot(QFFitFunction *function, const QString& spec=QString(), const QString& label=QString(), const QVector<double>& parameters=QVector<double>());
        void addBarPlot(const QVector<double>& X, const QVector<double>& Y, const QString& spec=QString(), const QString& label=QString());
        void addErrorPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& Xerrors, const QVector<double>& Yerrors, const QString& spec=QString(), const QString& label=QString(), JKQTPerrorPlotstyle errorstyle=JKQTPerrorBars);
        void addScatterPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& S, const QString& spec=QString(), const QString& label=QString());
        void addColorScatterPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& S, const QString& spec=QString(), const QString& label=QString(), const QString& colorscale=QString("matlab"));

    public slots:
        void clearPlots();
        void fitRanges();
        void fitXRange();
        void fitYRange();
        void setAxesEditable(bool editable);
        void setLogX(bool log);
        void setLogY(bool log);

    protected slots:
        void showHelp();
        void updatePlot();

        void on_chkLogX_toggled(bool checked);
        void on_chkLogY_toggled(bool checked);
        void on_chkGrid_toggled(bool checked);

        void emitLogX(bool log);
        void emitLogY(bool log);

    signals:
        void logXtoggled(bool log);
        void logYtoggled(bool log);

    private:
        Ui::QFSimplePlotWidget *ui;
};


class QFWIDLIB_EXPORT QFSimplePlotDialog : public QFDialog
{
        Q_OBJECT
    public:
        QFSimplePlotDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
        virtual ~QFSimplePlotDialog();
        inline QFSimplePlotWidget* getSimplePlot() const {
            return plot;
        }
    public slots:
        void setAxesEditable(bool editable) {
            plot->setAxesEditable(editable);
        }

        void setLogX(bool log) {
            plot->setLogX(log);
        }
        void setLogY(bool log) {
            plot->setLogY(log);
        }
    protected:
        QFSimplePlotWidget* plot;
};

#endif // QFSimplePlotWidget_H

