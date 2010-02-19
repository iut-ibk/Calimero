#include <QtGui>
#include <QtCore>
#include <QPainterPath>
#include "./DiagramGui.h"
#include "../core/Model.h"
#include "../core/IterationResult.h"
#include "../core/CalibrationVariable.h"
#include "../core/CalculationVariable.h"
#include <cmath>
#include <QSvgGenerator>
#include "ProgressBar.h"

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
    lastiteration=1;
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
   qDebug() << "DiagramGui::menuAction()";

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
        qDebug() << "DiagramGui::menuAction(): Export Ieration";
        int currentmouseiteration = currentscene->getXPositionOfCourser();
        QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "./",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if(dirname!="")
        {
            if(model->exportIteration(currentmouseiteration-1,dirname))
            {
                QMessageBox::information(this,tr("Information"),tr("Export complete"));
                qDebug() << "DiagramGui::menuAction(): Export complete";
            }
            else
            {
                QMessageBox::critical(this,tr("Export Error"),tr("Not possible do export iteration number ") + QString::number(currentmouseiteration));
                qWarning() << "DiagramGui::menuAction(): Not possible do export iteration number " << QString::number(currentmouseiteration);
            }
        }
    }
}

void DiagramGui::update(Model *m)
{
   if(updaterunning)
        return;

   if(m==NULL)
        return;



    QMutexLocker locker(&mutex);
    qDebug() << "DiagramGui::update()";


    if(m!=model)
        model=m;

    if(!isVisible())
        return;

    if(model->modelLocked())
        return;

    if(lastiteration == model->numberOfCompleteIterations())
        return;

    updaterunning=true;
    ::ProgressBar progress(this);
    progress.setRange(0,5);
    if(!follow)
        progress.setVisible(true);
    QCoreApplication::processEvents();
    lastiteration = 0;
    progress.setProgress(1);
    QCoreApplication::processEvents();

    progress.setProgress(2);
    QCoreApplication::processEvents();
    double value = 0;
    int iteration = model->getBestIteration(&value);
    progress.setProgress(3);

    qDebug() << "DiagramGui::update() Searching for best iteration done";
    QMap<QString,QVector<QVector<double> > > evalpar = model->getEvaluatingParametersResult();
    progress.setProgress(4);
    QCoreApplication::processEvents();
    QMap<QString,QVector<QVector<double> > > algpar = model->getAlgParametersResult();
    progress.setProgress(5);
    QCoreApplication::processEvents();
    progress.setVisible(false);
    QCoreApplication::processEvents();
    qDebug() << "DiagramGui::update() extracting data done";
    calibrationscene->setPositionText("Best calibration at iteration " + QString::number(iteration) + " with value " + QString::number(value));
    comparescene->setPositionText("Best calibration at iteration " + QString::number(iteration) + " with value " + QString::number(value));
    calibrationscene->updateScene(evalpar);
    comparescene->updateScene(algpar);

    updaterunning = false;
    lastiteration=model->numberOfCompleteIterations();
    qDebug() << "DiagramGui::update()....DONE";
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
    lastiteration=-1;
    update(model);
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
