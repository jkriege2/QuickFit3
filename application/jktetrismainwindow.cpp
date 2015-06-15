#include "jktetrismainwindow.h"
#include "ui_jktetrismainwindow.h"
#include "programoptions.h"

JKTetrisMainWindow::JKTetrisMainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JKTetrisMainWindow)
{
    ui->setupUi(this);

    ui->Tetris->setLevelLabel(ui->labLevel);
    ui->Tetris->setScoreLabel(ui->labPoints);
    ui->Tetris->setLinesRemovedLabel(ui->labLinesRemoved);
    ui->Tetris->setNextPieceLabel(ui->labNext);
    connect(ui->btnStart, SIGNAL(clicked()), ui->Tetris, SLOT(start()));
    connect(ui->btnPause, SIGNAL(toggled(bool)), ui->Tetris, SLOT(pause(bool)));
    connect(ui->Tetris, SIGNAL(pauseEnabled(bool)), ui->btnPause, SLOT(setEnabled(bool)));
    connect(ui->Tetris, SIGNAL(gameOver(int,int)), this, SLOT(gameOver(int,int)));
    connect(ui->Tetris, SIGNAL(setPauseState(bool)), ui->btnPause, SLOT(setChecked(bool)));
    ui->Tetris->start();
    ProgramOptions::getConfigWindowGeometry(this, "quickfit/entertain/window");
    ui->labHighscore->setText(tr("HIGHSCORE: %1 [level: %3] (%2)")
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_date", QDate::currentDate()).toDate().toString())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_level", 0).toInt()));
}

JKTetrisMainWindow::~JKTetrisMainWindow()
{
    delete ui;
}

void JKTetrisMainWindow::gameOver(int score, int level)
{
    QMessageBox::information(this, tr("Game Over!"), tr("Your game is over in level %2!\n\nSCORE = %1\n").arg(score).arg(level));
    if (score>ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt()) {
        ProgramOptions::setConfigValue("quickfit/entertain/highscore", score);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_level", level);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_date", QDate::currentDate());
    }
    ui->labHighscore->setText(tr("HIGHSCORE: %1 [level: %3] (%2)")
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_date", QDate::currentDate()).toDate().toString())
                              .arg(ProgramOptions::getConfigValue("quickfit/entertain/highscore_level", 0).toInt()));
}

void JKTetrisMainWindow::on_btnClose_clicked()
{
    close();
}

void JKTetrisMainWindow::closeEvent(QCloseEvent *event)
{
    ProgramOptions::setConfigWindowGeometry(this, "quickfit/entertain/window");

    int score=ui->Tetris->getScore();
    int level=ui->Tetris->getLevel();

    if (score>ProgramOptions::getConfigValue("quickfit/entertain/highscore", 0).toInt()) {
        ProgramOptions::setConfigValue("quickfit/entertain/highscore", score);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_level", level);
        ProgramOptions::setConfigValue("quickfit/entertain/highscore_date", QDate::currentDate());
    }

    QWidget::closeEvent(event);
}

void JKTetrisMainWindow::focusOutEvent(QFocusEvent *event)
{
    if (ui->btnPause->isEnabled()) {
        ui->btnPause->setChecked(true);
    }
    QWidget::focusOutEvent(event);
}
