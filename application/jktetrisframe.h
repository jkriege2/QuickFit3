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

#ifndef JKTETRISFRAME_H
#define JKTETRISFRAME_H

#include <QFrame>
#include <QPointer>
#include <QLabel>
#include "jktetrispiece.h"
#include <QTimer>
#include <QKeyEvent>
#include <QPaintEvent>

class JKTetrisFrame : public QFrame {
        Q_OBJECT
    public:
        explicit JKTetrisFrame(QWidget* parent=NULL);
        virtual ~JKTetrisFrame();

        void resizeBoard(int w, int h);

        QSize sizeHint() const;
        QSize minimumSizeHint();
        int getScore() const;
        int getLevel() const;
        int getBoardWidth() const;
        int getBoardHeight() const;
        QSize getTileSize() const;
        void setTileSize(QSize size);

        void setNextPieceLabel(QLabel* label);
        void setScoreLabel(QLabel* label);
        void setLevelLabel(QLabel* label);
        void setLinesRemovedLabel(QLabel* label);
        int getInterval();
    public slots:
        void start();
        void pause(bool enabled);
        void nextStep();
        void savePieceToBoard(int &interval);
    signals:
        void pauseEnabled(bool enabled);
        void gameOver(int score, int level);
        void setPauseState(bool checked);
    protected slots:

    protected:
        void gameIsOver();
        void paintEvent(QPaintEvent* event);
        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);
        void setScore(int score);
        void setLevel(int level);
        void setLinesRemoved(int removed, int sum);
        void showPiece();
        void dropDown();

        bool removeLines(int lines);


        QTimer* timer;
        JKTetrisPiece piece;
        JKTetrisPiece nextPiece;
        int* board;
        int boardWidth;
        int boardHeight;
        int level;
        int score;
        bool running;
        bool fastDown;
        bool dropping;
        int linesRemoved;
        int linesRemovedSum;
        QSize tileSize;
        QPointer<QLabel> labNextPiece;
        QPointer<QLabel> labScore;
        QPointer<QLabel> labLevel;
        QPointer<QLabel> labLinesRemoved;
};

#endif // JKTETRISFRAME_H
