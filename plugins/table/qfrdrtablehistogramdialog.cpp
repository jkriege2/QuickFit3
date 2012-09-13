#include "qfrdrtablehistogramdialog.h"
#include "ui_qfrdrtablehistogramdialog.h"

QFRDRTableHistogramDialog::QFRDRTableHistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableHistogramDialog)
{
    ui->setupUi(this);
}

QFRDRTableHistogramDialog::~QFRDRTableHistogramDialog()
{
    delete ui;
}

void QFRDRTableHistogramDialog::setColumnData(const QMap<int, QList<double> > &data)
{
    this->data=data;
}


void QFRDRTableHistogramDialog::readProperties(QFProperties *props)
{
    this->props=props;
}

void QFRDRTableHistogramDialog::setColumnTitles(const QStringList &titles)
{
    columnTitles=titles;
}

void QFRDRTableHistogramDialog::init() {
    QMapIterator<int, QList<double> > it(data);
    while (it.hasNext()) {
        it.next();

    }
}

QMap<int, QList<double> > QFRDRTableHistogramDialog::getHistograms() const
{
    return histograms;
}

QMap<int, QString> QFRDRTableHistogramDialog::getHeaderNames() const
{
    return headerNames;
}

void QFRDRTableHistogramDialog::updateHistograms()
{
    histograms.clear();
    headerNames.clear();
}
