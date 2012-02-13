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

#ifndef DIAGRAMGUI_H_INCLUDED
#define DIAGRAMGUI_H_INCLUDED

#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMutex>
#include <QtCore>
#include <diagramscene.h>
#include <QStatusBar>

#define PREC 100

class QMenu;
class Calibration;

class DiagramGui : public QGraphicsView
{
    Q_OBJECT

    public:
        DiagramScene *calibrationscene, *comparescene;
        QAction *actionminvar;
        QAction *actioncalvar;
        QMenu *menu;
        bool updateting;

    public:
        DiagramGui(QWidget *parent);
        ~DiagramGui();
        void contextMenuEvent(QContextMenuEvent *event);
        void resizeEvent ( QResizeEvent * event );

    public Q_SLOTS:
        void showResults(Calibration *calibration, QStatusBar *status);
        void menuAction(QAction *a);
        void mouseMoveEvent ( QMouseEvent * event );

    protected:
        virtual void changeEvent(QEvent *e);
};

class DiagramUpdaterThread : public QThread
{
private:
    QMap<QString,QVector<QPointF> > *evalpar;
    QMap<QString,QVector<QPointF> > *algpar;
    Calibration *calibration;

public:
    DiagramUpdaterThread(Calibration *calibration, QMap<QString,QVector<QPointF> > *evalpar, QMap<QString,QVector<QPointF> > *algpar);
    virtual ~DiagramUpdaterThread();
    virtual void run();
};

#endif // DIAGRAMGUI_H_INCLUDED
