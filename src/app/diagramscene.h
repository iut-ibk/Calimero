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
