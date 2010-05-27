#include <QtGui>
#include <QtCore>
#include <QPainterPath>
#include <diagramgui.h>
#include <cmath>
#include <QSvgGenerator>
#include <Calibration.h>
#include <IterationResult.h>

#include <iostream>

DiagramGui::~DiagramGui()
{
     QObject::disconnect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuAction(QAction*)));
}

DiagramGui::DiagramGui(QWidget *parent) : QGraphicsView(parent)
{
    calibrationparameters=true;
    follow=false;
    forceupdate=false;
    updaterunning=false;
    calibration=0;

    //creating cotextmenue
    menu = new QMenu(this);
    actionsave = new QAction(tr("Export iteration"), menu);
    actionminvar = new QAction(tr("Compare parameters"),menu);
    actioncalvar = new QAction(tr("Calibration parameters"),menu);
    actionsvg = new QAction(tr("Export to Svg"),menu);

    menu->addAction(actionsave);
    menu->addAction(actionminvar);
    menu->addAction(actioncalvar);
    menu->addAction(actionsvg);

    QObject::connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuAction(QAction*)));

    //setting up Scene
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    comparescene = new DiagramScene(this);
    calibrationscene = new DiagramScene(this);
    currentscene = calibrationscene;
    currentscene->enableScene();
    lastiteration=-1;
    comparescene->updateScene(QMap<QString,QVector<QVector<double> > >());
    calibrationscene->updateScene(QMap<QString,QVector<QVector<double> > >());
}

void DiagramGui::resizeEvent ( QResizeEvent * )
{
    fitInView(0,qRound(-(qRound(currentscene->getMinValue()))),100,qRound(fabs((currentscene->getMinValue()))+fabs((currentscene->getMaxValue()))),Qt::IgnoreAspectRatio);
}

void DiagramGui::contextMenuEvent(QContextMenuEvent *event)
{
    if(updaterunning)
        return;

    menu->exec(event->globalPos());
}

void DiagramGui::menuAction(QAction *a)
{
   if(updaterunning)
        return;

    if(a==actioncalvar)
    {
        if(calibrationparameters)
            return;

        calibrationparameters=true;
        currentscene=calibrationscene;
        currentscene->enableScene();
        currentscene->setFollow(follow);
    }

    if(a==actionminvar)
    {
        if(!calibrationparameters)
            return;

        calibrationparameters=false;
        currentscene=comparescene;
        currentscene->enableScene();
        currentscene->setFollow(follow);
    }

    if(a==actionsvg)
    {
        QString filename = QFileDialog::getSaveFileName(this,tr("Save file"), "./", "*.svg");

        if(filename!="")
        {
            if(!filename.contains(".svg"))
                filename+=".svg";
            currentscene->saveSvg(filename);
        }
    }

    if(a==actionsave)
    {
        int currentmouseiteration = currentscene->getXPositionOfCourser();
        QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "./",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if(dirname!="")
        {
            /*if(model->exportIteration(currentmouseiteration-1,dirname))
            {
                QMessageBox::information(this,tr("Information"),tr("Export complete"));
                qDebug() << "DiagramGui::menuAction(): Export complete";
            }
            else
            {
                QMessageBox::critical(this,tr("Export Error"),tr("Not possible do export iteration number ") + QString::number(currentmouseiteration));
                qWarning() << "DiagramGui::menuAction(): Not possible do export iteration number " << QString::number(currentmouseiteration);
            }
            */
        }
    }
}

void DiagramGui::showResults(Calibration *c)
{
   QMutexLocker locker(&mutex);
   if(updaterunning)
        return;

   if(c==NULL)
        return;

    if(c!=calibration)
        calibration=c;

    if(lastiteration == calibration->getNumOfComplete())
        return;

    updaterunning=true;
    lastiteration = 0;

    map<int,IterationResult*> results = calibration->getIterationResults();
    QMap<QString,QVector<QVector<double> > > evalpar;
    QMap<QString,QVector<QVector<double> > > algpar;
    set<string> calibrationparameters = calibration->evalCalibrationParameters();
    set<string> objectivefunctionparameters = calibration->evalObjectiveFunctionParameters();

    set<string>::iterator iterator;

    for(iterator=calibrationparameters.begin(); iterator!=calibrationparameters.end(); iterator++)
    {
        QVector<QVector<double> > result;
        for(uint index = 0; index < results.size(); index++)
        {
            if(!results[index])
                break;
            QVector<double> tmp = QVector<double>::fromStdVector(results[index]->getCalibrationParameterResults(*iterator));
            if(tmp.size() > 0)
                result.append(tmp);
        }

        algpar[QString::fromStdString(*iterator)]=result;
    }


    for(iterator=objectivefunctionparameters.begin(); iterator!=objectivefunctionparameters.end(); iterator++)
    {
        QVector<QVector<double> > result;
        for(uint index = 0; index < results.size(); index++)
        {
            if(!results[index])
                break;
            QVector<double> tmp = QVector<double>::fromStdVector(results[index]->getObjectiveFunctionParameterResults(*iterator));
            if(tmp.size() > 0)
                result.append(tmp);
        }

        evalpar[QString::fromStdString(*iterator)]=result;
    }

    //calibrationscene->setPositionText("Best calibration at iteration " + QString::number(iteration) + " with value " + QString::number(value));
    //comparescene->setPositionText("Best calibration at iteration " + QString::number(iteration) + " with value " + QString::number(value));
    calibrationscene->updateScene(algpar);
    comparescene->updateScene(evalpar);

    updaterunning = false;
    lastiteration=calibration->getNumOfComplete();
}

void DiagramGui::mouseMoveEvent ( QMouseEvent * event )
{
    if(updaterunning)
        return;

    QMutexLocker locker(&mutex);

    if(QGraphicsView::scene()==NULL)
        return;

    QPointF point = mapToScene(event->pos().x(),event->pos().y());
    currentscene->setMousePosition(qRound(point.x()),point.y());
}

void DiagramGui::enableFollow(bool f)
{
    if(f==follow)
        return;

    comparescene->setFollow(f);
    calibrationscene->setFollow(f);
    follow=f;

    if(follow)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    lastiteration=-1;
    showResults(calibration);
}

void DiagramGui::changeEvent(QEvent *e)
{
    QGraphicsView::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        menu->removeAction(actionsave);
        menu->removeAction(actionminvar);
        menu->removeAction(actioncalvar);
        menu->removeAction(actionsvg);

        delete actionsave;
        delete actionminvar;
        delete actioncalvar;
        delete actionsvg;

        actionsave = new QAction(tr("Export iteration"), menu);
        actionminvar = new QAction(tr("Compare parameters"),menu);
        actioncalvar = new QAction(tr("Calibration parameters"),menu);
        actionsvg = new QAction(tr("Export to Svg"),menu);

        menu->addAction(actionsave);
        menu->addAction(actionminvar);
        menu->addAction(actioncalvar);
        menu->addAction(actionsvg);
        break;
    default:
        break;
    }

}
