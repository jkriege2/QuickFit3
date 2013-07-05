#include "qfimfccsfitchi2landscapedialog.h"
#include "ui_qfimfccsfitchi2landscapedialog.h"

QFImFCCSFitChi2LandscapeDialog::QFImFCCSFitChi2LandscapeDialog(QFImFCCSFitEvaluationItem *item, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFImFCCSFitChi2LandscapeDialog)
{
    this->item=item;
    ui->setupUi(this);

    ui->cmbParameterX->clear();
    ui->cmbParameterY->clear();

    for (int r=0; r<item->getFitFileCount(); r++) {
        QFRawDataRecord* rec=item->getFitFile(r);
        QFFitFunction* ff=item->getFitFunction(rec);
        QStringList pids=ff->getParameterIDs();
        for (int p=0; p<pids.size(); p++) {
            QFFitFunction::ParameterDescription d=ff->getDescription(pids[p]);
            QString name=tr("file %1: %2").arg(r+1).arg(d.name);
            QPoint pnt(r, p);
            ui->cmbParameterX->addItem(name, pnt);
            ui->cmbParameterY->addItem(name, pnt);
        }
    }
    updateInputs();
    ui->cmbParameterX->setCurrentIndex(0);
    ui->cmbParameterY->setCurrentIndex(1);

}

QFImFCCSFitChi2LandscapeDialog::~QFImFCCSFitChi2LandscapeDialog()
{
    delete ui;
}

void QFImFCCSFitChi2LandscapeDialog::updateInputs()
{
    ui->spinXMax->setMaximum(DBL_MAX);
    ui->spinYMax->setMaximum(DBL_MAX);
    if (ui->cmbModeX->currentIndex()==0) ui->spinXMin->setMinimum(-DBL_MAX);
    else ui->spinXMin->setMinimum(1e-15);
    if (ui->cmbModeY->currentIndex()==0) ui->spinYMin->setMinimum(-DBL_MAX);
    else ui->spinYMin->setMinimum(1e-15);
}

void QFImFCCSFitChi2LandscapeDialog::on_btnPlot_clicked()
{
    ui->plotter->set_doDrawing(false);
    ui->plotter->getDatastore()->clear();
    ui->plotter->clearGraphs();
    double* d=(double*)calloc(ui->spinXPixels->value()*ui->spinYPixels->value(), sizeof(double));



    QVector<double> xData, yData;
    if (ui->cmbModeX->currentIndex()==0) {
        double x=ui->spinXMin->value();
        for (int i=0; i<ui->spinXPixels->value(); i++) {
            xData<<x;
            x=x+(ui->spinXMax->value()-ui->spinXMin->value())/double(ui->spinXPixels->value());
        }
    } else {
        double x=log10(ui->spinXMin->value());
        for (int i=0; i<ui->spinXPixels->value(); i++) {
            xData<<pow(10,x);
            x=x+(log10(ui->spinXMax->value())-log10(ui->spinXMin->value()))/double(ui->spinXPixels->value());
        }
    }

    if (ui->cmbModeY->currentIndex()==0) {
        double x=ui->spinYMin->value();
        for (int i=0; i<ui->spinYPixels->value(); i++) {
            yData<<x;
            x=x+(ui->spinYMax->value()-ui->spinYMin->value())/double(ui->spinYPixels->value());
        }
    } else {
        double x=log10(ui->spinYMin->value());
        for (int i=0; i<ui->spinYPixels->value(); i++) {
            yData<<pow(10,x);
            x=x+(log10(ui->spinYMax->value())-log10(ui->spinYMin->value()))/double(ui->spinYPixels->value());
        }
    }

    QPoint pX=ui->cmbParameterX->itemData(ui->cmbParameterX->currentIndex()).toPoint();
    QPoint pY=ui->cmbParameterY->itemData(ui->cmbParameterY->currentIndex()).toPoint();
    item->calcChi2Landscape(d, pX.x(), pX.y(), xData, pY.x(), pY.y(), yData, item->getFitFiles(), item->getCurrentIndex());
    if (ui->chkLogscale->isChecked()) {
        for (int i=0; i<ui->spinXPixels->value()*ui->spinYPixels->value(); i++) {
            d[i]=log10(d[i]);
        }
    }

    int col=ui->plotter->getDatastore()->addCopiedImageAsColumn(d,ui->spinXPixels->value(),ui->spinYPixels->value(),tr("image: %1/%2").arg(ui->cmbParameterX->currentText()).arg(ui->cmbParameterY->currentText()));


    JKQTPColumnMathImage* img=new JKQTPColumnMathImage(ui->plotter->get_plotter());
    img->set_imageColumn(col);
    img->set_Nx(ui->spinXPixels->value());
    img->set_Ny(ui->spinYPixels->value());
    img->set_x(ui->spinXMin->value());
    img->set_y(ui->spinYMin->value());
    img->set_width(ui->spinXMax->value()-ui->spinXMin->value());
    img->set_height(ui->spinYMax->value()-ui->spinYMin->value());
    img->set_palette(JKQTPMathImage::MATLAB);
    img->set_autoImageRange(true);

    ui->plotter->addGraph(img);
    ui->plotter->setAbsoluteXY(ui->spinXMin->value(), ui->spinXMax->value(), ui->spinYMin->value(), ui->spinYMax->value());
    ui->plotter->setXY(ui->spinXMin->value(), ui->spinXMax->value(), ui->spinYMin->value(), ui->spinYMax->value());
    ui->plotter->getXAxis()->set_logAxis(ui->cmbModeX->currentIndex()==1);
    ui->plotter->getXAxis()->set_axisLabel(ui->cmbParameterX->currentText());
    ui->plotter->getYAxis()->set_logAxis(ui->cmbModeY->currentIndex()==1);
    ui->plotter->getYAxis()->set_axisLabel(ui->cmbParameterY->currentText());
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();

    free(d);
}
