#include "jktetrispiece.h"
#include <QtGlobal>
#include <QDebug>

int JKTetrisPiece::PieceData[JKTetrisPiece::PieceCount][4][JKTetrisPiece::PieceSize*JKTetrisPiece::PieceSize] = {
    { {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}, {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0} },
    { {0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0}, {1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0}, {0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0}, {0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0} },
    { {1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0}, {1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0}, {1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0}, {1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0} },
    { {1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0}, {1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0}, {1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0}, {0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0} },
    { {0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0}, {1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0}, {1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0}, {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0} },
    { {0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0}, {1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0}, {0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0}, {1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0} },
    { {1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}, {0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0}, {1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}, {0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0} },
};

QColor JKTetrisPiece::PieceColor[JKTetrisPiece::PieceCount] = {
    QColor("red") ,
    QColor("goldenrod") ,
    QColor("darkgreen") ,
    QColor("magenta") ,
    QColor("blue") ,
    QColor("orange") ,
    QColor("green") ,
};

JKTetrisPiece::JKTetrisPiece(int *board, int boardWidth, int boardHeight)
{
    rotation=0;
    type=0;
    x=0;
    y=0;
    this->board=board;
    this->boardWidth=boardWidth;
    this->boardHeight=boardHeight;
}

JKTetrisPiece::~JKTetrisPiece()
{

}

bool JKTetrisPiece::checkHit() const
{
    const int* p=JKTetrisPiece::PieceData[type][rotation];
    int i=0;
    for (int yy=0; yy<JKTetrisPiece::PieceSize; yy++) {
        for (int xx=0; xx<JKTetrisPiece::PieceSize; xx++) {
            if (p[i]>0) {
                if (x+xx<0 || x+xx>=boardWidth) return true;
                if (y+yy<0 || y+yy>=boardHeight) return true;
                if (board[(y+yy)*boardWidth+ (x+xx)]>=0) {
                    return true;
                }
            }
            i++;
        }
    }
    return false;
}

void JKTetrisPiece::setBoard(int *board, int boardWidth, int boardHeight)
{
    this->board=board;
    this->boardWidth=boardWidth;
    this->boardHeight=boardHeight;
    initTop();
}

bool JKTetrisPiece::init(const JKTetrisPiece &other)
{
    this->x=other.x;
    this->y=other.y;
    this->type=other.type;
    this->rotation=other.rotation;
    return !checkHit();
}

bool JKTetrisPiece::initTop(int type)
{
    this->x=boardWidth/2-JKTetrisPiece::PieceSize/2;//qrand()%boardWidth;
    this->y=0;
    rotation=0;
    this->type=type;
    if (type<0) {
        this->type=qrand()%JKTetrisPiece::PieceCount;
    }

    /*while (checkHit()) {
        x=qrand()%boardWidth;
    }*/
    return !checkHit();
}

bool JKTetrisPiece::moveDown()
{
    if (y>=boardHeight) return false;
    int oldy=y;
    y++;
    if (!checkHit()) {
        return true;
    } else {
        y=oldy;
        return false;
    }
}

bool JKTetrisPiece::moveLeft()
{
    int oldx=x;
    x--;
    if (!checkHit()) {
        return true;
    } else {
        x=oldx;
        return false;
    }
}

bool JKTetrisPiece::moveRight()
{
    int oldx=x;
    x++;
    if (!checkHit()) {
        return true;
    } else {
        x=oldx;
        return false;
    }
}

bool JKTetrisPiece::rotateRight()
{
    int oldrot=rotation;
    rotation=rotation+1;
    if (rotation>3) rotation=0;
    if (!checkHit()) {
        return true;
    } else {
        rotation=oldrot;
        return false;
    }
}

bool JKTetrisPiece::rotateLeft()
{
    int oldrot=rotation;
    rotation=rotation-1;
    if (rotation<0) rotation=3;
    if (!checkHit()) {
        return true;
    } else {
        rotation=oldrot;
        return false;
    }
}

void JKTetrisPiece::saveToBoard(int *rowsRemoved)
{
    const int* p=JKTetrisPiece::PieceData[type][rotation];
    int i=0;
    for (int yy=0; yy<JKTetrisPiece::PieceSize; yy++) {
        for (int xx=0; xx<JKTetrisPiece::PieceSize; xx++) {
            if (p[i]>0) {
                if (x+xx>=0 && x+xx<boardWidth && y+yy>=0 && y+yy<boardHeight) {
                    board[(y+yy)*boardWidth+x+xx]=type;
                }
            }
            i++;
        }
    }

    if (rowsRemoved) *rowsRemoved=0;
    bool removed=true;
    while (removed) {
        bool remove=true;
        removed=false;
        int removeRow=-1;

        for (int yy=boardHeight-1; yy>=0; yy--) {
            remove=true;
            for (int xx=0; xx<boardWidth; xx++) {
                remove=remove&&(board[yy*boardWidth+xx]>=0);
                if (!remove) break;
            }
            //qDebug()<<"   yy="<<yy<<"  remove="<<remove;
            if (remove) {
                removeRow=yy;
                break;
            }
        }
        //qDebug()<<"remove "<<remove<<" row="<<removeRow;
        if (remove && removeRow>=0) {
            if (rowsRemoved) *rowsRemoved=*rowsRemoved+1;
            for (int yy=removeRow; yy>=0; yy--) {
                if (yy>=1) {
                    for (int xx=0; xx<boardWidth; xx++) {
                        board[yy*boardWidth+xx]=board[(yy-1)*boardWidth+xx];
                    }
                } else {
                    for (int xx=0; xx<boardWidth; xx++) {
                        board[yy*boardWidth+xx]=-1;
                    }
                }
            }
            removed=true;
        }
    }

    /*if (initNew) {
        initTop();
    }*/
}

QPixmap JKTetrisPiece::drawPiece(QSize tilesize, int xoffset, int yoffset) const
{
    QPixmap pix(tilesize.width()*JKTetrisPiece::PieceSize, tilesize.height()*JKTetrisPiece::PieceSize);
    pix.fill(QColor("grey"));
    {
        QPainter painter(&pix);
        drawPiece(&painter, tilesize, xoffset, yoffset);
        painter.end();
    }
    return pix;
}

void JKTetrisPiece::drawPiece(QPainter *painter, QSize tilesize, int xoffset, int yoffset) const
{
    QColor col=JKTetrisPiece::PieceColor[type];
    QColor borderCol=col.darker();
    int i=0;
    const int* p=JKTetrisPiece::PieceData[type][rotation];
    for (int yy=0; yy<JKTetrisPiece::PieceSize; yy++) {
        for (int xx=0; xx<JKTetrisPiece::PieceSize; xx++) {
            if (p[i]>0) {
                painter->save();
                QRect r(xoffset+xx*tilesize.width(), yoffset+yy*tilesize.height(), tilesize.width()-1, tilesize.height()-1);
                painter->fillRect(r,col);
                painter->setPen(borderCol);
                painter->drawRect(r);
                painter->restore();
            }
            i++;
        }
    }
}

void JKTetrisPiece::drawBoard(QPainter *painter, QSize tilesize, bool withPiece) const
{
    painter->save();
    QPen p(QColor("black"));
    p.setWidthF(2.5);
    painter->setPen(p);
    painter->drawLine(0,boardHeight*tilesize.height(), boardWidth*tilesize.width(), boardHeight*tilesize.height());
    painter->restore();
    int i=0;
    for (int yy=0; yy<boardHeight; yy++) {
        for (int xx=0; xx<boardWidth; xx++) {
            if (board[i]>=0){
                QColor col=JKTetrisPiece::PieceColor[board[i]];
                QColor borderCol=col.darker();
                painter->save();
                QRect r(xx*tilesize.width(), yy*tilesize.height(), tilesize.width()-1, tilesize.height()-1);
                painter->fillRect(r,col);
                painter->setPen(borderCol);
                painter->drawRect(r);
                painter->restore();
            }
            i++;
        }
    }
    if (withPiece) {
        drawPiece(painter, tilesize, x*tilesize.width(), y*tilesize.height());
    }

}

