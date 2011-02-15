#include <diagramscene.h>
#include <QtCore>
#include <QSvgGenerator>
#include <QPainter>
#include <Logger.h>
#include <CalibrationEnv.h>

DiagramScene::DiagramScene(QGraphicsView *view)  : QGraphicsScene(view)
{
    prec=1000.0;
    update=1.0;
    yscale=1.0;
    mouseline=0;
}

qreal DiagramScene::getMinValueX()
{
    return minvaluex;
}

qreal DiagramScene::getMaxValueX()
{
    return maxvaluex;
}

qreal DiagramScene::getMinValueY()
{
    return minvaluey;
}

qreal DiagramScene::getMaxValueY()
{
    return maxvaluey;
}

void DiagramScene::setPrec(qreal value)
{
    prec=value;
}

void DiagramScene::setUpdate(uint value)
{
    update=value;
}

void DiagramScene::setValues(QMap<QString, QVector<QPointF> > values)
{
    if(!values.keys().size())
    {
        mouseline=0;
        clear();
        return;
    }

    if(!values[values.keys().at(0)].size())
    {
        mouseline=0;
        clear();
        return;
    }

    bool dirty = true;
    int searchiteration=0;
    while(dirty && searchiteration<1000)
    {
        searchiteration++;
        mouseline=0;
        clear();

        for(int index=0; index < value.size(); index++)
            delete value[value.keys().at(index)];

        value.clear();
        minvaluex=0.0;
        maxvaluex=0.0;
        minvaluey=0.0;
        maxvaluey=0.0;

        if(!yscale)
            yscale=1;

        data=values;

        if(!values.size())
            return;

        for(int index=0; index < values.size(); index++)
        {
            QPen pen(QColor(((index+1)*948)%200+50,((index+1)*123)%200+50,((index+1)*11)%200+50));
            QPainterPath tmppath;
            QVector<QPointF> result = values[values.keys().at(index)];
            value[data.keys().at(index)] = new QMap<qreal,qreal>();

            for(int pointindex=0; pointindex < result.size(); pointindex++)
            {
                qreal x = (qreal)(result[pointindex].x());
                qreal y = (qreal)(result[pointindex].y());


                (*value[data.keys().at(index)])[x]=y;

                if(!pointindex && !index)
                {
                    minvaluex=x;
                    maxvaluex=x;
                    minvaluey=(double)y/(double)yscale;
                    maxvaluey=(double)y/(double)yscale;
                }

                if(!pointindex)
                    tmppath.moveTo(x*prec,(-y/yscale)*prec);
                else
                    tmppath.lineTo(x*prec,(-y/yscale)*prec);

                if(x > maxvaluex)
                    maxvaluex=(double)x;
                if(x < minvaluex)
                    minvaluex=(double)x;
                if((double)y/yscale > maxvaluey)
                    maxvaluey=(double)y/yscale;
                if((double)y/yscale < minvaluey)
                    minvaluey=(double)y/yscale;
            }

            addPath(tmppath,pen);
        }

        dirty = false;
        qreal maxyvalue = maxvaluey-minvaluey;

        if(maxvaluey==minvaluey)
            maxyvalue = qAbs(maxvaluey);

        qreal LOWY = 20.0;
        qreal UPPERY = 80.0;

        if(maxyvalue < LOWY || maxyvalue > UPPERY)
        {
            yscale *=maxyvalue/((double)(UPPERY-LOWY)/2.0 + LOWY);
            dirty = true;
        }
    }
    showGrid();

    qreal w , h;

    h = maxvaluey-minvaluey;
    w = maxvaluex-minvaluex;
    if(w < h * 3)
        w = h * 3;

    setSceneRect(minvaluex*prec,-maxvaluey*prec,w*prec, h*prec);

    QList<QGraphicsView*> viewers = views();
    for(int index=0; index < viewers.size(); index++)
    {
        viewers.at(index)->fitInView(sceneRect(),Qt::KeepAspectRatioByExpanding);
        viewers.at(index)->centerOn(maxvaluex*prec,qRound(sceneRect().topRight().y()));
    }
}

bool DiagramScene::showGrid()
{
    if(maxvaluey==minvaluey)
        maxvaluey=minvaluey+10;

    int split = 10;
    qreal xsteps = (maxvaluey-minvaluey)/4;
    QFont font;
    font.setPointSizeF((maxvaluey-minvaluey)*2*(prec/100));

    grid = new QGraphicsLineItem();
    //mouseline
    mouseline = new QGraphicsLineItem(minvaluex*prec,-minvaluey*prec,minvaluex*prec,-maxvaluey*prec);
    addItem(mouseline);

    QGraphicsLineItem *yline = new QGraphicsLineItem(minvaluex*prec,-maxvaluey*prec,minvaluex*prec,-minvaluey*prec,grid);
    Q_UNUSED(yline);
    qreal stepsize = (maxvaluey-minvaluey)/split;

    //y-axe
    for(qreal value=minvaluey + stepsize; value <= maxvaluey; value=value+stepsize)
    {
        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem ( QString::number(value*yscale), grid);
        text->setPos((minvaluex*prec+(maxvaluey-minvaluey)/100)*prec,-value*prec);
        text->setFont(font);
    }

    xaxes = new QGraphicsLineItem(grid);
    //x-axe
    QGraphicsLineItem *xline = new QGraphicsLineItem(minvaluex*prec,-minvaluey*prec,maxvaluex*prec,-minvaluey*prec,xaxes);
    Q_UNUSED(xline);

    for(qreal value=minvaluex; value < maxvaluex; value=value+xsteps)
    {
        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem ( QString::number(value), xaxes);
        text->setFont(font);
        qreal th = text->boundingRect().bottomLeft().y()-text->boundingRect().topLeft().y();
        text->setPos(value*prec+(minvaluex+(maxvaluey-minvaluey)/100)*prec, -minvaluey*prec-th);
    }

    //draw legend
    legend.clear();

    for(int index=0; index < data.size(); index++)
    {
        legend.push_back(new QGraphicsSimpleTextItem (data.keys().at(index), mouseline));
        legend[index]->setFont(font);
        qreal th = legend[index]->boundingRect().bottomLeft().y()-legend[index]->boundingRect().topLeft().y();
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

    qreal realx = (int)(x/prec)/(int)update;

    for(int index=0; index < legend.size(); index++)
    {
        QString text = value.keys().at(index) + ": x=" + QString::number(realx) + " y=" +  QString::number((*value[value.keys().at(index)])[realx]);
        legend[index]->setText(text);
    }
    return true;

}
