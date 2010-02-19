#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QMap>
#include <QMutex>
#include "ProgressBar.h"
#include <QTime>

class DiagramScene
{
    private:
        QString positiontext;
        QMap<QString,QVector<QVector<double> > > currentparameters;
        QMutex *mutex;
        QGraphicsView *viewer;
        QGraphicsLineItem *xline;
        QGraphicsRectItem *rect;
        QGraphicsTextItem *position;
        QGraphicsScene *currentscene, *scenebuffer, *svgbuffer;
        QGraphicsItem *grid;
        QMap<QString, QGraphicsPathItem*> map;
        double minvalue, maxvalue, fontsize;
        int maxiterations, startiteration, currentmouseposition;
        bool follow;
        double diny;
        QTime timer;
        int elapsed, parameterindex;
        ::ProgressBar *progressbar;


    public:
        DiagramScene(QGraphicsView *view);
        double getMinValue();
        double getMaxValue();
        bool updateScene(QMap<QString,QVector<QVector<double> > > parametermap, bool onlysvgbuffer=false);
        QGraphicsScene* getScene();
        void setFollow(bool followbool);
        bool setMousePosition(qreal x, qreal y);
        double getMaxIteration();
        bool enableScene();
        void setPositionText(QString text);
        int getXPositionOfCourser();
        bool saveSvg(QString filename);

    private:
        bool showResultOfParameter(QString name, QPen pen);
        bool showGrid();
        void clear();
};

#endif // DIAGRAMSCENE_H
