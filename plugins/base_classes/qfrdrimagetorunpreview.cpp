#include "qfrdrimagetorunpreview.h"

QFRDRImageToRunPreview::QFRDRImageToRunPreview(QWidget *parent) :
    QWidget(parent)
{

    runSelectWidget=false;
    int row=0;
    spinRun=0;
    labRun=0;
    gl=new QGridLayout(this);
    gl->setContentsMargins(2,2,2,2);
    gl->setHorizontalSpacing(2);
    gl->setVerticalSpacing(2);
    gl->setColumnStretch(1,1);
    setLayout(gl);
    labRun=new QLabel(this);
    labRun->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLabel* l;
    gl->addWidget(l=new QLabel(tr("pixel/run:"), this), row, 0);
    spinRun=new QSpinBox(this);
    spinRun->setSpecialValueText(tr("average"));
    l->setBuddy(spinRun);
    spinRun->setVisible(false);
    row++;

    pltOverview=new JKQTFastPlotter(this);
    pltOverview->set_aspectRatio(1);
    pltOverview->set_maintainAspectRatio(true);
    pltOverview->set_xAxisLabelVisible(false);
    pltOverview->set_yAxisLabelVisible(false);
    pltOverview->set_tickFontSize(6);
    pltOverview->set_tickFontName(font().family());
    QFont fo=font();
    fo.setPointSizeF(pltOverview->get_tickFontSize());
    QFontMetricsF fm(fo);
    pltOverview->set_plotBorderBottom(2.0*fm.ascent());
    pltOverview->set_plotBorderLeft(fm.width("00000"));
    pltOverview->set_drawGrid(false);
    connect(pltOverview, SIGNAL(clicked(double,double,Qt::KeyboardModifiers)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));
    gl->addWidget(pltOverview, row, 0, 2, 3);
    gl->setRowStretch(row, 2);
    row+=2;
    labInfo=new QLabel(this);
    labInfo->setTextFormat(Qt::RichText);
    gl->addWidget(labInfo, row, 0, 1, 2);

    actEditSelection=new QAction(QIcon(":/qfrdrmaskeditor/mask.png"), tr("edit mask"), this);
    actEditSelection->setCheckable(true);
    actEditSelection->setChecked(false);

    btnEditSlected=new QToolButton(this);
    btnEditSlected->setDefaultAction(actEditSelection);
    gl->addWidget(btnEditSlected, row, 2);
    row++;

    plteOverview=new JKQTFPimagePlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1, JKQTFP_GRAY);
    plteOverview->setVisible(true);
    pltOverview->addPlot(plteOverview);
    plteOverviewRGB=new JKQTFPRGBImageOverlayPlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1);
    plteOverviewRGB->setVisible(false);
    pltOverview->addPlot(plteOverviewRGB);

    QColor col("orange");
    col.setAlphaF(0.5);
    plteOverviewSelected=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, col);
    plteOverviewSelected->set_showAsSymbols(true);
    pltOverview->addPlot(plteOverviewSelected);


    col=QColor("darkblue");
    col.setAlphaF(0.5);
    plteOverviewExcluded=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, col);
    pltOverview->addPlot(plteOverviewExcluded);


    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSelectedSize=0;

    setRDR(NULL);
    setRunSelectWidgetActive(false);
}

QFRDRImageToRunPreview::~QFRDRImageToRunPreview()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
}

void QFRDRImageToRunPreview::setRDR(QFRawDataRecord *record)
{
    this->record=record;
    rrRecord=dynamic_cast<QFRDRImageToRunInterface*>(record);
    rfcs=dynamic_cast<QFRDRFCSDataInterface*>(record);
    currentRun=-1;
    runmax=0;
    runmin=0;
    if (rrRecord) {
        runmax=rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight()-1;
    }
    if (rfcs) {
        runmax=rfcs->getCorrelationRuns()-1;
        runmin=-1;
    }

    setCurrentRun(currentRun, true);
}

QFRawDataRecord *QFRDRImageToRunPreview::getRDR() const
{
    return record;
}

int QFRDRImageToRunPreview::getCurrentRun() const
{
    return currentRun;
}

void QFRDRImageToRunPreview::setRunSelectWidgetActive(bool active)
{
    runSelectWidget=active;
    gl->removeWidget(spinRun);
    gl->removeWidget(labRun);
    if (runSelectWidget) {
        spinRun->setVisible(true);
        gl->addWidget(spinRun, 0, 1);
        gl->addWidget(labRun, 0, 2);
        labRun->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    } else {
        spinRun->setVisible(false);
        gl->addWidget(labRun, 0, 1, 1, 2);
        labRun->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    }

}

void QFRDRImageToRunPreview::draw(QPainter *painter, QSize *size)
{
    painter->save();
    painter->drawText(0, double(spinRun->height())*0.7, tr("current run: %1 %2").arg(labRun->text()).arg(labInfo->text()));
    painter->save();
    painter->translate(0,spinRun->height());
    pltOverview->draw(painter, size);
    painter->restore();
    painter->restore();
}

void QFRDRImageToRunPreview::replotOverview()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool doDraw=pltOverview->get_doDrawing();
    pltOverview->set_doDrawing(false);

    if (!rrRecord) {
        plteOverview->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewRGB->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);
        labInfo->setText("");
        labRun->setText("");
    } else {
        double* ov=rrRecord->getImageFromRunsPreview(0);
        double* ov2=rrRecord->getImageFromRunsPreview(1);
        int channels=rrRecord->getImageFromRunsChannelsAdvised();
        double w=rrRecord->getImageFromRunsWidth();
        double h=rrRecord->getImageFromRunsHeight();
        int runs=rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight();
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        if (channels==2 && record && record->getProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", false).toBool()) qSwap(ov, ov2);

        //qDebug()<<"replotOverview:  "<<w<<" x "<<h;

        pltOverview->setXRange(0,w);
        pltOverview->setYRange(0,h);
        pltOverview->set_xTickDistance(dx);
        pltOverview->set_yTickDistance(dy);
        pltOverview->set_aspectRatio(w/h);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);

        if (plteOverviewSelectedSize!=w*h) {
            plteOverviewSelectedSize=w*h;
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSelectedSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSelectedSize*sizeof(bool));
        }

        plteOverviewSelected->set_data(plteOverviewSelectedData, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
        plteOverviewSelected->set_xmax(w);
        plteOverviewSelected->set_ymax(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
        plteOverviewExcluded->set_xmax(w);
        plteOverviewExcluded->set_ymax(h);
        if (channels==1) {
            plteOverview->set_image(ov, JKQTFP_double, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
            plteOverviewRGB->setVisible(false);
            plteOverview->setVisible(true);
        } else if (channels>1) {
            plteOverviewRGB->set_image(ov2, JKQTFP_double, ov, JKQTFP_double, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
            plteOverview->setVisible(false);
            plteOverviewRGB->setVisible(true);

        }
        plteOverview->set_xmax(w);
        plteOverview->set_ymax(h);
        plteOverviewRGB->set_xmax(w);
        plteOverviewRGB->set_ymax(h);
        int exclCnt=0;
        if (plteOverviewSelectedData && plteOverviewExcludedData) {
            QFRDRRunSelectionsInterface* rintf=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
            if (rintf) {
                for (int i=0; i<runs; i++) {
                    int x=rrRecord->runToX(i);
                    int y=rrRecord->runToY(i);
                    int idx=y*rrRecord->getImageFromRunsWidth()+x;
                    plteOverviewSelectedData[idx]=(idx==currentRun);
                    plteOverviewExcludedData[idx]=rintf->leaveoutRun(i);
                    if (plteOverviewExcludedData[idx]) exclCnt++;
                }
            } else {
                for (int i=0; i<runs; i++) {
                    int x=rrRecord->runToX(i);
                    int y=rrRecord->runToY(i);
                    int idx=y*rrRecord->getImageFromRunsWidth()+x;
                    plteOverviewSelectedData[idx]=(idx==currentRun);
                    plteOverviewExcludedData[idx]=false;
                }
            }
        }
        labInfo->setText(tr("size: %1&times;%2, %3 masked").arg(rrRecord->getImageFromRunsWidth()).arg(rrRecord->getImageFromRunsHeight()).arg(exclCnt));
        if (rfcs) {
            labRun->setText(rfcs->getCorrelationRunName(currentRun));
        } else {
            int x=rrRecord->runToX(currentRun);
            int y=rrRecord->runToY(currentRun);
            labRun->setText(QString("(%1, %2)").arg(x).arg(y));
        }
    }

    if (doDraw) {
        pltOverview->set_doDrawing(true);
        pltOverview->update_data();
    }
    QApplication::restoreOverrideCursor();
}

void QFRDRImageToRunPreview::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers)
{
    QFRDRRunSelectionsInterface* rintf=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
    if (!rrRecord) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=rrRecord->xyToRun(xx, yy);


    if (rintf && actEditSelection->isChecked()) {
        if (rintf->leaveoutRun(idx)) rintf->leaveoutRemoveRun(idx);
        else rintf->leaveoutAddRun(idx);
    }

    setCurrentRun(idx);
}

void QFRDRImageToRunPreview::setCurrentRun(int runIn, bool replotAlways)
{
    int run=runIn;
    if (run<runmin) run=runmin;
    if (run>runmax) run=runmax;
    if (run==currentRun && !replotAlways) return;
    currentRun=run;
    if (runSelectWidget) {
        disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));
        spinRun->setRange(runmin, runmax);
        if (currentRun!=spinRun->value()) spinRun->setValue(currentRun);
        spinRun->setSuffix(QString(" / %1").arg(runmax));
        connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));
    }
    replotOverview();
    emit currentRunChanged(run);
    if (rrRecord) {
        emit currentPixelChanged(rrRecord->runToX(run), rrRecord->runToY(run));
    }
}

void QFRDRImageToRunPreview::setCurrentPixel(int x, int y)
{
    if (!rrRecord) return;

    int idx=rrRecord->xyToRun(x, y);

    setCurrentRun(idx);
}

void QFRDRImageToRunPreview::setMaskEditable(bool editable)
{
    actEditSelection->setEnabled(editable);
    if (!editable) actEditSelection->setChecked(false);
}
