/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QMap>
#include <QMutex>
#include <QTime>

class DiagramScene : public QGraphicsScene
{
    private:
        QGraphicsItem *grid, *xaxes;
        QVector<QGraphicsSimpleTextItem*> legend;
        QGraphicsLineItem *mouseline;
        QMap<QString, QVector<QPointF> > data;
        QMap<QString, QMap<qreal,qreal>* > value;
        qreal maxvaluey;
        qreal maxvaluex;
        qreal minvaluey;
        qreal minvaluex;
        qreal prec;
        qreal yscale;
        uint update;

    public:
        DiagramScene(QGraphicsView *view);
        bool setMousePosition(qreal x, qreal y);
        void setValues(QMap<QString, QVector<QPointF> > values);
        void setPrec(qreal value);
        void setUpdate(uint value);
        qreal getMinValueX();
        qreal getMaxValueX();
        qreal getMinValueY();
        qreal getMaxValueY();

    private:
        bool showGrid();
};

#endif // DIAGRAMSCENE_H
