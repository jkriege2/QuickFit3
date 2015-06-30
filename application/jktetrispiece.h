/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
