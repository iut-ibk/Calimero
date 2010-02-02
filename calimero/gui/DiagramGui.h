#ifndef DIAGRAMGUI_H_INCLUDED
#define DIAGRAMGUI_H_INCLUDED

#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMutex>
#include <QtCore>
#include "./Diagramscene.h"



class QMenu;
class Model;

class DiagramGui : public QGraphicsView
{
    Q_OBJECT

    public:
        DiagramScene *calibrationscene, *comparescene, *currentscene;
        QAction *actionminvar;
        QAction *actioncalvar;
        QAction *actionsave;
        QAction *actionsvg;
        QMenu *menu;
        bool follow, forceupdate, updaterunning, calibrationparameters;
        Model *model;
        QMutex mutex;
        int lastiteration;

    public:
        DiagramGui(QWidget *parent);
        ~DiagramGui();
        void reset();
        void contextMenuEvent(QContextMenuEvent *event);
        void resizeEvent ( QResizeEvent * event );
        void enableFollow(bool f);

    public slots:
        void update(Model *m);
        void menuAction(QAction *a);
        void mouseMoveEvent ( QMouseEvent * event );

    protected:
        virtual void changeEvent(QEvent *e);
};

#endif // DIAGRAMGUI_H_INCLUDED
