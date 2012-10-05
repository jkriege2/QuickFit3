#include "qfrdrimagingfcs_dataeditor_image.h"
#include "qfrdrimagingfcscopydataselectdialog.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>
#include <math.h>
#include "jkqttools.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "datatable2.h"
#include "jkimage.h"
#include "qmoretextobject.h"
#include "statistics_tools.h"
#include "jkqtpelements.h"
#include "qfrdrimagingfcsmaskbyintensity.h"
#include "qftools.h"
#include "qfrawdatapropertyeditor.h"

#define sqr(x) ((x)*(x))

#define CLICK_UPDATE_TIMEOUT 500
//#define DEBUG_TIMIMNG
#undef DEBUG_TIMIMNG

#define OverlayRectanglesAsImageOverlay true

QFRDRImagingFCSImageEditor::QFRDRImagingFCSImageEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    m_fitFunctions=services->getFitFunctionManager()->getModels("", this);
    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSize=0;
    plteImageData=NULL;
    plteGofImageData=NULL;
    plteImageSize=0;
    lastSavePath="";
    connectImageWidgetsCounter=0;
    connectParameterWidgetsCounter=0;
    datahist=datahistsel=NULL;
    datasize=datasizesel=0;
    timUpdateAfterClick=new QTimer(this);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->setInterval(CLICK_UPDATE_TIMEOUT);
    timUpdateAfterClick->stop();
    connect(timUpdateAfterClick, SIGNAL(timeout()), this, SLOT(updateAfterClick()));
    createWidgets();
    //QTimer::singleShot(500, this, SLOT(debugInfo()));
}

QFRDRImagingFCSImageEditor::~QFRDRImagingFCSImageEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
    //if (datahist) free(datahist);
    datahist=NULL;
    //if (datahistsel) free(datahistsel);
    datahistsel=NULL;
}




void QFRDRImagingFCSImageEditor::createWidgets() {
    QLabel* l;
    QGridLayout* lb=new QGridLayout(this);
    setLayout(lb);


    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    excludedColor=ovlExCol;


    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter selection group box
    ///////////////////////////////////////////////////////////////

    QGroupBox* grpTop=new QGroupBox(tr(" Parameter Image Plot Settings "), this);
    grpTop->setFlat(true);
    QGridLayout* topgrid=new QGridLayout(this);
    grpTop->setLayout(topgrid);

    int row=0;
    cmbResultGroup=new QComboBox(this);
    cmbResultGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((l=new QLabel(tr("&result set:"))), row, 0);
    l->setBuddy(cmbResultGroup);
    topgrid->addWidget(cmbResultGroup, row, 1);

    cmbParameter=new QComboBox(this);
    cmbParameter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((labParameter=new QLabel(tr("&parameter:"))), row, 3);
    labParameter->setBuddy(cmbParameter);
    topgrid->addWidget(cmbParameter, row, 4);
    cmbParameterTransform=new QComboBox(this);
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labParameterTransform=new QLabel(tr("    &transform:"))), row, 5);
    labParameterTransform->setBuddy(cmbParameterTransform);
    topgrid->addWidget(cmbParameterTransform, row, 6);

    row++;
    cmbGofParameter=new QComboBox(this);
    cmbGofParameter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((labGofParameter=new QLabel(tr("&goodness of fit:"))), row, 3);
    labGofParameter->setBuddy(cmbGofParameter);
    topgrid->addWidget(cmbGofParameter, row, 4);
    cmbGofParameterTransform=new QComboBox(this);
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labGofParameterTransform=new QLabel(tr("    tr&ansform:"))), row, 5);
    labGofParameterTransform->setBuddy(cmbGofParameterTransform);
    topgrid->addWidget(cmbGofParameterTransform, row, 6);



    topgrid->addWidget(new QWidget(), 0, 2);
    topgrid->addWidget(new QWidget(), 0, 5);
    topgrid->addWidget(new QWidget(), 0, 7);
    topgrid->setColumnStretch(0, 0);
    topgrid->setColumnStretch(1, 0);
    topgrid->setColumnStretch(2, 1);
    topgrid->setColumnStretch(3, 0);
    topgrid->setColumnStretch(4, 0);
    topgrid->setColumnStretch(5, 0);
    topgrid->setColumnStretch(6, 0);
    topgrid->setColumnStretch(7, 1);


    JKVerticalScrollArea* area=new JKVerticalScrollArea(this);
    QWidget* w=new QWidget(this);
    w->setMinimumWidth(300);
    area->setWidgetResizable(true);
    area->setWidget(w);
    QVBoxLayout* vbl=new QVBoxLayout();
    w->setLayout(vbl);







    ///////////////////////////////////////////////////////////////
    // GROUPBOX: visible plots
    ///////////////////////////////////////////////////////////////
    QGroupBox* grpVisiblePlots=new QGroupBox(tr(" general options "), this);
    grpVisiblePlots->setFlat(true);
    QGridLayout* glVisPlots=new QGridLayout(this);
    grpVisiblePlots->setLayout(glVisPlots);
    chkOverviewVisible=new QCheckBox(tr("overview"), grpVisiblePlots);
    chkOverviewVisible->setChecked(true);
    chkGofVisible=new QCheckBox(tr("goodnes of fit"), grpVisiblePlots);
    chkGofVisible->setChecked(true);
    chkMaskVisible=new QCheckBox(tr("mask"), grpVisiblePlots);
    chkMaskVisible->setChecked(false);
    glVisPlots->addWidget(chkOverviewVisible, 0,0);
    glVisPlots->addWidget(chkGofVisible, 0,1);
    glVisPlots->addWidget(chkMaskVisible, 0,2);

    cmbDualView=new QComboBox(this);
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvnone.png"), tr("none"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvhor.png"), tr("split horizontal"));
    cmbDualView->addItem(QIcon(":/imaging_fcs/dvver.png"), tr("split vertical"));
    glVisPlots->addWidget(new QLabel("DualView mode:"), 1, 0);
    glVisPlots->addWidget(cmbDualView, 1, 1, 1, 2);
    vbl->addWidget(grpVisiblePlots);



    ///////////////////////////////////////////////////////////////
    // GROUPBOX: mask options
    ///////////////////////////////////////////////////////////////
    QGroupBox* wmask=new QGroupBox(tr(" mask options "), this);
    wmask->setFlat(true);
    vbl->addWidget(wmask);
    QGridLayout* glmask=new QGridLayout(this);
    glmask->setHorizontalSpacing(2);
    glmask->setVerticalSpacing(2);
    wmask->setLayout(glmask);

    int mskgrpRow=0;
    btnDontUse=createButtonAndActionShowText(actDontUse, QIcon(":/imaging_fcs/mask.png"), tr("&mask sel."), w);
    actDontUse->setToolTip(tr("add the selected pixels to the current mask (so don't use it's data)\nand recalculate the average correlation curve accordingly"));
    connect(actDontUse, SIGNAL(triggered()), this, SLOT(excludeRuns()));
    glmask->addWidget(btnDontUse, mskgrpRow, 0);
    btnUse=createButtonAndActionShowText(actUse, QIcon(":/imaging_fcs/unmask.png"), tr("&unmask sel."), w);
    actUse->setToolTip(tr("remove the selected pixels from the current mask (so use it's data)\nand recalculate the average correlation curve accordingly"));
    connect(actUse, SIGNAL(triggered()), this, SLOT(includeRuns()));
    glmask->addWidget(btnUse, mskgrpRow, 1);

    btnUseAll=createButtonAndActionShowText(actUseAll, QIcon(":/imaging_fcs/clearmask.png"), tr("&clear "), w);
    actUseAll->setToolTip(tr("clear the mask and recalculate the average correlation curve accordingly"));
    glmask->addWidget(btnUseAll, mskgrpRow, 2);
    connect(actUseAll, SIGNAL(triggered()), this, SLOT(includeAll()));
    btnInvertMask=createButtonAndActionShowText(actInvertMask, QIcon(":/imaging_fcs/invertmask.png"), tr("&invert mask"), w);
    actInvertMask->setToolTip(tr("invert the current mask (all masked pixel are unmasked and vice versa)\nand recalculate the average correlation curve accordingly"));
    glmask->addWidget(btnInvertMask, mskgrpRow, 3);
    connect(actInvertMask, SIGNAL(triggered()), this, SLOT(invertMask()));
    mskgrpRow++;

    btnSaveMask=createButtonAndActionShowText(actSaveMask, QIcon(":/imaging_fcs/savemask.png"), tr("&save"), w);
    actSaveMask->setToolTip(tr("save the mask to harddisk"));
    glmask->addWidget(btnSaveMask, mskgrpRow, 0);
    connect(actSaveMask, SIGNAL(triggered()), this, SLOT(saveMask()));
    btnLoadMask=createButtonAndActionShowText(actLoadMask, QIcon(":/imaging_fcs/loadmask.png"), tr("&load"), w);
    actLoadMask->setToolTip(tr("load a mask from harddisk"));
    glmask->addWidget(btnLoadMask, mskgrpRow, 1);
    connect(actLoadMask, SIGNAL(triggered()), this, SLOT(loadMask()));
    btnCopyMask=createButtonAndActionShowText(actCopyMask, QIcon(":/imaging_fcs/copymask.png"), tr("&copy"), w);
    actCopyMask->setToolTip(tr("copy the mask to clipboard"));
    glmask->addWidget(btnCopyMask, mskgrpRow, 2);
    connect(actCopyMask, SIGNAL(triggered()), this, SLOT(copyMask()));
    btnPasteMask=createButtonAndActionShowText(actPasteMask, QIcon(":/imaging_fcs/pastemask.png"), tr("&paste"), w);
    actPasteMask->setToolTip(tr("paste a mask from clipboard"));
    glmask->addWidget(btnPasteMask, mskgrpRow, 3);
    connect(actPasteMask, SIGNAL(triggered()), this, SLOT(pasteMask()));


    mskgrpRow++;
    QFrame* frame=new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    glmask->addWidget(frame, mskgrpRow, 0, 1, 4);

    mskgrpRow++;
    btnMaskByIntensity=createButtonAndActionShowText(actMaskByIntensity, tr("m. by &overview"), w);
    actMaskByIntensity->setToolTip(tr("create a mask according to the <b>overview image</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByIntensity, mskgrpRow, 0);
    connect(actMaskByIntensity, SIGNAL(triggered()), this, SLOT(excludeByIntensity()));
    btnMaskByGofIntensity=createButtonAndActionShowText(actMaskByGofIntensity, tr("m. by &GOF"), w);
    actMaskByGofIntensity->setToolTip(tr("create a mask according to the <b>goodnes-of-fit image</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByGofIntensity, mskgrpRow, 1);
    connect(actMaskByGofIntensity, SIGNAL(triggered()), this, SLOT(excludeByGOFIntensity()));
    btnMaskByParamIntensity=createButtonAndActionShowText(actMaskByParamIntensity, tr("m. by &param"), w);
    actMaskByParamIntensity->setToolTip(tr("create a mask according to the <b>parameter image</b>:\n"
                                      "A dialog will open up, which allows to mask some pixels\n"
                                      "according to a given threshold. The mask created by this\n"
                                      "is combined with the current mask using the set <i>mask edit mode</i>"));
    glmask->addWidget(btnMaskByParamIntensity, mskgrpRow, 2);
    connect(actMaskByParamIntensity, SIGNAL(triggered()), this, SLOT(excludeByParamIntensity()));


    QGroupBox* wsel=new QGroupBox(tr(" selection options "), this);
    wsel->setFlat(true);
    vbl->addWidget(wsel);
    QGridLayout* glsel=new QGridLayout(this);
    glsel->setHorizontalSpacing(2);
    glsel->setVerticalSpacing(2);
    wsel->setLayout(glsel);

    int selgrpRow=0;

    btnSaveSelection=createButtonAndActionShowText(actSaveSelection, QIcon(":/imaging_fcs/saveselection.png"), tr("&save sel."), w);
    actSaveSelection->setToolTip(tr("save the selection to harddisk"));
    glsel->addWidget(btnSaveSelection, selgrpRow, 0);
    connect(actSaveSelection, SIGNAL(triggered()), this, SLOT(saveSelection()));
    btnLoadSelection=createButtonAndActionShowText(actLoadSelection, QIcon(":/imaging_fcs/loadselection.png"), tr("&load sel."), w);
    actLoadSelection->setToolTip(tr("load a selection from harddisk"));
    glsel->addWidget(btnLoadSelection, selgrpRow, 1);
    connect(actLoadSelection, SIGNAL(triggered()), this, SLOT(loadSelection()));


    btnCopySelection=createButtonAndActionShowText(actCopySelection, QIcon(":/imaging_fcs/copyselection.png"), tr("&copy sel."), w);
    actCopySelection->setToolTip(tr("copy the selection to clipboard"));
    glsel->addWidget(btnCopySelection, selgrpRow, 2);
    connect(actCopySelection, SIGNAL(triggered()), this, SLOT(copySelection()));
    btnPasteSelection=createButtonAndActionShowText(actPasteSelection, QIcon(":/imaging_fcs/pasteselection.png"), tr("&paste sel."), w);
    actPasteSelection->setToolTip(tr("paste a selection from clipboard"));
    glsel->addWidget(btnPasteSelection, selgrpRow, 3);
    connect(actPasteSelection, SIGNAL(triggered()), this, SLOT(pasteSelection()));
    selgrpRow++;
    frame=new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    glsel->addWidget(frame, selgrpRow, 0, 1, 4);
    selgrpRow++;

    glsel->addWidget(new QLabel(tr("stored selections:")), selgrpRow, 0);
    QWidget* ssel=new QWidget(this);
    QHBoxLayout* ssell=new QHBoxLayout(this);
    ssell->setContentsMargins(0,0,0,0);
    ssel->setLayout(ssell);
    glsel->addWidget(ssel, selgrpRow, 1,1,3);
    selgrpRow++;
    cmbStoredSelections=new QComboBox(this);
    ssell->addWidget(cmbStoredSelections, 1);
    btnSaveSelectionToStored=createButtonAndActionShowText(actSaveSelectionToStored, QIcon(":/imaging_fcs/storeselection.png"), tr("store selection "), w);
    btnSaveSelectionToStored->setToolButtonStyle(Qt::ToolButtonIconOnly);
    actSaveSelectionToStored->setToolTip(tr("save the current selection under a new name to the drop-down field above (record)"));
    ssell->addWidget(btnSaveSelectionToStored);
    btnDeleteStoredSelection=createButtonAndActionShowText(actDeleteStoredSelection, QIcon(":/imaging_fcs/deleteselection.png"), tr("delete selection "), w);
    btnDeleteStoredSelection->setToolButtonStyle(Qt::ToolButtonIconOnly);
    actDeleteStoredSelection->setToolTip(tr("delete the current named selection from the drop-down field above"));
    ssell->addWidget(btnDeleteStoredSelection);
    connect(actDeleteStoredSelection, SIGNAL(triggered()), this, SLOT(deleteSelection()));
    connect(actSaveSelectionToStored, SIGNAL(triggered()), this, SLOT(storeSelection()));
    connect(cmbStoredSelections, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSelectionInCombo(int)));






    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter image style
    ///////////////////////////////////////////////////////////////
    QGroupBox* wimg=new QGroupBox(tr(" parameter image style "), this);
    wimg->setFlat(true);
    vbl->addWidget(wimg);
    QFormLayout* gli=new QFormLayout(this);
    gli->setSpacing(2);
    wimg->setLayout(gli);

    cmbImageStyle=new QComboBox(wimg);
    cmbImageStyle->addItem(tr("masked image, black"));
    cmbImageStyle->addItem(tr("masked image, white"));
    cmbImageStyle->addItem(tr("masked image, grey"));
    cmbImageStyle->addItem(tr("image + overlay mask"));
    cmbImageStyle->addItem(tr("image only"));
    gli->addRow(tr("st&yle:"), cmbImageStyle);


    cmbColorbar=new QComboBox(wimg);
    QStringList sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorbar->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    gli->addRow(tr("color &palette:"), cmbColorbar);

    chkImageAutoScale=new QCheckBox("auto", wimg);
    chkImageAutoScale->setChecked(true);
    gli->addRow(tr("color &range:"), chkImageAutoScale);

    edtColMin=new QFDoubleEdit(wimg);
    edtColMin->setCheckBounds(false, false);
    edtColMax=new QFDoubleEdit(wimg);
    edtColMax->setCheckBounds(false, false);
    QHBoxLayout* coll=new QHBoxLayout();
    coll->addWidget(edtColMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtColMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);

    cmbOutOfRangeMode=new QComboBox(wimg);
    cmbOutOfRangeMode->addItem(tr("min/max color"));
    cmbOutOfRangeMode->addItem(tr("black"));
    cmbOutOfRangeMode->addItem(tr("white"));
    cmbOutOfRangeMode->addItem(tr("transparent"));
    cmbOutOfRangeMode->addItem(tr("red"));
    cmbOutOfRangeMode->addItem(tr("green"));
    cmbOutOfRangeMode->addItem(tr("blue"));
    gli->addRow(tr("out-&of-range mode:"), cmbOutOfRangeMode);

    chkDisplayImageOverlay=new QCheckBox(wimg);
    gli->addRow(tr("show &overlays:"), chkDisplayImageOverlay);

    cmbSelectionStyle=new QComboBox(wimg);
    QPixmap selPix(15,15);
    selPix.fill(ovlSelCol);
    cmbSelectionStyle->addItem(QIcon(selPix), tr("overlay image"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, ovlSelCol, ovlSelCol.lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("black"), QColor("black").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("black symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("white"), QColor("white").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("white symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("grey"), QColor("grey").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("grey symbol"));
    gli->addRow(tr("selection style:"), cmbSelectionStyle);


    ///////////////////////////////////////////////////////////////
    // GROUPBOX: overview image style
    ///////////////////////////////////////////////////////////////
    QGroupBox* wovr=new QGroupBox(tr(" overview image style "), this);
    wovr->setFlat(true);
    vbl->addWidget(wovr);
    gli=new QFormLayout(this);
    wovr->setLayout(gli);

    cmbColorbarOverview=new QComboBox(wovr);
    sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorbarOverview->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    gli->addRow(tr("color &palette:"), cmbColorbarOverview);

    chkAutorangeOverview=new QCheckBox("auto", wovr);
    chkAutorangeOverview->setChecked(true);
    gli->addRow(tr("color &range:"), chkAutorangeOverview);

    edtOvrMin=new QFDoubleEdit(wovr);
    edtOvrMin->setCheckBounds(false, false);
    edtOvrMax=new QFDoubleEdit(wovr);
    edtOvrMax->setCheckBounds(false, false);
    coll=new QHBoxLayout();
    coll->addWidget(edtOvrMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtOvrMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);




    ///////////////////////////////////////////////////////////////
    // GROUPBOX: correlation plot styles
    ///////////////////////////////////////////////////////////////
    QGroupBox* wcp=new QGroupBox(tr(" correlation plot styles "), this);
    wcp->setFlat(true);
    vbl->addWidget(wcp);
    QFormLayout* gl=new QFormLayout(this);
    gl->setSpacing(2);

    wcp->setLayout(gl);

    QHBoxLayout* cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    chkLogTauAxis=new QCheckBox("log tau-axis", w);

    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    cpsHBox->addWidget(chkDisplayAverage);
    cpsHBox->addWidget(chkLogTauAxis);
    l->setTextFormat(Qt::RichText);
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setBuddy(chkLogTauAxis);

    gl->addRow(tr("display average:"), cpsHBox);

    cmbAverageStyle=new QComboBox(w);
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbAverageErrorStyle=new QComboBox(w);
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_epoly.png"), tr("polygons"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_epolybars.png"), tr("polygons+bars"));
    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    cpsHBox->addWidget(cmbAverageStyle);
    cpsHBox->addWidget(cmbAverageErrorStyle);
    gl->addRow((l=new QLabel(tr("average options:"))), cpsHBox);
    l->setBuddy(cmbAverageStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));

    cmbRunStyle=new QComboBox(w);
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbRunErrorStyle=new QComboBox(w);
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_epoly.png"), tr("polygons"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_epolybars.png"), tr("polygons+bars"));
    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    cpsHBox->addWidget(cmbRunStyle);
    cpsHBox->addWidget(cmbRunErrorStyle);
    gl->addRow((labRunOptions=new QLabel(tr("pixel options:"))), cpsHBox);
    labRunOptions->setBuddy(cmbRunStyle);


    cpsHBox=new QHBoxLayout(this);
    cpsHBox->setContentsMargins(0,0,0,0);
    chkDisplayResiduals=new QCheckBox(w);
    chkDisplayResiduals->setChecked(true);

    chkKeys=new QCheckBox(tr("display &keys"), w);
    chkKeys->setChecked(true);

    cpsHBox->addWidget(chkDisplayResiduals);
    cpsHBox->addWidget(chkKeys);
    l->setTextFormat(Qt::RichText);
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setBuddy(chkKeys);
    gl->addRow(tr("display residuals:"), cpsHBox);

    cmbSeletionCorrDisplayMode=new QComboBox(this);
    cmbSeletionCorrDisplayMode->addItem(tr("display average"));
    cmbSeletionCorrDisplayMode->addItem(tr("display all"));
    gl->addRow(tr("selection display:"), cmbSeletionCorrDisplayMode);


    connectImageWidgets(true);






    ///////////////////////////////////////////////////////////////
    // GROUPBOX: overview image plot
    ///////////////////////////////////////////////////////////////
    QWidget* wpltOverview=new QWidget(this);
    QVBoxLayout* lpltOverview=new QVBoxLayout();
    lpltOverview->setSpacing(2);
    wpltOverview->setLayout(lpltOverview);
    pltOverview=new JKQtPlotter(wpltOverview);
    pltOverview->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltOverview->setObjectName("pltOverview");
    //lpltOverview->addWidget(new QLabel(tr("Overview:")));
    lpltOverview->addWidget(pltOverview, 1);
    pltOverview->get_plotter()->set_plotLabel(tr("\\textbf{Overview}"));
    pltOverview->get_plotter()->set_plotLabelFontSize(10);
    pltOverview->set_zoomByDoubleAndRightMouseClick(false);
    pltOverview->set_displayMousePosition(false);
    pltOverview->set_displayToolbar(true);
    pltOverview->get_plotter()->set_maintainAspectRatio(true);
    pltOverview->get_plotter()->set_aspectRatio(1);
    pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
    pltOverview->get_plotter()->set_axisAspectRatio(1);
    pltOverview->setXY(0,0,1,1);
    pltOverview->setAbsoluteXY(0,1,0,1);
    pltOverview->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltOverview->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltOverview->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getXAxis()->set_axisLabel("");
    pltOverview->get_plotter()->getYAxis()->set_axisLabel("");
    pltOverview->get_plotter()->setGrid(false);
    pltOverview->get_plotter()->set_useAntiAliasingForSystem(true);
    pltOverview->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltOverview->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltOverview, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltOverview, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltOverview, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltOverview, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltOverview, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteOverview=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, JKQTPMathImage::GRAY, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverview);

    plteOverviewSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltOverview->get_plotter());
    plteOverviewSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    pltOverview->addGraph(plteOverviewSelected);


    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltOverview->get_plotter());
    plteOverviewExcluded->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    pltOverview->addGraph(plteOverviewExcluded);

    plteImageData=NULL;






    ///////////////////////////////////////////////////////////////
    // WIDGET: mask image plot
    ///////////////////////////////////////////////////////////////
    QWidget* wpltMask=new QWidget(this);
    wpltMask->setVisible(false);
    QVBoxLayout* lpltMask=new QVBoxLayout();
    wpltMask->setLayout(lpltMask);
    pltMask=new JKQtPlotter(wpltMask);
    pltMask->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltMask->setObjectName("pltMask");
    pltMask->get_plotter()->set_plotLabel(tr("\\textbf{Mask/Selected}"));
    pltMask->get_plotter()->set_plotLabelFontSize(10);

    //lpltMask->addWidget(new QLabel(tr("Mask/Selected:")));
    lpltMask->addWidget(pltMask, 1);
    pltMask->set_zoomByDoubleAndRightMouseClick(false);
    pltMask->set_displayMousePosition(false);
    pltMask->set_displayToolbar(true);
    pltMask->get_plotter()->set_maintainAspectRatio(true);
    pltMask->get_plotter()->set_aspectRatio(1);
    pltMask->get_plotter()->set_maintainAxisAspectRatio(true);
    pltMask->get_plotter()->set_axisAspectRatio(1);
    pltMask->setXY(0,0,1,1);
    pltMask->setAbsoluteXY(0,1,0,1);
    pltMask->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltMask->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltMask->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getXAxis()->set_axisLabel("");
    pltMask->get_plotter()->getYAxis()->set_axisLabel("");
    pltMask->get_plotter()->setGrid(false);
    pltMask->get_plotter()->set_useAntiAliasingForSystem(true);
    pltMask->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltMask->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltMask, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltMask, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltMask, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltMask, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltMask, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteMask=new JKQTPOverlayImage(0,0,1,1,NULL, 0,0, QColor("black"), pltMask->get_plotter());
    plteMask->set_falseColor(QColor("white"));
    pltMask->addGraph(plteMask);

    plteMaskSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltMask->get_plotter());
    plteMaskSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);

    pltMask->addGraph(plteMaskSelected);










    ///////////////////////////////////////////////////////////////
    // WIDGET: parameter image plot
    ///////////////////////////////////////////////////////////////

    QWidget* wpltImage=new QWidget(this);
    QVBoxLayout* lpltImage=new QVBoxLayout();
    wpltImage->setLayout(lpltImage);
    pltImage=new JKQtPlotter(wpltImage);
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltImage->setObjectName("pltImage");
    pltImage->get_plotter()->set_plotLabel(tr("\\textbf{Parameter Image}"));
    pltImage->get_plotter()->set_plotLabelFontSize(10);

    //lpltImage->addWidget((labParamImage=new QLabel(tr("Parameter Image:"))));
    lpltImage->addWidget(pltImage, 1);

    pltImage->set_zoomByDoubleAndRightMouseClick(false);
    pltImage->set_displayMousePosition(false);
    pltImage->set_displayToolbar(true);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);

    pltImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltImage->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltImage, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltImage, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltImage, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltImage->get_plotter());
    pltImage->addGraph(plteImage);


    plteImageSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltImage->get_plotter());
    plteImageSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    //pltImage->addGraph(plteImageSelected);


    plteImageExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltImage->get_plotter());
    plteImageExcluded->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    //pltImage->addGraph(plteImageExcluded);

    plteImageData=NULL;








    ///////////////////////////////////////////////////////////////
    // WIDGET: goodnes of fit image plot
    ///////////////////////////////////////////////////////////////

    QWidget* wpltGofImage=new QWidget(this);
    QVBoxLayout* lpltGofImage=new QVBoxLayout();
    wpltGofImage->setLayout(lpltGofImage);
    pltGofImage=new JKQtPlotter(wpltGofImage);
    pltGofImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltGofImage->setObjectName("pltGofImage");
    pltGofImage->get_plotter()->set_plotLabel(tr("\\textbf{\"Goodnes of Fit\" Image}"));
    pltGofImage->get_plotter()->set_plotLabelFontSize(10);

    //lpltGofImage->addWidget((labParamImage=new QLabel(tr("\"Goodnes of Fit\" Image:"))));
    lpltGofImage->addWidget(pltGofImage, 1);

    pltGofImage->set_zoomByDoubleAndRightMouseClick(false);
    pltGofImage->set_displayMousePosition(false);
    pltGofImage->set_displayToolbar(true);
    pltGofImage->get_plotter()->set_maintainAspectRatio(true);
    pltGofImage->get_plotter()->set_aspectRatio(1);
    pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltGofImage->get_plotter()->set_axisAspectRatio(1);
    pltGofImage->setXY(0,0,1,1);
    pltGofImage->setAbsoluteXY(0,1,0,1);

    pltGofImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltGofImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltGofImage->get_plotter()->setGrid(false);
    pltGofImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltGofImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltGofImage->set_userActionCompositionMode(QPainter::CompositionMode_Xor);
    connect(pltGofImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltGofImage, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltGofImage, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltGofImage, SIGNAL(plotMouseMove(double,double)), this, SLOT(imageMouseMoved(double,double)));
    connect(pltGofImage, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltGofImage, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltGofImage, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltGofImage, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));


    plteGofImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltGofImage->get_plotter());
    pltGofImage->addGraph(plteGofImage);


    plteGofImageSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltGofImage->get_plotter());
    plteGofImageSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);
    plteGofImageExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltGofImage->get_plotter());
    plteGofImageExcluded->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);

    plteImageData=NULL;

    connect(pltOverview, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));
    connect(pltImage, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));
    connect(pltGofImage, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));






    ///////////////////////////////////////////////////////////////
    // WIDGET: correlation&residuals plot
    ///////////////////////////////////////////////////////////////

    plotter = new JKQtPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->setObjectName("plotter");

    plotter->get_plotter()->set_showKey(true);
    plotterResid = new JKQtPlotter(true, this);
    plotterResid->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotterResid->setObjectName("plotterResid");

    plotterResid->get_plotter()->set_showKey(true);


    plotter->get_plotter()->set_gridPrinting(true);
    plotter->get_plotter()->addGridPrintingPlotter(0,1,plotterResid->get_plotter());
    plotter->set_displayToolbar(true);
    plotterResid->set_displayToolbar(false);
    plotterResid->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotterResid->getXAxis()->set_labelFontSize(11);
    plotterResid->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_axisLabel(tr("residuals"));
    plotterResid->getYAxis()->set_labelFontSize(11);
    plotterResid->getYAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_minTicks(5);
    plotter->getXAxis()->set_axisLabel("");
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->getYAxis()->set_labelFontSize(11);
    plotter->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_drawMode1(JKQTPCADMticks);
    plotter->getXAxis()->set_tickLabelFontSize(10);
    plotter->getYAxis()->set_minTicks(5);
    plotterResid->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    plotterResid->get_plotter()->setBorder(1,1,1,1);
    plotter->get_plotter()->setBorder(1,1,1,1);
    plotterResid->synchronizeToMaster(plotter, true, false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotterResid->get_plotter()->set_useAntiAliasingForSystem(true);
    plotterResid->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotter->set_displayMousePosition(false);
    plotterResid->set_displayMousePosition(false);
    plotter->get_plotter()->set_keyFontSize(9);
    plotter->get_plotter()->set_keyXMargin(2);
    plotter->get_plotter()->set_keyYMargin(2);
    plotterResid->get_plotter()->set_keyFontSize(9);
    plotterResid->get_plotter()->set_keyXMargin(2);
    plotterResid->get_plotter()->set_keyYMargin(2);
    plotterResid->useExternalDatastore(plotter->getDatastore());
    connect(plotter, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(acfZoomChangedLocally(double,double,double,double,JKQtPlotter*)));



    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    lp->setContentsMargins(0,0,0,0);
    wp->setLayout(lp);
    lp->addWidget(plotterResid,1);
    sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);


    tvParams=new QEnhancedTableView();
    tvParams->setItemDelegate(new QFHTMLDelegate(tvParams));
    tvParams->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvParams->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));
    tabFitvals=new QFTableModel(tvParams);
    tabFitvals->setReadonly(true);



    splitterTopBot=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterTop=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterTop->addWidget(wpltImage);
    splitterTop->addWidget(wpltGofImage);
    splitterTop->addWidget(wpltMask);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    /*splitterBotPlots=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterBotPlots->addWidget(plotter);
    splitterBotPlots->addWidget(wp);*/
    QVBoxLayout* layPlotsBot=new QVBoxLayout(this);
    QWidget* wPlotsBot=new QWidget(this);
    wPlotsBot->setLayout(layPlotsBot);
    layPlotsBot->setContentsMargins(0,0,0,0);
    layPlotsBot->addWidget(plotter,3);
    layPlotsBot->addWidget(wp,2);
    splitterBot=new QVisibleHandleSplitter(Qt::Horizontal, this);
    //splitterBot->addWidget(splitterBotPlots);
    splitterBot->addWidget(wPlotsBot);
    splitterBot->addWidget(tvParams);
    splitterTopBot->addWidget(splitterBot);

    vbl->addWidget(new QWidget(this), 1);








    ///////////////////////////////////////////////////////////////
    // GROUPBOX: buttons for data/report saving
    ///////////////////////////////////////////////////////////////

    QGridLayout* grdTop=new QGridLayout(this);
    btnPrintReport = createButtonAndActionShowText(actPrintReport, QIcon(":/imaging_fcs/report_print.png"), tr("&Print report"), this);
    actPrintReport->setToolTip(tr("print a report which contains all data on the current screen:<br><ul>"
                                  "<li>all images (parameter, mask, goodnes-of-fit, overview</li>"
                                  "<li>correlation curves and fit parameters</li>"
                                  "<li>histpgram and statistics</li>"
                                  "<li>additional data (files, description configuration ...)</li>"
                                  "</ul>"));

    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    btnSaveReport = createButtonAndActionShowText(actSaveReport, QIcon(":/imaging_fcs/report_save.png"), tr("&Save report"), this);
    actSaveReport->setToolTip(tr("save a report which contains all data on the current screen as PDF or PostScript file:<br><ul>"
                                  "<li>all images (parameter, mask, goodnes-of-fit, overview)</li>"
                                  "<li>correlation curves and fit parameters</li>"
                                  "<li>histpgram and statistics</li>"
                                  "<li>additional data (files, description configuration ...)</li>"
                                  "</ul>"));
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    btnSaveData = createButtonAndActionShowText(actSaveData, QIcon(":/imaging_fcs/preview_savedata.png"), tr("Save &data"), this);
    actSaveData->setToolTip(tr("save the currently displayed images (parameter, mask, goodnes-of-fit, overview)\nas image files (e.g. TIFF), so they can be processed in other programs."));
    connect(actSaveData, SIGNAL(triggered()), this, SLOT(saveData()));

    actInsertSelectedCorrelationsAsFCSRDR=new QAction(tr("Insert Correlation Curves as new RDR into Project"), this);
    connect(actInsertSelectedCorrelationsAsFCSRDR, SIGNAL(triggered()), this, SLOT(insertSelectedCorrelationsAsFCSRDR()));

    btnCopyDataToMatlab = createButtonAndActionShowText(actCopyDataToMatlab, QIcon(":/imaging_fcs/copydatatomatlab.png"), tr("Copy Images to &Matlab"), this);
    actCopyDataToMatlab->setToolTip(tr("copy the currently dispalyed images (parameter, mask, goodnes-of-fit, overview) as a Matlab script."));
    connect(actCopyDataToMatlab, SIGNAL(triggered()), this, SLOT(copyToMatlab()));
    btnCopyDataAsColumns = createButtonAndActionShowText(actCopyDataAsColumns, QIcon(":/imaging_fcs/copydata.png"), tr("Copy Images as &Columns"), this);
    actCopyDataAsColumns->setToolTip(tr("copy the currently dispalyed images (parameter, mask, goodnes-of-fit, overview) as columns of data to the clipboard. The data may be pasted e.g. into a spreadsheet program like Excel"));
    connect(actCopyDataAsColumns, SIGNAL(triggered()), this, SLOT(copyDataAsColumns()));
    grdTop->addWidget(grpTop, 0, 2, 3, 1);
    grdTop->addWidget(btnSaveData, 0, 0);
    grdTop->addWidget(btnCopyDataToMatlab, 1, 0);
    grdTop->addWidget(btnCopyDataAsColumns, 2, 0);
    grdTop->addWidget(btnSaveReport, 0, 1);
    grdTop->addWidget(btnPrintReport, 1, 1);
    grdTop->setColumnStretch(1,0);
    grdTop->setColumnStretch(0,0);









    ///////////////////////////////////////////////////////////////
    // TOOLBAR & ACTIONS: edit image plots
    ///////////////////////////////////////////////////////////////
    actImagesZoom=new QAction(QIcon(":/imaging_fcs/zoom.png"), tr("zoom"), this);
    actImagesZoom->setToolTip(tr("in this mode the user may zoom into a plot by drawing a rectangle (draging with the left mouse key)\nA click toggles the current selection/mask position."));
    actImagesZoom->setCheckable(true);
    actImagesDrawRectangle=new QAction(QIcon(":/imaging_fcs/draw_rectangle.png"), tr("rectangular selection"), this);
    actImagesDrawRectangle->setToolTip(tr("in this mode the user may draw a rectangle.<br>"
                                 "All pixels inside the rectangle will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawRectangle->setCheckable(true);

    actImagesDrawPoints=new QAction(QIcon(":/imaging_fcs/draw_point.png"), tr("point-wise selection"), this);
    actImagesDrawPoints->setToolTip(tr("in this mode the user may click on single points.<br>"
                                 "to add and remove them to/from the mask/selection. A click<br>"
                                 "will toggle the state of the clicked pixel<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawPoints->setCheckable(true);

    actImagesDrawCircle=new QAction(QIcon(":/imaging_fcs/draw_circle.png"), tr("circular selection"), this);
    actImagesDrawCircle->setToolTip(tr("in this mode the user may draw a circle.<br>"
                                 "All pixels inside the cirle will be selected<br>"
                                 "when the user releases the left mouse key. The point of first<br>"
                                 "click will be the center of the circle. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawCircle->setCheckable(true);
    actImagesDrawEllipse=new QAction(QIcon(":/imaging_fcs/draw_ellipse.png"), tr("elliptical selection"), this);
    actImagesDrawEllipse->setToolTip(tr("in this mode the user may draw a ellipse.<br>"
                                 "All pixels inside the ellipse will be selected<br>"
                                "when the user releases the left mouse key. The point of first<br>"
                                "click will be the center of the ellipse. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawEllipse->setCheckable(true);
    actImagesDrawLine=new QAction(QIcon(":/imaging_fcs/draw_line.png"), tr("line selection"), this);
    actImagesDrawLine->setToolTip(tr("in this mode the user may draw a line.<br>"
                                 "All pixels on the line will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawLine->setCheckable(true);

    actImagesScribble=new QAction(QIcon(":/imaging_fcs/draw_scribble.png"), tr("scribble selection"), this);
    actImagesScribble->setToolTip(tr("in this mode the user may select/deselect pixels by.<br>"
                                 "keeping the left mouse button pressed and moving the mouse<br>"
                                 "over the image. Depending on the key pressed on the keyboard,<br>"
                                 "different actions are executed:<ul>"
                                 "<li>CTRL: the old selection/mask is not deleted, so the result is appended to it.</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesScribble->setCheckable(true);
    agImageSelectionActions=new QActionGroup(this);
    agImageSelectionActions->setExclusive(true);
    agImageSelectionActions->addAction(actImagesZoom);
    agImageSelectionActions->addAction(actImagesDrawPoints);
    agImageSelectionActions->addAction(actImagesDrawRectangle);
    agImageSelectionActions->addAction(actImagesScribble);
    agImageSelectionActions->addAction(actImagesDrawLine);
    agImageSelectionActions->addAction(actImagesDrawCircle);
    agImageSelectionActions->addAction(actImagesDrawEllipse);
    tbParameterImage=new QToolBar(this);
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomAll());
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomIn());
    tbParameterImage->addAction(pltImage->get_plotter()->get_actZoomOut());
    tbParameterImage->addSeparator();
    tbParameterImage->addWidget(new QLabel(tr("    edit "), this));
    cmbMaskEditMode=new QComboBox(this);
    cmbMaskEditMode->addItem("selection");
    cmbMaskEditMode->addItem("mask");
    QWidget* spacer=new QWidget(this);
    spacer->setMinimumWidth(20);
    tbParameterImage->addWidget(cmbMaskEditMode);
    tbParameterImage->addWidget(spacer);
    tbParameterImage->addAction(actImagesZoom);
    tbParameterImage->addAction(actImagesDrawPoints);
    tbParameterImage->addAction(actImagesDrawRectangle);
    tbParameterImage->addAction(actImagesScribble);
    tbParameterImage->addAction(actImagesDrawLine);
    tbParameterImage->addAction(actImagesDrawCircle);
    tbParameterImage->addAction(actImagesDrawEllipse);
    labImagePositionDisplay=new QLabel(this);
    labImageAvg=new QLabel(this);
    tbParameterImage->addSeparator();
    spacer=new QWidget(this);
    spacer->setMinimumWidth(20);
    tbParameterImage->addWidget(spacer);
    tbParameterImage->addWidget(labImagePositionDisplay);
    tbParameterImage->addSeparator();
    tbParameterImage->addWidget(labImageAvg);
    connect(agImageSelectionActions, SIGNAL(triggered(QAction*)), this, SLOT(setImageEditMode()));
    actImagesDrawPoints->setChecked(true);
    actImagesZoom->setChecked(true);
    actImagesDrawPoints->setChecked(true);
    setImageEditMode();






    //////////////////////////////////////////////////////////////////////////////////////////
    // CORRELATION FUNCTION TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widACFs=new QWidget(this);
    QGridLayout* layACFs=new QGridLayout();
    widACFs->setLayout(layACFs);
    layACFs->addWidget(tbParameterImage, 0, 0, 1, 2);
    layACFs->addWidget(splitterTopBot, 1, 0);
    layACFs->addWidget(area, 1, 1); // !!!
    layACFs->setColumnStretch(0,10);
    layACFs->setRowStretch(1,0);
    layACFs->setContentsMargins(0,0,0,0);



    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    histogram=new QFHistogramView(this);
    histogram->setMinimumHeight(200);
    histogram2=new QFHistogramView(this);
    histogram2->setMinimumHeight(200);
    chkExcludeExcludedRunsFromHistogram=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    chkExcludeExcludedRunsFromHistogram2=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram2->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    histogram->addSettingsWidget(tr("mind excluded runs:"), chkExcludeExcludedRunsFromHistogram);
    histogram2->addSettingsWidget(tr("mind excluded runs:"), chkExcludeExcludedRunsFromHistogram2);
    histogram2->setVisible(false);

    QWidget* widHist=new QWidget(this); //=histogram;
    QVBoxLayout* histLay=new QVBoxLayout(this);
    widHist->setLayout(histLay);
    histLay->addWidget(histogram, 5);
    histLay->addWidget(histogram2, 5);
    histLay->addStretch(3);



    //////////////////////////////////////////////////////////////////////////////////////////
    // PUT EVERYTHING TOGETHER
    //////////////////////////////////////////////////////////////////////////////////////////
    tabDisplay=new QTabWidget(this);
    tabDisplay->addTab(widACFs, tr("&Correlations"));
    tabDisplay->addTab(widHist, tr("&Histogram"));


    //lb->addWidget(grpTop, 1, 0, 1, 2);
    lb->addLayout(grdTop, 0,0);
    lb->addWidget(tabDisplay, 1,0);



    connect(chkOverviewVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkGofVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkMaskVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));



    connectParameterWidgets();





    menuData=propertyEditor->addMenu("&Data", 0);
    menuData->addAction(actSaveData);
    menuData->addAction(actCopyDataToMatlab);
    menuData->addAction(actCopyDataAsColumns);
    menuData->addAction(actInsertSelectedCorrelationsAsFCSRDR);
    menuData->addSeparator();
    menuData->addAction(actSaveReport);
    menuData->addAction(actPrintReport);

    menuMask=propertyEditor->addMenu("&Mask", 0);
    menuMask->addAction(actUse);
    menuMask->addAction(actDontUse);
    menuMask->addAction(actUseAll);
    menuMask->addAction(actInvertMask);
    menuMask->addAction(actMaskByIntensity);
    menuMask->addAction(actMaskByGofIntensity);
    menuMask->addAction(actMaskByParamIntensity);
    menuMask->addSeparator();
    menuMask->addAction(actSaveMask);
    menuMask->addAction(actLoadMask);
    menuMask->addSeparator();
    menuMask->addAction(actCopyMask);
    menuMask->addAction(actPasteMask);

    menuSelection=propertyEditor->addMenu("&Selection", 0);
    menuSelection->addAction(actSaveSelection);
    menuSelection->addAction(actLoadSelection);
    menuSelection->addSeparator();
    menuSelection->addAction(actCopySelection);
    menuSelection->addAction(actPasteSelection);


}

void QFRDRImagingFCSImageEditor::saveImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else {
            current->setQFProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param), cmbColorbar->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), chkDisplayImageOverlay->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_ovstyle_%1_%2").arg(egroup).arg(param), cmbSelectionStyle->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), chkImageAutoScale->isChecked(), false, false);
            if (!chkImageAutoScale->isChecked()) {
                current->setQFProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), edtColMin->value(), false, false);
                current->setQFProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), edtColMax->value(), false, false);
            }
            current->setQFProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), histogram->getBins(), false, false);
            current->setQFProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), histogram->getNormalized(), false, false);
            current->setQFProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), histogram->getLog(), false, false);
            current->setQFProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), chkExcludeExcludedRunsFromHistogram->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), histogram->getAutorange(), false, false);
            if (!histogram->getAutorange()) {
                current->setQFProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), histogram->getMin(), false, false);
                current->setQFProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), histogram->getMax(), false, false);
            }

            current->setQFProperty(QString("imfcs_imed_hist2bins_%1_%2").arg(egroup).arg(param), histogram2->getBins(), false, false);
            current->setQFProperty(QString("imfcs_imed_hist2norm_%1_%2").arg(egroup).arg(param), histogram2->getNormalized(), false, false);
            current->setQFProperty(QString("imfcs_imed_hist2log_%1_%2").arg(egroup).arg(param), histogram2->getLog(), false, false);
            current->setQFProperty(QString("imfcs_imed_hist2ex_%1_%2").arg(egroup).arg(param), chkExcludeExcludedRunsFromHistogram2->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_hist2rauto_%1_%2").arg(egroup).arg(param), histogram2->getAutorange(), false, false);
            if (!histogram2->getAutorange()) {
                current->setQFProperty(QString("imfcs_imed_hist2rmin_%1_%2").arg(egroup).arg(param), histogram2->getMin(), false, false);
                current->setQFProperty(QString("imfcs_imed_hist2rmax_%1_%2").arg(egroup).arg(param), histogram2->getMax(), false, false);
            }

            current->setQFProperty(QString("imfcs_imed_ovrcolorbar_%1").arg(egroup), cmbColorbarOverview->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_ovrautorange_%1").arg(egroup), chkAutorangeOverview->isChecked(), false, false);
            if (!chkAutorangeOverview->isChecked()) {
                current->setQFProperty(QString("imfcs_imed_ovrcolmin_%1").arg(egroup), edtOvrMin->value(), false, false);
                current->setQFProperty(QString("imfcs_imed_ovrcolmax_%1").arg(egroup), edtOvrMax->value(), false, false);
            }


        }

    }
}

void QFRDRImagingFCSImageEditor::loadImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else if (settings) {
            double mi=0, ma=1;
            connectParameterWidgets(false);
            //histogram->connectParameterWidgets(false);
            plteImage->getDataMinMax(mi, ma);
            int d=current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                                       settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex())).toInt();
            if (d>=0) cmbColorbar->setCurrentIndex(d);
            else if (cmbColorbar->count()>0) cmbColorbar->setCurrentIndex(0);
            cmbImageStyle->setCurrentIndex(current->getProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex()).toInt());
            cmbOutOfRangeMode->setCurrentIndex(current->getProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex()).toInt());
            chkDisplayImageOverlay->setChecked(current->getProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), true).toBool());
            cmbSelectionStyle->setCurrentIndex(current->getProperty(QString("imfcs_imed_ovstyle_%1_%2").arg(egroup).arg(param), false).toBool());
            chkImageAutoScale->setChecked(current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!chkImageAutoScale->isChecked()) {
                edtColMin->setValue(current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi).toDouble());
                edtColMax->setValue(current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma).toDouble());
            }
            histogram->setBins(current->getProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), 100).toInt());
            histogram->setNormalized(current->getProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram->setLog(current->getProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), false).toBool());
            chkExcludeExcludedRunsFromHistogram->setChecked(current->getProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram->setAutorange(current->getProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!histogram->getAutorange()) {
                histogram->setMin(current->getProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
                histogram->setMax(current->getProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
            }

            histogram2->setBins(current->getProperty(QString("imfcs_imed_hist2bins_%1_%2").arg(egroup).arg(param), 100).toInt());
            histogram2->setNormalized(current->getProperty(QString("imfcs_imed_hist2norm_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram2->setLog(current->getProperty(QString("imfcs_imed_hist2log_%1_%2").arg(egroup).arg(param), false).toBool());
            chkExcludeExcludedRunsFromHistogram2->setChecked(current->getProperty(QString("imfcs_imed_hist2ex_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram2->setAutorange(current->getProperty(QString("imfcs_imed_hist2rauto_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!histogram2->getAutorange()) {
                histogram2->setMin(current->getProperty(QString("imfcs_imed_hist2rmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
                histogram2->setMax(current->getProperty(QString("imfcs_imed_hist2rmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
            }
            mi=0, ma=1;
            plteOverview->getDataMinMax(mi, ma);
            d=current->getProperty(QString("imfcs_imed_ovrcolorbar_%1").arg(egroup),
                                       settings->getQSettings()->value(QString("imfcsimageeditor/ovrcolorbar"), cmbColorbarOverview->currentIndex())).toInt();
            if (d>=0) cmbColorbarOverview->setCurrentIndex(d);
            else if (cmbColorbarOverview->count()>0) cmbColorbarOverview->setCurrentIndex(0);
            chkAutorangeOverview->setChecked(current->getProperty(QString("imfcs_imed_ovrautorange_%1").arg(egroup), true).toBool());
            edtOvrMin->setEnabled(!chkAutorangeOverview->isChecked());
            edtOvrMax->setEnabled(!chkAutorangeOverview->isChecked());
            if (!chkAutorangeOverview->isChecked()) {
                edtOvrMin->setValue(current->getProperty(QString("imfcs_imed_ovrcolmin_%1").arg(egroup), mi).toDouble());
                edtOvrMax->setValue(current->getProperty(QString("imfcs_imed_ovrcolmax_%1").arg(egroup), ma).toDouble());
            }

            connectParameterWidgets(true);

            pltImage->update_plot();
        }

    }
}

/*void QFRDRImagingFCSImageEditor::replotOverlays() {
    pltOverview->set_doDrawing(false);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);
    pltMask->set_doDrawing(false);
    updateOverlaySettings();
    pltOverview->set_doDrawing(true);
    pltImage->set_doDrawing(true);
    pltGofImage->set_doDrawing(true);
    pltMask->set_doDrawing(true);
    if (replot) {
        pltOverview->update_plot();
        pltImage->update_plot();
        pltGofImage->update_plot();
        pltMask->update_plot();
    }
}*/

void QFRDRImagingFCSImageEditor::updateOverlaySettings() {
    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteGofImageSelected, false);
        pltGofImage->deleteGraph(plteGofImageExcluded, false);
        pltMask->deleteGraph(plteMaskSelected, false);
    } else {
        pltImage->addGraph(plteImageExcluded);
        pltImage->addGraph(plteImageSelected);
        pltGofImage->addGraph(plteGofImageSelected);
        pltGofImage->addGraph(plteGofImageExcluded);
        pltMask->addGraph(plteMaskSelected);
    }

    setSelectedOverlayStyle(plteOverviewSelected);
    setSelectedOverlayStyle(plteImageSelected);
    setSelectedOverlayStyle(plteGofImageSelected);
    setSelectedOverlayStyle(plteMaskSelected);

    if (cmbImageStyle->currentIndex()<cmbImageStyle->count()-1) {
        pltImage->deleteGraph(plteImageExcluded, false);
        pltImage->addGraph(plteImageExcluded);
    }
    if (cmbImageStyle->currentIndex()==cmbImageStyle->count()-1) {
        pltImage->deleteGraph(plteImageExcluded, false);
    }
    if (cmbImageStyle->currentIndex()==0) {
        plteImageExcluded->set_trueColor(QColor("black"));
    } else if (cmbImageStyle->currentIndex()==1) {
        plteImageExcluded->set_trueColor(QColor("white"));
    } else if (cmbImageStyle->currentIndex()==2) {
        plteImageExcluded->set_trueColor(QColor("grey"));
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(excludedColor);
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(Qt::transparent);
    }

    if (pltImage->containsGraph(plteImageSelected)) pltImage->moveGraphTop(plteImageSelected);
    saveImageSettings();

}

void QFRDRImagingFCSImageEditor::paletteChanged() {
    bool oldDoDraw=pltImage->get_doDrawing();
    pltImage->set_doDrawing(false);

    plteImage->set_palette(cmbColorbar->currentIndex());
    plteImage->set_autoImageRange(chkImageAutoScale->isChecked());
    //plteImage->set_autoImageRange(false);
    if (chkImageAutoScale->isChecked() && plteImageData!=NULL && plteOverviewExcludedData!=NULL) {
        double mi, ma;
        statisticsMaskedMinMax(plteImageData, plteOverviewExcludedData, plteImageSize, mi, ma, false);
        plteImage->set_imageMin(mi);
        plteImage->set_imageMax(ma);
    }
    switch(cmbOutOfRangeMode->currentIndex()) {
        case 0:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::LastPaletteColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::LastPaletteColor);
            break;
        default:
        case 1:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("black"));
            plteImage->set_rangeMaxFailColor(QColor("black"));
            break;
        case 2:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("white"));
            plteImage->set_rangeMaxFailColor(QColor("white"));
            break;
        case 3:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::Transparent);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::Transparent);
            break;
        case 4:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("red"));
            plteImage->set_rangeMaxFailColor(QColor("red"));
            break;
        case 5:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("green"));
            plteImage->set_rangeMaxFailColor(QColor("green"));
            break;
        case 6:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("blue"));
            plteImage->set_rangeMaxFailColor(QColor("blue"));
            break;
    }

    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    if (!chkImageAutoScale->isChecked()) {
        plteImage->set_imageMin(edtColMin->value());
        plteImage->set_imageMax(edtColMax->value());
    } else {
        double mi=0, ma=0;
        plteImage->getDataMinMax(mi, ma);
        edtColMin->setValue(mi);
        edtColMax->setValue(ma);
    }
    saveImageSettings();
    pltImage->set_doDrawing(oldDoDraw);
    if (oldDoDraw) pltImage->update_plot();
}


void QFRDRImagingFCSImageEditor::ovrPaletteChanged() {
    bool oldDoDraw=pltOverview->get_doDrawing();
    pltOverview->set_doDrawing(false);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    plteOverview->set_palette(cmbColorbarOverview->currentIndex());
    plteOverview->set_autoImageRange(chkAutorangeOverview->isChecked());
    if (m && chkAutorangeOverview->isChecked() && m->getImageFromRunsPreview()!=NULL && plteOverviewExcludedData!=NULL) {
        double mi, ma;
        statisticsMaskedMinMax(m->getImageFromRunsPreview(), plteOverviewExcludedData, m->getImageFromRunsWidth()*m->getImageFromRunsHeight(), mi, ma, false);
        plteOverview->set_imageMin(mi);
        plteOverview->set_imageMax(ma);
    }

    edtOvrMin->setEnabled(!chkAutorangeOverview->isChecked());
    edtOvrMax->setEnabled(!chkAutorangeOverview->isChecked());
    if (!chkAutorangeOverview->isChecked()) {
        plteOverview->set_imageMin(edtOvrMin->value());
        plteOverview->set_imageMax(edtOvrMax->value());
    } else {
        double mi=0, ma=0;
        plteOverview->getDataMinMax(mi, ma);
        //qDebug()<<"ovrPaletteChanged: "<<oldDoDraw<<"   range: "<<mi<<"..."<<ma;
        edtOvrMin->setValue(mi);
        edtOvrMax->setValue(ma);
    }
    saveImageSettings();
    pltOverview->set_doDrawing(oldDoDraw);
    if (oldDoDraw) pltOverview->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::histogramSettingsChanged() {
    histogram->histogramSettingsChanged(false);
    histogram2->histogramSettingsChanged(false);
    updateHistogram();
    saveImageSettings();
}

void QFRDRImagingFCSImageEditor::excludeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutAddRun(run);
        }
        m->recalcCorrelations();
    }
    rawDataChanged();
}

void QFRDRImagingFCSImageEditor::includeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutRemoveRun(run);
        }
        m->recalcCorrelations();
    }
    rawDataChanged();
}



void QFRDRImagingFCSImageEditor::excludeByImage(double* imageIn) {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QFRDRImagingFCSMaskByIntensity* dialog=new QFRDRImagingFCSMaskByIntensity(this);
        bool* mask=(bool*)malloc(m->getImageFromRunsWidth()*m->getImageFromRunsHeight()*sizeof(bool));
        double* image=(double*)malloc(m->getImageFromRunsWidth()*m->getImageFromRunsHeight()*sizeof(double));
        for (int i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
            mask[i]=false;
            image[i]=imageIn[i];
        }
        dialog->init(mask, image, m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), cmbDualView->currentIndex());
        if (dialog->exec()==QDialog::Accepted) {
            if (dialog->getMaskMode()==2) {
                bool* newMask=m->maskGet();
                for (int i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
                    newMask[i]=newMask[i] && (!mask[i]);
                }
            } else if (dialog->getMaskMode()==1) {
                bool* newMask=m->maskGet();
                for (int i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
                    newMask[i]=newMask[i]||mask[i];
                }
            } else {
                memcpy(m->maskGet(), mask, m->getImageFromRunsWidth()*m->getImageFromRunsHeight()*sizeof(bool));
            }
            m->recalcCorrelations();
            rawDataChanged();
        }
        free(mask);
        free(image);
    }
}

void QFRDRImagingFCSImageEditor::excludeByParamIntensity() {
    excludeByImage(plteImageData);
}

void QFRDRImagingFCSImageEditor::excludeByGOFIntensity() {
    excludeByImage(plteGofImageData);
}

void QFRDRImagingFCSImageEditor::loadMask() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString filename= qfGetOpenFileName(this, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        if (m) {
            m->maskLoad(filename);
            m->recalcCorrelations();
        }
    }

    rawDataChanged();
}

void QFRDRImagingFCSImageEditor::saveMask() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString filename= qfGetSaveFileName(this, tr("save mask as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        m->maskSave(filename);
    }
}

void QFRDRImagingFCSImageEditor::pasteMask() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        m->maskLoadFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
        m->recalcCorrelations();
    }

    rawDataChanged();
}


void QFRDRImagingFCSImageEditor::copyMask() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString mask=m->maskToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    mime->setData("quickfit3/pixelselection", mask.toUtf8());
    clipboard->setMimeData(mime);
}

QString QFRDRImagingFCSImageEditor::selectionToString()  {
    if (!current) return "";
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return "";

    QString data="";
    QTextStream str(&data);
    QSet<int32_t>::iterator i = selected.begin();
    while (i != selected.end()) {
        int x=m->runToX(*i);
        int y=m->runToY(*i);
        str<<x<<", "<<y<<"\n";
        ++i;
    }
    return data;
}

void QFRDRImagingFCSImageEditor::loadSelectionFromString(QString sel) {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QTextStream str(&sel);
    selected.clear();
    while (!str.atEnd())  {
        QVector<double> d=csvReadline(str, ',', '#', -1);
        if (d.size()==2) {
            int idx=m->xyToRun(d[0], d[1]);
            if (idx>=0 && idx<m->getImageFromRunsWidth()*m->getImageFromRunsHeight()) selectedInsert(idx);
        }
    }

}

bool QFRDRImagingFCSImageEditor::indexIsDualView2(int32_t sel) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return false;
    return m->indexIsDualView2(sel);
}


void QFRDRImagingFCSImageEditor::saveSelection() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString filename= qfGetSaveFileName(this, tr("save selection as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        QFile f(filename);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream str(&f);
            str<<selectionToString();
            f.close();
        }
    }
}

void QFRDRImagingFCSImageEditor::loadSelection() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString filename= qfGetOpenFileName(this, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            loadSelectionFromString(f.readAll());
            f.close();
            selectionEdited();
        }
    }

    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}

void QFRDRImagingFCSImageEditor::copySelection() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    QString mask=selectionToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    mime->setData("quickfit3/pixelselection", mask.toUtf8());
    clipboard->setMimeData(mime);

}

void QFRDRImagingFCSImageEditor::pasteSelection() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        loadSelectionFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
        selectionEdited();

        replotSelection(true);
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    }

}


void QFRDRImagingFCSImageEditor::excludeByIntensity() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QFRDRImagingFCSMaskByIntensity* dialog=new QFRDRImagingFCSMaskByIntensity(this);
        double* image=m->getImageFromRunsPreview();
        excludeByImage(image);
    }
}

void QFRDRImagingFCSImageEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        //saveImageSettings();
        disconnect(old, SIGNAL(resultsChanged(QString,QString,bool)), this, SLOT(resultsChanged(QString,QString,bool)));
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        disconnect(cmbDualView, SIGNAL(currentIndexChanged(int)), this, SLOT(dualviewChanged(int)));
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("imfcs_imed_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("imfcs_imed_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();
        selected.clear();
        cmbDualView->setCurrentIndex(int(m->dualViewMode()));
        selectedInsert(0);
        selectionEdited();
        connect(current, SIGNAL(resultsChanged(QString,QString,bool)), this, SLOT(resultsChanged(QString,QString,bool)));
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        connect(cmbDualView, SIGNAL(currentIndexChanged(int)), this, SLOT(dualviewChanged(int)));
    } else {
        selected.clear();
    }
    updateSelectionCombobox();
    loadImageSettings();
    fillParameterSet();
}


void QFRDRImagingFCSImageEditor::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);

    //qDebug()<<"imageClicked("<<x<<y<<modifiers<<")";

    int idx=m->xyToRun(xx, yy);
    if (xx>=0 && xx<m->getImageFromRunsWidth() && yy>=0 && yy<m->getImageFromRunsHeight()) {

        if (cmbMaskEditMode->currentIndex()==0) {
            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                if (selected.contains(idx)) selectedRemove(idx);
                else selectedInsert(idx);
            } else if (modifiers==Qt::ShiftModifier) {
                selectedRemove(idx);
            } else {
                if (!actImagesScribble->isChecked()) selected.clear();
                selectedInsert(idx);
            }
            selectionEdited();
        } else {
            if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
                m->maskToggle(xx,yy);
            } else if (modifiers==Qt::ShiftModifier) {
                m->maskSet(xx,yy);
            } else {
                if (!actImagesScribble->isChecked()) m->maskClear();
                m->maskUnset(xx,yy);
            }
        }
        replotSelection(true);
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    }
}

void QFRDRImagingFCSImageEditor::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool /*last*/) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);
    if (xx>=0 && xx<m->getImageFromRunsWidth() && yy>=0 && yy<m->getImageFromRunsHeight()) {

        if (cmbMaskEditMode->currentIndex()==0) {
            if (first && modifiers==Qt::NoModifier) selected.clear();
            if (modifiers==Qt::ShiftModifier) {
                selectedRemove(idx);
            } else {
                selectedInsert(idx);
            }
            selectionEdited();
        } else {
            if (first && modifiers==Qt::NoModifier) m->maskClear();
            if (modifiers==Qt::ShiftModifier) {
                m->maskSet(xx,yy);
            } else {
                m->maskUnset(xx,yy);
            }
        }
        replotSelection(true);
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    }
}

void QFRDRImagingFCSImageEditor::imageMouseMoved(double x, double y) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);

    QString name="";
    double value=0;
    if (sender()==pltOverview) {
        name=tr("overview");
        double* ovr=m->getImageFromRunsPreview();
        if (ovr) value =ovr[idx];
    }
    if (sender()==pltImage) {
        name=tr("paramImg");
        if (plteImageData) value=plteImageData[idx];
    }
    if (sender()==pltGofImage) {
        name=tr("GOF");
        if (plteGofImageData) value=plteGofImageData[idx];
    }
    if (sender()==pltMask) {
        name=tr("mask");
        if (plteOverviewExcludedData) value=(plteOverviewExcludedData[idx])?0:1;
    }
    labImagePositionDisplay->setText(tr("%3(%1, %2) = %4").arg(xx).arg(yy).arg(name).arg(value));
}

void QFRDRImagingFCSImageEditor::imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;



    int xx1=qBound(0,(int)floor(x), m->getImageFromRunsWidth()-1);
    int yy1=qBound(0,(int)floor(y), m->getImageFromRunsHeight()-1);
    int xx2=qBound(0,(int)floor(x+width), m->getImageFromRunsWidth()-1);
    int yy2=qBound(0,(int)floor(y+height), m->getImageFromRunsHeight()-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (cmbMaskEditMode->currentIndex()==0) {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    int idx=m->xyToRun(xx, yy);
                    selectedInsert(idx);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    int idx=m->xyToRun(xx, yy);
                    selectedRemove(idx);
                }
            }
        } else {
            selected.clear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    int idx=m->xyToRun(xx, yy);
                    selectedInsert(idx);
                }
            }
        }
        selectionEdited();
    } else {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    m->maskUnset(xx,yy);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    m->maskSet(xx,yy);
                }
            }
        } else {
            m->maskClear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    m->maskUnset(xx,yy);
                }
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}


void QFRDRImagingFCSImageEditor::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;



    int xx1=qBound(0,(int)floor(x-radiusX), m->getImageFromRunsWidth()-1);
    int yy1=qBound(0,(int)floor(y-radiusY), m->getImageFromRunsHeight()-1);
    int xx2=qBound(0,(int)floor(x+radiusX), m->getImageFromRunsWidth()-1);
    int yy2=qBound(0,(int)floor(y+radiusY), m->getImageFromRunsHeight()-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (cmbMaskEditMode->currentIndex()==0) {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                        int idx=m->xyToRun(xx, yy);
                        selectedInsert(idx);
                    }
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                        int idx=m->xyToRun(xx, yy);
                        selectedRemove(idx);
                    }
                }
            }
        } else {
            selected.clear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                        int idx=m->xyToRun(xx, yy);
                        selectedInsert(idx);
                    }
                }
            }
        }
        selectionEdited();
    } else {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) m->maskUnset(xx,yy);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) m->maskSet(xx,yy);
                }
            }
        } else {
            m->maskClear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) m->maskUnset(xx,yy);
                }
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}

void QFRDRImagingFCSImageEditor::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;



    int xx1=qBound(0,(int)floor(x-radius), m->getImageFromRunsWidth()-1);
    int yy1=qBound(0,(int)floor(y-radius), m->getImageFromRunsHeight()-1);
    int xx2=qBound(0,(int)floor(x+radius), m->getImageFromRunsWidth()-1);
    int yy2=qBound(0,(int)floor(y+radius), m->getImageFromRunsHeight()-1);


    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (cmbMaskEditMode->currentIndex()==0) {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                        int idx=m->xyToRun(xx, yy);
                        selectedInsert(idx);
                    }
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                        int idx=m->xyToRun(xx, yy);
                        selectedRemove(idx);
                    }
                }
            }
        } else {
            selected.clear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                        int idx=m->xyToRun(xx, yy);
                        selectedInsert(idx);
                    }
                }
            }
        }
        selectionEdited();
    } else {
        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) m->maskUnset(xx,yy);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) m->maskSet(xx,yy);
                }
            }
        } else {
            m->maskClear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) m->maskUnset(xx,yy);
                }
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}

void QFRDRImagingFCSImageEditor::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    QLineF line(x1, y1, x2, y2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (cmbMaskEditMode->currentIndex()==0) {
        if (modifiers==Qt::ControlModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                int idx=m->xyToRun(xx, yy);
                selectedInsert(idx);
            }
        } else if (modifiers==Qt::ShiftModifier) {
        for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                int idx=m->xyToRun(xx, yy);
                selectedRemove(idx);
            }
        } else {
            selected.clear();
            for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                int idx=m->xyToRun(xx, yy);
                selectedInsert(idx);
            }
        }
        selectionEdited();
    } else {
        if (modifiers==Qt::ControlModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                m->maskUnset(xx,yy);
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                m->maskSet(xx,yy);
            }
        } else {
            m->maskClear();
            for (double i=0; i<1.0; i=i+0.5/double(qMax(m->getImageFromRunsWidth(), m->getImageFromRunsHeight()))) {
                QPointF p=line.pointAt(i);
                int xx=qBound(0,(int)floor(p.x()), m->getImageFromRunsWidth()-1);
                int yy=qBound(0,(int)floor(p.y()), m->getImageFromRunsHeight()-1);
                m->maskUnset(xx,yy);
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}

void QFRDRImagingFCSImageEditor::setImageEditMode() {
    if (actImagesZoom->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
        pltOverview->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
        pltMask->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
        pltGofImage->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
    } else if (actImagesDrawPoints->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ClickEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::ClickEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::ClickEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::ClickEvents);
    } else if (actImagesDrawRectangle->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::RectangleEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::RectangleEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::RectangleEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::RectangleEvents);
    } else if (actImagesDrawLine->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::LineEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::LineEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::LineEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::LineEvents);
    } else if (actImagesDrawCircle->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::CircleEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::CircleEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::CircleEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::CircleEvents);
    } else if (actImagesDrawEllipse->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::EllipseEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::EllipseEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::EllipseEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::EllipseEvents);
    } else if (actImagesScribble->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
        pltOverview->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
        pltMask->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
        pltGofImage->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
    }

}

void QFRDRImagingFCSImageEditor::updateAfterClick() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    replotData();
    //replotSelection(true);
    replotMask();
    updateSelectionHistogram(true);
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::rawDataChanged() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    updateSelectionCombobox();
    replotSelection(false);
    replotOverview();
    replotImage();
    replotData();
    replotMask();
    /*if (!chkImageAutoScale->isChecked()) */paletteChanged();
    /*if (!chkAutorangeOverview->isChecked()) */ovrPaletteChanged();
    updateHistogram();
    QApplication::restoreOverrideCursor();
};

void QFRDRImagingFCSImageEditor::resultsChanged(const QString& evalName, const QString& resultName,bool deleted) {
    //if (evalName.isEmpty()) {
        QString egroup=currentEvalGroup();
        QString fp=currentFitParameter();
        QString gfp=currentGofParameter();
        //qDebug()<<"QFRDRImagingFCSImageEditor::resultsChanged(evalName="<<evalName<<"   resultName="<<resultName<<"   delete="<<deleted<<")\n   egroup="<<egroup<<"  fp="<<fp<<"   gfp="<<gfp;
        fillParameterSet();
        int grp=cmbResultGroup->findData(egroup);
        if (grp>=0) {
            cmbResultGroup->setCurrentIndex(grp);
            int p=cmbParameter->findData(fp);
            if (p>=0) {
                cmbParameter->setCurrentIndex(p);
            } else if (cmbParameter->count()>0) {
                int dc=      cmbParameter->findData("fitparam_diff_coeff1");
                if (dc<0) dc=cmbParameter->findData("fitparam_diff_coeff");
                if (dc<0) dc=cmbParameter->findData("fitparam_diff_tau1");
                if (dc<0) dc=cmbParameter->findData("fitparam_diff_tau");
                if (dc<0) dc=cmbParameter->findData("fitparam_diff_coeff2");

                if (dc<0) dc=0;
                cmbParameter->setCurrentIndex(dc);
            }

            p=cmbGofParameter->findData(gfp);
            if (p>=0) {
                cmbGofParameter->setCurrentIndex(p);
            } else if (cmbGofParameter->count()>0) {
                cmbGofParameter->setCurrentIndex(0);
            }
        } else if (cmbResultGroup->count()>0) {
            cmbResultGroup->setCurrentIndex(0);
        }
    //}
};

void QFRDRImagingFCSImageEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("imfcs_imed_datacut_min", userMin, false, false);
    current->setQFProperty("imfcs_imed_datacut_max", userMax, false, false);
    //replotData();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        return;
    }

    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    plotter->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotterResid->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
    plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotterResid->setX(plotter->getXMin(), plotter->getXMax());
    plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);

    QApplication::restoreOverrideCursor();
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
    plotterResid->update_plot();

}

void QFRDRImagingFCSImageEditor::replotImage() {
    //qDebug()<<"replotImage";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);



    if (!m) {
        plteImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteImage->get_colorBarRightAxis()->set_axisLabel("");
        plteImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteImage->get_colorBarTopAxis()->set_axisLabel("");
        plteGofImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteGofImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteGofImage->get_colorBarRightAxis()->set_axisLabel("");
        plteGofImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteGofImage->get_colorBarTopAxis()->set_axisLabel("");
        //qDebug()<<"replotImage !m";
    } else {
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }
        //qDebug()<<w<<h;
        /*double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));*/
        pltImage->getDatastore()->clear();
        pltGofImage->getDatastore()->clear();

        pltImage->setAbsoluteXY(0, w, 0, h);
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltImage->get_plotter()->set_axisAspectRatio(1.0*w/h);
        pltGofImage->setAbsoluteXY(0, w, 0, h);
        pltGofImage->get_plotter()->set_maintainAspectRatio(true);
        pltGofImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltGofImage->get_plotter()->set_axisAspectRatio(1.0*w/h);

        plteImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteImage->get_colorBarRightAxis()->set_axisLabel(formatTransformAndParameter(cmbParameter, cmbParameterTransform));
        plteImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteImage->get_colorBarTopAxis()->set_axisLabel(formatTransformAndParameter(cmbParameter, cmbParameterTransform));
        plteGofImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteGofImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteGofImage->get_colorBarRightAxis()->set_axisLabel(formatTransformAndParameter(cmbGofParameter, cmbGofParameterTransform));
        plteGofImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteGofImage->get_colorBarTopAxis()->set_axisLabel(formatTransformAndParameter(cmbGofParameter, cmbGofParameterTransform));

        if (w>3*h) {
            pltImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarRightAxis()->set_minTicks(3);
            plteImage->get_colorBarTopAxis()->set_minTicks(5);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteGofImage->get_colorBarRightAxis()->set_minTicks(3);
            plteGofImage->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            pltImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteImage->get_colorBarRightAxis()->set_minTicks(5);
            plteImage->get_colorBarTopAxis()->set_minTicks(3);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteGofImage->get_colorBarRightAxis()->set_minTicks(5);
            plteGofImage->get_colorBarTopAxis()->set_minTicks(3);
        }
        pltImage->setXY(0, w, 0, h);
        pltGofImage->setXY(0, w, 0, h);

        if (plteImageSize!=m->getImageFromRunsWidth()*m->getImageFromRunsHeight()) {
            plteImageSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
            plteImageData=(double*)realloc(plteImageData, plteImageSize*sizeof(double));
            plteGofImageData=(double*)realloc(plteGofImageData, plteImageSize*sizeof(double));
        }

        readParameterImage(plteImageData, plteGofImageData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), currentEvalGroup(), currentFitParameter(), currentFitParameterTransfrom(), currentGofParameter(), currentGofParameterTransfrom());

        plteImage->set_data(plteImageData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), JKQTPMathImageBase::DoubleArray);
        plteImage->set_width(w);
        plteImage->set_height(h);
        plteImage->set_palette(cmbColorbar->currentIndex());

        plteGofImage->set_data(plteGofImageData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), JKQTPMathImageBase::DoubleArray);
        plteGofImage->set_width(w);
        plteGofImage->set_height(h);
        plteGofImage->set_autoImageRange(false);
        double mi, ma;
        statisticsMaskedMinMax(plteGofImageData, plteOverviewExcludedData, plteImageSize, mi, ma, false);
        plteGofImage->set_imageMin(mi);
        plteGofImage->set_imageMax(ma);

    }
    setCopyableData();

    pltImage->set_doDrawing(true);
    pltImage->update_plot();
    pltGofImage->set_doDrawing(true);
    pltGofImage->update_plot();
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotImage ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::updateSelectionArrays() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //qDebug()<<"updateSelectionArrays";
    //pltOverview->set_doDrawing(false);


    if (m) {
        int siz=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
        if (plteOverviewSize!=m->getImageFromRunsWidth()*m->getImageFromRunsHeight()) {
            plteOverviewSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSize*sizeof(bool));
        }
        for (register int i=0; i<siz; i++) {
            int x=m->runToX(i);
            int y=m->runToY(i);
            int idx=y*m->getImageFromRunsWidth()+x;
            plteOverviewSelectedData[idx]=selected.contains(i);
            plteOverviewExcludedData[idx]=m->leaveoutRun(i);
        }
    } else {
        for (register int i=0; i<plteOverviewSize; i++) {
            plteOverviewSelectedData[i]=false;
            plteOverviewExcludedData[i]=false;
        }
    }
    //qDebug()<<"updateSelectionArrays ... end";
}

void QFRDRImagingFCSImageEditor::setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot) {
    if (cmbSelectionStyle->currentIndex()==1) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("red"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==2) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("black"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==3) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("white"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==4) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("grey"));
        plot->set_symbolSizeFactor(0.7);
    } else {
        plot->set_drawAsRectangles(true);
        plot->set_trueColor(selectionColor);
        plot->set_symbolSizeFactor(0.7);
    }
}

void QFRDRImagingFCSImageEditor::selectedInsert(int idx)
{
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        int x=m->runToX(idx);
        int y=m->runToY(idx);
        selected.insert(idx);
        if (m->dualViewMode()==QFRDRImagingFCSData::dvHorizontal) {
            int idx1=m->xyToRun(x+m->getImageFromRunsWidth()/2, y);
            if (x>=m->getImageFromRunsWidth()/2) {
                idx1=m->xyToRun(x-m->getImageFromRunsWidth()/2, y);
                //qDebug()<<"-idx!!!";
            }
            selected.insert(idx1);
            //qDebug()<<"h: "<<idx<<idx1<<"   ("<<x<<y<<m->getImageFromRunsWidth()/2<<m->getImageFromRunsHeight()<<")  -->  ("<<x+m->getImageFromRunsWidth()/2<<x-m->getImageFromRunsWidth()/2<<")";
        } else if (m->dualViewMode()==QFRDRImagingFCSData::dvVertical) {
            int idx1=m->xyToRun(x, y+m->getImageFromRunsHeight()/2);
            if (y>=m->getImageFromRunsHeight()/2) idx1=m->xyToRun(x, y-m->getImageFromRunsHeight()/2);
            selected.insert(idx1);
            //qDebug()<<"v: "<<idx<<idx1<<"   ("<<x<<y<<m->getImageFromRunsWidth()<<m->getImageFromRunsHeight()<<")";
        } else {
            //qDebug()<<"n: "<<idx;
        }
    } else {
        selected.insert(idx);
        //qDebug()<<"n: "<<idx;
    }

}

void QFRDRImagingFCSImageEditor::selectedRemove(int idx)
{
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        int x=m->runToX(idx);
        int y=m->runToY(idx);
        selected.remove(idx);
        if (m->dualViewMode()==QFRDRImagingFCSData::dvHorizontal) {
            int idx1=m->xyToRun(x+m->getImageFromRunsWidth()/2, y);
            if (x>=m->getImageFromRunsWidth()/2) idx1=m->xyToRun(x-m->getImageFromRunsWidth()/2, y);
            selected.remove(idx1);
        } else if (m->dualViewMode()==QFRDRImagingFCSData::dvVertical) {
            int idx1=m->xyToRun(x, y+m->getImageFromRunsHeight()/2);
            if (y>=m->getImageFromRunsHeight()/2) idx1=m->xyToRun(x, y-m->getImageFromRunsHeight()/2);
            selected.remove(idx1);
        }
    } else {
        selected.remove(idx);
    }

}

void QFRDRImagingFCSImageEditor::replotSelection(bool replot) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotSelection";
    QElapsedTimer time;
    time.start();
#endif
    pltOverview->set_doDrawing(false);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);
    pltMask->set_doDrawing(false);

    saveImageSettings();

    updateSelectionArrays();

    updateOverlaySettings();

    if (!m) {
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
        plteImageSelected->set_data(NULL, 1, 1);
        plteImageExcluded->set_data(NULL, 1, 1);
        plteGofImageSelected->set_data(NULL, 1, 1);
        plteGofImageExcluded->set_data(NULL, 1, 1);
        plteMask->set_data(NULL, 1, 1);
        plteMaskSelected->set_data(NULL, 1, 1);
        labImageAvg->clear();
    } else {
        //uint16_t* ov=m->getDataImagePreview();
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        double ovrAvg=0;
        double ovrVar=0;
        bool *msk=(bool*)calloc(w*h, sizeof(bool));
        int cnt=0;
        for (int i=0; i<w*h; i++) {
            msk[i]=plteOverviewSelectedData[i]&&(!plteOverviewExcludedData[i]);
            if (msk[i]) cnt++;
        }
        //ovrAvg=statisticsAverageVarianceMasked(ovrVar, plteOverviewSelectedData, m->getImageFromRunsPreview(), qMin(plteOverviewSize, w*h));
        if (cnt>1){
            ovrAvg=statisticsAverageVarianceMasked(ovrVar, msk, m->getImageFromRunsPreview(), w*h);
        } else {
            ovrAvg=statisticsAverageVarianceMasked(ovrVar, plteOverviewExcludedData, m->getImageFromRunsPreview(), w*h, false);
        }

        plteOverviewSelected->set_width(w);
        plteOverviewSelected->set_height(h);
        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());


        plteOverviewExcluded->set_width(w);
        plteOverviewExcluded->set_height(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());


        double imgAvg=0;
        double imgVar=0;
        //imgAvg=statisticsAverageVarianceMasked(imgVar, plteOverviewSelectedData, plteImageData, qMin(plteOverviewSize, w*h));
        if (cnt>1){
            imgAvg=statisticsAverageVarianceMasked(imgVar, msk, plteImageData, w*h);
        } else {
            imgAvg=statisticsAverageVarianceMasked(imgVar, plteOverviewExcludedData, plteImageData, w*h, false);
        }
        free(msk);

        plteImageSelected->set_width(w);
        plteImageSelected->set_height(h);
        plteImageSelected->set_data(plteOverviewSelectedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());


        plteImageExcluded->set_width(w);
        plteImageExcluded->set_height(h);
        plteImageExcluded->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());

        plteGofImageSelected->set_width(w);
        plteGofImageSelected->set_height(h);
        plteGofImageSelected->set_data(plteOverviewSelectedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());

        plteGofImageExcluded->set_width(w);
        plteGofImageExcluded->set_height(h);
        plteGofImageExcluded->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());

        plteMaskSelected->set_width(w);
        plteMaskSelected->set_height(h);
        plteMaskSelected->set_data(plteOverviewSelectedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());

        plteMask->set_width(w);
        plteMask->set_height(h);
        plteMask->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());


        labImageAvg->setTextFormat(Qt::RichText);
        labImageAvg->setText(tr("avg&plusmn;SD(param img) = %1 &plusmn; %2&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;avg&plusmn;SD(overview) = %3 &plusmn; %4").arg(imgAvg).arg(sqrt(imgVar)).arg(ovrAvg).arg(sqrt(ovrVar)));


    }

    setCopyableData();

    pltOverview->set_doDrawing(true);
    pltImage->set_doDrawing(true);
    pltGofImage->set_doDrawing(true);
    pltMask->set_doDrawing(true);
    if (replot) {
        pltOverview->update_plot();
        pltImage->update_plot();
        pltGofImage->update_plot();
        pltMask->update_plot();
    }
    QApplication::restoreOverrideCursor();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotSelection ... DONE = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
}

void QFRDRImagingFCSImageEditor::replotOverview() {
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotOverview";
    QElapsedTimer time;
    time.start();
#endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_data(NULL, 1, 1, JKQTPMathImageBase::UInt16Array);
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
    } else {
        //uint16_t* ov=m->getDataImagePreview();
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        //qDebug()<<"replotOverview()  w="<<w<<"   h="<<h;
        pltOverview->setAbsoluteXY(0, w, 0, h);
        pltOverview->get_plotter()->set_maintainAspectRatio(true);
        pltOverview->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
        pltOverview->get_plotter()->set_axisAspectRatio(1.0*w/h);

        if (w>3*h) {
            pltOverview->get_plotter()->getXAxis()->set_minTicks(3);
            plteOverview->get_colorBarRightAxis()->set_minTicks(3);
            plteOverview->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            pltOverview->get_plotter()->getXAxis()->set_minTicks(7);
            plteOverview->get_colorBarRightAxis()->set_minTicks(5);
            plteOverview->get_colorBarTopAxis()->set_minTicks(3);
        }
        pltOverview->setXY(0, w, 0, h);

        if (m->getImageFromRunsPreview()) {
            plteOverview->set_autoImageRange(chkAutorangeOverview->isChecked());
            double mi=0, ma=0;
            updateSelectionArrays();
            statisticsMaskedMinMax(m->getImageFromRunsPreview(), plteOverviewExcludedData, m->getImageFromRunsWidth()*m->getImageFromRunsHeight(), mi, ma, false);
            plteOverview->set_data(m->getImageFromRunsPreview(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), JKQTPMathImageBase::DoubleArray);
            plteOverview->set_palette(cmbColorbarOverview->currentIndex());
            ovrPaletteChanged();
            //qDebug()<<"image range:"<<mi<<" ... "<<ma<<"  imgsize="<<plteImageSize<<"   fromRunsSize="<<m->getImageFromRunsWidth()*m->getImageFromRunsHeight();

        } else plteOverview->set_data(NULL, m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), JKQTPMathImageBase::UInt16Array);
        plteOverview->set_width(w);
        plteOverview->set_height(h);

    }
    setCopyableData();

    pltOverview->set_doDrawing(true);
    pltOverview->update_plot();
    QApplication::restoreOverrideCursor();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotOverview ... DONE = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
}

void QFRDRImagingFCSImageEditor::replotMask() {
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotMask";
    QElapsedTimer time;
    time.start();
#endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltMask->set_doDrawing(false);

    if (!m) {
        plteMask->set_data(NULL, 1, 1);
        plteMaskSelected->set_data(NULL, 1, 1);
    } else {
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        pltMask->setAbsoluteXY(0, w, 0, h);
        pltMask->get_plotter()->set_maintainAspectRatio(true);
        pltMask->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltMask->get_plotter()->set_maintainAxisAspectRatio(true);
        pltMask->get_plotter()->set_axisAspectRatio(1.0*w/h);

        pltMask->setXY(0, w, 0, h);

        if (plteOverviewExcludedData) plteMask->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        else plteMask->set_data(NULL, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        plteMask->set_width(w);
        plteMask->set_height(h);
    }
    setCopyableData();

    pltMask->set_doDrawing(true);
    pltMask->update_plot();
    QApplication::restoreOverrideCursor();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotMask ... DONE = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
}

void QFRDRImagingFCSImageEditor::replotData() {
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData";
    QElapsedTimer time;
    time.start();
#endif
    QElapsedTimer t;
    t.start();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    plotterResid->setVisible(chkDisplayResiduals->isChecked());

    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        //qDebug()<<"replotData ... done (!m) ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   settings sliders: "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif
    plotter->clearGraphs();
    plotterResid->clearGraphs();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   clearing graphs: "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif
    plotter->get_plotter()->set_showKey(chkKeys->isChecked());
    plotterResid->get_plotter()->set_showKey(chkKeys->isChecked());
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   setting show_key: "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif
    ds->clear();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   ds->clear: "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif


    if (m->getCorrelationN()>0) {


        JKQTPerrorPlotstyle runerrorstyle=JKQTPnoError;
        switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
            case 4: runerrorstyle=JKQTPerrorPolygons; break;
            case 5: runerrorstyle=JKQTPerrorBarsPolygons; break;
        }
        bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;

        JKQTPerrorPlotstyle avgerrorstyle=JKQTPnoError;
        switch (cmbAverageErrorStyle->currentIndex()) {
            case 1: avgerrorstyle=JKQTPerrorBars; break;
            case 2: avgerrorstyle=JKQTPerrorLines; break;
            case 3: avgerrorstyle=JKQTPerrorBarsLines; break;
            case 4: avgerrorstyle=JKQTPerrorPolygons; break;
            case 5: avgerrorstyle=JKQTPerrorBarsPolygons; break;
        }
        bool avgLine=(cmbAverageStyle->currentIndex()!=1);
        JKQTPgraphSymbols avgSymbol=JKQTPcross;
        if (cmbAverageStyle->currentIndex()==0) avgSymbol=JKQTPnoSymbol;



        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");


        //QTime t;
        //t.start();
        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (chkDisplayAverage->isChecked()) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
            //g->set_color(QColor("blue"));
            QColor cerr=g->get_color().lighter();
            g->set_errorColor(cerr);
            cerr.setAlphaF(0.5);
            g->set_errorFillColor(cerr);
            g->set_lineWidth(2);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(tr("average"));
            g->set_yErrorStyle(avgerrorstyle);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(sliders->get_userMin());
            g->set_datarange_end(sliders->get_userMax());
            g->set_drawLine(avgLine);
            g->set_symbolSize(5);
            g->set_symbol(avgSymbol);
            plotter->addGraph(g);
        }
#ifdef DEBUG_TIMIMNG
        //qDebug()<<"replotData   add avg: "<<t.elapsed(); t.start();
#endif

       //////////////////////////////////////////////////////////////////////////////////
       // Plot ALL RUNS (left out runs in gray)
       // also evaluates and plots the fit function (if possible) + the residuals
       //////////////////////////////////////////////////////////////////////////////////
        tvParams->setModel(NULL);
        tabFitvals->setReadonly(false);
        tabFitvals->clear();
        tabFitvals->appendColumn();
        tabFitvals->setColumnTitle(0, tr("parameter"));
        int maxSingleItems=1;
        if (cmbDualView->currentIndex()>0) maxSingleItems=2;
        if ((cmbSeletionCorrDisplayMode->currentIndex()==1) || (cmbSeletionCorrDisplayMode->currentIndex()==0 && selected.size()<=maxSingleItems)) {
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (selected.contains(i)) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("pixel error %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_drawLine(runLine);
                    g->set_symbol(runSymbol);
                    g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    g->set_datarange_start(sliders->get_userMin());
                    g->set_datarange_end(sliders->get_userMax());
                    g->set_yErrorColumn(c_rune);
                    g->set_yErrorStyle(runerrorstyle);
                    g->set_xErrorStyle(JKQTPnoError);
                    g->set_symbolSize(5);
                    g->set_errorWidth(1);
                    if (m->leaveoutRun(i)) {
                        g->set_color(QColor("grey"));
                    }
                    QColor cerr=g->get_color().lighter();
                    g->set_errorColor(cerr);
                    cerr.setAlphaF(0.5);
                    g->set_errorFillColor(cerr);
                    plotter->addGraph(g);

    #ifdef DEBUG_TIMIMNG
                     //qDebug()<<"replotData   add graph "<<i<<": "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
    #endif

                    double* corr=(double*)calloc(m->getCorrelationN(), sizeof(double));
                    double* resid=(double*)calloc(m->getCorrelationN(), sizeof(double));
                    QStringList names, units, unitlabels, namelabels;
                    QList<double> values, errors;
                    QList<bool> fix;

                    // search for the evaluationID that matches the current group(has to be in evals)  and run (i)
    #ifdef DEBUG_TIMIMNG
                    QElapsedTimer t1;
                    t1.start();
    #endif
                    QStringList evals=current->resultsCalcEvaluationsInGroup(currentEvalGroup());
    #ifdef DEBUG_TIMIMNG
                    double resultsCalcEvaluationsInGroupElapsed=t1.nsecsElapsed()/1000;
    #endif

                    bool evalFound=false;
                    bool listEval=false;
                    QString resultID="";


                    for (register int ev=0; ev<evals.size(); ev++) {
                        //en=evals[i];
                        if (current->resultsGetEvaluationGroupIndex(evals[ev])==i) {
                            resultID=evals[ev];
                            evalFound=true;
                            break;
                        }
                    }

                    if (!evalFound) {
                        if (evals.size()>0 && evals.size()<=2) {
                            for (register int ev=0; ev<evals.size(); ev++) {
                                if (current->resultsGetEvaluationGroupIndex(evals[ev])>=0) {
                                    resultID=evals[ev];
                                    listEval=true;
                                    evalFound=true;
                                }
                            }
                        }
                    }

    #ifdef DEBUG_TIMIMNG
                    //qDebug()<<"replotData   search eval "<<i<<" (evals.size()="<<evals.size()<< "   resultsCalcEvaluationsInGroup="<<resultsCalcEvaluationsInGroupElapsed<<" nsecs): "<<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
    #endif

                    // try to evaluate the fit function. If it succeeds, add plots and store the parameters & description to the display model!
                    if (evaluateFitFunction(m->getCorrelationT(), corr, m->getCorrelationN(), names, namelabels, values, errors, fix, units, unitlabels, resultID, i)) {
                        double* acf=m->getCorrelationRun(i);
                        for (int nn=0; nn< m->getCorrelationN(); nn++) {
                            resid[nn]=corr[nn]-acf[nn];
                        }
                        size_t c_fit=ds->addCopiedColumn(corr, m->getCorrelationN(), QString("fit to pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                        size_t c_resid=ds->addCopiedColumn(resid, m->getCorrelationN(), QString("residuals for pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                        JKQTPxyLineGraph* gfit=new JKQTPxyLineGraph();
                        gfit->set_lineWidth(1.5);
                        gfit->set_xColumn(c_tau);
                        gfit->set_yColumn(c_fit);
                        gfit->set_drawLine(true);
                        gfit->set_style(Qt::DotLine);
                        gfit->set_symbol(JKQTPnoSymbol);
                        gfit->set_title(tr("fit to run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                        gfit->set_datarange_start(sliders->get_userMin());
                        gfit->set_datarange_end(sliders->get_userMax());
                        gfit->set_symbolSize(5);
                        gfit->set_color(g->get_color());
                        plotter->addGraph(gfit);
    #ifdef DEBUG_TIMIMNG
                        //qDebug()<<"replotData   add fit graph "<<i<<": " <<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
    #endif
                        JKQTPxyLineGraph* gr=new JKQTPxyLineGraph();
                        gr->set_lineWidth(1.5);
                        gr->set_xColumn(c_tau);
                        gr->set_yColumn(c_resid);
                        gr->set_drawLine(runLine);
                        gr->set_style(Qt::DotLine);
                        gr->set_symbol(runSymbol);
                        gr->set_title(tr("residuals for run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                        gr->set_datarange_start(sliders->get_userMin());
                        gr->set_datarange_end(sliders->get_userMax());
                        gr->set_symbolSize(5);
                        gr->set_color(g->get_color());
                        plotterResid->addGraph(gr);
    #ifdef DEBUG_TIMIMNG
                        //qDebug()<<"replotData   add resid graph "<<i<<": " <<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
    #endif

                        tabFitvals->appendColumn();
                        tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                        int col=tabFitvals->columnCount()-1;
                        tabFitvals->appendColumn();
                        tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("error"));
                        tabFitvals->appendColumn();
                        tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString(""));
                        for (int ii=0; ii<names.size(); ii++) {
                            int row=tabFitvals->getAddRow(0, namelabels[ii]);
                            tabFitvals->setCellEditRole(row, 0, names[ii]);
                            tabFitvals->setCell(row, col, values[ii]);
                            tabFitvals->setCell(row, col+1, errors[ii]);
                            tabFitvals->setCell(row, col+2, unitlabels[ii]);
                            tabFitvals->setCellEditRole(row, col+2, units[ii]);
                            QBrush c=QBrush(g->get_color().lighter(170));
                            tabFitvals->setCellBackgroundRole(row, col, c);
                            tabFitvals->setCellBackgroundRole(row, col+1, c);
                            tabFitvals->setCellBackgroundRole(row, col+2, c);
                            if (fix[ii]) tabFitvals->setCellCheckedRole(row, col, Qt::Checked);
                            else tabFitvals->setCellCheckedRole(row, col, Qt::Unchecked);
                        }
    #ifdef DEBUG_TIMIMNG
                        //qDebug()<<"replotData   set table cells "<<i<<": "<< t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
    #endif

                    }

                    free(corr);
                    free(resid);
                }
            }
        } else if (cmbSeletionCorrDisplayMode->currentIndex()==0) {
            int avgs=1;
            if (cmbDualView->currentIndex()>0)  avgs=2;
            for (int avgIdx=0; avgIdx<avgs; avgIdx++) {
                double* corr=(double*)calloc(m->getCorrelationN(), sizeof(double));
                double* cerr=(double*)calloc(m->getCorrelationN(), sizeof(double));
                double* corr1=(double*)calloc(m->getCorrelationN(), sizeof(double));
                QList<QList<double> > gvalues;
                QStringList names, units, unitlabels, namelabels;
                QList<Qt::CheckState> gfix;
                for (int i=0; i<m->getCorrelationN(); i++) { corr[i]=0; cerr[i]=0; }
                double N=0, Nfit=0;
                for (int i=0; i<m->getCorrelationRuns(); i++) {
                    //qDebug()<<"r"<<i<<"  "<<indexIsDualView2(i);
                    if (selected.contains(i) && !m->leaveoutRun(i) && ((avgs==1)||(avgIdx==0 && !indexIsDualView2(i)) || (avgIdx==1 && indexIsDualView2(i)))) {
                        double* tmp=m->getCorrelationRun(i);
                        for (int jj=0; jj<m->getCorrelationN(); jj++) {
                            corr[jj]=corr[jj]+tmp[jj];
                            cerr[jj]=cerr[jj]+tmp[jj]*tmp[jj];
                        }

                        QList<double> values, errors;
                        QList<bool> fix;

                        QStringList evals=current->resultsCalcEvaluationsInGroup(currentEvalGroup());
                        bool evalFound=false;
                        bool listEval=false;
                        QString resultID="";


                        for (register int ev=0; ev<evals.size(); ev++) {
                            //en=evals[i];
                            if (current->resultsGetEvaluationGroupIndex(evals[ev])==i) {
                                resultID=evals[ev];
                                evalFound=true;
                                break;
                            }
                        }

                        if (!evalFound) {
                            if (evals.size()>0 && evals.size()<=2) {
                                for (register int ev=0; ev<evals.size(); ev++) {
                                    if (current->resultsGetEvaluationGroupIndex(evals[ev])>=0) {
                                        resultID=evals[ev];
                                        listEval=true;
                                        evalFound=true;
                                    }
                                }
                            }
                        }


                        if (evaluateFitFunction(m->getCorrelationT(), corr1, m->getCorrelationN(), names, namelabels, values, errors, fix, units, unitlabels, resultID, i)) {
                            if (Nfit==0) {
                                for (int jj=0; jj<fix.size(); jj++) {
                                    gfix.append(fix[jj]?Qt::Checked:Qt::Unchecked);
                                }
                                for (int jj=0; jj<values.size(); jj++) {
                                    QList<double> v;
                                    v.append(values[jj]);
                                    gvalues.append(v);
                                }

                            } else {
                                for (int jj=0; jj<fix.size(); jj++) {
                                    if (gfix[jj]!=fix[jj]) gfix[jj]=Qt::PartiallyChecked;
                                }
                                for (int jj=0; jj<values.size(); jj++) {
                                    gvalues[jj].append(values[jj]);
                                }
                            }

                            Nfit++;
                        }

                        N++;
                    }
                }


                for (int jj=0; jj<m->getCorrelationN(); jj++) {
                    cerr[jj]=sqrt((cerr[jj]-corr[jj]*corr[jj]/N)/(N-1.0));
                    corr[jj]=corr[jj]/N;
                }
                if (N==1) {
                    for (int jj=0; jj<m->getCorrelationN(); jj++) {
                        cerr[jj]=0;
                    }
                }

                QColor graphCol;
                if (avgIdx==0) graphCol=QColor("darkgreen");
                else graphCol=QColor("darkred");
                if (N>0) {
                    size_t c_avg=ds->addCopiedColumn(corr, m->getCorrelationN(), QString("avg. of selected pixels"));
                    size_t c_sd=ds->addCopiedColumn(cerr, m->getCorrelationN(), QString("std. dev. of selected pixels"));
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_avg);
                    g->set_drawLine(runLine);
                    g->set_lineWidth(2);
                    g->set_symbol(runSymbol);
                    g->set_title(tr("avg. over %1 runs").arg(N));
                    if (avgIdx==0 && avgs>1) g->set_title(tr("CH1: avg. over %1 runs").arg(N));
                    if (avgIdx==1) g->set_title(tr("CH2: avg. over %1 runs").arg(N));
                    g->set_datarange_start(sliders->get_userMin());
                    g->set_datarange_end(sliders->get_userMax());
                    g->set_yErrorColumn(c_sd);
                    g->set_yErrorStyle(runerrorstyle);
                    g->set_xErrorStyle(JKQTPnoError);
                    g->set_symbolSize(5);
                    g->set_errorWidth(1);
                    g->set_color(graphCol);
                    QColor colerr=g->get_color().lighter();
                    g->set_errorColor(colerr);
                    colerr.setAlphaF(0.5);
                    g->set_errorFillColor(colerr);
                    plotter->addGraph(g);
                }



                if (Nfit>0) {
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("avg. %1 pixels").arg((int)round(Nfit)));
                    if (avgIdx==0 && avgs>1) tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("CH1: avg. %1 pixels").arg((int)round(Nfit)));
                    if (avgIdx==1) tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("CH2: avg. %1 pixels").arg((int)round(Nfit)));
                    int col=tabFitvals->columnCount()-1;
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("std. dev."));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString(""));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("25% quantile"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("median"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("75% quantile"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("min"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString("max"));
                    for (int ii=0; ii<names.size(); ii++) {
                        int row=tabFitvals->getAddRow(0, namelabels[ii]);
                        tabFitvals->setCellEditRole(row, 0, names[ii]);
                        tabFitvals->setCell(row, col, qfstatisticsAverage(gvalues[ii]));
                        tabFitvals->setCell(row, col+1, sqrt(qfstatisticsVariance(gvalues[ii])));
                        tabFitvals->setCell(row, col+2, unitlabels[ii]);
                        tabFitvals->setCellEditRole(row, col+2, units[ii]);

                        qSort(gvalues[ii]);
                        tabFitvals->setCell(row, col+3, qfstatisticsSortedQuantile(gvalues[ii], 0.25));
                        tabFitvals->setCell(row, col+4, qfstatisticsSortedMedian(gvalues[ii]));
                        tabFitvals->setCell(row, col+5, qfstatisticsSortedQuantile(gvalues[ii], 0.75));
                        tabFitvals->setCell(row, col+6, qfstatisticsSortedMin(gvalues[ii]));
                        tabFitvals->setCell(row, col+7, qfstatisticsSortedMax(gvalues[ii]));

                        QBrush c=QBrush(graphCol.lighter(170));
                        tabFitvals->setCellBackgroundRole(row, col, c);
                        tabFitvals->setCellBackgroundRole(row, col+1, c);
                        tabFitvals->setCellBackgroundRole(row, col+2, c);
                        tabFitvals->setCellBackgroundRole(row, col+3, c);
                        tabFitvals->setCellBackgroundRole(row, col+4, c);
                        tabFitvals->setCellBackgroundRole(row, col+5, c);
                        tabFitvals->setCellBackgroundRole(row, col+6, c);
                        tabFitvals->setCellBackgroundRole(row, col+7, c);
                        tabFitvals->setCellCheckedRole(row, col, gfix[ii]);
                    }
                }


                free(corr);
                free(cerr);
                free(corr1);
            }


        }
        tabFitvals->setReadonly(true);
        tvParams->setModel(tabFitvals);
        tvParams->resizeColumnsToContents();


#ifdef DEBUG_TIMIMNG
        //qDebug()<<"replotData   add plots: "<< t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif



        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
        plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotterResid->setX(plotter->getXMin(), plotter->getXMax());
        plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);
#ifdef DEBUG_TIMIMNG
        //qDebug()<<"replotData   zoom: " <<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif

    }
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   emit signals: " <<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif
    //QTime t;
    //t.start();
    plotter->update_plot();
    plotterResid->update_plot();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData   update plots: " <<t.nsecsElapsed()/1000<<" usecs = "<<(double)t.nsecsElapsed()/1000000.0<<" msecs"; t.start();
#endif
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotData ... done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
#ifdef DEBUG_TIMIMNG
    //qDebug()<<"replotData ... DONE = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
};


void QFRDRImagingFCSImageEditor::readSettings() {
    if (!settings) return;
    connectParameterWidgets(false);
    connectImageWidgets(false);
    lastSavePath=settings->getQSettings()->value(QString("imfcsimageeditor/last_save_path"), lastSavePath).toString();
    lastMaskDir=settings->getQSettings()->value(QString("imfcsimageeditor/last_mask_path"), lastMaskDir).toString();
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsimageeditor/corrplot"));
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_tau_axis"), true).toBool());
    chkOverviewVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), true).toBool());
    chkGofVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/gof_visible"), true).toBool());
    chkMaskVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), true).toBool());
    chkKeys->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_keys"), false).toBool());
    cmbSeletionCorrDisplayMode->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/corr_seldisplaymode"), 0).toInt());
    chkDisplayResiduals->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_resid"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), 2).toInt());
    cmbColorbar->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), 0).toInt());
    cmbColorbarOverview->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/ovrcolorbar"), 6).toInt());
    cmbImageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/paramstyle"), 0).toInt());
    cmbOutOfRangeMode->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/outofrange_mode"), 1).toInt());
    chkDisplayImageOverlay->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlays"), true).toBool());
    cmbSelectionStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlay_style"), 0).toBool());
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), 1).toInt());
    //chkLogHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_histogram"), false).toBool());
    //chkNormalizedHistograms->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/normalized_histograms"), true).toBool());
    //chkExcludeExcludedRunsFromHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/exclude_in_histograms"), true).toBool());
    loadSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    loadSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    //loadSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    //loadSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
    histogram->readSettings(*(settings->getQSettings()), "imfcsimageeditor/");
    histogram2->readSettings(*(settings->getQSettings()), "imfcsimageeditor/histogram2/");
    connectParameterWidgets(true);
    rawDataChanged();
    connectImageWidgets(true);
}


void QFRDRImagingFCSImageEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsimageeditor/last_save_path"), lastSavePath);
    settings->getQSettings()->setValue(QString("imfcsimageeditor/last_mask_path"), lastMaskDir);
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/autoscale"), chkImageAutoScale->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/colmin"), edtColMin->value());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/colmax"), edtColMax->value());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/corr_seldisplaymode"), cmbSeletionCorrDisplayMode->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_keys"), chkKeys->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlays"), chkDisplayImageOverlay->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlay_style"), cmbSelectionStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_resid"), chkDisplayResiduals->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/ovrcolorbar"), cmbColorbarOverview->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/paramstyle"), cmbImageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/outofrange_mode"), cmbOutOfRangeMode->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), chkOverviewVisible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/gof_visible"), chkGofVisible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), chkMaskVisible->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/log_histogram"), chkLogHistogram->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/normalized_histograms"), chkNormalizedHistograms->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/exclude_in_histograms"), chkExcludeExcludedRunsFromHistogram->isChecked());
    saveSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    saveSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    //saveSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    saveSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    //saveSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
    histogram->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/");
    histogram2->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/histogram2/");
}


void QFRDRImagingFCSImageEditor::parameterSetChanged() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    int grp=cmbResultGroup->currentIndex();
    if ((!m)||(grp<0)) {
        labParameter->setEnabled(false);
        cmbParameter->setEnabled(false);
        labParameterTransform->setEnabled(false);
        cmbParameterTransform->setEnabled(false);
        labGofParameter->setEnabled(false);
        cmbGofParameter->setEnabled(false);
        labGofParameterTransform->setEnabled(false);
        cmbGofParameterTransform->setEnabled(false);
        clearImage();
        cmbParameter->clear();
        cmbGofParameter->clear();
        //cmbParameter->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        m->setQFProperty("imfcs_imed_evalgroup", grp, false, false);
        labParameter->setEnabled(true);
        cmbParameter->setEnabled(true);
        labParameterTransform->setEnabled(true);
        cmbParameterTransform->setEnabled(true);
        labGofParameter->setEnabled(true);
        cmbGofParameter->setEnabled(true);
        labGofParameterTransform->setEnabled(true);
        cmbGofParameterTransform->setEnabled(true);
        //QStringList egroup=m->resultsCalcEvalGroups();
        QString egroup=currentEvalGroup();
        QList<QPair<QString, QString> > params=m->resultsCalcNamesAndLabels("", "fit results", egroup);
        cmbParameter->clear();
        cmbGofParameter->clear();
        for (int i=0; i<params.size(); i++) {
            if (!params[i].second.endsWith("_fix")) {
                cmbParameter->addItem(params[i].first, params[i].second);
                cmbGofParameter->addItem(params[i].first, params[i].second);
                //qDebug()<<params[i].second;
            }
        }
        for (int i=0; i<params.size(); i++) {
            if (params[i].second.endsWith("_fix")) {
                cmbParameter->addItem(params[i].first, params[i].second);
                cmbGofParameter->addItem(params[i].first, params[i].second);
            }
        }

        params=m->resultsCalcNamesAndLabels("", "fit properties", egroup);
        for (int i=0; i<params.size(); i++) {
            if (!params[i].second.endsWith("_fix")) {
                cmbParameter->addItem(params[i].first, params[i].second);
                cmbGofParameter->insertItem(0, params[i].first, params[i].second);
            }
        }
        for (int i=0; i<params.size(); i++) {
            if (params[i].second.endsWith("_fix")) {
                cmbParameter->addItem(params[i].first, params[i].second);
                cmbGofParameter->insertItem(0, params[i].first, params[i].second);
            }
        }
        int d=cmbParameter->findData(current->getProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), ""));
        if (d<0) d=cmbParameter->findData("fitparam_diff_coeff1");
        if (d<0) d=cmbParameter->findData("fitparam_diff_coeff");
        if (d<0) d=cmbParameter->findData("fitparam_diff_tau1");
        if (d<0) d=cmbParameter->findData("fitparam_diff_tau");
        if (d<0) d=cmbParameter->findData("fitparam_diff_coeff2");
        if (d>=0) cmbParameter->setCurrentIndex(d);
        else cmbParameter->setCurrentIndex(0);
        d=cmbGofParameter->findData(current->getProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), "fitalg_error_sum"));
        if (d>=0) cmbGofParameter->setCurrentIndex(d);
        cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
        cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());
        connectParameterWidgets(true);

    }
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
    parameterChanged();
    replotData();
    //updateHistogram();
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::parameterChanged() {
    if (!current) return;
    QString egroup=currentEvalGroup();

    connectParameterWidgets(false);
    cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
    cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());

    current->setQFProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), cmbParameter->itemData(cmbParameter->currentIndex()).toString(), false, false);
    current->setQFProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString(), false, false);
    connectParameterWidgets(true);

    transformChanged();
}

void QFRDRImagingFCSImageEditor::transformChanged() {
    QString egroup=currentEvalGroup();
    current->setQFProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), cmbGofParameterTransform->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), cmbParameterTransform->currentIndex(), false, false);
    connectParameterWidgets(false);
    loadImageSettings();
    replotOverview();
    replotImage();
    paletteChanged();
    updateOverlaySettings();
    connectParameterWidgets(true);
    updateHistogram();
}


void QFRDRImagingFCSImageEditor::clearImage() {
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    plteImageSize=0;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
}

void QFRDRImagingFCSImageEditor::fillParameterSet() {
    //qDebug()<<"fillParameterSet";

    if (!current) {
        //cmbResultGroup->setEnabled(false);
        cmbResultGroup->clear();
        //cmbResultGroup->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        //cmbResultGroup->setEnabled(true);
        cmbResultGroup->clear();

        // fill the list of available fit result groups
        QStringList egroups=current->resultsCalcEvalGroups();
        //qDebug()<<"egroups="<<egroups;
        for (int i=0; i<egroups.size(); i++) {
            cmbResultGroup->addItem(QString("%1").arg(current->resultsGetLabelForEvaluationGroup(egroups[i])), egroups[i]);
            //qDebug()<<"egroup["<<i<<"/"<<cmbResultGroup->count()<<"]: "<<egroups[i];
        }
        int d=cmbResultGroup->findData(current->getProperty("imfcs_imed_evalgroup", "").toString());
        if (d>=0) cmbResultGroup->setCurrentIndex(d);
        else if (cmbResultGroup->count()>0) cmbResultGroup->setCurrentIndex(0);
        connectParameterWidgets(true);

    }
    // update the list of available fit parameters
    parameterSetChanged(); // this also replots !!!
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;

}

void QFRDRImagingFCSImageEditor::debugInfo() {
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
    QTimer::singleShot(500, this, SLOT(debugInfo()));
}

void QFRDRImagingFCSImageEditor::connectImageWidgets(bool connectTo) {
    if (connectTo) {
        connectImageWidgetsCounter--;
        if (connectImageWidgetsCounter<=0) {
            connectImageWidgetsCounter=0;
            connect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(cmbSeletionCorrDisplayMode, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        }
    } else {
        connectImageWidgetsCounter++;
        disconnect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(cmbSeletionCorrDisplayMode, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    }
}

void QFRDRImagingFCSImageEditor::connectParameterWidgets(bool connectTo) {
    if (connectTo) {
        connectParameterWidgetsCounter--;
        if (connectParameterWidgetsCounter<=0) {
            connectParameterWidgetsCounter=0;
            connect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
            connect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
            connect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
            connect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
            connect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
            connect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
            connect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
            connect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
            connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(replotSelection()));
            connect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
            connect(cmbSelectionStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
            connect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkExcludeExcludedRunsFromHistogram2, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));

            connect(cmbColorbarOverview, SIGNAL(currentIndexChanged(int)), this, SLOT(ovrPaletteChanged()));
            connect(chkAutorangeOverview, SIGNAL(toggled(bool)), this, SLOT(ovrPaletteChanged()));
            connect(edtOvrMin, SIGNAL(valueChanged(double)), this, SLOT(ovrPaletteChanged()));
            connect(edtOvrMax, SIGNAL(valueChanged(double)), this, SLOT(ovrPaletteChanged()));



            histogram->connectParameterWidgets(connectTo);
            histogram2->connectParameterWidgets(connectTo);
            connect(histogram, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));
            connect(histogram2, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));

        }
    } else {
        connectParameterWidgetsCounter++;
        disconnect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
        disconnect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
        disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
        disconnect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(replotSelection()));
        disconnect(cmbSelectionStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
        disconnect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkExcludeExcludedRunsFromHistogram2, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));

        disconnect(cmbColorbarOverview, SIGNAL(currentIndexChanged(int)), this, SLOT(ovrPaletteChanged()));
        disconnect(chkAutorangeOverview, SIGNAL(toggled(bool)), this, SLOT(ovrPaletteChanged()));
        disconnect(edtOvrMin, SIGNAL(valueChanged(double)), this, SLOT(ovrPaletteChanged()));
        disconnect(edtOvrMax, SIGNAL(valueChanged(double)), this, SLOT(ovrPaletteChanged()));


        histogram->connectParameterWidgets(connectTo);
        histogram2->connectParameterWidgets(connectTo);
        disconnect(histogram, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));
        disconnect(histogram2, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));
    }

    //qDebug()<<"connectParameterWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();

}

void QFRDRImagingFCSImageEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==pltImage) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltGofImage) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltOverview) {
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
        pltImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltMask) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
        pltImage->setXY(newxmin, newxmax, newymin, newymax);
    }
}
void QFRDRImagingFCSImageEditor::acfZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==plotter) {
        plotterResid->setX(newxmin, newxmax);
        //qDebug()<<newxmin << newxmax;
    }
}




QString QFRDRImagingFCSImageEditor::currentEvalGroup() const {
    int grp=cmbResultGroup->currentIndex();
    if (grp<0) return "";
    return cmbResultGroup->itemData(grp).toString();

}

QString QFRDRImagingFCSImageEditor::currentFitParameter() const {
    if (cmbParameter->currentIndex()<0) return "";
    return cmbParameter->itemData(cmbParameter->currentIndex()).toString();
}

QString QFRDRImagingFCSImageEditor::currentGofParameter() const {
    if (cmbGofParameter->currentIndex()<0) return "";
    return cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString();
}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentFitParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itNone;
    if (cmbParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbParameterTransform->currentIndex();

}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentGofParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itSqrt;
    if (cmbGofParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbGofParameterTransform->currentIndex();
}

QString QFRDRImagingFCSImageEditor::formatTransformAndParameter(QComboBox *cmbParameter, QComboBox *cmbTransform)
{
    if (cmbTransform->currentIndex()==1) return tr("abs(%1)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==2) return tr("log_{10}\\left({%1}\\right)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==3) return tr("1/\\left({%1}\\right)").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else if (cmbTransform->currentIndex()==4) return tr("\\sqrt{%1}").arg(cmbParameter->currentText().replace('_', "{\\_}"));
    else return cmbParameter->currentText().replace('_', "{\\_}");
}


void QFRDRImagingFCSImageEditor::transformImage(double* image, uint16_t width, uint16_t height, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam) {
    switch(tranFitParam) {
        case QFRDRImagingFCSImageEditor::itAbs: {
                for (uint32_t i=0; i<width*height; i++) {
                    image[i]=fabs(image[i]);
                }
            } break;
        case QFRDRImagingFCSImageEditor::itLog: {
                for (uint32_t i=0; i<width*height; i++) {
                    if (image[i]>0) image[i]=log(image[i])/log(10.0);
                    else  image[i]=NAN;
                }
            } break;
        case QFRDRImagingFCSImageEditor::itReciprocal: {
                for (uint32_t i=0; i<width*height; i++) {
                    if (image[i]!=0) image[i]=1.0/image[i];
                    else  image[i]=NAN;
                }
            } break;
        case QFRDRImagingFCSImageEditor::itSqrt: {
                for (uint32_t i=0; i<width*height; i++) {
                    if (image[i]>=0) image[i]=sqrt(image[i]);
                    else  image[i]=NAN;
                }
            } break;
        default: break;
    }
}



void QFRDRImagingFCSImageEditor::readParameterImage(double* image, double* gof_image, uint16_t width, uint16_t height, QString evalGroup, QString fitParam, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam, QString gofParam, QFRDRImagingFCSImageEditor::ImageTransforms tranGofParam) {
    uint16_t arraysize=width*height;
    for (register uint16_t i=0; i<arraysize; i++) {
        image[i]=NAN;
        gof_image[i]=0;
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if ( (!m) || evalGroup.isEmpty() || fitParam.isEmpty() ) return;

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

    QStringList evals=current->resultsCalcEvaluationsInGroup(evalGroup);
    //qDebug()<<"evals.size() = "<<evals.size()<<"\n   "<<evals;
    bool readImage=false;
    bool readGOF=false;
    if (evals.size()>0  && evals.size()<=2) {
        QString usedEval="";
        QString usedGof="";
        for (int i=0; i<evals.size(); i++) {
            if (current->resultsExists(evals[i], fitParam)) {
                QFRawDataRecord::evaluationResultType typ=current->resultsGetType(evals[i], fitParam);

                switch (typ) {
                    case QFRawDataRecord::qfrdreNumberVector:
                    case QFRawDataRecord::qfrdreNumberMatrix:
                    case QFRawDataRecord::qfrdreNumberErrorVector:
                    case QFRawDataRecord::qfrdreNumberErrorMatrix:
                    case QFRawDataRecord::qfrdreIntegerVector:
                    case QFRawDataRecord::qfrdreIntegerMatrix:
                    case QFRawDataRecord::qfrdreBooleanVector:
                    case QFRawDataRecord::qfrdreBooleanMatrix:
                        usedEval=evals[i];
                        break;
                    default:
                        break;
                }
                typ=current->resultsGetType(evals[i], gofParam);

                switch (typ) {
                    case QFRawDataRecord::qfrdreNumberVector:
                    case QFRawDataRecord::qfrdreNumberMatrix:
                    case QFRawDataRecord::qfrdreNumberErrorVector:
                    case QFRawDataRecord::qfrdreNumberErrorMatrix:
                    case QFRawDataRecord::qfrdreIntegerVector:
                    case QFRawDataRecord::qfrdreIntegerMatrix:
                    case QFRawDataRecord::qfrdreBooleanVector:
                    case QFRawDataRecord::qfrdreBooleanMatrix:
                        usedGof=evals[i];
                        break;
                    default:
                        break;
                }
            }
        }
        if (!usedEval.isEmpty()) {
            readImage=true;
            QVector<double> dvec=current->resultsGetAsDoubleList(usedEval, fitParam);
            for (uint32_t i=0; i<qMin(dvec.size(), width*height); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                image[y*width+x]=dvec[i];
            }
        }
        if (!usedGof.isEmpty()) {
            readGOF=true;
            QVector<double> dvec=current->resultsGetAsDoubleList(usedEval, gofParam);
            for (uint32_t i=0; i<qMin(dvec.size(), width*height); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                gof_image[y*width+x]=dvec[i];
            }
        }
    }

    if (!readImage) {
        readImage=true;
        for (register int i=0; i<evals.size(); i++) {
            const QString& en=evals[i];
            int grpIdx=current->resultsGetEvaluationGroupIndex(en);
            int x=m->runToX(grpIdx);
            int y=m->runToY(grpIdx);
            if (x>=0 && x<width && y>=0 && y<height) {
                if (current->resultsExists(en, fitParam)) {
                    image[y*width+x]=current->resultsGetAsDouble(en, fitParam);
                }
            }
        }
    }
    if (!readGOF) {
        readGOF=true;
        for (register int i=0; i<evals.size(); i++) {
            const QString& en=evals[i];
            int grpIdx=current->resultsGetEvaluationGroupIndex(en);
            int x=m->runToX(grpIdx);
            int y=m->runToY(grpIdx);
            if (x>=0 && x<width && y>=0 && y<height) {
                if (current->resultsExists(en, gofParam)) {
                    gof_image[y*width+x]=current->resultsGetAsDouble(en, gofParam);
                }
            }
        }
    }
    transformImage(image, width, height, tranFitParam);
    transformImage(gof_image, width, height, tranGofParam);
#ifdef DEBUG_TIMING
    //qDebug()<<"QFRDRImagingFCSImageEditor::readParameterImage("<<evalGroup<<fitParam<<") finished after "<<time.elapsed()<<"ms";
#endif

}

bool QFRDRImagingFCSImageEditor::evaluateFitFunction(const double* tau, double* fit, uint32_t N, QStringList& names, QStringList& namelabels, QList<double>& values, QList<double>& errors, QList<bool>& fix, QStringList& units, QStringList& unitlabels, QString evaluation, int index) {
    QString fitfunc="";
    bool isMatrixResults=false;
    //qDebug()<<evaluation<<fitfunc<<m_fitFunctions.size();
    if (index<0) {
        fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");
    } else {
        if (current->resultsExists(evaluation, "fit_model_name")) {
            switch (current->resultsGetType(evaluation, "fit_model_name")) {
                case QFRawDataRecord::qfrdreStringVector:
                case QFRawDataRecord::qfrdreStringMatrix:
                    fitfunc=current->resultsGetInStringList(evaluation, "fit_model_name", index);
                    isMatrixResults=true;
                    break;
                case QFRawDataRecord::qfrdreString:
                    fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");
                    break;
                default:
                    break;
            }

        }
    }
    //qDebug()<<"evaluateFitFunction()  "<<evaluation<<index<<"  => "<<fitfunc<<isMatrixResults;
    QFFitFunction* ff=m_fitFunctions.value(fitfunc, NULL);
    //qDebug()<<evaluation<<fitfunc<<m_fitFunctions.size()<<ff;
    if (!ff) return false;
    double* params=(double*)calloc(ff->paramCount(),sizeof(double));
    double* errs=(double*)calloc(ff->paramCount(),sizeof(double));
    bool* fixs=(bool*)calloc(ff->paramCount(),sizeof(bool));
    names.clear();
    namelabels.clear();
    values.clear();
    errors.clear();
    fix.clear();
    units.clear();
    unitlabels.clear();

    QStringList pids;
    //qDebug()<<"ff->paramCount()="<<ff->paramCount();
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        pids.append(id);
        errs[i]=0;
        fixs[i]=ff->getDescription(id).initialFix;
        params[i]=ff->getDescription(id).initialValue;
        /*double value=0;
        if (overrideFitFunctionPreset(id, value)) params[i]=value;*/

        if (!isMatrixResults) {
            if (current->resultsExists(evaluation, "fitparam_"+id)) {
                params[i]=current->resultsGetAsDouble(evaluation, "fitparam_"+id);
                errs[i]=current->resultsGetErrorAsDouble(evaluation, "fitparam_"+id);
            }
            if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
                fixs[i]=current->resultsGetAsBoolean(evaluation, "fitparam_"+id+"_fix");
            }
        } else {
            if (current->resultsExists(evaluation, "fitparam_"+id)) {
                params[i]=current->resultsGetInNumberList(evaluation, "fitparam_"+id, index);
                errs[i]=current->resultsGetErrorInNumberErrorList(evaluation, "fitparam_"+id, index);
            }
            if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
                fixs[i]=current->resultsGetInBooleanList(evaluation, "fitparam_"+id+"_fix", index);
            }
        }
    }
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        if (ff->isParameterVisible(i, params)) {
            namelabels.append(ff->getDescription(id).label);
            unitlabels.append(ff->getDescription(id).unitLabel);
            names.append(ff->getDescription(id).name);
            units.append(ff->getDescription(id).unit);
            values.append(params[i]);
            errors.append(errs[i]);
            fix.append(fixs[i]);
        }
    }

    if (tau && fit) {
        for (unsigned int i=0; i<N; i++) {
            fit[i]=ff->evaluate(tau[i], params);
        }
    }

    free(params);
    free(errs);
    free(fixs);
    return true;
}

void QFRDRImagingFCSImageEditor::saveReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = qfGetSaveFileName(this, tr("Save Report"),
                                lastSavePath,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        lastSavePath=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        int i=tabDisplay->currentIndex();
        tabDisplay->setCurrentIndex(0);
        tabDisplay->setCurrentIndex(1);
        tabDisplay->setCurrentIndex(i);


        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        doc->print(printer);
        delete doc;
        delete printer;
        QApplication::restoreOverrideCursor();
    }
}


void QFRDRImagingFCSImageEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int i=tabDisplay->currentIndex();
    tabDisplay->setCurrentIndex(0);
    tabDisplay->setCurrentIndex(1);
    tabDisplay->setCurrentIndex(i);
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::setCopyableData()
{
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltImage->getDatastore()->clear();
    pltGofImage->getDatastore()->clear();
    pltOverview->getDatastore()->clear();
    pltMask->getDatastore()->clear();

    if (m) {
        pltImage->getDatastore()->addCopiedColumn(plteImageData, plteImageSize, formatTransformAndParameter(cmbParameter, cmbParameterTransform));
        pltImage->getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteOverviewSize, tr("selection"));
        pltImage->getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteOverviewSize, tr("mask"));

        pltGofImage->getDatastore()->addCopiedColumn(plteGofImageData, m->getImageFromRunsWidth()*m->getImageFromRunsHeight(), formatTransformAndParameter(cmbGofParameter, cmbGofParameterTransform));
        pltGofImage->getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteOverviewSize, tr("selection"));
        pltGofImage->getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteOverviewSize, tr("mask"));

        pltMask->getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteOverviewSize, tr("selection"));
        pltMask->getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteOverviewSize, tr("mask"));

        pltOverview->getDatastore()->addCopiedColumn(m->getImageFromRunsPreview(), m->getImageFromRunsWidth()*m->getImageFromRunsHeight(), tr("overview image"));
        pltOverview->getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteOverviewSize, tr("selection"));
        pltOverview->getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteOverviewSize, tr("mask"));
    }
}

void QFRDRImagingFCSImageEditor::updateSelectionCombobox()
{
    bool en=cmbStoredSelections->updatesEnabled();
    QString idx=cmbStoredSelections->currentText();
    cmbStoredSelections->setUpdatesEnabled(false);
    disconnect(cmbStoredSelections, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSelectionInCombo(int)));
    cmbStoredSelections->clear();
    cmbStoredSelections->addItem("--- custom ---");

    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        for (int s=0; s<m->getImageSelectionCount(); s++) {
            cmbStoredSelections->addItem(m->getImageSelectionName(s));
        }
    }

    connect(cmbStoredSelections, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSelectionInCombo(int)));
    cmbStoredSelections->setCurrentIndex(cmbStoredSelections->findText(idx));
    cmbStoredSelections->setUpdatesEnabled(en);
}

void QFRDRImagingFCSImageEditor::selectedSelectionInCombo(int index) {
    if (index>0) {
        QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
        if (m) {
            if (index-1<m->getImageSelectionCount()) {
                selected.clear();
                bool* sel=m->loadImageSelection(index-1);
                for (int i=0; i<m->getImageSelectionWidth()*m->getImageSelectionHeight(); i++)  {
                    if (sel[i]) {
                        selectedInsert(i);
                    }
                }
                replotSelection(true);
                timUpdateAfterClick->setSingleShot(true);
                timUpdateAfterClick->stop();
                timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);

            }
        }
    }
}

void QFRDRImagingFCSImageEditor::storeSelection()
{
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        bool ok=false;
        QStringList items;
        items<<"";
        for (int i=1; i<cmbStoredSelections->count(); i++) {
            items<<cmbStoredSelections->itemText(i);
        }
        QString name= QInputDialog::getItem(this, tr("store selection"), tr("new name"), items, 0, true, &ok);
        if (ok) {
            bool* sel=(bool*)calloc(m->getImageSelectionHeight()*m->getImageSelectionWidth(), sizeof(bool));
            for (int i=0; i<m->getImageSelectionHeight()*m->getImageSelectionWidth(); i++) sel[i]=false;

            QSetIterator<int32_t> it(selected);
            while (it.hasNext()) {
                int idx=it.next();
                if (idx>=0 && idx<m->getImageSelectionHeight()*m->getImageSelectionWidth()) {
                    sel[idx]=true;
                }
            }

            m->addImageSelection(sel, name);

            free(sel);
            updateSelectionCombobox();
            cmbStoredSelections->setCurrentIndex(cmbStoredSelections->findText(name));
        }
    }
}

void QFRDRImagingFCSImageEditor::deleteSelection() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        int idx=cmbStoredSelections->currentIndex();
        if (idx>0 && idx-1<m->getImageSelectionCount()) {
            m->deleteImageSelection(idx-1);
            updateSelectionCombobox();
        }
    }
}

void QFRDRImagingFCSImageEditor::selectionEdited()
{
    cmbStoredSelections->setCurrentIndex(0);
}

void QFRDRImagingFCSImageEditor::dualviewChanged(int mode) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        if (mode==1) m->setDualViewMode(QFRDRImagingFCSData::dvHorizontal);
        else if (mode==2) m->setDualViewMode(QFRDRImagingFCSData::dvVertical);
        else m->setDualViewMode(QFRDRImagingFCSData::dvNone);
        histogram2->setVisible(mode!=QFRDRImagingFCSData::dvNone);
        rawDataChanged();
    }
}

void QFRDRImagingFCSImageEditor::insertSelectedCorrelationsAsFCSRDR() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        QStringList CSV;
        QString runs="";
        double* tau=m->getCorrelationT();
        long long N=m->getCorrelationN();
        for (long long i=0; i<N; i++) {
            CSV.append(CDoubleToQString(tau[i]));
        }
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (selected.contains(i)) {
                double* c=m->getCorrelationRun(i);
                double* ce=m->getCorrelationRunError(i);
                for (int j=0; j<N; j++) {
                    CSV[j]=CSV[j]+", "+CDoubleToQString(c[j])+", "+CDoubleToQString(ce[j]);
                }
                if (!runs.isEmpty()) runs+=", ";
                runs+=QString::number(i);
            }
        }
        QMap<QString, QVariant> p;
        p["FILETYPE"]="INTERNAL";
        p["INTERNAL_CSV"]=CSV.join("\n");
        p["INTERNAL_CSVMODE"]="tcecece";
        QStringList paramsReadonly;
        paramsReadonly<<"FILETYPE"<<"INTERNAL_CSV"<<"INTERNAL_CSVMODE";


        QFRawDataRecord* e=m->getProject()->addRawData("fcs", tr("subplot of '%1'").arg(m->getName()), QStringList(), p, paramsReadonly);
        if (e->error()) {
            QMessageBox::critical(NULL, tr("QuickFit 3.0"), tr("Error while importing simulated FCS/DLS curve:\n%1").arg(e->errorDescription()));
            services->log_error(tr("Error while importing simulated FCS/DLS curve:\n    %1\n").arg(e->errorDescription()));
            m->getProject()->deleteRawData(e->getID());
        } else {
            e->setFolder(m->getFolder());
            e->setDescription(m->getDescription());
        }

    }
}

void QFRDRImagingFCSImageEditor::copyToMatlab() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        JKImage<double> image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<double> gof_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<uint16_t> mask_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<double> overview_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        readParameterImage(image.data(), gof_image.data(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), currentEvalGroup(), currentFitParameter(), QFRDRImagingFCSImageEditor::itNone, currentGofParameter(), QFRDRImagingFCSImageEditor::itNone);
        overview_image.assign(m->getImageFromRunsPreview(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        //mask_image.assign(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        for (int32_t i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
            mask_image(i)=(plteOverviewExcludedData[i])?1:0;
        }

        QString data="";
        data.append(image.to_matlab("parameterImage").c_str());
        data.append(gof_image.to_matlab("goodnesOfFit").c_str());
        data.append(overview_image.to_matlab("overview").c_str());
        data.append(mask_image.to_matlab("mask").c_str());

        QClipboard *clipboard = QApplication::clipboard();

        clipboard->setText(data);
    }
}


void QFRDRImagingFCSImageEditor::copyDataAsColumns() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        QFRDRImagingFCSCopyDataSelectDialog* dlg=new QFRDRImagingFCSCopyDataSelectDialog(this);
        if (dlg->exec()) {
            JKImage<double> image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            JKImage<double> gof_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            JKImage<uint16_t> mask_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            JKImage<double> overview_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            readParameterImage(image.data(), gof_image.data(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), currentEvalGroup(), currentFitParameter(), QFRDRImagingFCSImageEditor::itNone, currentGofParameter(), QFRDRImagingFCSImageEditor::itNone);
            overview_image.assign(m->getImageFromRunsPreview(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            //mask_image.assign(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
            for (int32_t i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
                mask_image(i)=(plteOverviewExcludedData[i])?1:0;
            }

            QLocale loc=QLocale::system();
            loc.setNumberOptions(QLocale::OmitGroupSeparator);

            QStringList pim= QString(image.to_csv_column("\n", '.').c_str()).split('\n');
            QStringList gim= QString(gof_image.to_csv_column("\n", '.').c_str()).split('\n');
            QStringList oim= QString(overview_image.to_csv_column("\n", '.').c_str()).split('\n');
            QStringList mim= QString(mask_image.to_csv_column("\n", '.').c_str()).split('\n');

            QString data="";
            QString qfdata="";
            int cnt=pim.size();
            if (gim.size()>cnt) cnt=gim.size();
            if (oim.size()>cnt) cnt=oim.size();
            if (mim.size()>cnt) cnt=mim.size();

            if (dlg->copyID()) data+="\"pixel number\"";
            if (dlg->copyCoordinates()) { if (!data.isEmpty()) data.append("; "); data+="\"x\"; \"y\""; }
            if (dlg->copyParam()) { if (!data.isEmpty()) data.append("; ");  data+="\""+formatTransformAndParameter(cmbParameter, cmbParameterTransform).replace('\"', '_')+"\""; }
            if (dlg->copyGOF()) { if (!data.isEmpty()) data.append("; ");  data+="\""+formatTransformAndParameter(cmbGofParameter, cmbGofParameterTransform).replace('\"', '_')+"\""; }
            if (dlg->copyOvf()) { if (!data.isEmpty()) data.append("; ");  data+="\"overview\""; }
            if (dlg->copyMask()) { if (!data.isEmpty()) data.append("; ");  data+="\"mask\""; }
            data+="\n";
            qfdata="";
            if (dlg->copyID()) qfdata+="\"pixel number\"";
            if (dlg->copyCoordinates()) { if (!qfdata.isEmpty()) qfdata.append(", "); qfdata+="\"x\", \"y\""; }
            if (dlg->copyParam()) { if (!qfdata.isEmpty()) qfdata.append(", ");  qfdata+="\""+formatTransformAndParameter(cmbParameter, cmbParameterTransform).replace('\"', '_')+"\""; }
            if (dlg->copyGOF()) { if (!qfdata.isEmpty()) qfdata.append(", ");  qfdata+="\""+formatTransformAndParameter(cmbGofParameter, cmbGofParameterTransform).replace('\"', '_')+"\""; }
            if (dlg->copyOvf()) { if (!qfdata.isEmpty()) qfdata.append(", ");  qfdata+="\"overview\""; }
            if (dlg->copyMask()) { if (!qfdata.isEmpty()) qfdata.append(", ");  qfdata+="\"mask\""; }
            qfdata="#! "+qfdata+"\n";
            for (int i=0; i<cnt; i++) {
                QString line="";
                if (dlg->copyID()) { if (!line.isEmpty()) line.append("; "); line+=QString::number(i); }
                if (dlg->copyCoordinates()) { if (!line.isEmpty()) line.append("; "); line+=QString::number(i%image.width())+"; "+QString::number(i/image.width()); }
                if (dlg->copyParam()) { if (!line.isEmpty()) line.append("; "); line+=pim.value(i, "").replace('.', loc.decimalPoint()); }
                if (dlg->copyGOF()) { if (!line.isEmpty()) line.append("; ");  line+=gim.value(i, "").replace('.', loc.decimalPoint()); }
                if (dlg->copyOvf())  { if (!line.isEmpty()) line.append("; "); line+=oim.value(i, "").replace('.', loc.decimalPoint()); }
                if (dlg->copyMask())  { if (!line.isEmpty()) line.append("; ");  line+=mim.value(i, "").replace('.', loc.decimalPoint()); }
                data+=line+"\n";

                line="";
                if (dlg->copyID()) { if (!line.isEmpty()) line.append(", "); line+=QString::number(i); }
                if (dlg->copyCoordinates()) { if (!line.isEmpty()) line.append(", "); line+=QString::number(i%image.width())+", "+QString::number(i/image.width()); }
                if (dlg->copyParam()) { if (!line.isEmpty()) line.append(", "); line+=pim.value(i, ""); }
                if (dlg->copyGOF()) { if (!line.isEmpty()) line.append(", ");  line+=gim.value(i, ""); }
                if (dlg->copyOvf())  { if (!line.isEmpty()) line.append(", "); line+=oim.value(i, ""); }
                if (dlg->copyMask())  { if (!line.isEmpty()) line.append(", ");  line+=mim.value(i, ""); }
                qfdata+=(line+"\n");


            }

            QClipboard *clipboard = QApplication::clipboard();
            QMimeData* mime=new QMimeData();
            mime->setText(data);
            mime->setData("quickfit/csv", qfdata.toUtf8());
            clipboard->setMimeData(mime);
        }
        delete dlg;
    }

}


void QFRDRImagingFCSImageEditor::saveData() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        QStringList filters;
        filters<< tr("Comma-Separated value (*.dat *.csv)");
        filters<< tr("Semicolon-Separated value [German Excel] (*.txt)");
        filters<< tr("SYLK (*.slk)");
        filters<< tr("float-TIFF (*.tif)");
        filters<< tr("16-bit-TIFF (*.tif *.tif16)");
        filters<< tr("color-coded PNG image (*.png)");
        filters<< tr("Matlab Script (*.m)");
        filters<< tr("TAB-Separated value (*.dat *.csv)");
        filters<< tr("Semicolon-Separated value [decimal point] (*.dat *.csv)");

        QString selFilter=filters[0];

        QString fileName = qfGetSaveFileName(this, tr("Save Data"),
                                lastSavePath,
                                filters.join(";;"), &selFilter);
        if (fileName.isEmpty()) return;
        lastSavePath=QFileInfo(fileName).absolutePath();
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString ext=QFileInfo(fileName).suffix();
        QString base=QFileInfo(fileName).completeBaseName();
        QString path=QFileInfo(fileName).absolutePath();
        if (path.right(1)!="/") path=path+"/";

        QString fileNameParam=path+base+".param."+ext;
        QString fileNameGof=path+base+".gof."+ext;
        QString fileNameMask=path+base+".mask."+ext;
        QString fileNameOverview=path+base+".overview."+ext;
        QString fileNameMatlab=path+base+".m";

        bool saveParam=true;
        bool saveGof=true;
        bool saveMask=true;
        bool saveOverview=true;
        bool saveMatlab=true;
        if (selFilter!=filters[6]) {
            if (QFile::exists(fileNameParam)) {
                saveParam=(QMessageBox::question(this, tr("imFCS: Save Parameter Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameParam), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
            }
            if (QFile::exists(fileNameGof)) {
                saveGof=(QMessageBox::question(this, tr("imFCS: Save Goodnes-of-Fit Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameGof), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
            }
            if (QFile::exists(fileNameMask)) {
                saveMask=(QMessageBox::question(this, tr("imFCS: Save Mask Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameMask), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
            }
            if (QFile::exists(fileNameOverview)) {
                saveOverview=(QMessageBox::question(this, tr("imFCS: Save Overview Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameOverview), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
            }
        } else {
            if (QFile::exists(fileNameMatlab)) {
                saveMatlab=(QMessageBox::question(this, tr("imFCS: Save Images to Matlab script"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameMatlab), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
            }
        }

        JKImage<double> image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<double> gof_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<uint16_t> mask_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        JKImage<double> overview_image(m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        readParameterImage(image.data(), gof_image.data(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight(), currentEvalGroup(), currentFitParameter(), QFRDRImagingFCSImageEditor::itNone, currentGofParameter(), QFRDRImagingFCSImageEditor::itNone);
        //mask_image.assign(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        overview_image.assign(m->getImageFromRunsPreview(), m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        for (int32_t i=0; i<m->getImageFromRunsWidth()*m->getImageFromRunsHeight(); i++) {
            mask_image(i)=(plteOverviewExcludedData[i])?1:0;
        }
        if (selFilter==filters[0]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), ", ", '.');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), ", ", '.');
            if (saveMask) mask_image.save_csv(fileNameMask.toStdString(), ", ", '.');
            if (saveOverview) overview_image.save_csv(fileNameOverview.toStdString(), ", ", '.');
        } else if (selFilter==filters[1]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), "; ", ',');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), "; ", ',');
            if (saveMask) mask_image.save_csv(fileNameMask.toStdString(), "; ", ',');
            if (saveOverview) overview_image.save_csv(fileNameOverview.toStdString(), "; ", ',');
        } else if (selFilter==filters[7]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), "\t", '.');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), "\t", '.');
            if (saveMask) mask_image.save_csv(fileNameMask.toStdString(), "\t", '.');
            if (saveOverview) overview_image.save_csv(fileNameOverview.toStdString(), "\t", '.');
        } else if (selFilter==filters[8]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), "; ", '.');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), "; ", '.');
            if (saveMask) mask_image.save_csv(fileNameMask.toStdString(), "; ", '.');
            if (saveOverview) overview_image.save_csv(fileNameOverview.toStdString(), "; ", '.');
        } else if (selFilter==filters[2]) {
            if (saveParam)  image.save_sylk(fileNameParam.toStdString());
            if (saveGof) gof_image.save_sylk(fileNameGof.toStdString());
            if (saveMask) mask_image.save_sylk(fileNameMask.toStdString());
            if (saveOverview) overview_image.save_sylk(fileNameOverview.toStdString());
        } else if (selFilter==filters[3]) {
            if (saveParam) image.save_tifffloat(fileNameParam.toStdString());
            if (saveGof) gof_image.save_tifffloat(fileNameGof.toStdString());
            if (saveMask) mask_image.save_tifffloat(fileNameMask.toStdString());
            if (saveOverview) overview_image.save_tifffloat(fileNameOverview.toStdString());
        } else if (selFilter==filters[4]) {
            if (saveParam) image.save_tiffuint16(fileNameParam.toStdString());
            if (saveGof) gof_image.save_tiffuint16(fileNameGof.toStdString());
            if (saveMask) mask_image.save_tiffuint16(fileNameMask.toStdString());
            if (saveOverview) overview_image.save_tiffuint16(fileNameOverview.toStdString());
        } else if (selFilter==filters[6]) {
            if (saveMatlab) {
                QString data="";
                data.append(image.to_matlab("parameterImage").c_str());
                data.append(gof_image.to_matlab("goodnesOfFit").c_str());
                data.append(mask_image.to_matlab("mask").c_str());
                data.append(overview_image.to_matlab("overview").c_str());
                QFile file(fileNameMatlab);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out<<data;
                    file.close();
                }
            }
        } else if (selFilter==filters[5]) {
            if (saveParam) plteImage->drawImage().save(fileNameParam, "PNG");
            if (saveGof) plteGofImage->drawImage().save(fileNameGof, "PNG");
            if (saveMask) plteMask->drawImage().save(fileNameMask, "PNG");
            if (saveOverview) plteOverview->drawImage().save(fileNameOverview, "PNG");
        }


        QApplication::restoreOverrideCursor();

    }
}


void QFRDRImagingFCSImageEditor::createReportDoc(QTextDocument* document) {

    // make sure all widgets ahave the right size
    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;


    QTextCursor cursor(document);
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextSmaller=fText;
    fTextSmaller.setFontPointSize(0.7*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    fHeading1.setFontPointSize(1.4*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    QTextCharFormat fHeading2=fText;
    fHeading2.setFontPointSize(1.2*fText.fontPointSize());
    fHeading2.setFontWeight(QFont::Bold);
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    cursor.insertText(tr("imagingFCS Report: %1\n").arg(m->getName()), fHeading1);
    cursor.movePosition(QTextCursor::End);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTableFormat tableFormat1=tableFormat;
    tableFormat1.setWidth(QTextLength(QTextLength::VariableLength, 1));
    QTextTable* table = cursor.insertTable(5, 2, tableFormat);
    table->mergeCells(0,0,1,2);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>result set:</i> <b>%1</b></small>").arg(cmbResultGroup->currentText())));
        tabCursor=table->cellAt(1, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>parameter:</i> <b>%1</b></small>").arg(cmbParameter->currentText())));
        tabCursor=table->cellAt(1, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>transform:</i> <b>%1</b></small>").arg(cmbParameterTransform->currentText())));
        tabCursor=table->cellAt(2, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>goodnes of fit:</i> <b>%1</b></small>").arg(cmbGofParameter->currentText())));
        tabCursor=table->cellAt(2, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>transform:</i> <b>%1</b></small>").arg(cmbGofParameterTransform->currentText())));
        tabCursor=table->cellAt(3, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>color palette:</i> <b>%1</b></small>").arg(cmbColorbar->currentText())));
        tabCursor=table->cellAt(3, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>image style:</i> <b>%1</b></small>").arg(cmbImageStyle->currentText())));
        tabCursor=table->cellAt(4, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>color range:</i> <b>%1 ... %2</b></small>").arg(edtColMin->value()).arg(edtColMax->value())));
        tabCursor=table->cellAt(4, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>out-of-range mode:</i> <b>%1</b></small>").arg(cmbOutOfRangeMode->currentText())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Images:\n").arg(m->getName()), fHeading2);
    double w1=pltImage->width();
    double w2=(chkGofVisible->isChecked())?pltGofImage->width():0;
    double w3=(chkOverviewVisible->isChecked())?pltOverview->width():0;
    double w4=(chkMaskVisible->isChecked())?pltMask->width():0;

    double allwidth=w1+w2+w3+w4;
    table = cursor.insertTable(1, 4, tableFormat);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltImage->get_plotter()->draw(*painter, QRect(0,0,pltImage->width(),pltImage->height()));
        delete painter;
        double scale=document->textWidth()*w1/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("parameter image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    if (chkGofVisible->isChecked()) {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltGofImage->get_plotter()->draw(*painter, QRect(0,0,pltGofImage->width(),pltGofImage->height()));
        delete painter;
        double scale=document->textWidth()*w2/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("goodnes of fit image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    if (chkMaskVisible->isChecked()){
        QTextCursor tabCursor=table->cellAt(0, 2).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltMask->get_plotter()->draw(*painter, QRect(0,0,pltMask->width(),pltMask->height()));
        delete painter;
        double scale=document->textWidth()*w4/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("mask:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    if (chkOverviewVisible->isChecked()){
        QTextCursor tabCursor=table->cellAt(0, 3).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltOverview->get_plotter()->draw(*painter, QRect(0,0,pltOverview->width(),pltOverview->height()));
        delete painter;
        double scale=document->textWidth()*w3/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("overview image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    table = cursor.insertTable(2,1, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        plotter->get_plotter()->draw(*painter, QRect(0,0,plotter->width(),plotter->height()+plotterResid->height()));
        delete painter;
        double scale=0.7*document->textWidth()/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation curves:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(1,0).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        int tfsize=8;
        QString htmltable=tvParams->toHtml(0, true);
        int colcnt=htmltable.count("<th ",Qt::CaseInsensitive);
        if (colcnt>=24) tfsize=7;
        if (colcnt>=27) tfsize=6;
        if (colcnt>=30) tfsize=5;
        if (colcnt>=36) tfsize=4;
        if (colcnt>=42) tfsize=3;
        if (colcnt>=48) tfsize=2;
        if (colcnt>=54) tfsize=1;
        //qDebug()<<colcnt<<tfsize;
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: %2pt;\">%1</span></nobr></center>").arg(htmltable).arg(tfsize)));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();



    histogram->writeReport(cursor, document);
    if (cmbDualView->currentIndex()>0) histogram2->writeReport(cursor, document);
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();


    cursor.insertBlock();
    cursor.insertText(tr("Data Source:\n").arg(m->getName()), fHeading2);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(tr("files:\n"), fTextSmall);
    QStringList f=m->getFiles();
    QStringList t=m->getFilesTypes();
    for (int i=0; i<f.size(); i++) {
        QString typ="";
        if (i<t.size()) {
            if (!t[i].isEmpty()) {
                typ=QString("[%1]").arg(t[i]);
            }
        }
        cursor.insertText(tr("%1 %2\n").arg(f[i]).arg(typ), fTextSmaller);

    }

    if (!m->getDescription().isEmpty()) {
        cursor.insertText(tr("description: "), fTextBoldSmall);
        cursor.insertText(m->getDescription(), fTextSmall);
    }
    cursor.movePosition(QTextCursor::End);

}


void QFRDRImagingFCSImageEditor::replotHistogram() {
    histogram->replotHistogram();
    if (cmbDualView->currentIndex()>0) histogram2->replotHistogram();
}

void QFRDRImagingFCSImageEditor::updateHistogram() {

    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;


    bool dv=cmbDualView->currentIndex()>0;
    histogram->clear();

    if (plteImageData && (plteImageSize>=m->getImageFromRunsWidth()*m->getImageFromRunsHeight())) {
        int imageSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
        datahist=(double*)malloc(imageSize*sizeof(double));
        datasize=0;
        if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
            for (register int32_t i=0; i<imageSize; i++) {
                if (!m->leaveoutRun(i) && (!dv || (dv && !indexIsDualView2(i)))) {
                    datahist[datasize]=plteImageData[i];
                    datasize++;
                }
            }
        } else  {
            for (register int32_t i=0; i<imageSize; i++) {
                if (!dv || (dv && !indexIsDualView2(i))) {
                    datahist[datasize]=plteImageData[i];
                    datasize++;
                }
            }
        }
        histogram->addHistogram(tr("complete"), datahist, datasize);
        histogram->setHistogramXLabel(cmbParameter->currentText());
    }
    if (dv) {
        histogram2->setVisible(true);
        histogram2->clear();
        if (plteImageData && (plteImageSize>=m->getImageFromRunsWidth()*m->getImageFromRunsHeight())) {
            int imageSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
            datahist=(double*)malloc(imageSize*sizeof(double));
            datasize=0;
            if (chkExcludeExcludedRunsFromHistogram2->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (!m->leaveoutRun(i) && indexIsDualView2(i)) {
                        datahist[datasize]=plteImageData[i];
                        datasize++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (indexIsDualView2(i)) {
                        datahist[datasize]=plteImageData[i];
                        datasize++;
                    }
                }
            }
            histogram2->addHistogram(tr("complete"), datahist, datasize);
            histogram2->setHistogramXLabel(cmbParameter->currentText());
        }
    }

    updateSelectionHistogram(false);
    histogram->updateHistogram(true);
    if (dv) histogram2->updateHistogram(true);

}

void QFRDRImagingFCSImageEditor::updateSelectionHistogram(bool replot) {


    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    bool dv=cmbDualView->currentIndex()>0;

    if (plteImageData && (plteImageSize>=m->getImageFromRunsWidth()*m->getImageFromRunsHeight())) {
        int imageSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
        datahistsel=(double*)malloc(imageSize*sizeof(double));
        datasizesel=0;
        int32_t ii=0;
        if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
            for (register int32_t i=0; i<imageSize; i++) {
                if (selected.contains(i) && !m->leaveoutRun(i) && (!dv || (dv && !indexIsDualView2(i)))) {
                    datahistsel[datasizesel]=plteImageData[i];
                    datasizesel++;
                }
            }
        } else  {
            for (register int32_t i=0; i<imageSize; i++) {
                if (selected.contains(i) && (!dv || (dv && !indexIsDualView2(i)))) {
                    datahistsel[datasizesel]=plteImageData[i];
                    datasizesel++;
                }
            }
        }
        if (datasizesel>2) {
            if (histogram->histogramCount()>1) {
                histogram->setHistogram(1, tr("selection"), datahistsel, datasizesel);
                histogram->setHistogramXLabel(cmbParameter->currentText());
            } else {
                histogram->addHistogram(tr("selection"), datahistsel, datasizesel);
                histogram->setHistogramXLabel(cmbParameter->currentText());
            }
        }
    }
    if (dv) {
        histogram2->setVisible(true);
        if (plteImageData && (plteImageSize>=m->getImageFromRunsWidth()*m->getImageFromRunsHeight())) {
            int imageSize=m->getImageFromRunsWidth()*m->getImageFromRunsHeight();
            datahistsel=(double*)malloc(imageSize*sizeof(double));
            datasizesel=0;
            int32_t ii=0;
            if (chkExcludeExcludedRunsFromHistogram2->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i) && !m->leaveoutRun(i) && indexIsDualView2(i)) {
                        datahistsel[datasizesel]=plteImageData[i];
                        datasizesel++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i) && indexIsDualView2(i)) {
                        datahistsel[datasizesel]=plteImageData[i];
                        datasizesel++;
                    }
                }
            }
            if (datasizesel>2) {
                if (histogram2->histogramCount()>1) {
                    histogram2->setHistogram(1, tr("selection"), datahistsel, datasizesel);
                    histogram2->setHistogramXLabel(cmbParameter->currentText());
                } else {
                    histogram2->addHistogram(tr("selection"), datahistsel, datasizesel);
                    histogram2->setHistogramXLabel(cmbParameter->currentText());
                }
            }
        }
        if (replot) histogram2->updateHistogram(true, 1);
    }

    if (replot) histogram->updateHistogram(true, 1);

}

void QFRDRImagingFCSImageEditor::moveColorbarsAuto() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    bool rightVIsible=false;
    if (m && m->getImageFromRunsHeight() < m->getImageFromRunsWidth()) { // wider than high, i.e. "landscape"
        rightVIsible=!rightVIsible;
    }
    if ((double)pltImage->width()<=(double)pltImage->height()) {
        plteImage->set_colorBarRightVisible(rightVIsible);
        plteImage->set_colorBarTopVisible(!rightVIsible);
    } else {
        plteImage->set_colorBarRightVisible(!rightVIsible);
        plteImage->set_colorBarTopVisible(rightVIsible);
    }
    if ((double)pltGofImage->width()<=(double)pltGofImage->height()) {
        plteGofImage->set_colorBarRightVisible(rightVIsible);
        plteGofImage->set_colorBarTopVisible(!rightVIsible);
    } else {
        plteGofImage->set_colorBarRightVisible(!rightVIsible);
        plteGofImage->set_colorBarTopVisible(rightVIsible);
    }
    if ((double)pltOverview->width()<=(double)pltOverview->height()) {
        plteOverview->set_colorBarRightVisible(rightVIsible);
        plteOverview->set_colorBarTopVisible(!rightVIsible);
    } else {
        plteOverview->set_colorBarRightVisible(!rightVIsible);
        plteOverview->set_colorBarTopVisible(rightVIsible);
    }
}

void QFRDRImagingFCSImageEditor::showHidePlots()  {
    bool changed=  (splitterTop->widget(1)->isVisible() != chkGofVisible->isChecked())
                 ||(splitterTop->widget(2)->isVisible() != chkMaskVisible->isChecked())
                 ||(splitterTop->widget(3)->isVisible() != chkOverviewVisible->isChecked());

    if (changed) {
        int oldcnt=1;
        if (splitterTop->widget(1)->isVisible()) oldcnt++;
        if (splitterTop->widget(2)->isVisible()) oldcnt++;
        if (splitterTop->widget(3)->isVisible()) oldcnt++;

        int cnt=1;
        if (chkGofVisible->isChecked()) cnt++;
        if (chkMaskVisible->isChecked()) cnt++;
        if (chkOverviewVisible->isChecked()) cnt++;

        splitterTop->widget(1)->setVisible(chkGofVisible->isChecked());
        splitterTop->widget(2)->setVisible(chkMaskVisible->isChecked());
        splitterTop->widget(3)->setVisible(chkOverviewVisible->isChecked());

        if (cnt!=oldcnt) {
            QList<int> list;
            int corrWidth=splitterTop->width()-(cnt-1)*splitterTop->handleWidth();
            for (int i=0; i<splitterTop->count(); i++) {
                if (splitterTop->widget(i)->isVisible()) list<<corrWidth/cnt;
                else list<<0;
            }
            splitterTop->setSizes(list);
        }
    }
}

void QFRDRImagingFCSImageEditor::includeAll() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        m->leaveoutClear();
        m->recalcCorrelations();
    }
    rawDataChanged();
}

void QFRDRImagingFCSImageEditor::invertMask() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        m->maskInvert();
        m->recalcCorrelations();
    }
    rawDataChanged();
}

