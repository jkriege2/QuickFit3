#include "qfrdrimagetorunpreview.h"
#include "qfrdrimagemaskbyintensity.h"

QFRDRImageToRunPreview::QFRDRImageToRunPreview(QWidget *parent) :
    QWidget(parent)
{

    actMaskByImage=new QAction(QIcon(":/qfrdrmaskeditor/maskbyimage.png"), tr("create a mask from the current overview image ..."), this);
    connect(actMaskByImage, SIGNAL(triggered()), this, SLOT(excludeByImage()));
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

    maskEditTools=new QFRDRImageMaskEditTools(this, "QFRDRImageToRunPreview/");
    tbEdit=new QToolBar(tr("mask edit toolbar"), this);
    tbEdit->setVisible(false);
    maskEditTools->registerMaskToolsToToolbar(tbEdit);
    tbEdit->addAction(actMaskByImage);
    tbEdit->addSeparator();
    maskEditTools->registerPlotterMaskToolsToToolbar(tbEdit);
    gl->addWidget(tbEdit, row, 0,1,3);
    row++;

    pltOverview=new QFPlotter(this);
    pltOverview->setObjectName("pltOverview");
    pltOverview->get_plotter()->set_plotLabel("");
    pltOverview->get_plotter()->set_plotLabelFontSize(10);
    pltOverview->set_displayMousePosition(false);
    pltOverview->set_displayToolbar(false);
    pltOverview->get_plotter()->set_maintainAspectRatio(true);
    pltOverview->get_plotter()->set_aspectRatio(1);
    pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
    pltOverview->get_plotter()->set_axisAspectRatio(1);
    pltOverview->setXY(0,0,1,1);
    pltOverview->setAbsoluteXY(0,1,0,1);
    pltOverview->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltOverview->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltOverview->get_plotter()->getXAxis()->set_tickLabelFontSize(6);
    pltOverview->get_plotter()->getYAxis()->set_tickLabelFontSize(6);
    pltOverview->get_plotter()->getXAxis()->set_axisLabel("");
    pltOverview->get_plotter()->getYAxis()->set_axisLabel("");
    pltOverview->get_plotter()->setGrid(false);
    pltOverview->get_plotter()->set_useAntiAliasingForSystem(true);
    pltOverview->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltOverview->set_userActionCompositionMode(QPainter::CompositionMode_Xor);

//    pltOverview->set_aspectRatio(1);
//    pltOverview->set_maintainAspectRatio(true);
//    pltOverview->set_xAxisLabelVisible(false);
//    pltOverview->set_yAxisLabelVisible(false);
//    pltOverview->set_tickFontSize(6);
//    pltOverview->set_tickFontName(font().family());
//    QFont fo=font();
//    fo.setPointSizeF(pltOverview->get_tickFontSize());
//    QFontMetricsF fm(fo);
//    pltOverview->set_plotBorderBottom(2.0*fm.ascent());
//    pltOverview->set_plotBorderLeft(fm.width("00000"));
//    pltOverview->set_drawGrid(false);
//    connect(pltOverview, SIGNAL(clicked(double,double,Qt::KeyboardModifiers)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));
//    gl->addWidget(pltOverview, row, 0, 2, 3);
    connect(pltOverview, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)));
    maskEditTools->registerPlotter(pltOverview);
    gl->addWidget(pltOverview, row, 0, 2, 3);
    gl->setRowStretch(row, 2);
    row+=2;
    labInfo=new QLabel(this);
    labInfo->setTextFormat(Qt::RichText);
    gl->addWidget(labInfo, row, 0, 1, 2);

    actEditSelection=new QAction(QIcon(":/qfrdrmaskeditor/mask.png"), tr("edit mask"), this);
    actEditSelection->setCheckable(true);
    actEditSelection->setChecked(false);
    actEditSelection->setVisible(false);

    btnEditSlected=new QToolButton(this);
    btnEditSlected->setDefaultAction(actEditSelection);
    gl->addWidget(btnEditSlected, row, 2);
    row++;

    /*plteOverview=new JKQTFPimagePlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1, JKQTFP_GRAY);
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
    pltOverview->addPlot(plteOverviewExcluded);*/

    QColor ovlSelCol("orange");
    ovlSelCol.setAlphaF(0.5);

    QColor ovlExCol=QColor("darkblue");
    ovlExCol.setAlphaF(0.5);

    plteOverview=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, JKQTPMathImageGRAY, pltOverview->get_plotter());
    plteOverview->set_visible(true);
    pltOverview->addGraph(plteOverview);
    plteOverviewRGB=new JKQTPRGBMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, pltOverview->get_plotter());
    plteOverviewRGB->set_visible(false);
    pltOverview->addGraph(plteOverviewRGB);

    plteOverviewSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltOverview->get_plotter());
    plteOverviewSelected->set_rectanglesAsImageOverlay(true);
    plteOverviewSelected->set_drawAsRectangles(true);
    plteOverviewSelected->set_symbol(JKQTPcircle);
    pltOverview->addGraph(plteOverviewSelected);


    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltOverview->get_plotter());
    plteOverviewExcluded->set_rectanglesAsImageOverlay(true);
    pltOverview->addGraph(plteOverviewExcluded);

    plteCurrentPixel=new JKQTPxyLineGraph(pltOverview->get_plotter());
    plteCurrentPixel->set_symbol(JKQTPtarget);
    plteCurrentPixel->set_symbolSize(10);
    plteCurrentPixel->set_color(ovlSelCol);
    pltOverview->addGraph(plteCurrentPixel);

    connect(pltOverview, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));


    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSelectedSize=0;
    plteOverviewSelectedW=0;
    plteOverviewSelectedH=0;

    setRDR(NULL);
    setSelectionEditable(false);
    setMaskEditable(false);
    setRunSelectWidgetActive(false);
    connect(maskEditTools, SIGNAL(rawDataChanged()), this, SLOT(replotOverview()));
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

    maskEditTools->setRDR(record);
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

void QFRDRImageToRunPreview::draw(JKQTPEnhancedPainter *painter, QSize *size)
{
    painter->save();
    painter->drawText(0, double(spinRun->height())*0.7, tr("current run: %1 %2").arg(labRun->text()).arg(labInfo->text()));
    painter->save();
    painter->translate(0,spinRun->height());
    pltOverview->get_plotter()->draw(*painter);
    if (size) *size=pltOverview->size();
    painter->restore();
    painter->restore();
}

void QFRDRImageToRunPreview::replotOverview()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool doDraw=pltOverview->get_doDrawing();
    pltOverview->set_doDrawing(false);

    if (!rrRecord) {
        plteOverview->set_data(NULL,  0,0, JKQTPMathImageBase::DoubleArray);
        plteOverviewRGB->set_data(NULL, NULL,NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);
        plteCurrentPixel->set_xColumn(-1);
        plteCurrentPixel->set_yColumn(-1);
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

        pltOverview->get_plotter()->set_aspectRatio(w/h);
        pltOverview->get_plotter()->set_axisAspectRatio(w/h);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);
        plteCurrentPixel->set_xColumn(-1);
        plteCurrentPixel->set_yColumn(-1);
        plteOverview->set_data(NULL,  0,0, JKQTPMathImageBase::DoubleArray);
        plteOverviewRGB->set_data(NULL, NULL,NULL, 0, 0, JKQTPMathImageBase::DoubleArray);

        if (plteOverviewSelectedSize!=w*h) {
            plteOverviewSelectedSize=w*h;
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSelectedSize*sizeof(bool));
            for (int i=0; i<plteOverviewSelectedSize; i++) plteOverviewSelectedData[i]=false;
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSelectedSize*sizeof(bool));
        }
        plteOverviewSelectedW=w;
        plteOverviewSelectedH=h;
        maskEditTools->setAllowEditSelection(maskEditTools->getAllowEditSelection(), plteOverviewSelectedData, plteOverviewSelectedW, plteOverviewSelectedH);


        pltOverview->getDatastore()->clear();

        plteOverviewSelected->set_data(plteOverviewSelectedData, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
        plteOverviewSelected->set_width(w);
        plteOverviewSelected->set_height(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
        plteOverviewExcluded->set_width(w);
        plteOverviewExcluded->set_height(h);
        if (channels==1) {
            pltOverview->getDatastore()->addCopiedColumn(ov,  rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(), tr("overview image"));
            plteOverview->set_data(ov, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight(), JKQTPMathImageBase::DoubleArray);
            plteOverviewRGB->set_visible(false);
            plteOverview->set_visible(true);
        } else if (channels>1) {
            pltOverview->getDatastore()->addCopiedColumn(ov,  rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(), tr("overview image, ch1"));
            pltOverview->getDatastore()->addCopiedColumn(ov2,  rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(), tr("overview image, ch2"));
            plteOverviewRGB->set_data(ov2, ov, NULL, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight(), JKQTPMathImageBase::DoubleArray);
            plteOverview->set_visible(false);
            plteOverviewRGB->set_visible(true);

        }
        plteOverview->set_width(w);
        plteOverview->set_height(h);
        plteOverviewRGB->set_width(w);
        plteOverviewRGB->set_height(h);
        int exclCnt=0;
        if (plteOverviewSelectedData && plteOverviewExcludedData) {
            QFRDRRunSelectionsInterface* rintf=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
            double cx=-1;
            double cy=-1;
            if (rintf) {
                for (int i=0; i<runs; i++) {
                    int x=rrRecord->runToX(i);
                    int y=rrRecord->runToY(i);
                    int idx=y*rrRecord->getImageFromRunsWidth()+x;
                    //plteOverviewSelectedData[idx]=(idx==currentRun);
                    if (idx==currentRun) {
                        cx=x+0.5;
                        cy=y+0.5;
                    }
                    plteOverviewExcludedData[idx]=rintf->leaveoutRun(i);
                    if (plteOverviewExcludedData[idx]) exclCnt++;
                }
            } else {
                for (int i=0; i<runs; i++) {
                    int x=rrRecord->runToX(i);
                    int y=rrRecord->runToY(i);
                    int idx=y*rrRecord->getImageFromRunsWidth()+x;
                    if (idx==currentRun) {
                        cx=x+0.5;
                        cy=y+0.5;
                    }
                    //plteOverviewSelectedData[idx]=(idx==currentRun);
                    plteOverviewExcludedData[idx]=false;
                }
            }
            pltOverview->getDatastore()->addCopiedColumn(plteOverviewSelectedData,  rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(), tr("selection"));
            pltOverview->getDatastore()->addCopiedColumn(plteOverviewExcludedData,  rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(), tr("mask"));
            int rcx=pltOverview->getDatastore()->addCopiedColumn(&cx,  1, tr("current_x"));
            int rcy=pltOverview->getDatastore()->addCopiedColumn(&cy,  1, tr("current_y"));
            if (cx>=0 && cy>=0)  {
                plteCurrentPixel->set_xColumn(rcx);
                plteCurrentPixel->set_yColumn(rcy);
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
        pltOverview->setAbsoluteXY(0,w, 0, h);
        pltOverview->setXY(0,w, 0, h);
    }

    if (doDraw) {
        pltOverview->set_doDrawing(true);
        pltOverview->update_plot();
    }
    QApplication::restoreOverrideCursor();
}

void QFRDRImageToRunPreview::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers, Qt::MouseButton button)
{
    if (button==Qt::LeftButton) {
        QFRDRRunSelectionsInterface* rintf=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
        if (!rrRecord) return;

        int xx=(int)floor(x);
        int yy=(int)floor(y);

        int idx=rrRecord->xyToRun(xx, yy);


        /*if (rintf && actEditSelection->isChecked()) {
            if (rintf->leaveoutRun(idx)) rintf->leaveoutRemoveRun(idx);
            else rintf->leaveoutAddRun(idx);
        }*/

        setCurrentRun(idx);
    }
}

void QFRDRImageToRunPreview::moveColorbarsAuto()
{
    bool rightVisible=false;
    if (rrRecord && rrRecord->getImageFromRunsHeight() < rrRecord->getImageFromRunsWidth()) { // wider than high, i.e. "landscape"
        rightVisible=!rightVisible;
    }

    if ((double)pltOverview->width()<=(double)pltOverview->height()) {
        plteOverview->set_colorBarRightVisible(rightVisible);
        plteOverview->set_colorBarTopVisible(!rightVisible);
        plteOverviewRGB->set_colorBarRightVisible(rightVisible);
        plteOverviewRGB->set_colorBarTopVisible(!rightVisible);
    } else {
        plteOverview->set_colorBarRightVisible(!rightVisible);
        plteOverview->set_colorBarTopVisible(rightVisible);
        plteOverviewRGB->set_colorBarRightVisible(!rightVisible);
        plteOverviewRGB->set_colorBarTopVisible(rightVisible);
    }
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
    /*actEditSelection->setEnabled(editable);
    if (!editable) actEditSelection->setChecked(false);*/
    maskEditTools->setMaskEditing(editable);
    tbEdit->setVisible(maskEditTools->getMaskEditing() || maskEditTools->getAllowEditSelection());
}

void QFRDRImageToRunPreview::setSelectionEditable(bool editable)
{
    if (editable) {
        maskEditTools->setAllowEditSelection(true, plteOverviewSelectedData, plteOverviewSelectedW, plteOverviewSelectedH);
    } else {
        maskEditTools->setAllowEditSelection(false);
    }
    tbEdit->setVisible(maskEditTools->getMaskEditing() || maskEditTools->getAllowEditSelection());
}


void QFRDRImageToRunPreview::excludeByImage() {
    if (!(rrRecord&&maskEditTools->getMaskEditing()&&rrRecord)) return;


        bool* mask=(bool*)malloc(rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight()*sizeof(bool));
        bool* newMask=duplicateArray(mask, rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight());
        maskEditTools->copyMask(mask, rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight());

        QFRDRImageMaskByIntensity* dialog=new QFRDRImageMaskByIntensity(this);

        int channel=0;
        if (rrRecord->getImageFromRunsChannels()>1) {
            bool ok=false;
            QStringList sl;
            for (int i=0; i<rrRecord->getImageFromRunsChannels(); i++) {
                sl<<tr("Channel %1").arg(i+1);
            }
            channel=sl.indexOf(QInputDialog::getItem(this, tr("Mask by image"), tr("select the channel to use for masking"), sl, channel, false, &ok));
            if (!ok) channel=-1;
        }
        if (channel>=0) {
            double* image=rrRecord->getImageFromRunsPreview(channel);

            dialog->init(mask, image, rrRecord->getImageFromRunsWidth(), rrRecord->getImageFromRunsHeight());
            if (dialog->exec()==QDialog::Accepted) {
                if (dialog->getMaskMode()==2) {
                    for (int i=0; i<rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(); i++) {
                        newMask[i]=newMask[i] && (!mask[i]);
                    }
                } else if (dialog->getMaskMode()==1) {
                    for (int i=0; i<rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight(); i++) {
                        newMask[i]=newMask[i]||mask[i];
                    }
                } else {
                    memcpy(newMask, mask, rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight()*sizeof(bool));
                }
                //if (rfcs) rfcs->recalcCorrelations();
                maskEditTools->setMask(newMask, rrRecord->getImageFromRunsWidth()*rrRecord->getImageFromRunsHeight());

            }
        }
        delete dialog;
        free(mask);
        free(newMask);
        replotOverview();

}



