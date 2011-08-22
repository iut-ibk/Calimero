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
#include <QtGui>
#include <QtCore>
#include <QPainterPath>
#include <diagramgui.h>
#include <cmath>
#include <QSvgGenerator>
#include <Calibration.h>
#include <IterationResult.h>
#include <boost/shared_ptr.hpp>
//#include <QtOpenGL/QGLWidget>

#include <iostream>

using namespace boost;

DiagramGui::~DiagramGui()
{
     QObject::disconnect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuAction(QAction*)));
}


void DiagramGui::resizeEvent ( QResizeEvent * event)
{
    fitInView(scene()->sceneRect(),Qt::KeepAspectRatioByExpanding);
    centerOn(scene()->sceneRect().topLeft().x(),qRound(scene()->sceneRect().topLeft().y()));
}

DiagramGui::DiagramGui(QWidget *parent) : QGraphicsView(parent)
{
    //creating cotextmenue
    //setViewport(new QGLWidget);
    //setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing | QPainter::NonCosmeticDefaultPen);
    menu = new QMenu(this);
    actionminvar = new QAction(tr("Compare parameters"),menu);
    actioncalvar = new QAction(tr("Calibration parameters"),menu);

    menu->addAction(actionminvar);
    menu->addAction(actioncalvar);

    QObject::connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuAction(QAction*)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    comparescene = new DiagramScene(this);
    calibrationscene = new DiagramScene(this);

    comparescene->setPrec(PREC);
    calibrationscene->setPrec(PREC);

    setScene(comparescene);
    show();
}



void DiagramGui::contextMenuEvent(QContextMenuEvent *event)
{
    menu->exec(event->globalPos());
}

void DiagramGui::menuAction(QAction *a)
{
    if(a==actioncalvar)
    {
        if(scene()==calibrationscene)
            return;
        setScene(calibrationscene);
        fitInView(scene()->sceneRect(),Qt::KeepAspectRatioByExpanding);
        centerOn(scene()->sceneRect().topLeft().x(),qRound(scene()->sceneRect().topRight().y()));
    }

    if(a==actionminvar)
    {
        if(scene()==comparescene)
            return;

        setScene(comparescene);
        fitInView(scene()->sceneRect(),Qt::KeepAspectRatioByExpanding);
        centerOn(scene()->sceneRect().topLeft().x(),qRound(scene()->sceneRect().topRight().y()));
    }
}

void DiagramGui::showResults(Calibration *calibration)
{
    vector<IterationResult * > resultvec = calibration->getIterationResults();
    map<int,IterationResult * > results;

    BOOST_FOREACH(IterationResult * res, resultvec)
            results[res->getIterationNumber()]=res;

    QMap<QString,QVector<QPointF> > evalpar;
    QMap<QString,QVector<QPointF> > algpar;
    set<string> calibrationparameters = calibration->evalCalibrationParameters();
    set<string> objectivefunctionparameters = calibration->evalObjectiveFunctionParameters();

    set<string>::iterator iterator;

    //calibrationparameters
    for(iterator=calibrationparameters.begin(); iterator!=calibrationparameters.end(); iterator++)
    {
        QVector<QPointF> result;
        for(uint index = 0; index < results.size(); index++)
        {
            if(!results[index])
                continue;

            if(!results[index]->getCalibrationParameterResults(*iterator).size())
                continue;

            QPointF tmp(index,results[index]->getCalibrationParameterResults(*iterator)[0]);
            result.append(tmp);
        }

        algpar[QString::fromStdString(*iterator)]=result;
    }

    //objectivefunctionparameters
    for(iterator=objectivefunctionparameters.begin(); iterator!=objectivefunctionparameters.end(); iterator++)
    {
        QVector<QPointF> result;
        for(uint index = 0; index < results.size(); index++)
        {
            if(!results[index])
                continue;

            if(!results[index]->getObjectiveFunctionParameterResults(*iterator).size())
                continue;

            QPointF tmp(index,results[index]->getObjectiveFunctionParameterResults(*iterator)[0]);
            result.append(tmp);
        }

        evalpar[QString::fromStdString(*iterator)]=result;
    }


    calibrationscene->setValues(algpar);
    comparescene->setValues(evalpar);
}

void DiagramGui::mouseMoveEvent ( QMouseEvent * event )
{
    QPointF point = mapToScene(event->pos().x(),event->pos().y());
    static_cast<DiagramScene*>(scene())->setMousePosition(point.x(),point.y());
}

void DiagramGui::changeEvent(QEvent *e)
{
    QGraphicsView::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        menu->removeAction(actionminvar);
        menu->removeAction(actioncalvar);

        delete actionminvar;
        delete actioncalvar;

        actionminvar = new QAction(tr("Compare parameters"),menu);
        actioncalvar = new QAction(tr("Calibration parameters"),menu);

        menu->addAction(actionminvar);
        menu->addAction(actioncalvar);
        break;
    default:
        break;
    }

}
