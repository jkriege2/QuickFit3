#include "jktetrisframe.h"
#include <QDebug>
#include <QApplication>
JKTetrisFrame::JKTetrisFrame(QWidget *parent):
    QFrame(parent)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);

    timer=new QTimer(this);
    timer->setSingleShot(true);
    timer->stop();
    board=NULL;
    tileSize=QSize(16,16);
    resizeBoard(11,24);
    level=0;
    score=0;
    fastDown=false;
    dropping=false;
    linesRemoved=0;
    linesRemovedSum=0;
    running=false;
    connect(timer, SIGNAL(timeout()), this, SLOT(nextStep()));
}

JKTetrisFrame::~JKTetrisFrame()
{
    if (board) free(board);
}

void JKTetrisFrame::resizeBoard(int w, int h)
{
    boardWidth=w;
    boardHeight=h;
    if (board) free(board);
    board=(int*)malloc(w*h*sizeof(int));
    for (int i=0; i<w*h; i++) {
        board[i]=-1;
    }
    piece.setBoard(board, boardWidth, boardHeight);
    nextPiece.setBoard(board, boardWidth, boardHeight);
    piece.initTop();
    nextPiece.initTop();
    setMinimumSize(minimumSizeHint());
    setMaximumHeight(minimumSizeHint().height());
}

QSize JKTetrisFrame::sizeHint() const
{
    return QSize(boardWidth * tileSize.width() + frameWidth() * 2,
                 boardHeight * tileSize.height() + frameWidth() * 2);
}

QSize JKTetrisFrame::minimumSizeHint()
{
    return sizeHint();
}

int JKTetrisFrame::getScore() const
{
    return score;
}

int JKTetrisFrame::getLevel() const
{
    return level;
}

void JKTetrisFrame::setNextPieceLabel(QLabel *label)
{
    labNextPiece=label;
}

void JKTetrisFrame::setScoreLabel(QLabel *label)
{
    labScore=label;
}

void JKTetrisFrame::setLevelLabel(QLabel *label)
{
    labLevel=label;
}

void JKTetrisFrame::setLinesRemovedLabel(QLabel *label)
{
    labLinesRemoved=label;
}

int JKTetrisFrame::getInterval()
{
    int factor=1;
    if (fastDown) factor=10;
    if (dropping) factor=20;
    return qBound(1, 1750 / ((1 + level)*factor), 1000);
}

void JKTetrisFrame::start()
{
    setFocus();
    timer->stop();
    resizeBoard(boardWidth, boardHeight);
    showPiece();
    setLevel(0);
    setScore(0);
    setLinesRemoved(0,0);
    fastDown=false;
    dropping=false;
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(getInterval());
    timer->setSingleShot(true);
    timer->start();
    running=true;
    emit setPauseState(false);
    emit pauseEnabled(true);
}

void JKTetrisFrame::pause(bool enabled)
{

    if (!enabled) {
        running=true;
        timer->start();
        //emit pauseEnabled(true);
    } else {
        timer->stop();
        running=false;

    }
    emit setPauseState(enabled);
    emit pauseEnabled(true);
}

void JKTetrisFrame::nextStep()
{
    int interval=getInterval();
    if (!piece.moveDown()) {
        savePieceToBoard(interval);

    }
    repaint();


    if (running) {
        //qDebug()<<"nextStep: "<<interval<<"  "<<fastDown;
        timer->setInterval(interval);
        timer->setSingleShot(true);
        timer->start();
    }
}

void JKTetrisFrame::savePieceToBoard(int &interval)
{
    dropping=false;
    interval=getInterval();
    int removed=0;
    piece.saveToBoard(&removed);
    if (piece.init(nextPiece)) {
        nextPiece.initTop(-1);
        setScore(score+1);

        if (removeLines(removed)) {
            interval=getInterval()+2000;
        } else if (removed>0) {
            interval=getInterval()+500;
        }

        showPiece();
    } else {
        gameIsOver();
    }
}

void JKTetrisFrame::gameIsOver()
{
    timer->stop();
    running=false;
    emit setPauseState(false);
    emit pauseEnabled(false);
    emit gameOver(score, level);
}

void JKTetrisFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    QRect rect = contentsRect();
    painter.translate(rect.left(), rect.bottom()-2-boardHeight*tileSize.height());
    piece.drawBoard(&painter, tileSize, true);
}

void JKTetrisFrame::keyPressEvent(QKeyEvent *event)
{
    if (!running) {
        QFrame::keyPressEvent(event);
        return;
    }

    bool moved=false;
    switch (event->key()) {
        case Qt::Key_Left:
            moved=piece.moveLeft();
            break;
        case Qt::Key_Right:
            moved=piece.moveRight();
            break;
        case Qt::Key_Down:
            if (!fastDown && !event->isAutoRepeat()) {
                //qDebug()<<"fastDown="<<fastDown<<"  (keyPressEvent())";
                fastDown=true;
                //moved=piece.rotateRight();
                timer->stop();
                timer->setInterval(getInterval());
                timer->start();
            }
            break;
        case Qt::Key_Up:
            moved=piece.rotateLeft();
            break;
        case Qt::Key_Space:
            if (!dropping) dropDown();
            else dropping=false;
            break;
        case Qt::Key_Pause:
        case Qt::Key_Escape:
            pause(running);
            break;
        default:
            QFrame::keyPressEvent(event);
    }

    if (moved) {
        repaint();
    }
    QApplication::processEvents();
}

void JKTetrisFrame::keyReleaseEvent(QKeyEvent *event)
{
    if (!running || dropping) {
        QFrame::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_Down:
            if (fastDown && !event->isAutoRepeat()) {
                fastDown=false;
                //qDebug()<<"fastDown="<<fastDown<<" (keyReleaseEvent())";
                timer->stop();
                timer->setInterval(getInterval());
                timer->start();
            }
            break;
        default:
            QFrame::keyReleaseEvent(event);
    }
}

void JKTetrisFrame::setScore(int score)
{
    this->score=score;
    if (labScore) labScore->setText(QString::number(this->score));
}

void JKTetrisFrame::setLevel(int level)
{
    this->level=level;
    if (labLevel) labLevel->setText(QString::number(this->level));
}

void JKTetrisFrame::setLinesRemoved(int removed, int sum)
{
    this->linesRemoved=removed;
    this->linesRemovedSum=sum;
    if (labLinesRemoved) labLinesRemoved->setText(QString("%1 (%2)").arg(linesRemovedSum).arg(linesRemoved));
}

void JKTetrisFrame::showPiece()
{
    if (labNextPiece) {
        labNextPiece->setPixmap(nextPiece.drawPiece(tileSize));
    }
}

void JKTetrisFrame::dropDown()
{
    fastDown=false;
    //qDebug()<<"fastDown="<<fastDown<<" (dropDown())";
    dropping=true;
    timer->stop();
    timer->setInterval(getInterval());
    timer->start();
}


bool JKTetrisFrame::removeLines(int removed)
{
    int oldlevel=level;
    linesRemoved+=removed;
    linesRemovedSum+=removed;
    int score4=removed/4;
    removed=removed%4;
    int score2=removed/2;
    removed=removed%2;
    int score1=removed;

    score=score+score4*80+score2*30+score1*10;
    if (linesRemoved>10) {
        linesRemoved=linesRemoved-10;
        level++;
    }
    setLinesRemoved(linesRemoved,linesRemovedSum);
    setLevel(level);
    setScore(score);
    return oldlevel!=level;
}

