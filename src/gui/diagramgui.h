#ifndef DIAGRAMGUI_H_INCLUDED
#define DIAGRAMGUI_H_INCLUDED

#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMutex>
#include <QtCore>
#include <diagramscene.h>

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

    public:
        DiagramGui(QWidget *parent);
        ~DiagramGui();
        void contextMenuEvent(QContextMenuEvent *event);
        void resizeEvent ( QResizeEvent * event );

    public Q_SLOTS:
        void showResults(Calibration *calibration);
        void menuAction(QAction *a);
        void mouseMoveEvent ( QMouseEvent * event );

    protected:
        virtual void changeEvent(QEvent *e);
};

#endif // DIAGRAMGUI_H_INCLUDED
