#include "Diagramscene.h"
#include <QtCore>
#include <QSvgGenerator>
#include <QPainter>
#include "ProgressBar.h"

DiagramScene::DiagramScene(QGraphicsView *view)
{
    if(view==NULL)
        qFatal("DiagramScene::DiagramScene()");

    viewer=view;
    currentscene = new QGraphicsScene(viewer);
    fontsize=0;
    startiteration = 0;
    maxiterations = 0;
    maxvalue = 10;
    minvalue = -10;
    follow=false;   
    positiontext="";
    currentmouseposition=0;
    diny=false;
    svgbuffer=NULL;
    progressbar = new ProgressBar(viewer);
    elapsed = 100;
    parameterindex = 1;
    mutex = new QMutex(QMutex::Recursive);
}

QGraphicsScene* DiagramScene::getScene()
{
    return currentscene;
}

bool DiagramScene::updateScene(QMap<QString,QVector<QVector<double> > > parametermap, bool onlysvgbuffer)
{
    if(!mutex->tryLock())
        return false;

    QMutexLocker locker(mutex);
    qDebug("DiagramScene::updateScene()");

    if(parametermap.size()>0 && startiteration>0 && parametermap.value(parametermap.keys().at(0)).size()==startiteration)
    {
        qDebug("DiagramScene::updateScene() Nothing to do");
        return true;
    }

    qDebug("DiagramScene::updateScene() Stage:1");
    maxiterations=0;

    if(parametermap.size()>0)
        maxiterations=parametermap.value(parametermap.keys().at(0)).size();

    currentparameters = parametermap;

    timer.restart();


    qDebug("DiagramScene::updateScene() Stage:2");
    if(follow)
    {
        int maxit = maxiterations;
        clear();
        maxiterations=maxit;
        startiteration= std::max(0,static_cast<int>(maxiterations-150));
    }
    else
    {
        clear();
        if(parametermap.size()>0)
            maxiterations=parametermap.value(parametermap.keys().at(0)).size();
    }
    delete progressbar;
    progressbar = new ProgressBar(viewer);
    progressbar->setRange(0,parametermap.size()*(maxiterations-startiteration));
    scenebuffer = new QGraphicsScene(viewer);
    qDebug("DiagramScene::updateScene() Stage:3");

    for(int counter=0; counter < currentparameters.size(); counter++)
    {
        parameterindex=counter;
        if(!showResultOfParameter(currentparameters.keys().at(counter),QPen(QColor(150,(counter*100)%256,(counter*50)%256))))
            return false;
    }
    qDebug("DiagramScene::updateScene() Stage:4");
    progressbar->setVisible(false);
    if(maxiterations>0)
    {
        startiteration=maxiterations-1;
    }
    else
    {
        startiteration=maxiterations;
    }

    if(!showGrid())
        return false;

    if(!onlysvgbuffer)
    {
        if(viewer->scene()==currentscene)
        {
            QGraphicsScene *oldscene = viewer->scene();
            currentscene=scenebuffer;
            enableScene();
            delete oldscene;
        }
        else
        {
            delete currentscene;
            currentscene=scenebuffer;
        }
    }
    else
    {
        if(svgbuffer!=NULL)
            delete svgbuffer;

        svgbuffer=scenebuffer;
    }

    qDebug("DiagramScene::updateScene(): DONE");
    return true;
}

bool DiagramScene::showResultOfParameter(QString name, QPen pen)
{
    qDebug("DiagramScene::showResultOfParameter()");

    bool visible = !follow;

    if(diny)
        pen.setWidthF(0.2);

    if(map.contains(name) && map[name]==NULL)
    {
        map.remove(name);
    }

    if(!map.contains(name))
    {
        map[name] = scenebuffer->addPath(QPainterPath(),pen);
    }

    QPainterPath tmppath = map[name]->path();
    qDebug() << "DiagramGui::showResultOfParameter(): Startiteration=" << startiteration << " Maxiterations=" << maxiterations;
    QVector<QVector<double> > result = currentparameters.value(name);

    for(int counter=startiteration; counter<currentparameters.value(name).size(); counter++)
    {
        if(visible && timer.elapsed()>elapsed)
        {
            progressbar->setVisible(true);
            progressbar->setProgress(parameterindex*(maxiterations-startiteration)+(counter-startiteration));
        }

        double value = 0;

        if(result.at(counter).size()>1)
        {
            qDebug() << "DiagramGui::showResultOfParameter(): Make sum of vector";
            for(int index=0; index<result.at(counter).size(); index++)
                value += result.at(counter).at(index);
        }
        else
        {
            value = result.at(counter).at(0);
        }

        if(counter==startiteration)
        {
            tmppath.moveTo(counter+1,-value);
        }

        if(!tmppath.contains(QPoint(counter+1,static_cast<int>(-value))))
        {

            tmppath.lineTo(counter+1,-value);
        }
        else
        {
            tmppath.moveTo(counter+1,-value);
        }

        if(maxvalue<value)
            maxvalue=value;

        if(minvalue>value)
            minvalue=value;
    }

    if(maxiterations!=0)
    {
        map[name] = scenebuffer->addPath(tmppath,pen);
    }

    return true;
}

bool DiagramScene::showGrid()
{
    qDebug("DiagramScene::showGrid()");

    QPen pen;
    QPen black;
    QPen dinypen;
    dinypen.setWidthF(0.1);

    if(!diny)
    {
        pen=Qt::NoPen;
        black=Qt::NoPen;
    }
    else
    {
        pen.setWidthF(0.1);
        black.setWidthF(0.1);
        pen.setColor(Qt::gray);
    }

    double steps = 10;

    if(maxiterations < 100)
        maxiterations = 100;

    double ydistance = qAbs(minvalue)+qAbs(maxvalue);
    double xsteps = 10;
    double ysteps = ((ydistance)/steps);

    grid = new QGraphicsLineItem();

    if(xsteps==0||ysteps==0)
        return grid;

    for(double counter=0; counter<maxiterations; counter+=xsteps)
    {
        QGraphicsLineItem *line = new QGraphicsLineItem(counter,-minvalue,counter,-maxvalue,grid);
        QGraphicsTextItem *text = new QGraphicsTextItem ( QString::number((int)counter), grid);

        if(counter!=0)
        {
            line->setPen(pen);
        }
        else
        {
            line->setPen(dinypen);
        }

        double size = text->font().pointSize();
        fontsize = ysteps/size*2;
        qreal xs = xsteps*0.1/size;
        qreal ys = ysteps*0.15/size;
        text->scale(xs,ys);
        text->setPos(counter,-(2*fontsize));
    }

    for(double counter=0; counter<qAbs(maxvalue); counter+=ysteps)
    {
        QGraphicsLineItem *line = new QGraphicsLineItem(0,-counter,maxiterations,-counter,grid);


        if(counter!=0)
        {
            if(diny)
                pen.setWidthF(0.05);

            line->setPen(pen);
            QGraphicsTextItem *text = new QGraphicsTextItem ( QString::number((int)counter), grid);

            double size = text->font().pointSize();
            qreal xs = xsteps*0.1/size;
            qreal ys = ysteps*0.15/size;
            text->scale(xs,ys);
            text->setPos(0,-counter);
        }
        else
        {
            if(diny)
            {
                dinypen.setWidthF(0.05);
                dinypen.setColor(Qt::black);
                line->setPen(dinypen);
            }
        }
    }

    for(double counter=ysteps; counter<qAbs(minvalue); counter+=ysteps)
    {
        QGraphicsLineItem *line = new QGraphicsLineItem(0,counter,maxiterations,counter,grid);
        QGraphicsTextItem *text = new QGraphicsTextItem ( QString::number(-(int)counter), grid);

        line->setPen(pen);

        double size = text->font().pointSize();
        qreal xs = xsteps*0.1/size;
        qreal ys = ysteps*0.15/size;
        text->scale(xs,ys);
        text->setPos(0,counter);
    }

    for(int counter=0; counter < currentparameters.size(); counter++)
    {
        QGraphicsTextItem *text = new QGraphicsTextItem (currentparameters.keys().at(counter), grid);
        text->setDefaultTextColor(QColor(150,(counter*100)%256,(counter*50)%256));
        double size = text->font().pointSize();
        qreal xs = xsteps*0.1/size;
        qreal ys = ysteps*0.15/size;
        text->scale(xs,ys);
        text->setPos(12,-maxvalue + counter*(4*ysteps*0.1) );


    }

    if(!diny)
    {

        xline = new QGraphicsLineItem(0,-minvalue,0,-maxvalue,grid);
        position = new QGraphicsTextItem ("");
        double size = position->font().pointSize();
        qreal xs = xsteps*0.1/size;
        qreal ys = ysteps*0.15/size;
        position->scale(xs,ys);
        position->setPos(1,1);
        rect = new QGraphicsRectItem(1,1,40,ysteps*0.75,grid);
        QColor color = Qt::lightGray;
        color.setAlpha(200);
        QBrush brush(color);
        rect->setBrush(brush);
        rect->setPen(Qt::NoPen);
        position->setParentItem(rect);
        rect->setVisible(false);

    }
    scenebuffer->addItem(grid);
    return true;
}

void DiagramScene::clear()
{
    startiteration=0;
    maxiterations=0;
    maxvalue=10;
    minvalue=-10;
    map.clear();
}

void DiagramScene::setFollow(bool followbool)
{
    if(follow==followbool)
        return;

    follow = followbool;

    if(!follow)
    {
        startiteration=0;
        maxiterations=0;
    }
}

bool DiagramScene::setMousePosition(qreal x, qreal y)
{
    if(viewer->scene()!=currentscene)
        return false;

    QMutexLocker locker(mutex);
    qDebug() << "DiagramScene::mouseMoveEvent:";

    if(maxiterations < x || x < 0)
        return false;

    double texty = -y;
    xline->setPos(x,0);
    rect->setPos(x,y);
    position->setPlainText("Position: Iteration=" + QString::number(x) + " Y=" + QString::number(texty) + "\n" + positiontext);
    currentmouseposition=static_cast<int>(x);
    qDebug() << "DiagramGui::mouseMoveEvent: Iteration=" << x << " y=" << y;
    rect->setVisible(true);
    return true;

}

double DiagramScene::getMinValue()
{
    return minvalue;
}

double DiagramScene::getMaxValue()
{
    return maxvalue;
}

double DiagramScene::getMaxIteration()
{
    return maxiterations;
}

bool DiagramScene::enableScene()
{
    qDebug() << "DiagramGui::enableScene: x=" << QString::number(currentscene->sceneRect().x()) << " y=" << QString::number(currentscene->sceneRect().y()) << " w=" << QString::number(currentscene->sceneRect().width()) << " h=" << QString::number(currentscene->sceneRect().height());
    currentscene->setSceneRect(0,qRound(-(getMaxValue())),getMaxIteration(),qRound(fabs((getMinValue()))+fabs((getMaxValue()))));
    viewer->setScene(currentscene);
    viewer->fitInView(0,qRound(-(getMinValue())),100,qRound(fabs((getMinValue()))+fabs((getMaxValue()))),Qt::IgnoreAspectRatio);

    if(currentparameters.size()>0 && follow)
        viewer->centerOn(currentparameters.value(currentparameters.keys().at(0)).size(),qRound(-getMinValue()));

    return true;
}

void DiagramScene::setPositionText(QString text)
{
    positiontext=text;
}

int DiagramScene::getXPositionOfCourser()
{
    return currentmouseposition;
}

bool DiagramScene::saveSvg(QString filename)
{
    qDebug() << "DiagramScene::saveSvg()";
    if(filename=="")
        return false;

    QString path = filename;
    int dpi = 200;
    QSvgGenerator generator;
    generator.setResolution(dpi);
    generator.setFileName(path);
    generator.setTitle("Calimero calibration results");
    generator.setDescription("Results of each calibration iteration");
    QPainter painter;
    painter.begin(&generator);
    clear();
    diny=true;
    updateScene(currentparameters,true);
    svgbuffer->render(&painter,QRectF(0,0,11*maxiterations,800),QRectF(0,qRound(-(getMaxValue())),maxiterations,qRound(fabs((getMinValue()))+fabs((getMaxValue())))),Qt::IgnoreAspectRatio);
    painter.end();
    diny=false;
    qDebug() << "DiagramScene::saveSvg() Done";
    return true;
}
