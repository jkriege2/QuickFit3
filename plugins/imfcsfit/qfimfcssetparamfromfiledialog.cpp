#include "qfimfcssetparamfromfiledialog.h"
#include "ui_qfimfcssetparamfromfiledialog.h"
#include "qfrdrimagetoruninterface.h"


QFImFCSSetParamFromFileDialog::QFImFCSSetParamFromFileDialog(QFImFCSFitEvaluation* eval, QStringList parameters, QStringList parameterIDs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFImFCSSetParamFromFileDialog)
{
    this->parameterIDs=parameterIDs;
    plt=NULL;
    ui->setupUi(this);
    ui->cmbRDR->init(eval->getProject(), eval->getMatchFunctor());
    ui->cmbResultGroup->setRDR(ui->cmbRDR->currentRDR());
    ui->cmbResult->setRDR(ui->cmbRDR->currentRDR());
    ui->cmbResult->setEvaluationGroup(ui->cmbResultGroup->currentEvaluationGroup());
    ui->cmbParameter->clear();
    ui->cmbParameter->addItems(parameters);
    ui->cmbParameter->setCurrentIndex(0);
    connect(ui->cmbRDR, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroup, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbRDR, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResult, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbResultGroup, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResult, SLOT(setEvaluationGroup(QString)));
    QStringList filt;
    filt<<"fit results"<<"results"<<"evaluation results";
    ui->cmbResult->setResultGroupFilters(filt);
    ui->cmbRDR->setCurrentIndex(0);
}


QFImFCSSetParamFromFileDialog::~QFImFCSSetParamFromFileDialog()
{
    delete ui;
}

QFRawDataRecord *QFImFCSSetParamFromFileDialog::getRDR() const
{
    return ui->cmbResult->getRDR();
}

QString QFImFCSSetParamFromFileDialog::getEvalID() const
{
    return ui->cmbResult->currentEvaluationID();
}

QString QFImFCSSetParamFromFileDialog::getResultID() const
{
    return ui->cmbResult->currentResult();
}

QString QFImFCSSetParamFromFileDialog::getParameter() const
{
    return parameterIDs.value(ui->cmbParameter->currentIndex(), "");
}

QVector<double> QFImFCSSetParamFromFileDialog::getData(bool *ok) const
{
    QFRawDataRecord* rdr=getRDR();
    if (rdr) return rdr->resultsGetAsDoubleList(getEvalID(), getResultID(), ok);
    else {
        if (ok) *ok=false;
        return QVector<double>();
    }
}

void QFImFCSSetParamFromFileDialog::replotOvr()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->pltData->set_doDrawing(false);
    ui->pltData->clearGraphs();
    JKQTPdatastore* ds=ui->pltData->getDatastore();
    ds->clear();

    QFRawDataRecord* rdr=getRDR();
    QFRDRImageToRunInterface* dataImg=dynamic_cast<QFRDRImageToRunInterface*>(rdr);

    if (dataImg) {
        int w=dataImg->getImageFromRunsWidth();
        int h=dataImg->getImageFromRunsHeight();

        bool ok=false;
        QVector<double> d=rdr->resultsGetAsDoubleList(getEvalID(), getResultID(), &ok);

        if (ok) {
            size_t col=ds->addCopiedColumn(d.data(), d.size(), getResultID());

            plt=new JKQTPColumnMathImage(ui->pltData->get_plotter());
            plt->set_palette(JKQTPMathImage::MATLAB);
            plt->set_imageColumn(col);
            plt->set_Nx(w);
            plt->set_Ny(h);
            plt->set_x(0);
            plt->set_y(0);
            plt->set_width(w);
            plt->set_height(h);
            plt->set_showColorBar(true);
            plt->set_autoImageRange(false);
            plt->set_imageMin(0);
            plt->set_imageMax(1);
            bool showTop=w>h;
            plt->set_colorBarTopVisible(showTop);
            plt->set_colorBarRightVisible(!showTop);
            plt->set_title(getResultID());
            ui->pltData->get_plotter()->set_showKey(false);
            ui->pltData->get_plotter()->set_axisAspectRatio(double(w)/double(h));
            ui->pltData->get_plotter()->set_aspectRatio(double(w)/double(h));
            ui->pltData->get_plotter()->set_maintainAspectRatio(true);
            ui->pltData->get_plotter()->set_maintainAxisAspectRatio(true);
            ui->pltData->addGraph(plt);
            ui->pltData->setAbsoluteXY(0,w,0,h);
            ui->pltData->setXY(0,w,0,h);
        }
    }

    //ui->pltData->zoomToFit();
    ui->pltData->set_doDrawing(true);
    ui->pltData->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFImFCSSetParamFromFileDialog::on_cmbParamter_currentIndexChanged(int index)
{
    ui->cmbResult->setCurrentResult(QString("fitparam_")+parameterIDs.value(index, ""));
    if (ui->cmbResult->currentIndex()<0) ui->cmbResult->setCurrentResult(parameterIDs.value(index, ""));
}


