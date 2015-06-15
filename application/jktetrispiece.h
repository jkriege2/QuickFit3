#ifndef JKTETRISPIECE_H
#define JKTETRISPIECE_H

#include <QPixmap>
#include <QPainter>

class JKTetrisPiece
{
    public:
        enum {
            PieceCount=7,
            PieceSize=4
        };



        explicit JKTetrisPiece(int* board=NULL, int boardWidth=0, int boardHeight=0);
        ~JKTetrisPiece();

        bool checkHit() const;

        inline int getX() const {
            return x;
        }
        inline int getY() const {
            return y;
        }

        void setBoard(int* board, int boardWidth, int boardHeight);
        bool init(const JKTetrisPiece& other);

        bool initTop(int type=-1);

        bool moveDown();
        bool moveLeft();
        bool moveRight();

        bool rotateRight();
        bool rotateLeft();

        void saveToBoard(int* rowsRemoved=NULL);

        QPixmap drawPiece(QSize tilesize=QSize(16,16), int xoffset=0, int yoffset=0) const;
        void drawPiece(QPainter* painter,QSize tilesize=QSize(16,16), int xoffset=0, int yoffset=0) const;
        void drawBoard(QPainter* painter, QSize tilesize=QSize(16,16), bool withPiece=true) const;


    protected:
        int type;
        int rotation;

        int x;
        int y;

        static int PieceData[PieceCount][4][16];
        static QColor PieceColor[PieceCount];

        int* board;
        int boardWidth;
        int boardHeight;
};

#endif // JKTETRISPIECE_H
