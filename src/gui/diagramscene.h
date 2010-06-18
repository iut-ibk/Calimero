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
        QMap<QString, QMap<double,double>* > value;
        double maxvaluey;
        double maxvaluex;
        double minvaluey;
        double minvaluex;
        double prec;
        uint update;

    public:
        DiagramScene(QGraphicsView *view);
        bool setMousePosition(qreal x, qreal y);
        void setValues(QMap<QString, QVector<QPointF> > values);
        void setPrec(double value);
        void setUpdate(uint value);
        double getMinValueX();
        double getMaxValueX();
        double getMinValueY();
        double getMaxValueY();

    private:
        bool showGrid();
};

#endif // DIAGRAMSCENE_H
