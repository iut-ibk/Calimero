#include <diagramscene.h>
#include <QtCore>
#include <QSvgGenerator>
#include <QPainter>
#include <Logger.h>

DiagramScene::DiagramScene(QGraphicsView *view)  : QGraphicsScene(view)
{
    minsceneh=0;
    minscenew=0;
    prec=100;
    update=1;
}

double DiagramScene::getMinValueX()
{
    return minvaluex;
}

double DiagramScene::getMaxValueX()
{
    return maxvaluex;
}

double DiagramScene::getMinValueY()
{
    return minvaluey;
}

double DiagramScene::getMaxValueY()
{
    return maxvaluey;
}

void DiagramScene::setMinViewBox(uint h, uint w)
{
    minscenew=w;
    minsceneh=h;
}

void DiagramScene::setPrec(double value)
{
    prec=value;
}

void DiagramScene::setUpdate(uint value)
{
    update=value;
}

void DiagramScene::setValues(QMap<QString, QVector<QPointF> > values)
{
    clear();
    for(int index=0; index < value.size(); index++)
        delete value[value.keys().at(index)];

    value.clear();
    minvaluex=0;
    maxvaluex=0;
    minvaluey=0;
    maxvaluey=0;

    data=values;

    if(!values.size())
        return;

    for(int index=0; index < values.size(); index++)
    {
        QPen pen(QColor(((index+1)*948)%200+50,((index+1)*123)%200+50,((index+1)*11)%200+50));
        QPainterPath tmppath;
        QVector<QPointF> result = values[values.keys().at(index)];
        value[data.keys().at(index)] = new QMap<double,double>();

        for(int pointindex=0; pointindex < result.size(); pointindex++)
        {
            double x = result[pointindex].x();
            double y = result[pointindex].y();
            (*value[data.keys().at(index)])[x]=y;

            if(!pointindex && !index)
            {
                minvaluex=x;
                maxvaluex=x;
                minvaluey=y;
                maxvaluey=y;
            }


            if(!pointindex)
                tmppath.moveTo(x*prec,-y*prec);
            else
                tmppath.lineTo(x*prec,-y*prec);

            if(x > maxvaluex)
                maxvaluex=x;
            if(x < minvaluex)
                minvaluex=x;
            if(y > maxvaluey)
                maxvaluey=y;
            if(y < minvaluey)
                minvaluey=y;
        }

        addPath(tmppath,pen);
    }

    showGrid();

    double w , h;

    if(maxvaluex-minvaluex < minscenew)
        w = minscenew;
    else
        w = maxvaluex-minvaluex;

    if(maxvaluey-minvaluey < minsceneh)
        h = minsceneh;
    else
        h = maxvaluey-minvaluey;



    setSceneRect(minvaluex*prec,-maxvaluey*prec,w*prec, h*prec);


    QList<QGraphicsView*> viewers = views();
    for(int index=0; index < viewers.size(); index++)
    {
        viewers.at(index)->fitInView(sceneRect(),Qt::KeepAspectRatioByExpanding);
        viewers.at(index)->centerOn(sceneRect().topRight().x(),qRound(sceneRect().topRight().y()));
    }
}

bool DiagramScene::showGrid()
{
    int split = 10;
    double xsteps = (maxvaluey-minvaluey)/4;
    QFont font;
    font.setPointSizeF((maxvaluey-minvaluey)*2*(prec/100));

    grid = new QGraphicsLineItem();
    //mouseline
    mouseline = new QGraphicsLineItem(minvaluex*prec,-minvaluey*prec,minvaluex*prec,-maxvaluey*prec);
    addItem(mouseline);

    QGraphicsLineItem *yline = new QGraphicsLineItem(minvaluex*prec,-maxvaluey*prec,minvaluex*prec,-minvaluey*prec,grid);
    double stepsize = (maxvaluey-minvaluey)/split;

    //y-axe
    for(double value=minvaluey + stepsize; value <= maxvaluey; value=value+stepsize)
    {
        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem ( QString::number(value), grid);
        text->setPos((minvaluex*prec+(maxvaluey-minvaluey)/100)*prec,-value*prec);
        text->setFont(font);
    }

    xaxes = new QGraphicsLineItem(grid);
    //x-axe
    QGraphicsLineItem *xline = new QGraphicsLineItem(minvaluex*prec,-minvaluey*prec,maxvaluex*prec,-minvaluey*prec,xaxes);
    for(double value=minvaluex; value < maxvaluex; value=value+xsteps)
    {
        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem ( QString::number(value), xaxes);
        text->setFont(font);
        double th = text->boundingRect().bottomLeft().y()-text->boundingRect().topLeft().y();
        text->setPos(value*prec+(minvaluex+(maxvaluey-minvaluey)/100)*prec, -minvaluey*prec-th);
    }

    //draw legend
    legend.clear();

    for(int index=0; index < data.size(); index++)
    {
        legend.push_back(new QGraphicsSimpleTextItem (data.keys().at(index), mouseline));
        legend[index]->setFont(font);
        double th = legend[index]->boundingRect().bottomLeft().y()-legend[index]->boundingRect().topLeft().y();
        QGraphicsRectItem *rect = new QGraphicsRectItem( (minvaluex*prec+((maxvaluey-minvaluey)/100)*3)*prec, -maxvaluey*prec + index*th*1.5 , th,th, mouseline);
        QBrush brush( QColor(((index+1)*948)%200+50,((index+1)*123)%200+50,((index+1)*11)%200+50));
        rect->setBrush(brush);
        legend[index]->setPos((minvaluex*prec+((maxvaluey-minvaluey)/100)*8)*prec,-maxvaluey*prec + index*th*1.5 );
    }

    addItem(grid);
    return true;
}

bool DiagramScene::setMousePosition(qreal x, qreal y)
{
    if(!mouseline)
        return false;

    if(minvaluex==maxvaluex)
        return false;

    mouseline->setPos(x,mouseline->pos().y());
    mouseline->cursor();

    double realx = (int)(x/prec)/(int)update;

    for(int index=0; index < legend.size(); index++)
    {
        QString text = value.keys().at(index) + ": x=" + QString::number(realx) + " y=" +  QString::number((*value[value.keys().at(index)])[realx]);
        legend[index]->setText(text);
    }
    return true;

}
